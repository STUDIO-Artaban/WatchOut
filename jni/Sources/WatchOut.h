#ifndef WATCHOUT_H_
#define WATCHOUT_H_

#include <libeng/Game/2D/Game2D.h>
#include <libeng/Intro/Intro.h>

#ifdef __ANDROID__
#include "WitchSPet/WitchSPet.h"
#include "WatchOut/Title/TitleLevel.h"
#include "WatchOut/Scores/Scores.h"
#include "WatchOut/Game/GameLevel.h"
#else
#include "WitchSPet.h"
#include "TitleLevel.h"
#include "Scores.h"
#include "GameLevel.h"
#endif

#define FILE_NAME       "WatchOut.backup"

using namespace eng;

//////
class WatchOut : public Game2D {

private:
    WatchOut();
    virtual ~WatchOut();

    unsigned char mLoadStep;
    bool mPlaying;

    Scores* mScores;

    WitchSPet* mWitchSPet;
    Element2D* mBackground;
    TitleLevel* mTitle;
    GameLevel* mGame;
    WaitConn* mWaitConn;

public:
    static WatchOut* getInstance() {
        if (!mThis)
            mThis = new WatchOut;
        return static_cast<WatchOut*>(mThis);
    }
    static void freeInstance() {
        if (mThis) {
            delete mThis;
            mThis = NULL;
        }
    }

    //////
    void init() { }

    bool start();
    void pause();
    void destroy();

    void wait(float millis);
    void update();

    void render() const;

};

#endif // WATCHOUT_H_
