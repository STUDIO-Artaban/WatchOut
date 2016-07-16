#ifndef WITCHSPET_H_
#define WITCHSPET_H_

#include "Global.h"

#include <libeng/Game/Level.h>
#include <libeng/Graphic/Object/2D/Element2D.h>

#ifdef __ANDROID__
#include "WitchSPet/Witch.h"
#else
#include "Witch.h"
#endif

using namespace eng;

//////
class WitchSPet : public Level {

private:
    Delay mDelay;

    enum { // Advertising step

        TITLE_DISPLAYED = 0,
        DISPLAYING_AVAILABLE,
        AVAILABLE_DISPLAYED,
        CANCELLING_AVAILABLE,
        DISPLAYING_STORES,
        STORES_DISPLAYED,
        CLOSE_DISPLAYED
    };
    unsigned char mStep;

    //
    Static2D* mPumpkin;
    Element2D* mProgress;
    Static2D* mClose;

    TouchArea mCloseArea;

    Static2D* mBack;
    Static2D* mMoon;
    Element2D* mScene;

    float mWitchY;
    float mTitleY;
    float mSceneY;

    Witch* mWitch;
    Static2D* mFace;
    Element2D* mTitle;
    Element2D* mAvailable;
    Element2D* mStores;

public:
    WitchSPet(Game* game);
    virtual ~WitchSPet();

    //////
    void init() { }
    void pause();

    void wait(float millis) { }

protected:
    bool loaded(const Game* game);
public:
    bool update(const Game* game);

    void renderLoad() const;
    void renderUpdate() const;

};

#endif // WITCHSPET_H_
