#ifndef TITLELEVEL_H_
#define TITLELEVEL_H_

#include <libeng/Game/Game.h>
#include <libeng/Graphic/Object/2D/Element2D.h>
#include <libeng/Graphic/Text/2D/Text2D.h>
#include <libeng/Advertising/Advertising.h>

#ifdef __ANDROID__
#include "WatchOut/Scores/Scores.h"
#include "WatchOut/Game/Triangle.h"
#else
#include "Scores.h"
#include "Triangle.h"
#endif

#define MAX_TITLE_LOAD          15

using namespace eng;

//////
class TitleLevel {

private:
    unsigned char mLoadStep;

#ifndef DEMO_VERSION
    Advertising* mAdvertising;
    bool mAdLoaded;
    void adDisplay(bool delay);
#endif

    Static2D* mTitle;
    Static2D* mChip;

    Element2D* mDisk;
    Text2D* mStart;
    short mDiskY; // Middle

    Element2D* mSquare;
    Static2D* mHidden; // Used to hide background when scores is displayed
    Text2D* mScores;
    TouchArea mScoresArea;

    Triangle* mTriangle;
    Scores* mScoresData;
    Text2D* mOk;
    float mOkConst;
    float mOkFactor;
    float mOkBottom;

public:
    TitleLevel();
    virtual ~TitleLevel();

    inline unsigned char getLoadStep() const { return mLoadStep; }

    //////
    inline void pause() {

        mLoadStep = 0;
        mScoresData->pause();

        if (mTitle) mTitle->pause();
        if (mChip) mChip->pause();
        if (mTriangle) mTriangle->pause();
        if (mDisk) mDisk->pause();
        if (mSquare) mSquare->pause();
        if (mHidden) mHidden->pause();
        if (mStart) mStart->pause();
        if (mScores) mScores->pause();
        if (mOk) mOk->pause();
    };

    bool load(const Game* game);
    bool update(const Game* game);

    void render() const;

};

#endif // TITLELEVEL_H_
