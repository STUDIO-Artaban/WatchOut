#ifndef GAMELEVEL_H_
#define GAMELEVEL_H_

#include "Global.h"

#include <libeng/Game/Game.h>
#include <libeng/Graphic/Object/2D/Element2D.h>
#include <libeng/Graphic/Text/2D/Text2D.h>
#include <libeng/Advertising/Advertising.h>

#ifdef __ANDROID__
#include "WatchOut/Scores/Scores.h"
#include "WatchOut/Share/Share.h"
#include "WatchOut/Game/Triangle.h"
#else
#include "Scores.h"
#include "Share.h"
#include "Triangle.h"
#endif

#define MAX_GAME_LOAD           17

#define TRANS_VELOCITY          0.01f
#define ROTATE_VELOCITY         (PI_F / 160.f)

using namespace eng;

//////
class GameLevel {

private:
    unsigned char mLoadStep;
    unsigned int mTouchID;

#ifndef DEMO_VERSION
    Advertising* mAdvertising;
    bool mAdLoaded;
    void adDisplay(bool delay);
#endif

    Element2D* mDisk;
    Triangle* mTriangle;
    Element2D* mSquare;
    Element2D* mStar;
    bool mTriangleRotate; // TRUE: Clockwise; FALSE: Anticlockwise
    bool mSquareRotate; // ...

    Element2D* mChip;
    TouchArea mScreenArea;

    enum {

        TRANS_VEL_X = 0,
        TRANS_VEL_Y
    };
    float mTransVel;

    BorderLeft mScreenLeft;
    BorderTop mScreenTop;
    BorderRight mScreenRight;
    BorderBottom mScreenBottom;

    enum {

        GAME_READY = 0,
        GAME_STARTED,
        GAME_OVER, // Chip has touched something
        GAME_SCORE,
        GAME_SHARE
    };
    unsigned char mStep;

    Text2D* mScore;
    Element2D* mStarScore;
    float mScoreW; // Half digit width (in unit)
    time_t mTimer;

    short mStarCount;

    Static2D* mGameOver;
    float mLooseY; // Bottom of the game over graph (in unit)

    Text2D* mMedalText;
    Text2D* mScoreText;
    Text2D* mBestText;
    Text2D* mStarText;

    Scores* mScoreData;
    Share* mShare;

    Triangle* mOkBack;
    Text2D* mOkText;
    float mOkConst;
    float mOkFactor;
    float mOkBottom;

public:
    GameLevel();
    virtual ~GameLevel();

    inline unsigned char getLoadStep() const { return mLoadStep; }
    inline void start(float yPos) {

        mTriangle->reset();
        mDisk->reset();
        mSquare->reset();

        mTriangle->setVertices(g_TriangleVertices);
        mDisk->scale(1.f, 1.f);
        mSquare->scale(1.f, 1.f);

        mTriangle->rotate(0.f);
        mSquare->rotate(0.f);

        // Position
        switch (std::rand() % 3) {
            case 0: {
                mTriangle->translate(-0.5f, yPos);
                switch (std::rand() % 2) {
                    case 0: {

                        LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Position A-0"), __PRETTY_FUNCTION__, __LINE__);
                        mDisk->translate(0.5f, yPos);
                        mSquare->translate(0.f, -yPos);
                        break;
                    }
                    case 1: {

                        LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Position A-1"), __PRETTY_FUNCTION__, __LINE__);
                        mSquare->translate(0.5f, yPos);
                        mDisk->translate(0.f, -yPos);
                        break;
                    }
                }
                break;
            }
            case 1: {
                mDisk->translate(-0.5f, yPos);
                switch (std::rand() % 2) {
                    case 0: {

                        LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Position B-0"), __PRETTY_FUNCTION__, __LINE__);
                        mTriangle->translate(0.5f, yPos);
                        mSquare->translate(0.f, -yPos);
                        break;
                    }
                    case 1: {

                        LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Position B-1"), __PRETTY_FUNCTION__, __LINE__);
                        mSquare->translate(0.5f, yPos);
                        mTriangle->translate(0.f, -yPos);
                        break;
                    }
                }
                break;
            }
            case 2: {
                mSquare->translate(-0.5f, yPos);
                //switch (std::rand() % 2) {
                switch (1) {
                    case 0: {

                        LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Position C-0"), __PRETTY_FUNCTION__, __LINE__);
                        mTriangle->translate(0.5f, yPos);
                        mDisk->translate(0.f, -yPos);
                        break;
                    }
                    case 1: {

                        LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Position C-1"), __PRETTY_FUNCTION__, __LINE__);
                        mDisk->translate(0.5f, yPos);
                        mTriangle->translate(0.f, -yPos);
                        break;
                    }
                }
                break;
            }
        }

        // Rotation
        mTriangleRotate = (std::rand() % 2)? true:false;
        mSquareRotate = !mTriangleRotate;
        LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Rotation: Square-%s; Triangle-%s"), __PRETTY_FUNCTION__, __LINE__,
                (mSquareRotate)? "true":"false", (mTriangleRotate)? "true":"false");

        // Move
        switch (std::rand() % 4) {
            case 0: {

                LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Triangle move: 0"), __PRETTY_FUNCTION__, __LINE__);
                mTriangle->getVelocities()[TRANS_VEL_X] = TRANS_VELOCITY;
                mTriangle->getVelocities()[TRANS_VEL_Y] = TRANS_VELOCITY;
                break;
            }
            case 1: {

                LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Triangle move: 1"), __PRETTY_FUNCTION__, __LINE__);
                mTriangle->getVelocities()[TRANS_VEL_X] = -TRANS_VELOCITY;
                mTriangle->getVelocities()[TRANS_VEL_Y] = TRANS_VELOCITY;
                break;
            }
            case 2: {

                LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Triangle move: 2"), __PRETTY_FUNCTION__, __LINE__);
                mTriangle->getVelocities()[TRANS_VEL_X] = -TRANS_VELOCITY;
                mTriangle->getVelocities()[TRANS_VEL_Y] = -TRANS_VELOCITY;
                break;
            }
            case 3: {

                LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Triangle move: 3"), __PRETTY_FUNCTION__, __LINE__);
                mTriangle->getVelocities()[TRANS_VEL_X] = TRANS_VELOCITY;
                mTriangle->getVelocities()[TRANS_VEL_Y] = -TRANS_VELOCITY;
                break;
            }
        }
        switch (std::rand() % 4) {
            case 0: {

                LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Disk move: 0"), __PRETTY_FUNCTION__, __LINE__);
                mDisk->getVelocities()[TRANS_VEL_X] = TRANS_VELOCITY;
                mDisk->getVelocities()[TRANS_VEL_Y] = TRANS_VELOCITY;
                break;
            }
            case 1: {

                LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Disk move: 1"), __PRETTY_FUNCTION__, __LINE__);
                mDisk->getVelocities()[TRANS_VEL_X] = -TRANS_VELOCITY;
                mDisk->getVelocities()[TRANS_VEL_Y] = TRANS_VELOCITY;
                break;
            }
            case 2: {

                LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Disk move: 2"), __PRETTY_FUNCTION__, __LINE__);
                mDisk->getVelocities()[TRANS_VEL_X] = -TRANS_VELOCITY;
                mDisk->getVelocities()[TRANS_VEL_Y] = -TRANS_VELOCITY;
                break;
            }
            case 3: {

                LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Disk move: 3"), __PRETTY_FUNCTION__, __LINE__);
                mDisk->getVelocities()[TRANS_VEL_X] = TRANS_VELOCITY;
                mDisk->getVelocities()[TRANS_VEL_Y] = -TRANS_VELOCITY;
                break;
            }
        }
        switch (std::rand() % 4) {
            case 0: {

                LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Square move: 0"), __PRETTY_FUNCTION__, __LINE__);
                mSquare->getVelocities()[TRANS_VEL_X] = TRANS_VELOCITY;
                mSquare->getVelocities()[TRANS_VEL_Y] = TRANS_VELOCITY;
                break;
            }
            case 1: {

                LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Square move: 1"), __PRETTY_FUNCTION__, __LINE__);
                mSquare->getVelocities()[TRANS_VEL_X] = -TRANS_VELOCITY;
                mSquare->getVelocities()[TRANS_VEL_Y] = TRANS_VELOCITY;
                break;
            }
            case 2: {

                LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Square move: 2"), __PRETTY_FUNCTION__, __LINE__);
                mSquare->getVelocities()[TRANS_VEL_X] = -TRANS_VELOCITY;
                mSquare->getVelocities()[TRANS_VEL_Y] = -TRANS_VELOCITY;
                break;
            }
            case 3: {

                LOGI(LOG_LEVEL_GAMELEVEL, 0, LOG_FORMAT(" - Square move: 3"), __PRETTY_FUNCTION__, __LINE__);
                mSquare->getVelocities()[TRANS_VEL_X] = TRANS_VELOCITY;
                mSquare->getVelocities()[TRANS_VEL_Y] = -TRANS_VELOCITY;
                break;
            }
        }
    }

    //////
    inline void pause() {

        mLoadStep = 0;

        if (mTriangle) mTriangle->pause();
        if (mSquare) mSquare->pause();
        if (mDisk) mDisk->pause();
        if (mChip) mChip->pause();
        if (mScore) mScore->pause();
        if (mGameOver) mGameOver->pause();
        if (mMedalText) mMedalText->pause();
        if (mScoreText) mScoreText->pause();
        if (mBestText) mBestText->pause();
        if (mOkBack) mOkBack->pause();
        if (mOkText) mOkText->pause();
        if (mStar) mStar->pause();
        if (mStarScore) mStarScore->pause();
        if (mStarText) mStarText->pause();
        if (mShare) mShare->pause();
    };

    bool load(const Game* game);
    bool update(const Game* game);

    void render() const;

};

#endif // GAMELEVEL_H_
