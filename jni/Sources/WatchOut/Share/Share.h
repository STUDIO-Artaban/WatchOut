#ifndef SHARE_H_
#define SHARE_H_

#include "Global.h"

#include <libeng/Graphic/Object/2D/Element2D.h>
#include <libeng/Graphic/Text/2D/Text2D.h>

#include <libeng/Social/Social.h>
#include <libeng/Social/Network.h>
#include <libeng/Features/Internet/Internet.h>

#ifdef __ANDROID__
#include "WatchOut/Game/Triangle.h"
#else
#include "Triangle.h"
#endif
#include <string>
#include <map>
#include <assert.h>

using namespace eng;

//////
class Share {

private:
#ifdef DEBUG
    const unsigned int* mLog;
#endif
    Social* mSocial;
    Internet* mInternet;
    Network::ID mNetworkID;
    short mScore;

    // External profile URL
    std::string mFacebookURL;
    std::string mTwitterURL;
    std::string mGoogleURL;

    std::map<Network::ID, bool> mDispPic;

    void setNetwork();
    void setPicture();
    bool select();
    void displayInfo(const Game* game, float looseY);

    std::string getUserURL() const;
    std::string getRequestURL() const;

    enum {

        ALL_HIDDEN = 0,
        PANEL_DISPLAYING,
        PANEL_DISPLAYED,
        NETWORKS_DISPLAYING,
        NETWORKS_DISPLAYED,
        SHARE_DISPLAYING,
        SHARE_DISPLAYED
    };
    unsigned char mStatus;

    Static2D mPanel;
    Static2D mNetworks;

    Triangle mUserBack;
    Text2D mUserText;

    Triangle mShareBack;
    Text2D mShareText;
    float mShareConst;
    float mShareFactor;
    float mShareBottom;

    Triangle mCloseBack;
    Text2D mCloseText;
    float mCloseConst;
    float mCloseFactor;
    float mCloseBottom;

    Static2D mLogBack;
    Text2D mLogoutText;
    short mLogoutX;
    short mLogoutY;

    Static2D mFrame;
    Static2D mPicture;
    Static2D mNetworkPic;
    Text2D mInfo;
    Text2D mGender;
    Text2D mBirthday;
    Text2D* mURL;

    TouchArea mFacebookArea;
    float mTwitterConst;
    float mTwitterFactor;
    float mTwitterTop;
    short mGoogleX;
    short mGoogleY;

    TouchArea mShareArea;
    TouchArea mCloseArea;

public:
    Share();
    virtual ~Share();

    inline void display() {

        if (mStatus != PANEL_DISPLAYED)
            mStatus = PANEL_DISPLAYING;
    }
    inline void reset() { mStatus = ALL_HIDDEN; }
    inline void setScore(short score) { mScore = score; }

    //////
    void start(const Game* game, float looseY);
    inline void pause() {

        mPanel.pause();
        mNetworks.pause();
        mPicture.pause();
        mNetworkPic.pause();
        mFrame.pause();

        mInfo.pause();
        mUserBack.pause();
        mUserText.pause();
        mShareBack.pause();
        mShareText.pause();
        mCloseBack.pause();
        mCloseText.pause();
        mLogBack.pause();
        mLogoutText.pause();

        mGender.pause();
        mBirthday.pause();
        if (mURL)
            mURL->pause();
    }
    void resume();

    bool update(const Game* game, float looseY);
    void render() const;

};

#endif // SHARE_H_
