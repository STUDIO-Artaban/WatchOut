#include "Scores.h"

#include <libeng/Game/2D/Game2D.h>
#include <libeng/Storage/Storage.h>
#include <libeng/Tools/Tools.h>

#include <ctime>

#define STAR_BONUS              30

#define DISP_SCORE_DELAY        8
#define DISP_MEDAL_SCALE        0.6f

#define SCORE_FACTOR_H          ((1.25f - 1.1f) / ((976.f / 600.f) - (1024.f / 768.f)))
#define SCORE_CONST_H           (1.1f - (SCORE_FACTOR_H * 1024.f / 768.f))
// SCORE_FACTOR_H * (976.f / 600.f) + SCORE_CONST_H -> 1.25f
// SCORE_FACTOR_H * (1024.f / 768.f) + SCORE_CONST_H -> 1.1f

#define DIGIT_COUNT(num)        ((num < 10)? 1:((num < 100)? 2:((num < 1000)? 3:(num < 10000)? 4:5)))

// Data key(s)
#define DATA_KEY_MONTH          "Month"
#define DATA_KEY_DAY            "Day"
#define DATA_KEY_YEAR           "Year"
#define DATA_KEY_SCORE          "Score"

Scores* Scores::mThis = NULL;

//////
Scores::Scores(StoreData* data) : mData(data), mCurScore(0), mBestScore(0), mDispTitle(true), mScore(0),
        mDispWidth(0.f), mNewBest(false), mStatus(SCORE_INIT), mStarCount(0) {

    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(" - d:%x"), __PRETTY_FUNCTION__, __LINE__, data);
    assert(data);

    std::memset(mScores, 0, sizeof(Text2D*) * MAX_SCORE_COUNT);
    std::memset(mMedals, 0, sizeof(Static2D*) * MAX_SCORE_COUNT);

    mDispScore = NULL;
    mDispBest = NULL;
    mDispMedal = NULL;

    mNew = NULL;

#ifdef DEBUG
    mLog = NULL;
#endif
}
Scores::~Scores() {

    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

        if (mScores[i]) delete mScores[i];
        if (mMedals[i]) delete mMedals[i];
    }
    if (mDispScore) delete mDispScore;
    if (mDispBest) delete mDispBest;
    if (mDispMedal) delete mDispMedal;
    if (mNew) delete mNew;
}

std::wstring Scores::getScore(unsigned char scoreIdx) const {

    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(" - s:%d"), __PRETTY_FUNCTION__, __LINE__, scoreIdx);
    assert(scoreIdx < MAX_SCORE_COUNT);

    // Rank
    std::wstring score;
    if (scoreIdx != 9)
        score.append(1, L'0');
    score.append(numToWStr<unsigned char>(scoreIdx + 1));
    score.append(1, L'-');

    // Month
    std::string field(DATA_KEY_MONTH);
    field.append(1, (scoreIdx | 0x30)); // "| 0x30" to ASCII code
    std::wstring data(numToWStr<unsigned char>(*static_cast<unsigned char*>(mData->get(DATA_CHAR, field.c_str()))));
    if (data.size() == 1) score.append(L"0");
    score.append(data);
    score.append(L"/");

    // Day
    field.assign(DATA_KEY_DAY);
    field.append(1, (scoreIdx | 0x30));
    data.assign(numToWStr<unsigned char>(*static_cast<unsigned char*>(mData->get(DATA_CHAR, field.c_str()))));
    if (data.size() == 1) score.append(L"0");
    score.append(data);
    score.append(L"/");

    // Year
    field.assign(DATA_KEY_YEAR);
    field.append(1, (scoreIdx | 0x30));
    data.assign(numToWStr<unsigned char>(*static_cast<unsigned char*>(mData->get(DATA_CHAR, field.c_str()))));
    if (data.size() == 1) score.append(L"0");
    score.append(data);

    // Score
    field.assign(DATA_KEY_SCORE);
    field.append(1, (scoreIdx | 0x30));

    mCurScore = *static_cast<short*>(mData->get(DATA_SHORT, field.c_str()));
    score.append(6 - DIGIT_COUNT(mCurScore), L' ');
    score.append(numToWStr<short>(mCurScore));

    return score;
}

void Scores::load(const Game* game, unsigned char step) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(" - g:%x; s:%d"), __PRETTY_FUNCTION__, __LINE__, game, step);
    mLog = game->getLog();
#endif
    static float yPos = (game->getScreen()->height - ((game->getScreen()->height - (((((game->getScreen()->height - (MAX_AD_HEIGHT *
                static_cast<short>(game->getScreen()->yDpi) / 160) - (game->getScreen()->height >> 1)) >> 2) * 3) +
                (game->getScreen()->height >> 1)) - (game->getScreen()->width >> 3))) << 1)) /
                 static_cast<float>(game->getScreen()->width);
    // yPos == GameLevel::mLooseY...

    switch (step) {
        case 0: { // Title scores

            for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

                if (!mScores[i]) {

                    mScores[i] = new Text2D;
                    mScores[i]->initialize(game2DVia(game));
                    mScores[i]->start(getScore(i));

                    float scale = game->getScreen()->width / FONT_SCALE_RATIO;
                    mScores[i]->scale(scale, scale);

                    float factor = ((game->getScreen()->height / static_cast<float>(game->getScreen()->width)) *
                            SCORE_FACTOR_H) + SCORE_CONST_H;
                    if (factor < 1.f)
                        factor = 1.f;

                    float height = ((FONT_HEIGHT << 1) / static_cast<float>(game->getScreen()->width)) * factor * scale;
                    mScores[i]->position(((2.f - (((17.f * FONT_WIDTH * scale) * 2.f) / game->getScreen()->width)) / 2.f) -
                            1.f, game->getScreen()->top - (((game->getScreen()->top * 2.f) - (height * 10.f)) / 2.f) -
                            ((i)? (height * i):0.f));

                    if (!i) // First score is best score
                        mBestScore = mCurScore;
                }
                else
                    mScores[i]->resume();

                if (!mMedals[i]) {

                    mMedals[i] = new Static2D;
                    mMedals[i]->initialize(game2DVia(game));
                    mMedals[i]->start(TEXTURE_ID_MEDAL);
                    mMedals[i]->setTexCoords(FULL_TEXCOORD_BUFFER);

                    float verCoords[8];
                    verCoords[0] = mScores[i]->getLeft() + (12.f * ((mScores[i]->getRight() - mScores[i]->getLeft()) / 17.f));
                    verCoords[1] = mScores[i]->getTop();                            // Put medal after the 12th letter in code above
                    verCoords[2] = verCoords[0];                                    // -> "/ 17.f" == sizeof("99-99/99/99   999")
                    verCoords[3] = mScores[i]->getBottom();
                    verCoords[4] = verCoords[0] + (verCoords[1] - verCoords[3]);
                    verCoords[5] = verCoords[3];
                    verCoords[6] = verCoords[4];
                    verCoords[7] = verCoords[1];
                    mMedals[i]->setVertices(verCoords);

                    setMedal(static_cast<Panel2D*>(mMedals[i]), mCurScore);
                }
                else
                    mMedals[i]->resume(TEXTURE_ID_MEDAL);
            }
            //if (mDispTitle)
            //    hide();
            break;
        }
        case 1: { // Score

            if (!mDispScore) {

                mDispScore = new Text2D;
                mDispScore->initialize(game2DVia(game));
                mDispScore->start(L"0");
                mDispScore->setColor(1.f, 1.f, 0.f);

                float scale = game->getScreen()->width / FONT_SCALE_RATIO;
                mDispScore->scale(scale, scale);

                yPos -= ((yPos - (game->getScreen()->bottom + (((MAX_AD_HEIGHT * static_cast<short>(game->getScreen()->yDpi) /
                        160) << 1) / static_cast<float>(game->getScreen()->width)))) - (1.f + (g_TriangleVertices[3] -
                        g_TriangleVertices[1]))) / 2.f;
                // ...now == GameLevel::mLooseY

                mDispWidth = ((FONT_WIDTH << 1) / static_cast<float>(game->getScreen()->width)) * scale;
                mDispScore->position(1.f - mDispWidth, yPos - (((FONT_HEIGHT << 1) /
                        static_cast<float>(game->getScreen()->width)) * scale));
            }
            else
                mDispScore->resume();
            break;
        }
        case 2: { // Best

            if (!mDispBest) {

                mDispBest = new Text2D;
                mDispBest->initialize(game2DVia(game));
                mDispBest->start(numToWStr<short>(mBestScore));
                mDispBest->setColor(1.f, 1.f, 0.f);

                float scale = game->getScreen()->width / FONT_SCALE_RATIO;
                mDispBest->scale(scale, scale);
                mDispBest->position(1.f - (mDispWidth * DIGIT_COUNT(mBestScore)), yPos - 0.5f - (((FONT_HEIGHT << 1) /
                        static_cast<float>(game->getScreen()->width)) * scale));
            }
            else
                mDispBest->resume();
            break;
        }
        case 3: { // Medal

            if (!mDispMedal) {

                mDispMedal = new Element2D;
                mDispMedal->initialize(game2DVia(game));
                mDispMedal->start(TEXTURE_ID_MEDAL);
                mDispMedal->setTexCoords(FULL_TEXCOORD_BUFFER);

                static const float verCoord[8] = { -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f };
                mDispMedal->setVertices(verCoord);
                mDispMedal->scale(DISP_MEDAL_SCALE, DISP_MEDAL_SCALE);

                mDispMedal->translate(-0.5f, yPos - 0.5f);
            }
            else
                mDispMedal->resume(TEXTURE_ID_MEDAL);
            break;
        }
        case 4: { // New (background)

            if (!mNew) {

                mNew = new Static2D;
                mNew->initialize(game2DVia(game));
                mNew->start(0xff, 0x00, 0x00);
                mNew->setTexCoords(FULL_TEXCOORD_BUFFER);

                float verCoord[8];
                verCoord[0] = 1.f - (8.f * mDispWidth); // New Best -> 8 letters
                verCoord[1] = mDispBest->getTop() + (((FONT_HEIGHT << 1) /
                        static_cast<float>(game->getScreen()->width)) * (game->getScreen()->width / FONT_SCALE_RATIO));
                verCoord[2] = verCoord[0];
                verCoord[3] = mDispBest->getTop();
                verCoord[4] = 1.f - (5.f * mDispWidth); // " Best" -> 5 letters
                verCoord[5] = verCoord[3];
                verCoord[6] = verCoord[4];
                verCoord[7] = verCoord[1];

                mNew->setVertices(verCoord);
            }
            else
                mNew->resume(0xff, 0x00, 0x00);
            break;
        }

#ifdef DEBUG
        default: {

            LOGW(LOG_FORMAT(" - Unknown load step: %d"), __PRETTY_FUNCTION__, __LINE__, step);
            assert(NULL);
            break;
        }
#endif
    }
}

void Scores::fill(unsigned char from) { // Fill 'mData' with empty data from 'from' index

    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(" - f:%d"), __PRETTY_FUNCTION__, __LINE__, from);
    for (unsigned char i = from; i < MAX_SCORE_COUNT; ++i) {

        // Month
        std::string field(DATA_KEY_MONTH);
        field.append(1, (i | 0x30)); // "| 0x30" to ASCII code
        mData->add(DATA_CHAR, field.c_str(), static_cast<void*>(new unsigned char(0)));

        // Day
        field.assign(DATA_KEY_DAY);
        field.append(1, (i | 0x30));
        mData->add(DATA_CHAR, field.c_str(), static_cast<void*>(new unsigned char(0)));

        // Year
        field.assign(DATA_KEY_YEAR);
        field.append(1, (i | 0x30));
        mData->add(DATA_CHAR, field.c_str(), static_cast<void*>(new unsigned char(0)));

        // Score
        field.assign(DATA_KEY_SCORE);
        field.append(1, (i | 0x30));
        mData->add(DATA_SHORT, field.c_str(), static_cast<void*>(new short(0)));
    }
}
void Scores::create() {

    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    mData->clear();
    fill(); // Fill 'mData' with empty data (00/00/00 - 0)
}
void Scores::read() { // 'mData' -> 'mScoreList'

    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

        ScoreData scoreData;

        // Month
        std::string field(DATA_KEY_MONTH);
        field.append(1, (i | 0x30)); // "| 0x30" to ASCII code
        scoreData.month = *static_cast<unsigned char*>(mData->get(DATA_CHAR, field.c_str()));
        if (!scoreData.month) // 00/00/00 - 0 ?
            break; // No more score (exit)

        // Day
        field.assign(DATA_KEY_DAY);
        field.append(1, (i | 0x30));
        scoreData.day = *static_cast<unsigned char*>(mData->get(DATA_CHAR, field.c_str()));

        // Year
        field.assign(DATA_KEY_YEAR);
        field.append(1, (i | 0x30));
        scoreData.year = *static_cast<unsigned char*>(mData->get(DATA_CHAR, field.c_str()));

        // Score
        field.assign(DATA_KEY_SCORE);
        field.append(1, (i | 0x30));
        scoreData.score = *static_cast<short*>(mData->get(DATA_SHORT, field.c_str()));

        mScoreList.push_back(scoreData);
    }
    // The list should be already sorted
}
void Scores::save() { // 'mScoreList' -> 'mData'

    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    time_t curDate = time(0);
    struct tm* now = localtime(&curDate);

    ScoreData scoreData;
    scoreData.month = static_cast<unsigned char>(now->tm_mon + 1);
    scoreData.day = static_cast<unsigned char>(now->tm_mday);
    scoreData.year = static_cast<unsigned char>(now->tm_year + 1900 - 2000); // 20xx

    scoreData.score = mScore;

    mScoreList.push_back(scoreData);
    //mScoreList.sort(); // Sort score list (after having added new score)

    // Remove last score if more than MAX_SCORE_COUNT in the score list
#ifdef DEBUG
    if (mScoreList.size() > MAX_SCORE_COUNT) {

        LOGI(LOG_LEVEL_SCORES, 0, LOG_FORMAT(" - Last score removed"), __PRETTY_FUNCTION__, __LINE__);
        mScoreList.pop_back();
    }
#else
    if (mScoreList.size() > MAX_SCORE_COUNT)
        mScoreList.pop_back();
#endif

    mData->clear();
    unsigned char scoreIdx = 0;
    for (std::list<ScoreData>::iterator iter = mScoreList.begin(); iter != mScoreList.end(); ++iter, ++scoreIdx) {

        assert(scoreIdx < MAX_SCORE_COUNT);

        // Month
        std::string field(DATA_KEY_MONTH);
        field.append(1, (scoreIdx | 0x30)); // "| 0x30" to ASCII code
        mData->add(DATA_CHAR, field.c_str(), static_cast<void*>(new unsigned char((*iter).month)));

        // Day
        field.assign(DATA_KEY_DAY);
        field.append(1, (scoreIdx | 0x30));
        mData->add(DATA_CHAR, field.c_str(), static_cast<void*>(new unsigned char((*iter).day)));

        // Year
        field.assign(DATA_KEY_YEAR);
        field.append(1, (scoreIdx | 0x30));
        mData->add(DATA_CHAR, field.c_str(), static_cast<void*>(new unsigned char((*iter).year)));

        // Score
        field.assign(DATA_KEY_SCORE);
        field.append(1, (scoreIdx | 0x30));
        mData->add(DATA_SHORT, field.c_str(), static_cast<void*>(new short((*iter).score)));
    }
    fill(scoreIdx); // Needed if 'mScoreData' length < MAX_SCORE_COUNT
    if (!Storage::saveFile(g_GameFile, mData)) {

        LOGW(LOG_FORMAT(" - Failed to save data into '%s' file"), __PRETTY_FUNCTION__, __LINE__, g_GameFile);
        assert(NULL);
    }
}

void Scores::increase(bool star, short &inc, short width, float looseY) {

    LOGV(LOG_LEVEL_SCORES, 0, LOG_FORMAT(" - s:%s; i:%d; w:%d; l:%f"), __PRETTY_FUNCTION__, __LINE__,
            (star)? "true":"false", inc, width, looseY);
    unsigned char digitCount = DIGIT_COUNT(inc);
    if (star) {

        assert(Player::getInstance()->getIndex(SOUND_ID_BONUS) == 3);
        Player::getInstance()->play(3);
        inc += STAR_BONUS;
    }
    else {

        assert(Player::getInstance()->getIndex(SOUND_ID_COLLISION) == 0);
        Player::getInstance()->play(0);
        ++inc;
    }
    setMedal(static_cast<Panel2D*>(mDispMedal), inc);
    mDispScore->update(numToWStr<short>(inc));
    if (DIGIT_COUNT(inc) != digitCount) {
        mDispScore->position(mDispScore->getLeft() - mDispWidth, looseY - (((FONT_HEIGHT << 1) /
                static_cast<float>(width)) * (width / FONT_SCALE_RATIO)));
        mDispScore->setColor(1.f, 1.f, 0.f);
    }
    if (inc > mBestScore) {

        if (!mNewBest) {

            mNew->setAlpha(1.f);
            mNewBest = true;
        }
        if (DIGIT_COUNT(inc) != DIGIT_COUNT(mBestScore))
            mDispBest->position(mDispBest->getLeft() - mDispWidth, looseY - 0.5f - (((FONT_HEIGHT << 1) /
                    static_cast<float>(width)) * (width / FONT_SCALE_RATIO)));
        mBestScore = inc;
        mDispBest->update(numToWStr<short>(inc));
        mDispBest->setColor(1.f, 1.f, 0.f);
    }
}

bool Scores::update(const Game* game, float looseY) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_SCORES, (*mLog % 100), LOG_FORMAT(" - g:%x; l:%f (d:%s)"), __PRETTY_FUNCTION__, __LINE__, game, looseY,
            (mDispTitle)? "true":"false");
#endif
    if (mDispTitle) // Title scores
        for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

            std::wstring score(getScore(i));
            if (mScores[i]->getText() != score) {
                mScores[i]->update(score);
                setMedal(static_cast<Panel2D*>(mMedals[i]), mCurScore);
            }
        }

    else { // Game score

        static short score = 0;
        switch (mStatus) {
            case SCORE_INIT: {

                if (mDispScore->getText() != L"0") {

                    mDispScore->position(mDispScore->getLeft() + ((mDispScore->getText().length() - 1) * mDispWidth),
                            looseY - (((FONT_HEIGHT << 1) / static_cast<float>(game->getScreen()->width)) *
                            (game->getScreen()->width / FONT_SCALE_RATIO)));
                    mDispScore->update(L"0");
                    mDispScore->setColor(1.f, 1.f, 0.f);
                }
                mDispMedal->setRed(1.f);
                mDispMedal->setGreen(1.f);
                mDispMedal->setBlue(1.f);

                score = 0;
                if (mScore)
                    mStatus = SCORE_COUNTER;
                else
                    mStatus = SCORE_DISPLAYED; // No star
                break;
            }
            case SCORE_COUNTER: {

                static unsigned char counter = 0;
                if (++counter != (DISP_SCORE_DELAY >> 1))
                    break;
                counter = 0;

                increase(false, score, game->getScreen()->width, looseY);
                if (score == mScore) {

                    mStatus = SCORE_STAR;
                    counter = 0;
                }
                break;
            }
            case SCORE_STAR: {

                if (!mStarCount) {

                    if (mScore)
                        save(); // Save score into 'mData' & into file game
                    mStatus = SCORE_DISPLAYED;
                    break;
                }
                static unsigned char counter = 0;
                if (++counter != (DISP_SCORE_DELAY << 2))
                    break;
                counter = 0;

                increase(true, score, game->getScreen()->width, looseY);
                if (--mStarCount)
                    break;
                mScore = score;

                save(); // Save score into 'mData' & into file game
                mStatus = SCORE_DISPLAYED;
                break;
            }
            case SCORE_DISPLAYED:
                return false;
        }
        return true;
    }
    return true;
}

void Scores::render() const {

#ifdef DEBUG
    LOGV(LOG_LEVEL_SCORES, (*mLog % 100), LOG_FORMAT(" - (d:%s)"), __PRETTY_FUNCTION__, __LINE__, (mDispTitle)? "true":"false");
    if (mDispTitle)
        for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

            assert(mScores[i]);
            assert(mMedals[i]);
        }
#endif
    if (mDispTitle) // Title scores
        for (unsigned char i = 0; i < MAX_SCORE_COUNT; ++i) {

            mScores[i]->render(true);
            mMedals[i]->render(true);
        }

    else { // Game score

        mDispScore->render(true);
        mDispBest->render(true);
        mDispMedal->render(true);

        mNew->render(true);
    }
}
