#include "WatchOut.h"
#include "Global.h"

#include <libeng/Log/Log.h>
#include <libeng/Storage/Storage.h>
#include <libeng/Advertising/Advertising.h>
#include <libeng/Features/Internet/Internet.h>

#ifndef __ANDROID__
#include "OpenGLES/ES2/gl.h"
#endif
#include <boost/thread.hpp>

#define MAX_LOAD_STEP               3
#define PROGRESS_TRANS              (2.f / (MAX_TITLE_LOAD + MAX_GAME_LOAD + 1))

//////
#ifdef DEBUG
WatchOut::WatchOut() : Game2D(1), mLoadStep(0), mPlaying(false) {
#else
WatchOut::WatchOut() : Game2D(0), mLoadStep(0), mPlaying(false) {
#endif
    LOGV(LOG_LEVEL_WATCHOUT, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    if (!mGameLevel)
        mGameIntro = new Intro(Intro::LANG_EN, true);

    mGameData = new StoreData;
    mScores = Scores::getInstance(mGameData);
    Dynamic2D::initIncreaseBuffer();
    mFonts->addFont(0, FONT_WIDTH, FONT_HEIGHT, static_cast<short>(FONT_TEXTURE_WIDTH),
            static_cast<short>(FONT_TEXTURE_HEIGHT));

    mWitchSPet = NULL;
    mBackground = NULL;
    mTitle = NULL;
    mGame = NULL;
    mWaitConn = NULL;
}
WatchOut::~WatchOut() {

    LOGV(LOG_LEVEL_WATCHOUT, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    Scores::freeInstance();
    Dynamic2D::freeIncreaseBuffer();
}

bool WatchOut::start() {

    LOGV(LOG_LEVEL_WATCHOUT, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    if (Game2D::start()) {

        if (Storage::loadFile(FILE_NAME)) {

            Storage* store = Storage::getInstance();
            if (!store->isEmpty(FILE_NAME)) {
                if (!Storage::File2Data(mGameData, (*store)[FILE_NAME])) {

                    LOGW(LOG_FORMAT(" - Failed to get data from '%s' file"), __PRETTY_FUNCTION__, __LINE__, FILE_NAME);
                    mScores->create();
                    assert(NULL);
                }
                else
                    mScores->read();
            }
            else // No data stored: Create data
                mScores->create();
        }
#ifdef DEBUG
        else {

            LOGW(LOG_FORMAT(" - Failed to open existing '%s' file"), __PRETTY_FUNCTION__, __LINE__, FILE_NAME);
            assert(NULL);
        }
#endif
        return true;
    }
    else if (mGameLevel) {

        if (!(mGameLevel & 0x01))
            --mGameLevel;

#ifndef DEMO_VERSION
        if (Advertising::getStatus() == Advertising::STATUS_DISPLAYED)
            Advertising::hide(0);
#endif

        // Check if still online
#ifdef __ANDROID__
        if (!Internet::getInstance()->isOnline(2000)) {
#else
        if (!Internet::getInstance()->isOnline()) {
#endif
            assert(!mWaitConn);
            mGameLevel = 0;

            mWaitConn = new WaitConn(true);
            mWaitConn->initialize(this);
            mWaitConn->start(mScreen);
        }
    }
    else if (mWaitConn) {

        // Check if still not online
#ifdef __ANDROID__
        if (!Internet::getInstance()->isOnline(2000))
#else
        if (!Internet::getInstance()->isOnline())
#endif
            mWaitConn->resume();

        else {

            delete mWaitConn;
            mWaitConn = NULL;

            ++mGameLevel;
        }
    }
    return false;
}
void WatchOut::pause() {

    LOGV(LOG_LEVEL_WATCHOUT, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    Game2D::pause();

    mLoadStep = 0;

    if (mWitchSPet) mWitchSPet->pause();
    if (mTitle) mTitle->pause();
    if (mGame) mGame->pause();
    if (mWaitConn) mWaitConn->pause();
}
void WatchOut::destroy() {

    LOGV(LOG_LEVEL_WATCHOUT, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    Game2D::destroy();

    if (mBackground) delete mBackground;
    if (mTitle) delete mTitle;
    if (mGame) delete mGame;
    if (mWaitConn) delete mWaitConn;

    if (mWitchSPet) delete mWitchSPet;

#ifdef DEBUG
    // Check all texture objects has been deleted correctly
    if (!mTextures->chkTextures()) {
        LOGW(LOG_FORMAT(" - Bad textures management"), __PRETTY_FUNCTION__, __LINE__);
    }
    else {
        LOGI(LIBENG_LOG_LEVEL, 0, LOG_FORMAT(" - Good textures management"), __PRETTY_FUNCTION__, __LINE__);
    }
#endif
    mTextures->delTextures();
    mTextures->rmvTextures();

    Player* player = Player::getInstance();
    player->pause(ALL_TRACK);
    player->free();
    player->clean();
}

void WatchOut::wait(float millis) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_WATCHOUT, (mLog % 100), LOG_FORMAT(" - Delay: %f milliseconds"), __PRETTY_FUNCTION__, __LINE__, millis);
#endif
    boost::this_thread::sleep(boost::posix_time::microseconds(static_cast<unsigned long>(millis * 500)));
}
void WatchOut::update() {

#ifdef DEBUG
    LOGV(LOG_LEVEL_WATCHOUT, (mLog % 100), LOG_FORMAT(" - g:%d; l:%d"), __PRETTY_FUNCTION__, __LINE__, mGameLevel, mLoadStep);
#endif
    switch (mGameLevel) {
        case 0: {

            if (!mWaitConn)
                updateIntro(); // Introduction

            else { // Wait Internet

                static unsigned char wait = 0;
                if (wait < (LIBENG_WAITCONN_DELAY << 2)) {

                    ++wait;
                    break;
                }
                mWaitConn->update();
            }
            break;
        }

        ////// Flappy Witch's Pet
        case 1: {

#ifndef DEMO_VERSION
            if (!mWitchSPet) {

                mWitchSPet = new WitchSPet(this);
                mWitchSPet->initialize();
            }
            if (!mWitchSPet->load(this))
                break;
#endif
            ++mGameLevel;
            //break;
        }
        case 2: {

#ifndef DEMO_VERSION
            if (mWitchSPet->update(this))
                break;

            delete mWitchSPet;
            mWitchSPet = NULL;
#endif
            ++mGameLevel;
            //break;
        }

        ////// WatchOut
        case 3: {

            switch (mLoadStep) {
                case 0: {

                    if (!mBackground) {

                        mBackground = new Element2D;
                        mBackground->initialize(this);
                        mBackground->start(0xff, 0xff, 0xff);
                        mBackground->setTexCoords(FULL_TEXCOORD_BUFFER);

                        short adHeight = MAX_AD_HEIGHT * static_cast<short>(mScreen->yDpi) / 160;
                        mBackground->setVertices(0, mScreen->height - adHeight, mScreen->width, adHeight);
                    }
                    else
                        mBackground->resume(0xff, 0xff, 0xff);

                    mBackground->reset();
                    mBackground->translate(-2.f, 0.f);

                    ++mLoadStep;
                    break;
                }
                case 1: {

                    if (!mTitle)
                        mTitle = new TitleLevel;

                    if (!mTitle->load(this))
                        break;

                    ++mLoadStep;
                    break;
                }
                case 2: {

                    if (!mGame)
                        mGame = new GameLevel;

                    if (!mGame->load(this))
                        break;

                    ++mLoadStep;
                    break;
                }
            }
            mBackground->translate(PROGRESS_TRANS, 0.f);
            if (mLoadStep != MAX_LOAD_STEP)
                break;

            resetInputs();
            Inputs::InputUse = USE_INPUT_TOUCH;
            Player::getInstance()->resume();
            mTouchPrecision = 3;

            ++mGameLevel;
            //break;
        }
        case 4: {

            if (!mPlaying) { // Title

                mPlaying = !mTitle->update(this);
                if (mPlaying)
                    mGame->start((mScreen->top - (((MAX_AD_HEIGHT * static_cast<short>(mScreen->yDpi) / 160) << 1) /
                                                  static_cast<float>(mScreen->width))) / 2.f);
            }
            else // Game
                mPlaying = mGame->update(this);
            break;
        }

#ifdef DEBUG
        default: {

            LOGW(LOG_FORMAT(" - Unknown game level to update: %d"), __PRETTY_FUNCTION__, __LINE__, mGameLevel);
            assert(NULL);
            break;
        }
#endif
    }
}

void WatchOut::render() const {
    switch (mGameLevel) {

        case 0: {

            if (!mWaitConn)
                renderIntro(); // Introduction
            else
                mWaitConn->render(); // Wait Internet
            break;
        }

        ////// Flappy Witch's Pet
        case 1: mWitchSPet->renderLoad(); break;
        case 2: mWitchSPet->renderUpdate(); break;

        ////// WatchOut
        case 3: {

            glDisable(GL_BLEND);
            mBackground->render(true);
            glEnable(GL_BLEND);
            break;
        }
        case 4: {

            glDisable(GL_BLEND);
            mBackground->render(false);
            glEnable(GL_BLEND);

            if (!mPlaying)
                mTitle->render();
            else
                mGame->render();
            break;
        }

#ifdef DEBUG
        default: {

            LOGW(LOG_FORMAT(" - Unknown game level to render: %d"), __PRETTY_FUNCTION__, __LINE__, mGameLevel);
            assert(NULL);
            break;
        }
#endif
    }
}
