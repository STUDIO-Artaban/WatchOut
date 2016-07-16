#ifndef SCORES_H_
#define SCORES_H_

#include <libeng/Game/Game.h>
#include <libeng/Storage/StoreData.h>
#include <libeng/Graphic/Text/2D/Text2D.h>
#include <libeng/Graphic/Object/2D/Element2D.h>

#include "Global.h"
#include <list>

#define MAX_SCORE_COUNT         10

// Textures IDs
#define TEXTURE_ID_MEDAL        22

// Sound IDs
#define SOUND_ID_COLLISION      2
#define SOUND_ID_BONUS          5

// Medals
#define MEDAL_WHITE             90
#define MEDAL_BRONZE            270
#define MEDAL_SILVER            480

using namespace eng;

inline void setMedal(Panel2D* medal, short score) {

    assert(medal);
    if (score < MEDAL_WHITE)
        return; // White

    if (score < MEDAL_BRONZE) { // Bronze

        medal->setRed(0xc0 / 255.f);
        medal->setGreen(0x6c / 255.f);
        medal->setBlue(0x44 / 255.f);
    }
    else if (score < MEDAL_SILVER) { // Silver

        medal->setRed(0x80 / 255.f);
        medal->setGreen(0x80 / 255.f);
        medal->setBlue(0x80 / 255.f);
    }
    else { // Gold

        medal->setRed(0xe8 / 255.f);
        medal->setGreen(0xde / 255.f);
        medal->setBlue(0x4d / 255.f);
    }
};

//////
class Scores {

private:
    static Scores* mThis;
#ifdef DEBUG
    const unsigned int* mLog;
#endif
    StoreData* mData;
    void fill(unsigned char from = 0);

#if __cplusplus > 199711L // C++ 11
public:
    class ScoreData {
        
    public:
#else
    typedef struct ScoreData {
#endif
        ScoreData() : score(0), month(0), day(0), year(0) { }
        ScoreData(const ScoreData& copy) : score(copy.score), month(copy.month), day(copy.day), year(copy.year) { }

        short score;

        unsigned char month;
        unsigned char day;
        unsigned char year; // 20xx

        inline bool operator<(const ScoreData& compare) {

            return ((compare.score != score)? (compare.score < score): // Highest to lowest score...
                    ((compare.year != year)? (compare.year < year): // ...Earliest to latest date
                    ((compare.month != month)? (compare.month < month):
                    ((compare.day != day)? (compare.day < day):false))));
        }

#if __cplusplus > 199711L // C++ 11
    };
    
private:
#else
    } ScoreData;
    
#endif
    std::list<ScoreData> mScoreList; // Sorted by score & date (see 'ScoreData::operator<' above)

    void save();

    Text2D* mScores[MAX_SCORE_COUNT];
    Static2D* mMedals[MAX_SCORE_COUNT];

    std::wstring getScore(unsigned char scoreIdx) const;

    Text2D* mDispScore;
    Text2D* mDispBest;
    Element2D* mDispMedal;

    Static2D* mNew;

    float mDispWidth; // Digit width (in unit)

    enum {

        SCORE_INIT = 0,
        SCORE_COUNTER,
        SCORE_STAR,
        SCORE_DISPLAYED
    };
    unsigned char mStatus;

private:
    Scores(StoreData* data);
    virtual ~Scores();

    mutable short mCurScore;
    short mBestScore;
    bool mNewBest; // TRUE: New best score has been done; FALSE: No best score

    void increase(bool star, short &inc, short width, float looseY);

public:
    static Scores* getInstance(StoreData* data) {
        if (!mThis)
            mThis = new Scores(data);
        return mThis;
    }
    static void freeInstance() {
        if (mThis) {
            delete mThis;
            mThis = NULL;
        }
    }

    short mScore; // Last score done
    short mStarCount;

    bool mDispTitle; // TRUE: Display all scores from title level; FALSE: Display last score done from game level

    inline void prepare() {

        LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
        mStatus = SCORE_INIT;
        mNewBest = false;

        mNew->setAlpha(0.f);
    }
    inline bool isBestScore() const { return mNewBest; }
    inline float getBottom() const { assert(mDispTitle); return mScores[MAX_SCORE_COUNT - 1]->getBottom(); }

    //////
    void load(const Game* game, unsigned char step);
    inline void pause() {

        LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
        for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

            if (mScores[i]) mScores[i]->pause();
            if (mMedals[i]) mMedals[i]->pause();
        }
        if (mDispScore) mDispScore->pause();
        if (mDispBest) mDispBest->pause();
        if (mDispMedal) mDispMedal->pause();
        if (mNew) mNew->pause();
    };

    void create();
    void read();
    bool update(const Game* game, float looseY);

    void render() const;

};

#endif // SCORES_H_
