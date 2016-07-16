#include "GameLevel.h"

#include <libeng/Game/2D/Game2D.h>
#include <libeng/Graphic/Bounds/2D/Overlap2D.h>
#include <libeng/Tools/Tools.h>

#ifndef __ANDROID__
#include "OpenGLES/ES2/gl.h"
#endif
#include <boost/thread.hpp>

#ifdef __ANDROID__
#define NO_TOUCH_ID             0x12345678
#else
#define NO_TOUCH_ID             0
#endif
#define CHIP_RADIUS             0.1f
#define STAR_SCALE              0.5f
#define MAX_TRANS_VEL           0.025f

#define INCREASE_VEL_DELAY      30 // Seconds
#define DISPLAY_STAR_EVERY      30 // ...
#define DISPLAY_STAR_DELAY      14 // ...

#define CHIP_LAG                (CHIP_RADIUS / 20.f)

// Texture IDs
#define TEXTURE_ID_LOOSE        23
#define TEXTURE_ID_STAR         24

// Sound IDs
#define SOUND_ID_START          3
#define SOUND_ID_GAMEOVER       4

//////
GameLevel::GameLevel() : mLoadStep(0), mTouchID(NO_TOUCH_ID), mTriangleRotate(false), mScreenLeft(0.f),
        mScreenTop(0.f), mScreenRight(0.f), mScreenBottom(0.f), mSquareRotate(false), mTimer(0), mStep(GAME_READY),
        mScoreW(0.f), mLooseY(0.f), mOkFactor(0.f), mOkConst(0.f), mOkBottom(0.f), mStarCount(0),
        mTransVel(TRANS_VELOCITY) {

    LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    std::memset(&mScreenArea, 0, sizeof(TouchArea));

#ifndef DEMO_VERSION
    mAdLoaded = false;
    mAdvertising = Advertising::getInstance();
#endif
    mScoreData = Scores::getInstance(NULL);

    mTriangle = NULL;
    mSquare = NULL;
    mDisk = NULL;
    mChip = NULL;
    mScore = NULL;
    mGameOver = NULL;
    mMedalText = NULL;
    mScoreText = NULL;
    mBestText = NULL;
    mOkBack = NULL;
    mOkText = NULL;
    mStar = NULL;
    mStarScore = NULL;
    mStarText = NULL;
    mShare = NULL;
}
GameLevel::~GameLevel() {

    LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    if (mTriangle) delete mTriangle;
    if (mSquare) delete mSquare;
    if (mDisk) delete mDisk;
    if (mChip) delete mChip;
    if (mScore) delete mScore;
    if (mGameOver) delete mGameOver;
    if (mMedalText) delete mMedalText;
    if (mScoreText) delete mScoreText;
    if (mBestText) delete mBestText;
    if (mOkBack) delete mOkBack;
    if (mOkText) delete mOkText;
    if (mStar) delete mStar;
    if (mStarScore) delete mStarScore;
    if (mStarText) delete mStarText;
    if (mShare) delete mShare;
}

bool GameLevel::load(const Game* game) {

    LOGV(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - l:%d"), __PRETTY_FUNCTION__, __LINE__, (mLoadStep + 1));
    switch (++mLoadStep) {
        case 1: {

#ifndef DEMO_VERSION
            unsigned char adStatus = Advertising::getStatus();
            assert(adStatus != Advertising::STATUS_NONE);

            mAdLoaded = false;
            if ((adStatus == Advertising::STATUS_READY) || (adStatus == Advertising::STATUS_FAILED))
                mAdvertising->load();
            else
                mAdLoaded = (adStatus > Advertising::STATUS_LOADED); // STATUS_DISPLAYING | STATUS_DISPLAYED
#endif
            // Screen bounds
            mScreenLeft.mLeft = -1.f;
            mScreenTop.mTop = game->getScreen()->top - (((MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) /
                            160) << 1) / static_cast<float>(game->getScreen()->width));
            mScreenRight.mRight = 1.f;
            mScreenBottom.mBottom = -mScreenTop.mTop;
            break;
        }
        case 2: {

            if (!mTriangle) {

                mTriangle = new Triangle;
                mTriangle->initialize(game2DVia(game));
                mTriangle->start(game);
            }
            else
                mTriangle->resume();

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 3: {

            if (!mDisk) {

                mDisk = new Element2D;
                mDisk->initialize(game2DVia(game));
                mDisk->start(TEXTURE_ID_DISK);
                mDisk->setTexCoords(FULL_TEXCOORD_BUFFER);

                mDisk->setRed(0.f);
                mDisk->setGreen(0.f);
                mDisk->setBlue(0.f);

                static const float verCoords[8] = { -UNIT_TRIANGLE_WIDTH / 2.f, UNIT_TRIANGLE_WIDTH / 2.f,
                        -UNIT_TRIANGLE_WIDTH / 2.f, -UNIT_TRIANGLE_WIDTH / 2.f, UNIT_TRIANGLE_WIDTH / 2.f,
                        -UNIT_TRIANGLE_WIDTH / 2.f, UNIT_TRIANGLE_WIDTH / 2.f, UNIT_TRIANGLE_WIDTH / 2.f };
                mDisk->setVertices(verCoords);
#ifdef DEBUG
                Circle2D* bound = new Circle2D(0.f, 0.f, UNIT_TRIANGLE_WIDTH / 2.f);
                bound->setLog(game->getLog());
                mDisk->addBound(bound);
#else
                mDisk->addBound(new Circle2D(0.f, 0.f, UNIT_TRIANGLE_WIDTH / 2.f));
#endif
                mDisk->addVelocities(2);
            }
            else
                mDisk->resume(TEXTURE_ID_DISK);

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 4: {

            if (!mSquare) {

                mSquare = new Element2D;
                mSquare->initialize(game2DVia(game));
                mSquare->start(0x00, 0x00, 0x00);
                mSquare->setTexCoords(FULL_TEXCOORD_BUFFER);

                static const float verCoords[8] = { -UNIT_TRIANGLE_WIDTH / 2.f, UNIT_TRIANGLE_WIDTH / 2.f,
                        -UNIT_TRIANGLE_WIDTH / 2.f, -UNIT_TRIANGLE_WIDTH / 2.f, UNIT_TRIANGLE_WIDTH / 2.f,
                        -UNIT_TRIANGLE_WIDTH / 2.f, UNIT_TRIANGLE_WIDTH / 2.f, UNIT_TRIANGLE_WIDTH / 2.f };
                mSquare->setVertices(verCoords);
#ifdef DEBUG
                Polygone2D* bound = new Polygone2D(verCoords[0], verCoords[1], verCoords[2], verCoords[3], verCoords[4],
                        verCoords[5], verCoords[6], verCoords[7]);
                bound->setLog(game->getLog());
                mSquare->addBound(bound);
#else
                mSquare->addBound(new Polygone2D(verCoords[0], verCoords[1], verCoords[2], verCoords[3], verCoords[4],
                        verCoords[5], verCoords[6], verCoords[7]));
#endif
                mSquare->addVelocities(2);
            }
            else
                mSquare->resume(0x00, 0x00, 0x00);

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 5: {

            Player::loadSound(SOUND_ID_COLLISION);
            Player::loadSound(SOUND_ID_START);
            Player::loadSound(SOUND_ID_GAMEOVER);
            Player::loadSound(SOUND_ID_BONUS);

            Textures::getInstance()->genTexture(Textures::getInstance()->loadTexture(Facebook::TEXTURE_ID));
            Textures::getInstance()->genTexture(Textures::getInstance()->loadTexture(Twitter::TEXTURE_ID));
            Textures::getInstance()->genTexture(Textures::getInstance()->loadTexture(Google::TEXTURE_ID));

            mScreenArea.left = 0;
            mScreenArea.top = MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) / 160;
            mScreenArea.right = game->getScreen()->width;
            mScreenArea.bottom = game->getScreen()->height - mScreenArea.top;
            break;
        }
        case 6: {

            if (!mChip) {

                mChip = new Element2D;
                mChip->initialize(game2DVia(game));
                mChip->start(TEXTURE_ID_DISK);
                mChip->setTexCoords(FULL_TEXCOORD_BUFFER);

                //mChip->setRed(0xff / MAX_COLOR);
                mChip->setGreen(0x00 / MAX_COLOR);
                mChip->setBlue(0x00 / MAX_COLOR);

                static const float verCoords[8] = { -CHIP_RADIUS, CHIP_RADIUS, -CHIP_RADIUS, -CHIP_RADIUS, CHIP_RADIUS,
                        -CHIP_RADIUS, CHIP_RADIUS, CHIP_RADIUS };
                mChip->setVertices(verCoords);
#ifdef DEBUG
                Circle2D* bound = new Circle2D(0.f, 0.f, CHIP_RADIUS * 0.8f); // 80%
                bound->setLog(game->getLog());
                mChip->addBound(bound);
#else
                mChip->addBound(new Circle2D(0.f, 0.f, CHIP_RADIUS * 0.8f));
#endif
            }
            else
                mChip->resume(TEXTURE_ID_DISK);

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 7: {

            if (!mScore) {

                mScore = new Text2D;
                mScore->initialize(game2DVia(game));
                mScore->start(L"0");

                float scale = (MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) / 160) / static_cast<float>(FONT_HEIGHT);
                mScoreW = ((FONT_WIDTH << 1) / static_cast<float>(game->getScreen()->width)) * (scale / 2.f); // Half
                mScore->scale(scale, scale);
                mScore->position(-mScoreW, game->getScreen()->bottom + (((MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) /
                        160) << 1) / static_cast<float>(game->getScreen()->width)));
            }
            else
                mScore->resume();

            mScore->setColor(1.f, 1.f, 0.f);

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 8: {

            if (!mGameOver) {

                mGameOver = new Static2D;
                mGameOver->initialize(game2DVia(game));
                mGameOver->start(TEXTURE_ID_LOOSE);
                mGameOver->setTexCoords(FULL_TEXCOORD_BUFFER);

                //mGameOver->setRed(0xff / MAX_COLOR);
                //mGameOver->setGreen(0xff / MAX_COLOR);
                mGameOver->setBlue(0x00 / MAX_COLOR);

                short yPos = (((game->getScreen()->height - (MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) / 160) -
                        (game->getScreen()->height >> 1)) >> 2) * 3) + (game->getScreen()->height >> 1);
                mGameOver->setVertices(0, yPos + (game->getScreen()->width >> 3), game->getScreen()->width, yPos -
                        (game->getScreen()->width >> 3));

                yPos -= game->getScreen()->width >> 3;
                mLooseY = (game->getScreen()->height - ((game->getScreen()->height - yPos) << 1)) /
                        static_cast<float>(game->getScreen()->width);
                mLooseY -= ((mLooseY - (game->getScreen()->bottom + (((MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) /
                        160) << 1) / static_cast<float>(game->getScreen()->width)))) - (1.f + (g_TriangleVertices[3] -
                        g_TriangleVertices[1]))) / 2.f;
            }
            else
                mGameOver->resume(TEXTURE_ID_LOOSE);

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 9: {

            if (!mShare) {

                mShare = new Share;
                mShare->start(game, mLooseY);
            }
            else
                mShare->resume();

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 10: {

            if (!mMedalText) {

                mMedalText = new Text2D;
                mMedalText->initialize(game2DVia(game));
                mMedalText->start(L"Medal");

                float scale = game->getScreen()->width / FONT_SCALE_RATIO;
                mMedalText->scale(scale, scale);

                mMedalText->position(((FONT_WIDTH << 1) / static_cast<float>(game->getScreen()->width)) * scale * -6.f,
                        mLooseY);
                // -6.f = 5 letters + 1
            }
            else
                mMedalText->resume();

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 11: {

            if (!mScoreText) {

                mScoreText = new Text2D;
                mScoreText->initialize(game2DVia(game));
                mScoreText->start(L"Score");

                float scale = game->getScreen()->width / FONT_SCALE_RATIO;
                mScoreText->scale(scale, scale);
                mScoreText->position(1.f - ((FONT_WIDTH << 1) / static_cast<float>(game->getScreen()->width)) *
                        scale * 5.f, mLooseY);
            }
            else
                mScoreText->resume();

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 12: {

            if (!mBestText) {

                mBestText = new Text2D;
                mBestText->initialize(game2DVia(game));
                mBestText->start(L"Best");

                float scale = game->getScreen()->width / FONT_SCALE_RATIO;
                mBestText->scale(scale, scale);
                mBestText->position(1.f - ((FONT_WIDTH << 1) / static_cast<float>(game->getScreen()->width)) *
                        scale * 4.f, mLooseY - 0.5f);
            }
            else
                mBestText->resume();

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 13: {

            if (!mOkBack) {

                mOkBack = new Triangle;
                mOkBack->initialize(game2DVia(game));
                mOkBack->start(game);

                mOkBack->translate(1.f + g_TriangleVertices[0], mLooseY - 1.f - g_TriangleVertices[3]);

                float yPos = mLooseY - 1.f - g_TriangleVertices[3];
                mOkConst = g_TriangleVertices[3] + yPos;
                mOkFactor = (g_TriangleVertices[1] - g_TriangleVertices[3]) / g_TriangleVertices[0];
                mOkBottom = g_TriangleVertices[1] + yPos;
            }
            else
                mOkBack->resume();

            mOkBack->setRed(0xff / MAX_COLOR);
            mOkBack->setGreen(0xff / MAX_COLOR);
            mOkBack->setBlue(0x00 / MAX_COLOR);

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 14: {

            if (!mOkText) {

                mOkText = new Text2D;
                mOkText->initialize(game2DVia(game));
                mOkText->start(L"Ok");

                float scale = game->getScreen()->width / FONT_SCALE_RATIO;
                mOkText->scale(scale, scale);
                mOkText->position(1.f - (((FONT_WIDTH << 1) / static_cast<float>(game->getScreen()->width)) * scale * 3.f),
                        mLooseY + (((FONT_HEIGHT << 1) / static_cast<float>(game->getScreen()->width)) * scale) - 1.f -
                        (g_TriangleVertices[3] - g_TriangleVertices[1]));
            }
            else
                mOkText->resume();

            mOkText->setColor(0.f, 0.f, 0.f);

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 15: {

            if (!mStar) {

                mStar = new Element2D;
                mStar->initialize(game2DVia(game));
                mStar->start(TEXTURE_ID_STAR);
                mStar->setTexCoords(FULL_TEXCOORD_BUFFER);

                //mStar->setRed(0xff / MAX_COLOR);
                //mStar->setGreen(0xff / MAX_COLOR);
                mStar->setBlue(0x00 / MAX_COLOR);

                mStar->copyVertices(*mSquare);
                mStar->scale(STAR_SCALE, STAR_SCALE);
                mStar->setAlpha(0.f);
#ifdef DEBUG
                Circle2D* bound = new Circle2D(0.f, 0.f, (UNIT_TRIANGLE_WIDTH / 2.f) * STAR_SCALE * 0.8f); // 80%
                bound->setLog(game->getLog());
                mStar->addBound(bound);
#else
                mStar->addBound(new Circle2D(0.f, 0.f, (UNIT_TRIANGLE_WIDTH / 2.f) * STAR_SCALE * 0.8f));
#endif
            }
            else
                mStar->resume(TEXTURE_ID_STAR);

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 16: {

            if (!mStarScore) {

                mStarScore = new Element2D;
                mStarScore->initialize(game2DVia(game));
                mStarScore->start(TEXTURE_ID_STAR);
                mStarScore->setTexCoords(FULL_TEXCOORD_BUFFER);

                //mStarScore->setRed(0xff / MAX_COLOR);
                //mStarScore->setGreen(0xff / MAX_COLOR);
                mStarScore->setBlue(0x00 / MAX_COLOR);

                short heightAd = MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) / 160;
                mStarScore->setVertices((game->getScreen()->width >> 1) - (heightAd >> 1),
                        (game->getScreen()->height >> 1) + (heightAd >> 1), (game->getScreen()->width >> 1) +
                        (heightAd >> 1), (game->getScreen()->height >> 1) - (heightAd >> 1));
                mStarScore->setAlpha(0.f);
                mStarScore->translate((heightAd / static_cast<float>(game->getScreen()->width)) - 1.f,
                        game->getScreen()->bottom + (heightAd / static_cast<float>(game->getScreen()->width)));
            }
            else
                mStarScore->resume(TEXTURE_ID_STAR);

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case MAX_GAME_LOAD: {

            if (!mStarText) {

                mStarText = new Text2D;
                mStarText->initialize(game2DVia(game));
                mStarText->start(L"x1");

                float scale = (game->getScreen()->width / FONT_SCALE_RATIO) * 0.8f; // 80%
                mStarText->scale(scale, scale);
                mStarText->position((((MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) / 160) << 1) /
                        static_cast<float>(game->getScreen()->width)) - 1.f, game->getScreen()->bottom +
                        (((FONT_HEIGHT << 1) / static_cast<float>(game->getScreen()->width)) * scale));
                mStarText->setAlpha(0.f);
            }
            else
                mStarText->resume();

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            mTimer = time(NULL) - mScoreData->mScore; // Avoid time elapsed during a pause/resume operation
            break;
        }

#ifdef DEBUG
        default: {

            LOGW(LOG_FORMAT(" - Unknown load step: %d"), __PRETTY_FUNCTION__, __LINE__, mLoadStep);
            assert(NULL);
            break;
        }
#endif
    }
    return (mLoadStep == MAX_GAME_LOAD);
}

#ifndef DEMO_VERSION
void GameLevel::adDisplay(bool delay) {

    if (!mAdLoaded) {

        static unsigned char counter = 0;
        if ((!delay) || (++counter == (DISPLAY_DELAY << 2))) { // Avoid to call 'Advertising::getStatus' continually

            LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Check to load/display advertising"), __PRETTY_FUNCTION__, __LINE__);
            switch (Advertising::getStatus()) {
                case Advertising::STATUS_FAILED: {

                    mAdvertising->load();
                    break;
                }
                case Advertising::STATUS_LOADED: {

                    mAdLoaded = true;
                    Advertising::display(0);
                    break;
                }
            }
            counter = 0;
        }
    }
}
#endif

bool GameLevel::update(const Game* game) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_GAMELEVEL, (*game->getLog() % 100), LOG_FORMAT(" - g:%x"), __PRETTY_FUNCTION__, __LINE__, game);
#endif
#ifdef __ANDROID__
    Player::getInstance()->resume();
#endif
#ifndef DEMO_VERSION
    // Advertising
    adDisplay(true);
#endif

    // Status
    static bool colTriangle = false;
    static bool colSquare = false;
    static bool colDisk = false;

    switch (mStep) {
        //case GAME_READY: break;
        case GAME_STARTED: {

            static unsigned char bonus = 0;
            static unsigned char flash = 0;
            static unsigned char inc = 0;

            short secElapsed = static_cast<short>(difftime(time(NULL), mTimer));
#ifdef __ANDROID__
            if (secElapsed > (mScoreData->mScore + 1)) {
                secElapsed = mScoreData->mScore;
                mTimer = time(NULL) - mScoreData->mScore;
            }
            // Needed after a lock/unlock operation
#endif
            if (secElapsed != mScoreData->mScore) {

                mScoreData->mScore = secElapsed;
                mScore->update(numToWStr<short>(secElapsed));
                if ((mScoreData->mScore == 10) || (mScoreData->mScore == 100) || (mScoreData->mScore == 1000) ||
                        (mScoreData->mScore == 10000)) {
                    mScore->setColor(1.f, 1.f, 0.f);
                    mScore->position(mScore->getLeft() - mScoreW, mScore->getTop());
                }
                if (++bonus == DISPLAY_STAR_EVERY) { // Display new star

                    const Dynamic2D* position = (std::rand() % 2)? static_cast<const Dynamic2D*>(mSquare):
                            static_cast<const Dynamic2D*>(mDisk);
                    mStar->reset();
                    mStar->translate(position->getTransform()[Dynamic2D::TRANS_X],
                            position->getTransform()[Dynamic2D::TRANS_Y]);
                    mStar->setAlpha(1.f);
                }
                if ((!(secElapsed % INCREASE_VEL_DELAY)) && (inc < (INCREASE_BUFFER_SIZE - 2))) { // Increase translation speed

                    inc += 2;
                    mTransVel = TRANS_VELOCITY + ((MAX_TRANS_VEL - TRANS_VELOCITY) * Dynamic2D::IncreaseBuffer[inc]);
                }
            }
            if (bonus > (DISPLAY_STAR_EVERY - 1)) { // Star management

                if ((bonus > (DISPLAY_STAR_EVERY + (DISPLAY_STAR_DELAY >> 1))) && (flash++ == 0x03)) {
                    mStar->setAlpha((mStar->getAlpha() > 0.5f)? 0.f:1.f);
                    flash = 0;
                }
                // Check star collision (BONUS)
                if (circ2DVia(mChip->getBounds(0))->overlap<Circle2D>(circ2DVia(mStar->getBounds(0)))) {

                    Player::getInstance()->play(3);
                    if (!mStarCount) {

                        mStarScore->setAlpha(1.f);
                        mStarText->setAlpha(1.f);
                        ++mStarCount;
                    }
                    else {

                        std::wstring starCount(L"x");
                        starCount.append(numToWStr<short>(++mStarCount));
                        mStarText->update(starCount);
                    }
                    mStar->setAlpha(0.f);
                    flash = 0;
                    bonus = 0;
                }
                else if (bonus > (DISPLAY_STAR_EVERY + DISPLAY_STAR_DELAY)) {

                    mStar->setAlpha(0.f);
                    flash = 0;
                    bonus = 0;
                }
            }

            // Check chip collision (GAME OVER)
            colTriangle = mTriangle->checkCollision(mChip, OBJECT_ID_CHIP);
            colSquare = poly2DVia(mSquare->getBounds(0))->overlap<Circle2D>(circ2DVia(mChip->getBounds(0)));
            colDisk = circ2DVia(mChip->getBounds(0))->overlap<Circle2D>(circ2DVia(mDisk->getBounds(0)));

            if ((!colTriangle) && (!colSquare) && (!colDisk))
                break;

            Player::getInstance()->play(2);
            mStep = GAME_OVER;
            bonus = 0;
            flash = 0;
            inc = 0;
            //break;
        }
        case GAME_OVER: {

            static unsigned char flash = 0;
            if (flash++ == 0x03) {

                flash = 0;
                static unsigned char score = 0;
                if (++score > 24) {
                    if (score == 40) {

                        score = 0;

                        colTriangle = false;
                        colSquare = false;
                        colDisk = false;

                        mDisk->reset();
                        mDisk->scale(1.f / UNIT_TRIANGLE_WIDTH, 1.f / UNIT_TRIANGLE_WIDTH);

                        mSquare->rotate(0.f);
                        mSquare->reset();
                        mSquare->scale(1.f / UNIT_TRIANGLE_WIDTH, 1.f / UNIT_TRIANGLE_WIDTH);

                        mTriangle->rotate(0.f);
                        mTriangle->reset();

                        static const float verCoord[8] = { -1.f, 0.f, -0.5f, (1.f / UNIT_TRIANGLE_WIDTH) * -TRIANGLE_HEIGHT,
                            0.f, 0.f };
                        mTriangle->setVertices(verCoord);

                        //
                        mTriangle->translate(0.f, mLooseY);
                        mDisk->translate(-0.5f, mLooseY - 0.5f);
                        mSquare->translate(0.5f, mLooseY - 0.5f);
                        mStar->setAlpha(0.f);

                        mScoreData->prepare();
                        mShare->reset();
                        mScoreData->mStarCount = mStarCount;
                        mStep = GAME_SCORE;
                    }
                    return true;
                }
                if (colTriangle)
                    mTriangle->setAlpha((mTriangle->getAlpha() > 0.5f)? 0.f:1.f);
                if (colSquare)
                    mSquare->setAlpha((mSquare->getAlpha() > 0.5f)? 0.f:1.f);
                if (colDisk)
                    mDisk->setAlpha((mDisk->getAlpha() > 0.5f)? 0.f:1.f);
            }
            return true;
        }
        case GAME_SCORE: {

            // Display score & best score
            if (mShare->update(game, mLooseY)) {

                mShare->setScore(mScoreData->mScore);
                mStep = GAME_SHARE;
                return true;
            }
            bool displaying = mScoreData->update(game, mLooseY);
            if (mScoreData->isBestScore()) {

                if (mBestText->getText()[0] != L'N') {

                    mBestText->update(L"New Best");
                    mBestText->position(1.f - ((FONT_WIDTH << 1) / static_cast<float>(game->getScreen()->width)) *
                            (game->getScreen()->width / FONT_SCALE_RATIO) * 8.f, mBestText->getTop());
                    // "New Best" contains 8 letters
                }
                mShare->display(); // Display share button
            }
            static short star = 0;
            if (star != (mStarCount - mScoreData->mStarCount)) {

                star = mStarCount - mScoreData->mStarCount;
                if (mStarCount != 1) {

                    std::wstring starCount(L"x");
                    starCount.append(numToWStr<short>(star));
                    mStarText->update(starCount);
                }
                if (star == 1) {

                    mStarScore->reset();
                    mStarScore->translate((MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) / 160) /
                            static_cast<float>(game->getScreen()->width), mLooseY - (((FONT_HEIGHT << 1) /
                            static_cast<float>(game->getScreen()->width)) * (game->getScreen()->width / FONT_SCALE_RATIO)) -
                            ((MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) / 160) /
                            static_cast<float>(game->getScreen()->width)));

                    mStarText->position(mStarScore->getTransform()[Dynamic2D::TRANS_X] - mStarScore->getLeft(),
                            mLooseY - (((FONT_HEIGHT << 1) / static_cast<float>(game->getScreen()->width)) *
                            (game->getScreen()->width / FONT_SCALE_RATIO)) - (mStarScore->getTop() -
                            mStarScore->getBottom()) + (((FONT_HEIGHT << 1) /
                            static_cast<float>(game->getScreen()->width)) * ((game->getScreen()->width /
                            FONT_SCALE_RATIO) * 0.8f)));
                }
            }
            if (displaying)
                return true; // Do not manage 'ok' click yet when displaying game score

            // Ok touched
            unsigned char touchCount = game->mTouchCount;
            while (touchCount--) {

                if (game->mTouchData[touchCount].Type != TouchInput::TOUCH_UP)
                    continue; // Avoid several touch

                float y = (game->getScreen()->height - (game->mTouchData[touchCount].Y << 1)) /
                        static_cast<float>(game->getScreen()->width);
                float x = ((game->mTouchData[touchCount].X << 1) / static_cast<float>(game->getScreen()->width)) - 1.f -
                        mOkBack->getTransform()[Dynamic2D::TRANS_X]; // Less TRANS_X to apply X translation
                // ...in unit

                if ((y > mOkBottom) && (y < ((mOkFactor * x) + mOkConst)) && (y < (mOkConst - (mOkFactor * x)))) {

                    mChip->reset();
                    mTouchID = NO_TOUCH_ID;
                    if (mScoreData->isBestScore()) {

                        mBestText->update(L"Best");
                        mBestText->position(1.f - ((FONT_WIDTH << 1) / static_cast<float>(game->getScreen()->width)) *
                                (game->getScreen()->width / FONT_SCALE_RATIO) * 4.f, mBestText->getTop());
                        // "Best" contains 4 letters
                    }
                    if (mScoreData->mScore) {

                        mScore->update(L"0");
                        mScore->position(-mScoreW, game->getScreen()->bottom + (((MAX_AD_HEIGHT *
                                static_cast<short>(game->getScreen()->yDpi) / 160) << 1) /
                                static_cast<float>(game->getScreen()->width)));

                        mScoreData->mScore = 0;
                    }
                    if (mStarCount) {

                        mStarScore->setAlpha(0.f);
                        mStarScore->reset();

                        short heightAd = MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) / 160;
                        mStarScore->translate((heightAd / static_cast<float>(game->getScreen()->width)) - 1.f,
                                game->getScreen()->bottom + (heightAd / static_cast<float>(game->getScreen()->width)));

                        mStarText->setAlpha(0.f);
                        if (mStarCount > 1)
                            mStarText->update(L"x1");
                        mStarText->position(((heightAd << 1) / static_cast<float>(game->getScreen()->width)) - 1.f,
                                game->getScreen()->bottom + (((FONT_HEIGHT << 1) / static_cast<float>(game->getScreen()->width)) *
                                ((game->getScreen()->width / FONT_SCALE_RATIO) * 0.8f)));

                        mStarCount = 0;
                    }
                    mScoreData->mDispTitle = true;
                    mScoreData->update(NULL, 0.f); // Update scores for title level

                    mStep = GAME_READY;
                    star = 0;
                    return false;
                }
            }
            return true;
        }
        case GAME_SHARE: {

            if (mShare->update(game, mLooseY))
                mStep = GAME_SCORE;

            return true;
        }
    }

    // Touch/Move...
    static short xOffset = 0, yOffset = 0, prevX = 0, prevY = 0;
    unsigned char touchCount = game->mTouchCount;
    while (touchCount--) {

        if ((mTouchID != NO_TOUCH_ID) && (mTouchID != game->mTouchData[touchCount].Id))
            continue; // Avoid several touch

        switch (game->mTouchData[touchCount].Type) {
            case TouchInput::TOUCH_DOWN: {
                if (mTouchID == game->mTouchData[touchCount].Id)
                    break; // BUG: Avoid unexpected several touch down event ?!?!

                if ((game->mTouchData[touchCount].X > mScreenArea.left) && (game->mTouchData[touchCount].X < mScreenArea.right) &&
                        (game->mTouchData[touchCount].Y > mScreenArea.top) && (game->mTouchData[touchCount].Y < mScreenArea.bottom)) {

                    if (mStep == GAME_READY) {

                        Player::getInstance()->play(1);

                        xOffset = 0;
                        yOffset = 0;

                        short x = game->mTouchData[touchCount].X;
                        short y = game->mTouchData[touchCount].Y;

                        short chipSize = static_cast<short>((CHIP_RADIUS + CHIP_LAG) * game->getScreen()->width) >> 1; // Half
                        if (x < chipSize)
                            x = chipSize;
                        else if (x > (game->getScreen()->width - chipSize))
                            x = game->getScreen()->width - chipSize;

                        short heightAd = MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) / 160;
                        if (y < (heightAd + chipSize))
                            y = heightAd + chipSize;
                        else if (y > (game->getScreen()->height - heightAd - chipSize))
                            y = game->getScreen()->height - heightAd - chipSize;

                        mChip->translate(((x << 1) / static_cast<float>(game->getScreen()->width)) - 1.f,
                                (game->getScreen()->height - (y << 1)) / static_cast<float>(game->getScreen()->width));
                        //
                        mStep = GAME_STARTED;
                        mScoreData->mDispTitle = false;
                        mTransVel = TRANS_VELOCITY;

                        mTimer = time(NULL);
                    }
                    else {

                        xOffset = game->mTouchData[touchCount].X - prevX;
                        yOffset = game->mTouchData[touchCount].Y - prevY;
                    }
                    mTouchID = game->mTouchData[touchCount].Id;

                    prevX = game->mTouchData[touchCount].X - xOffset;
                    prevY = game->mTouchData[touchCount].Y - yOffset;
                }
                break;
            }
            case TouchInput::TOUCH_MOVE: {

                if ((game->mTouchData[touchCount].X > mScreenArea.left) && (game->mTouchData[touchCount].X < mScreenArea.right) &&
                        (game->mTouchData[touchCount].Y > mScreenArea.top) && (game->mTouchData[touchCount].Y < mScreenArea.bottom)) {

                    if (mTouchID == NO_TOUCH_ID) {

                        xOffset = game->mTouchData[touchCount].X - prevX;
                        yOffset = game->mTouchData[touchCount].Y - prevY;

                        mTouchID = game->mTouchData[touchCount].Id;
                    }
                    short chipSize = static_cast<short>((CHIP_RADIUS + CHIP_LAG) * game->getScreen()->width) >> 1; // Half
                    if ((game->mTouchData[touchCount].X - xOffset) < chipSize)
                        xOffset -= chipSize - (game->mTouchData[touchCount].X - xOffset);
                    else if ((game->mTouchData[touchCount].X - xOffset) > (game->getScreen()->width - chipSize))
                        xOffset += (game->mTouchData[touchCount].X - xOffset) - (game->getScreen()->width - chipSize);

                    short heightAd = MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) / 160;
                    if ((game->mTouchData[touchCount].Y - yOffset) < (heightAd + chipSize))
                        yOffset -= (heightAd + chipSize) - (game->mTouchData[touchCount].Y - yOffset);
                    else if ((game->mTouchData[touchCount].Y - yOffset) > (game->getScreen()->height - heightAd - chipSize))
                        yOffset += (game->mTouchData[touchCount].Y - yOffset) - (game->getScreen()->height - heightAd - chipSize);

                    mChip->reset();
                    mChip->translate((((game->mTouchData[touchCount].X - xOffset) << 1) / static_cast<float>(game->getScreen()->width)) - 1.f,
                            (game->getScreen()->height - ((game->mTouchData[touchCount].Y - yOffset) << 1)) /
                            static_cast<float>(game->getScreen()->width));

                    prevX = game->mTouchData[touchCount].X - xOffset;
                    prevY = game->mTouchData[touchCount].Y - yOffset;
                }
                else
                    mTouchID = NO_TOUCH_ID;
                break;
            }
            default: { // TOUCH_UP & TOUCH_CANCEL

                mTouchID = NO_TOUCH_ID;
                break;
            }
        }
    }

    // Check collision(s)
    if (mTriangle->checkCollision(&mScreenLeft, BOUND_ID_SCREEN_LEFT)) {
        mTriangle->getVelocities()[TRANS_VEL_X] = mTransVel;
        if ((mTriangleRotate) && (mTriangle->getVelocities()[TRANS_VEL_Y] > 0.f))
            mTriangleRotate = false;
        else if ((!mTriangleRotate) && (mTriangle->getVelocities()[TRANS_VEL_Y] < 0.f))
            mTriangleRotate = true;
        Player::getInstance()->play(0);
    }
    else if (mTriangle->checkCollision(&mScreenTop, BOUND_ID_SCREEN_TOP)) {
        mTriangle->getVelocities()[TRANS_VEL_Y] = -mTransVel;
        if ((mTriangleRotate) && (mTriangle->getVelocities()[TRANS_VEL_X] > 0.f))
            mTriangleRotate = false;
        else if ((!mTriangleRotate) && (mTriangle->getVelocities()[TRANS_VEL_X] < 0.f))
            mTriangleRotate = true;
        Player::getInstance()->play(0);
    }
    else if (mTriangle->checkCollision(&mScreenRight, BOUND_ID_SCREEN_RIGHT)) {
        mTriangle->getVelocities()[TRANS_VEL_X] = -mTransVel;
        if ((mTriangleRotate) && (mTriangle->getVelocities()[TRANS_VEL_Y] < 0.f))
            mTriangleRotate = false;
        else if ((!mTriangleRotate) && (mTriangle->getVelocities()[TRANS_VEL_Y] > 0.f))
            mTriangleRotate = true;
        Player::getInstance()->play(0);
    }
    else if (mTriangle->checkCollision(&mScreenBottom, BOUND_ID_SCREEN_BOTTOM)) {
        mTriangle->getVelocities()[TRANS_VEL_Y] = mTransVel;
        if ((mTriangleRotate) && (mTriangle->getVelocities()[TRANS_VEL_X] < 0.f))
            mTriangleRotate = false;
        else if ((!mTriangleRotate) && (mTriangle->getVelocities()[TRANS_VEL_X] > 0.f))
            mTriangleRotate = true;
        Player::getInstance()->play(0);
    }

    if (circ2DVia(mDisk->getBounds(0))->overlap<BorderLeft>(&mScreenLeft)) {
        mDisk->getVelocities()[TRANS_VEL_X] = mTransVel;
        Player::getInstance()->play(0);
    }
    else if (circ2DVia(mDisk->getBounds(0))->overlap<BorderTop>(&mScreenTop)) {
        mDisk->getVelocities()[TRANS_VEL_Y] = -mTransVel;
        Player::getInstance()->play(0);
    }
    else if (circ2DVia(mDisk->getBounds(0))->overlap<BorderRight>(&mScreenRight)) {
        mDisk->getVelocities()[TRANS_VEL_X] = -mTransVel;
        Player::getInstance()->play(0);
    }
    else if (circ2DVia(mDisk->getBounds(0))->overlap<BorderBottom>(&mScreenBottom)) {
        mDisk->getVelocities()[TRANS_VEL_Y] = mTransVel;
        Player::getInstance()->play(0);
    }

    if (poly2DVia(mSquare->getBounds(0))->overlap<BorderLeft>(&mScreenLeft)) {
        mSquare->getVelocities()[TRANS_VEL_X] = mTransVel;
        if ((mSquareRotate) && (mSquare->getVelocities()[TRANS_VEL_Y] > 0.f))
            mSquareRotate = false;
        else if ((!mSquareRotate) && (mSquare->getVelocities()[TRANS_VEL_Y] < 0.f))
            mSquareRotate = true;
        Player::getInstance()->play(0);
    }
    else if (poly2DVia(mSquare->getBounds(0))->overlap<BorderTop>(&mScreenTop)) {
        mSquare->getVelocities()[TRANS_VEL_Y] = -mTransVel;
        if ((mSquareRotate) && (mSquare->getVelocities()[TRANS_VEL_X] > 0.f))
            mSquareRotate = false;
        else if ((!mSquareRotate) && (mSquare->getVelocities()[TRANS_VEL_X] < 0.f))
            mSquareRotate = true;
        Player::getInstance()->play(0);
    }
    else if (poly2DVia(mSquare->getBounds(0))->overlap<BorderRight>(&mScreenRight)) {
        mSquare->getVelocities()[TRANS_VEL_X] = -mTransVel;
        if ((mSquareRotate) && (mSquare->getVelocities()[TRANS_VEL_Y] < 0.f))
            mSquareRotate = false;
        else if ((!mSquareRotate) && (mSquare->getVelocities()[TRANS_VEL_Y] > 0.f))
            mSquareRotate = true;
        Player::getInstance()->play(0);
    }
    else if (poly2DVia(mSquare->getBounds(0))->overlap<BorderBottom>(&mScreenBottom)) {
        mSquare->getVelocities()[TRANS_VEL_Y] = mTransVel;
        if ((mSquareRotate) && (mSquare->getVelocities()[TRANS_VEL_X] < 0.f))
            mSquareRotate = false;
        else if ((!mSquareRotate) && (mSquare->getVelocities()[TRANS_VEL_X] > 0.f))
            mSquareRotate = true;
        Player::getInstance()->play(0);
    }

    // Move
    mTriangle->rotate(mTriangle->getTransform()[Dynamic2D::ROTATE_C] + ((mTriangleRotate)? -ROTATE_VELOCITY:ROTATE_VELOCITY));
    mSquare->rotate(mSquare->getTransform()[Dynamic2D::ROTATE_C] + ((mSquareRotate)? -ROTATE_VELOCITY:ROTATE_VELOCITY));

    mTriangle->translate(mTriangle->getVelocities()[TRANS_VEL_X], mTriangle->getVelocities()[TRANS_VEL_Y]);
    mDisk->translate(mDisk->getVelocities()[TRANS_VEL_X], mDisk->getVelocities()[TRANS_VEL_Y]);
    mSquare->translate(mSquare->getVelocities()[TRANS_VEL_X], mSquare->getVelocities()[TRANS_VEL_Y]);

    return true;
}

void GameLevel::render() const {

    switch (mStep) {

        case GAME_READY: {

            mTriangle->render();
            mDisk->render(true);

            glDisable(GL_BLEND);
            mSquare->render(true);
            glEnable(GL_BLEND);

            mScore->render(true);
            break;
        }
        case GAME_STARTED:
        case GAME_OVER: {

            mStar->render(true);
            mTriangle->render();
            mDisk->render(true);

            if (mStep == GAME_STARTED)
                glDisable(GL_BLEND);
            mSquare->render(true);
            if (mStep == GAME_STARTED)
                glEnable(GL_BLEND);

            mChip->render(true);

            mStarScore->render(true);
            mStarText->render(true);
            mScore->render(true);
            break;
        }
        case GAME_SCORE: {

            mGameOver->render(false);

            mTriangle->render();
            mDisk->render(true);

            glDisable(GL_BLEND);
            mSquare->render(true);
            glEnable(GL_BLEND);

            mMedalText->render(true);
            mScoreText->render(true);
            mOkBack->render();
            mOkText->render(true);

            if (mStarCount != mScoreData->mStarCount) {

                mStarScore->render(true);
                mStarText->render(true);
            }
            mScoreData->render();
            mBestText->render(true);
            mShare->render();
            break;
        }
        case GAME_SHARE: {

            mGameOver->render(false);
            mShare->render();
            break;
        }
    }
}
