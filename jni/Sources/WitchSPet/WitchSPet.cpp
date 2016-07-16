#include "WitchSPet.h"
#include <libeng/Game/2D/Game2D.h>

#ifndef __ANDROID__
#include "OpenGLES/ES2/gl.h"
#endif

#define MOON_GREEN              (247.f / 255.f)
#define MOON_BLUE               (84.f / 255.f)

#define MAX_LOAD_STEP           18
#define DISP_UPD_COUNT          16 // Update count B4 having fully displayed the title/witch
#define PROGRESS_TRANS          (2.f / MAX_LOAD_STEP)
#define FLOOR_HEIGHT            85
#define WIDTH_RATIO             7.f
#define ALPHA_VELOCITY          0.005f

#define WITCH_TRANS             -1.5f
#define WITCH_VELOCITY          (-WITCH_TRANS / DISP_UPD_COUNT)
#define WITCH_SCALE_RATIO       (0.5f * REF_SCREEN_RATIO) // Witch scale according that whitch size is 1
#define WITCH_TEXTURE_SIZE      256.f

#define AVAILABLE_TRANS         -2.f
#define AVAILABLE_VELOCITY      (-AVAILABLE_TRANS / DISP_UPD_COUNT)

// Delay(s)
#ifdef __ANDROID__ // Android
#ifdef DEBUG
#define DDELAY_DISPLAY          150
#endif
#define DELAY_DISPLAY           1323064.f

#else // iOS
#ifdef DEBUG
#define DDELAY_DISPLAY          100
#endif
#define DELAY_DISPLAY           297137.f

#endif

// Texture IDs
#define TEXTURE_ID_AVAILABLE    2
#define TEXTURE_ID_CLOSE        3
#define TEXTURE_ID_PUMPKIN      4
#define TEXTURE_ID_BACK         5
#define TEXTURE_ID_MOON         6
#define TEXTURE_ID_SCENE        7
#define TEXTURE_ID_TITLE        8
#define TEXTURE_ID_STORES       9
#define TEXTURE_ID_FACE         10

#define BACK_TEXTURE_SIZE       512.f
#define SCENE_TEXTURE_SIZE      512.f

#define STORES_TEXTURE_HEIGHT   128
#define STORES_TEXTURE_WIDTH    512

// Face
#define FACE_X0                 173
#define FACE_Y0                 109
#define FACE_X2                 205
#define FACE_Y2                 142

#define TITLE_TEXTURE_WIDTH     512.f
#define TITLE_HEIGHT            256.f
#define TITLE_WIDTH             473 // Title width in the texture

// Sound IDs
#define SOUND_ID_FLAP           1

//////
WitchSPet::WitchSPet(Game* game) : Level(game), mStep(0), mWitchY(0.f), mTitleY(0.f), mSceneY(0.f) {

    LOGV(LOG_LEVEL_WITCHSPET, 0, LOG_FORMAT(" - g:%x"), __PRETTY_FUNCTION__, __LINE__, game);
    std::memset(&mCloseArea, 0, sizeof(TouchArea));

    game->mAccelPrecision = 0.f;
    Inputs::InputUse = USE_INPUT_TOUCH;

    mPumpkin = NULL;
    mProgress = NULL;
    mClose = NULL;
    mBack = NULL;
    mMoon = NULL;
    mScene = NULL;
    mWitch = NULL;
    mFace = NULL;
    mTitle = NULL;
    mAvailable = NULL;
    mStores = NULL;
}
WitchSPet::~WitchSPet() {

    LOGV(LOG_LEVEL_WITCHSPET, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);

    if (mPumpkin) delete mPumpkin;
    if (mProgress) delete mProgress;
    if (mClose) delete mClose;
    if (mBack) delete mBack;
    if (mMoon) delete mMoon;
    if (mScene) delete mScene;
    if (mWitch) delete mWitch;
    if (mFace) delete mFace;
    if (mTitle) delete mTitle;
    if (mAvailable) delete mAvailable;
    if (mStores) delete mStores;
}

void WitchSPet::pause() {

    LOGV(LOG_LEVEL_WITCHSPET, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    Level::pause();

    // Stop all sounds when pause/resume operation (if needed)
    Player* player = Player::getInstance();
    if ((player->getIndex(SOUND_ID_FLAP) != SOUND_IDX_INVALID) && (player->getResumeFlag(0))) player->stop(0);

    if (mPumpkin) mPumpkin->pause();
    if (mProgress) mProgress->pause();
    if (mClose) mClose->pause();
    if (mBack) mBack->pause();
    if (mMoon) mMoon->pause();
    if (mScene) mScene->pause();
    if (mWitch) mWitch->pause();
    if (mFace) mFace->pause();
    if (mTitle) mTitle->pause();
    if (mAvailable) mAvailable->pause();
    if (mStores) mStores->pause();
}

bool WitchSPet::loaded(const Game* game) {

    LOGV(LOG_LEVEL_WITCHSPET, 0, LOG_FORMAT(" - l:%d"), __PRETTY_FUNCTION__, __LINE__, mLoadStep);
    switch (mLoadStep) {
        case 1: {

            if (!mPumpkin) {

                mPumpkin = new Static2D;
                mPumpkin->initialize(game2DVia(game));
                mPumpkin->start(TEXTURE_ID_PUMPKIN);
                mPumpkin->setTexCoords(FULL_TEXCOORD_BUFFER);

                static const float verCoords[8] = {-0.7f, 0.7f, -0.7f, -0.7f, 0.7f, -0.7f, 0.7f, 0.7f};
                mPumpkin->setVertices(verCoords);
            }
            else
                mPumpkin->resume(TEXTURE_ID_PUMPKIN);

            if (!mProgress) {

                mProgress = new Element2D;
                mProgress->initialize(game2DVia(game));
                mProgress->start(0, 0, 0);

                mProgress->setTexCoords(FULL_TEXCOORD_BUFFER);
                mProgress->copyVertices(*mPumpkin);
            }
            else
                mProgress->resume(0, 0, 0);

            mProgress->reset();
            break;
        }
        case 2: {

            if (!mClose) {

                mClose = new Static2D;
                mClose->initialize(game2DVia(game));
                mClose->start(TEXTURE_ID_CLOSE);
                mClose->setTexCoords(FULL_TEXCOORD_BUFFER);
                mClose->setVertices(game->getScreen()->width - (game->getScreen()->width >> 3), game->getScreen()->height,
                        game->getScreen()->width, game->getScreen()->height - (game->getScreen()->width >> 3));

                mCloseArea.left = game->getScreen()->width - (game->getScreen()->width >> 3);
                mCloseArea.right = game->getScreen()->width;
                mCloseArea.top = 0;
                mCloseArea.bottom = game->getScreen()->width >> 3;

                mClose->setAlpha(0.f);
            }
            else
                mClose->resume(TEXTURE_ID_CLOSE);
            break;
        }
        case 3: {

            if (!mTitle) {

                mTitle = new Element2D;
                mTitle->initialize(game2DVia(game));
                mTitle->start(TEXTURE_ID_TITLE);

                static float texCoords[8] = {0.f, 0.f, 0.f, 1.f, TITLE_WIDTH / TITLE_TEXTURE_WIDTH, 1.f,
                        TITLE_WIDTH / TITLE_TEXTURE_WIDTH, 0.f};
                mTitle->setTexCoords(texCoords);

                short halfHeight = static_cast<short>(game->getScreen()->height / 5.f) >> 1; // 80% of height / 2
                short halfWidth = static_cast<short>((TITLE_WIDTH / TITLE_HEIGHT) * halfHeight);
                if (halfWidth > (game->getScreen()->width >> 1)) {

                    halfWidth = static_cast<short>(0.8f * game->getScreen()->width) >> 1; // 80% of width
                    halfHeight = static_cast<short>((TITLE_HEIGHT / TITLE_WIDTH) * halfWidth);
                }
                mTitle->setVertices((game->getScreen()->width >> 1) - halfWidth, game->getScreen()->height + (halfHeight << 1),
                        (game->getScreen()->width >> 1) + halfWidth, game->getScreen()->height);

                mTitleY = (mTitle->getBottom() - mTitle->getTop()) - (0.05f * game->getScreen()->top);
            }
            else
                mTitle->resume(TEXTURE_ID_TITLE);
            break;
        }
        case 4: {

            if (!mMoon) {

                mMoon = new Static2D;
                mMoon->initialize(game2DVia(game));
                mMoon->start(TEXTURE_ID_MOON);
                mMoon->setTexCoords(FULL_TEXCOORD_BUFFER);
                mMoon->setVertices((game->getScreen()->width - (game->getScreen()->height >> 1)) >> 1, game->getScreen()->height,
                        game->getScreen()->width - ((game->getScreen()->width - (game->getScreen()->height >> 1)) >> 1),
                        game->getScreen()->height >> 1);

                //mMoon->setRed(1.f);
                mMoon->setGreen(MOON_GREEN);
                mMoon->setBlue(MOON_BLUE);
            }
            else
                mMoon->resume(TEXTURE_ID_MOON);
            break;
        }
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12: {

            if (!mWitch) {

                mWitch = new Witch;
                mWitch->initialize(game2DVia(game));
                mWitch->setTexCoords(FULL_TEXCOORD_BUFFER);

                float bottom = (game->getScreen()->bottom + (game->getScreen()->top / 2.f)) + (2.f / WIDTH_RATIO);
                mWitchY = bottom + (((game->getScreen()->top + mTitleY) - bottom) / 2.f);
                mWitch->translate(WITCH_TRANS, mWitchY);
            }
            mWitch->load(mLoadStep - 5);
            break;
        }
        case 13: {

            if (!mScene) {

                mScene = new Element2D;
                mScene->initialize(game2DVia(game));
                mScene->start(TEXTURE_ID_SCENE);
                mScene->setTexCoords(FULL_TEXCOORD_BUFFER);
                mScene->setVertices(0, game->getScreen()->width, game->getScreen()->width, 0);

                mSceneY = (FLOOR_HEIGHT << 1) / SCENE_TEXTURE_SIZE;
                mScene->translate(0.f, -mSceneY);
            }
            else
                mScene->resume(TEXTURE_ID_SCENE);
            break;
        }
        case 14: {

            if (!mFace) {

                mFace = new Static2D;
                mFace->initialize(game2DVia(game));
                mFace->start(TEXTURE_ID_FACE);
                mFace->setTexCoords(FULL_TEXCOORD_BUFFER);

                static const float verCoords[8] = { (FACE_X0 / WITCH_TEXTURE_SIZE) - 0.5f,
                        ((WITCH_TEXTURE_SIZE - FACE_Y0) / WITCH_TEXTURE_SIZE) - 0.5f + mWitchY,
                        (FACE_X0 / WITCH_TEXTURE_SIZE) - 0.5f, ((WITCH_TEXTURE_SIZE - FACE_Y2) / WITCH_TEXTURE_SIZE) - 0.5f + mWitchY,
                        (FACE_X2 / WITCH_TEXTURE_SIZE) - 0.5f, ((WITCH_TEXTURE_SIZE - FACE_Y2) / WITCH_TEXTURE_SIZE) - 0.5f + mWitchY,
                        (FACE_X2 / WITCH_TEXTURE_SIZE) - 0.5f, ((WITCH_TEXTURE_SIZE - FACE_Y0) / WITCH_TEXTURE_SIZE) - 0.5f + mWitchY };
                mFace->setVertices(verCoords);
                mFace->setAlpha(0.f);
            }
            else
                mFace->resume(TEXTURE_ID_FACE);
            break;
        }
        case 15: {

            if (!mBack) {

                mBack = new Static2D;
                mBack->initialize(game2DVia(game));
                mBack->start(TEXTURE_ID_BACK);
                mBack->setTexCoords(FULL_TEXCOORD_BUFFER);

                short halfWidth = (BACK_TEXTURE_SIZE / 2.f) * (1.f -
                        (game->getScreen()->width / static_cast<float>(game->getScreen()->height)));
                mBack->setVertices(-halfWidth, game->getScreen()->height, game->getScreen()->width + halfWidth, 0);

                //mBack->setRed(1.f);
                mBack->setGreen(0.f);
                //mBack->setBlue(1.f);
            }
            else
                mBack->resume(TEXTURE_ID_BACK);
            break;
        }
        case 16: Player::loadSound(SOUND_ID_FLAP); break;
        case 17: {

            if (!mAvailable) {

                mAvailable = new Element2D;
                mAvailable->initialize(game2DVia(game));
                mAvailable->start(TEXTURE_ID_AVAILABLE);
                mAvailable->setTexCoords(FULL_TEXCOORD_BUFFER);

                static const float verCoords[8] = { -1.f, 1.f, -1.f, -1.f, 1.f, -1.f, 1.f, 1.f };
                mAvailable->setVertices(verCoords);
                mAvailable->translate(AVAILABLE_TRANS, (game->getScreen()->bottom - mWitchY) / 2.f);
            }
            else
                mAvailable->resume(TEXTURE_ID_AVAILABLE);
            break;
        }
        case MAX_LOAD_STEP: {

            if (!mStores) {

                mStores = new Element2D;
                mStores->initialize(game2DVia(game));
                mStores->start(TEXTURE_ID_STORES);
                mStores->setTexCoords(FULL_TEXCOORD_BUFFER);

                mStores->setVertices(game->getScreen()->width >> 2, (game->getScreen()->height >> 1) +
                        (STORES_TEXTURE_HEIGHT * (game->getScreen()->width >> 2) / STORES_TEXTURE_WIDTH),
                        (game->getScreen()->width >> 1) + (game->getScreen()->width >> 2), (game->getScreen()->height >> 1) -
                        (STORES_TEXTURE_HEIGHT * (game->getScreen()->width >> 2) / STORES_TEXTURE_WIDTH));
                mStores->translate(-AVAILABLE_TRANS, (game->getScreen()->bottom - mWitchY) / 2.f);
                mStores->scale(1.5f, 1.5f);
            }
            else
                mStores->resume(TEXTURE_ID_STORES);
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
    if (mLoadStep != MAX_LOAD_STEP) {

        mProgress->translate(PROGRESS_TRANS, 0.f);
        return false;
    }
    delete mPumpkin;
    delete mProgress;

    mPumpkin = NULL;
    mProgress = NULL;

    return true;
}
void WitchSPet::renderLoad() const {

    mPumpkin->render(false);

    glDisable(GL_BLEND);
    mProgress->render(true);
    glEnable(GL_BLEND);
}

bool WitchSPet::update(const Game* game) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_WITCHSPET, (*game->getLog() % 100), LOG_FORMAT(" - g:%x"), __PRETTY_FUNCTION__, __LINE__, game);
#endif
#ifdef __ANDROID__
    // Restart player (needed for lock/unlock operation)
    Player::getInstance()->resume();
#endif
    if (mScene->getTransform()[Dynamic2D::TRANS_Y] < 0.f) {

        static float sceneVel = mSceneY / DISP_UPD_COUNT;
        static float titleVel = mTitleY / DISP_UPD_COUNT;

        mScene->translate(0.f, sceneVel);
        mTitle->translate(0.f, titleVel);
        mWitch->translate(WITCH_VELOCITY, 0.f);
        return true;
    }
    else
        mFace->setAlpha(1.f);

    // Touch...
    unsigned char touchCount = game->mTouchCount;
    while (touchCount--) {

        if (game->mTouchData[touchCount].Type != TouchInput::TOUCH_UP)
            continue;

        // Check 'Close' touched
        if ((mStep == CLOSE_DISPLAYED) && (game->mTouchData[touchCount].X > mCloseArea.left) &&
                (game->mTouchData[touchCount].X < mCloseArea.right) && (game->mTouchData[touchCount].Y > mCloseArea.top) &&
                (game->mTouchData[touchCount].Y < mCloseArea.bottom)) {

            Player::getInstance()->stop(0);
            return false;
        }

#ifdef __ANDROID__
        Launcher launchApp(GOOGLE_PLAY_PACKAGE, GOOGLE_PLAY_ACTIVITY, "market://details?id=com.studio.artaban.witchspet");
        launchApp.go();
#else
        [[UIApplication sharedApplication] openURL:[NSURL
                    URLWithString:@"itms-apps://itunes.apple.com/us/app/flappy-witchs-pet/id912119217"]];
#endif
        return true;
    }

    // Working...
    switch (mStep) {

        case TITLE_DISPLAYED: {

#ifdef DEBUG
            if (mDelay.isElapsed(game->getDelta(), DDELAY_DISPLAY, DELAY_DISPLAY, __PRETTY_FUNCTION__, __LINE__)) {
#else
            if (mDelay.isElapsed(game->getDelta(), DELAY_DISPLAY)) {
#endif
                LOGI(LOG_LEVEL_WITCHSPET, 0, LOG_FORMAT(" - Display available"), __PRETTY_FUNCTION__, __LINE__);
                mDelay.reset();
                ++mStep;
            }
            break;
        }
        case DISPLAYING_AVAILABLE: {

            if (mAvailable->getTransform()[Dynamic2D::TRANS_X] < 0.f)
                mAvailable->translate(AVAILABLE_VELOCITY, 0.f);
            else
                ++mStep;
            break;
        }
        case AVAILABLE_DISPLAYED: {

#ifdef DEBUG
            if (mDelay.isElapsed(game->getDelta(), DDELAY_DISPLAY, DELAY_DISPLAY, __PRETTY_FUNCTION__, __LINE__)) {
#else
            if (mDelay.isElapsed(game->getDelta(), DELAY_DISPLAY)) {
#endif
                LOGI(LOG_LEVEL_WITCHSPET, 0, LOG_FORMAT(" - Cancel available"), __PRETTY_FUNCTION__, __LINE__);
                mDelay.reset();
                ++mStep;
            }
            break;
        }
        case CANCELLING_AVAILABLE: {

            if (mAvailable->getTransform()[Dynamic2D::TRANS_X] > AVAILABLE_TRANS)
                mAvailable->translate(-AVAILABLE_VELOCITY, 0.f);
            else
                ++mStep;
            break;
        }
        case DISPLAYING_STORES: {

            if (mStores->getTransform()[Dynamic2D::TRANS_X] > 0.f)
                mStores->translate(-AVAILABLE_VELOCITY, 0.f);
            else {
#ifdef DEBUG
                if (mDelay.isElapsed(game->getDelta(), DDELAY_DISPLAY, DELAY_DISPLAY, __PRETTY_FUNCTION__, __LINE__))
#else
                if (mDelay.isElapsed(game->getDelta(), DELAY_DISPLAY))
#endif
                    ++mStep;
            }
            break;
        }
        case STORES_DISPLAYED: {

            if (mClose->getAlpha() < 1.f)
                mClose->setAlpha(mClose->getAlpha() + (ALPHA_VELOCITY * 4.f));
            else
                ++mStep;
            break;
        }
    }
    unsigned char animIdx = mWitch->mAnimIdx;
    mWitch->update(NULL, NULL);
    if ((!animIdx) && (mWitch->mAnimIdx))
        Player::getInstance()->play(0);

    return true;
}
void WitchSPet::renderUpdate() const {

    glDisable(GL_BLEND);
    mBack->render(false);
    glEnable(GL_BLEND);

    mMoon->render(false);

    mScene->render(true);
    mTitle->render(true);
    mWitch->render(true);
    mFace->render(true);

    mAvailable->render(true);
    mStores->render(true);

    mClose->render(true);
}
