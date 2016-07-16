#include "TitleLevel.h"
#include "Global.h"

#include <libeng/Game/2D/Game2D.h>
#ifdef __ANDROID__
#include <boost/math/tr1.hpp>
#else
#include <math.h>
#include "OpenGLES/ES2/gl.h"
#endif

#include <boost/thread.hpp>

#define TRIANGLE_SCALE          0.75f

// Texture IDs
#define TEXTURE_ID_TITLE        19

//////
TitleLevel::TitleLevel() : mLoadStep(0), mDiskY(0), mOkFactor(0.f), mOkConst(0.f), mOkBottom(0.f) {

    LOGV(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    std::memset(&mScoresArea, 0, sizeof(TouchArea));

    mScoresData = Scores::getInstance(NULL);
#ifndef DEMO_VERSION
    mAdLoaded = false;
    mAdvertising = Advertising::getInstance();
#endif

    mTitle = NULL;
    mChip = NULL;
    mTriangle = NULL;
    mDisk = NULL;
    mSquare = NULL;
    mHidden = NULL;
    mStart = NULL;
    mScores = NULL;
    mOk = NULL;
}
TitleLevel::~TitleLevel() {

    LOGV(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    if (mTitle) delete mTitle;
    if (mChip) delete mChip;
    if (mTriangle) delete mTriangle;
    if (mDisk) delete mDisk;
    if (mSquare) delete mSquare;
    if (mHidden) delete mHidden;
    if (mStart) delete mStart;
    if (mScores) delete mScores;
    if (mOk) delete mOk;
}

bool TitleLevel::load(const Game* game) {

    LOGV(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(" - l:%d"), __PRETTY_FUNCTION__, __LINE__, (mLoadStep + 1));
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
            break;
        }
        case 2: {

            if (!mTitle) {

                mTitle = new Static2D;
                mTitle->initialize(game2DVia(game));
                mTitle->start(TEXTURE_ID_TITLE);
                mTitle->setTexCoords(FULL_TEXCOORD_BUFFER);

                //mTitle->setRed(0xff / MAX_COLOR);
                //mTitle->setGreen(0xff / MAX_COLOR);
                mTitle->setBlue(0x00 / MAX_COLOR);

                short yPos = ((game->getScreen()->height - (MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) /
                            160) - (game->getScreen()->height >> 1)) >> 1) + (game->getScreen()->height >> 1);
                mTitle->setVertices(0, yPos + (game->getScreen()->width >> 2), game->getScreen()->width, yPos -
                        (game->getScreen()->width >> 2));
            }
            else
                mTitle->resume(TEXTURE_ID_TITLE);

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 3: {

            if (!mChip) {

                mChip = new Static2D;
                mChip->initialize(game2DVia(game));
                mChip->start(TEXTURE_ID_DISK);
                mChip->setTexCoords(FULL_TEXCOORD_BUFFER);

                //mChip->setRed(0xff / MAX_COLOR);
                mChip->setGreen(0x00 / MAX_COLOR);
                mChip->setBlue(0x00 / MAX_COLOR);

                short yPos = ((game->getScreen()->height - (MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) /
                            160) - (game->getScreen()->height >> 1)) >> 1) + (game->getScreen()->height >> 1);
                short size = (((game->getScreen()->width / 3) << 3) / 10) >> 1; // 80% of Width / 3 (half)
                mChip->setVertices((game->getScreen()->width >> 1) + (game->getScreen()->width >> 2) - size, yPos,
                        (game->getScreen()->width >> 1) + (game->getScreen()->width >> 2) + size, yPos - (size << 1));
            }
            else
                mChip->resume(TEXTURE_ID_DISK);

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 4:
        case 5:
        case 6:
        case 7:
        case 8: {

            mScoresData->load(game, mLoadStep - 4);
#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 9: {

            if (!mDisk) {

                mDisk = new Element2D;
                mDisk->initialize(game2DVia(game));
                mDisk->start(TEXTURE_ID_DISK);
                mDisk->setTexCoords(FULL_TEXCOORD_BUFFER);

                mDisk->setRed(0x00 / MAX_COLOR);
                mDisk->setGreen(0x00 / MAX_COLOR);
                mDisk->setBlue(0x00 / MAX_COLOR);

                short heightAd = MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) / 160;
                mDiskY = ((((((game->getScreen()->height - heightAd - (game->getScreen()->height >> 1)) >> 1) +
                        (game->getScreen()->height >> 1)) - (game->getScreen()->width >> 2)) - heightAd) >> 1) +
                        heightAd;

                short size = game->getScreen()->width / 6; // Half

                mDisk->setVertices((game->getScreen()->width >> 1) - size, (game->getScreen()->height >> 1) + size,
                        (game->getScreen()->width >> 1) + size, (game->getScreen()->height >> 1) - size);
                mDisk->translate(-0.5f, game->getScreen()->bottom + (mDiskY << 1) /
                        static_cast<float>(game->getScreen()->width));
            }
            else
                mDisk->resume(TEXTURE_ID_DISK);

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 10: {

            if (!mTriangle) {

                mTriangle = new Triangle;
                mTriangle->initialize(game2DVia(game));
                mTriangle->start(game);

                float yPos = mScoresData->getBottom() - ((mScoresData->getBottom() - game->getScreen()->bottom) / 2.f);
                mTriangle->scale(TRIANGLE_SCALE, TRIANGLE_SCALE);
                mTriangle->translate(1.f - (UNIT_TRIANGLE_WIDTH / 2.f), yPos);

                mOkConst = g_TriangleVertices[3] + yPos;
                mOkFactor = (g_TriangleVertices[1] - g_TriangleVertices[3]) / g_TriangleVertices[0];
                mOkBottom = g_TriangleVertices[1] + yPos;
            }
            else
                mTriangle->resume();

            mTriangle->setRed(0xff / MAX_COLOR);
            mTriangle->setGreen(0xff / MAX_COLOR);
            mTriangle->setBlue(0x00 / MAX_COLOR);

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 11: {

            if (!mSquare) {

                mSquare = new Element2D;
                mSquare->initialize(game2DVia(game));
                mSquare->start(0x00, 0x00, 0x00);
                mSquare->setTexCoords(FULL_TEXCOORD_BUFFER);

                short size = game->getScreen()->width / 6; // Half

                mScoresArea.left = (game->getScreen()->width >> 1) + (game->getScreen()->width >> 2) - size;
                mScoresArea.top = game->getScreen()->height - mDiskY - size;
                mScoresArea.right = mScoresArea.left + (size << 1);
                mScoresArea.bottom = mScoresArea.top + (size << 1);

                mSquare->setVertices((game->getScreen()->width >> 1) - size, (game->getScreen()->height >> 1) - size,
                        (game->getScreen()->width >> 1) + size, (game->getScreen()->height >> 1) + size);
                mSquare->translate(0.5f, game->getScreen()->bottom + ((mDiskY << 1) /
                        static_cast<float>(game->getScreen()->width)));
            }
            else
                mSquare->resume(0x00, 0x00, 0x00);

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 12: {

            if (!mStart) {

                mStart = new Text2D;
                mStart->initialize(game2DVia(game));
                mStart->start(L"Start");

                float scale = game->getScreen()->width / FONT_SCALE_RATIO;
                mStart->scale(scale, scale);
                mStart->position(mDisk->getLeft() - 0.5f, (((mDisk->getBottom() - mDisk->getTop()) / 2.f) +
                        mDisk->getTop()) + mDisk->getTransform()[Dynamic2D::TRANS_Y] + (((mStart->getTop() -
                        mStart->getBottom()) / 2.f) * scale));
            }
            else
                mStart->resume();

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 13: {

            if (!mScores) {

                mScores = new Text2D;
                mScores->initialize(game2DVia(game));
                mScores->start(L"Scores");

                float scale = game->getScreen()->width / FONT_SCALE_RATIO;
                mScores->scale(scale, scale);
                mScores->position(mSquare->getLeft() + 0.5f, mSquare->getTransform()[Dynamic2D::TRANS_Y] +
                        mSquare->getBottom() + (((FONT_HEIGHT << 1) / static_cast<float>(game->getScreen()->width)) *
                        scale) + (mSquare->getTop() - mSquare->getBottom()));
            }
            else
                mScores->resume();

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case 14: {

            if (!mHidden) {

                mHidden = new Static2D;
                mHidden->initialize(game2DVia(game));
                mHidden->start(0x00, 0x00, 0x00);
                mHidden->setTexCoords(FULL_TEXCOORD_BUFFER);

                mHidden->setVertices((game->getScreen()->width >> 1) - 1, game->getScreen()->height -
                        (MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) / 160) -
                        (game->getScreen()->width >> 1), game->getScreen()->width + 1, 0);
            }
            else
                mHidden->resume(0x00, 0x00, 0x00);

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
            break;
        }
        case MAX_TITLE_LOAD: {

            if (!mOk) {

                mOk = new Text2D;
                mOk->initialize(game2DVia(game));
                mOk->start(L"Ok");
                mOk->setColor(0.f, 0.f, 0.f);

                float scale = game->getScreen()->width / FONT_SCALE_RATIO;
                mOk->scale(scale, scale);
                mOk->position(1.f - (UNIT_TRIANGLE_WIDTH / 2.f), mScoresData->getBottom() - ((mScoresData->getBottom() -
                        game->getScreen()->bottom) / 2.f));
            }
            else
                mOk->resume();

#ifndef DEMO_VERSION
            adDisplay(false);
#endif
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
    return (mLoadStep == MAX_TITLE_LOAD);
}

#ifndef DEMO_VERSION
void TitleLevel::adDisplay(bool delay) {

    if (!mAdLoaded) {

        static unsigned char counter = 0;
        if ((!delay) || (++counter == (DISPLAY_DELAY << 2))) { // Avoid to call 'Advertising::getStatus' continually

            LOGI(LOG_LEVEL_TITLELEVEL, 0, LOG_FORMAT(" - Check to load/display advertising"), __PRETTY_FUNCTION__, __LINE__);
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

bool TitleLevel::update(const Game* game) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_TITLELEVEL, (*game->getLog() % 100), LOG_FORMAT(" - g:%x"), __PRETTY_FUNCTION__, __LINE__, game);
#endif
#ifndef DEMO_VERSION
    // Advertising
    adDisplay(true);
#endif

    // Touch...
    unsigned char touchCount = game->mTouchCount;
    while (touchCount--) {

        if (game->mTouchData[touchCount].Type != TouchInput::TOUCH_UP)
            continue;

        // Ok (scores)
        if (mTitle->getAlpha() < 0.5f) { // == 0.f

            float y = (game->getScreen()->height - (game->mTouchData[touchCount].Y << 1)) /
                    static_cast<float>(game->getScreen()->width);
            float x = ((game->mTouchData[touchCount].X << 1) / static_cast<float>(game->getScreen()->width)) - 1.f -
                    mTriangle->getTransform()[Dynamic2D::TRANS_X]; // Less TRANS_X to apply X translation
            // ...in unit

            if ((y > mOkBottom) && (y < ((mOkFactor * x) + mOkConst)) && (y < (mOkConst - (mOkFactor * x)))) {

                mTitle->setAlpha(1.f);

                mSquare->scale(1.f, 1.f);
                mSquare->reset();
                mSquare->translate(0.5f, game->getScreen()->bottom + ((mDiskY << 1) /
                        static_cast<float>(game->getScreen()->width)));

                mDisk->scale(1.f, 1.f);
                mDisk->reset();
                mDisk->translate(-0.5f, game->getScreen()->bottom + (mDiskY << 1) / static_cast<float>(game->getScreen()->width));
            }
            continue;
        }

        // Scores
        if ((game->mTouchData[touchCount].X > mScoresArea.left) && (game->mTouchData[touchCount].X < mScoresArea.right) &&
                (game->mTouchData[touchCount].Y > mScoresArea.top) && (game->mTouchData[touchCount].Y < mScoresArea.bottom)) {

            mTitle->setAlpha(0.f);

            mDisk->scale(3.1f, 3.1f); // 3.1f instaed of 3.f coz precision issue
            mDisk->reset();
            mDisk->translate(0.f, game->getScreen()->top - 1.f - (((MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) /
                    160) << 1) / static_cast<float>(game->getScreen()->width)));

            float scale = (3 * game->getScreen()->height) / static_cast<float>(game->getScreen()->width);
            mSquare->scale(scale, scale);
            mSquare->reset();
            mSquare->translate(game->getScreen()->bottom, 0.f);
            return true;
        }

        // Start
#ifdef __ANDROID__
        if (boost::math::tr1::hypotf((game->getScreen()->width >> 2) - game->mTouchData[touchCount].X,
                (game->getScreen()->height - mDiskY) - game->mTouchData[touchCount].Y) < (game->getScreen()->width / 6.f))
#else
        if (hypotf((game->getScreen()->width >> 2) - game->mTouchData[touchCount].X,
                (game->getScreen()->height - mDiskY) - game->mTouchData[touchCount].Y) < (game->getScreen()->width / 6.f))
#endif
            return false;
    }
    return true;
}

void TitleLevel::render() const {

    mDisk->render(true);

    glDisable(GL_BLEND);
    mSquare->render(true);
    glEnable(GL_BLEND);

    if (mTitle->getAlpha() < 0.5f) { // Scores

        glDisable(GL_BLEND);
        mHidden->render(true);
        glEnable(GL_BLEND);

        mChip->render(true);

        mScoresData->render();
        mTriangle->render();
        mOk->render(true);
    }
    else { // Title

        mChip->render(true);
        mTitle->render(true);

        mStart->render(true);
        mScores->render(true);
    }
}
