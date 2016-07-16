#include "Share.h"
#include "Global.h"

#ifdef __ANDROID__
#include <boost/math/tr1.hpp>
#else
#include <math.h>
#include "OpenGLES/ES2/gl.h"
#endif
#include <libeng/Game/2D/Game2D.h>
#include <libeng/Social/Session.h>
#include <libeng/Tools/Tools.h>

#include <string>
#include <time.h>

#define EMPTY_BIRTHDAY          L",,, ,," // Will be replace by "___ __" characters (see font texture)
#define PROFIL_INFO_LAG         0.04f
#define PROFIL_INFO_SCALE       0.66f
#define LOGOUT_RADIUS           0.25f

#define RING_TEXTURE_SIZE       256
#define RING_THICKNESS          38 // In pixel (ring texture)

#define WATCHOUT_HOST           "www.studio-artaban.com"
#define WATCHOUT_PHP            "/WatchOut.php?user="
#define URL_PHP_DATA            "&data="
#define URL_PHP_NAME            "&name="

#define DB_NAME_SIZE            250

#define STUDIO_ARTABAN_URL      "http://www.studio-artaban.com"
#define WATCHOUT_TITLE          "Watch Out !!!"
#define WATCHOUT_PICTURE        "/Images/WatchOut.png"
#define BEST_SCORE_CAPTION      "My new best score is now: "
#define BEST_SCORE_TWITTER      "My new best score on #WatchOut is now: #"
#define BEST_SCORE_DESC         " ...do better!"

#ifndef __ANDROID__
#define GOOGLE_REQ_TIMEOUT      (7 * 60) // 7 seconds
#endif

#define NETWORKS_TEX_WIDTH      512.f
#define NETWORKS_TEX_HEIGHT     128.f

// Networks coordinates
#define FACEBOOK_PIC_X0         64
#define FACEBOOK_PIC_X2         192
#define FACEBOOK_PIC_Y0         0
#define FACEBOOK_PIC_Y2         128

#define TWITTER_PIC_X0          192
#define TWITTER_PIC_X2          320
#define TWITTER_PIC_Y0          0
#define TWITTER_PIC_Y2          128

#define GOOGLE_PIC_X0           320
#define GOOGLE_PIC_X2           448
#define GOOGLE_PIC_Y0           0
#define GOOGLE_PIC_Y2           128

// Texture IDs
#define TEXTURE_ID_RING         21
#define TEXTURE_ID_NETWORKS     25

static const wchar_t* g_Month[12] = { L"Jan", L"Feb", L"Mar", L"Apr", L"May", L"Jun", L"Jul", L"Aug", L"Sep",
        L"Oct", L"Nov", L"Dec" };

//////
Share::Share() : mStatus(ALL_HIDDEN), mPicture(false), mNetworkID(Network::NONE), mURL(NULL), mScore(0),
        mTwitterFactor(0.f), mTwitterConst(0.f), mTwitterTop(0.f), mGoogleX(0), mGoogleY(0), mCloseFactor(0.f),
        mCloseConst(0.f), mCloseBottom(0.f), mLogoutX(0), mLogoutY(0), mShareFactor(0), mShareConst(0),
        mShareBottom(0) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    mLog = NULL;
#endif
    std::memset(&mFacebookArea, 0, sizeof(TouchArea));

    std::memset(&mShareArea, 0, sizeof(TouchArea));
    std::memset(&mCloseArea, 0, sizeof(TouchArea));

    mSocial = Social::getInstance(true);
    mInternet = Internet::getInstance();

    mDispPic[Network::FACEBOOK] = false;
    mDispPic[Network::TWITTER] = false;
    mDispPic[Network::GOOGLE] = false;
}
Share::~Share() {

    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    Social::freeInstance();
    Internet::freeInstance();

    if (mURL)
        delete mURL;
}

void Share::start(const Game* game, float looseY) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(" - g:%x; l:%f"), __PRETTY_FUNCTION__, __LINE__, game, looseY);
    mLog = game->getLog();
#endif
    mPanel.initialize(game2DVia(game));
    mNetworks.initialize(game2DVia(game));
    mShareBack.initialize(game2DVia(game));
    mShareText.initialize(game2DVia(game));
    mCloseBack.initialize(game2DVia(game));
    mCloseText.initialize(game2DVia(game));
    mLogBack.initialize(game2DVia(game));
    mLogoutText.initialize(game2DVia(game));
    mPicture.initialize(game2DVia(game));
    mNetworkPic.initialize(game2DVia(game));
    mFrame.initialize(game2DVia(game));
    mUserBack.initialize(game2DVia(game));
    mUserText.initialize(game2DVia(game));
    mInfo.initialize(game2DVia(game));
    mGender.initialize(game2DVia(game));
    mBirthday.initialize(game2DVia(game));

    //
    mPanel.start(0x00, 0x00, 0x00);
    mPanel.setTexCoords(FULL_TEXCOORD_BUFFER);

    mShareArea.left = 0;
    mShareArea.top = ((looseY - 1.f) - game->getScreen()->top) * (game->getScreen()->height / (-2.f *
            game->getScreen()->top));
    mShareArea.right = game->getScreen()->width >> 1;
    mShareArea.bottom = mShareArea.top + (TRIANGLE_HEIGHT * mShareArea.right);

    mShareText.start(L"Share");
    mShareText.setColor(0.f, 0.f, 0.f);

    float fontScale = game->getScreen()->width / FONT_SCALE_RATIO;
    mShareText.scale(fontScale, fontScale);
    mShareText.position(((FONT_WIDTH << 1) / static_cast<float>(game->getScreen()->width)) * fontScale * -5.f, looseY -
            1.f - TRIANGLE_HEIGHT + ((FONT_HEIGHT << 1) / static_cast<float>(game->getScreen()->width)) * fontScale);

    mNetworks.start(TEXTURE_ID_NETWORKS);
    mNetworks.setTexCoords(FULL_TEXCOORD_BUFFER);

    float midY = looseY - ((1.f + TRIANGLE_HEIGHT) / 2.f);
    float coords[8];
    coords[0] = -1.f;
    coords[1] = midY + 0.5f;
    coords[2] = -1.f;
    coords[3] = coords[1] - 0.5f;
    coords[4] = 1.f;
    coords[5] = coords[3];
    coords[6] = 1.f;
    coords[7] = coords[1];
    mNetworks.setVertices(coords);

    mCloseText.start(L"Close");
    mCloseText.scale(fontScale, fontScale);
    mCloseText.position(1.f - ((FONT_WIDTH << 1) / static_cast<float>(game->getScreen()->width)) * fontScale * 5.f,
            midY - 0.5f + ((FONT_HEIGHT << 1) / static_cast<float>(game->getScreen()->width)) * fontScale);

    mCloseArea.left = (game->getScreen()->width >> 1) + (game->getScreen()->width >> 2);
    mCloseArea.top = (midY - game->getScreen()->top) * (game->getScreen()->height / (-2.f * game->getScreen()->top));
    mCloseArea.right = game->getScreen()->width;
    mCloseArea.bottom = mCloseArea.top + (game->getScreen()->width >> 2);

    mFacebookArea.left = game->getScreen()->width >> 3;
    mFacebookArea.top = ((midY + 0.5f) - game->getScreen()->top) * (game->getScreen()->height /
            (-2.f * game->getScreen()->top));
    mFacebookArea.right = mFacebookArea.left + (game->getScreen()->width >> 2);
    mFacebookArea.bottom = mFacebookArea.top + (game->getScreen()->width >> 2);

    mTwitterConst = midY;
    mTwitterFactor = (midY - 0.5f) / 0.25f;
    mTwitterTop = midY + 0.5f;

    mGoogleX = (game->getScreen()->width >> 1) + (game->getScreen()->width >> 2);
    mGoogleY = ((midY + 0.25f) - game->getScreen()->top) *
            (game->getScreen()->height / (-2.f * game->getScreen()->top));

    mFrame.start(TEXTURE_ID_RING);
    mFrame.setTexCoords(FULL_TEXCOORD_BUFFER);

    mFrame.setRed(0x00 / MAX_COLOR);
    mFrame.setGreen(0x00 / MAX_COLOR);
    mFrame.setBlue(0x00 / MAX_COLOR);

    //coords[0] = -1.f;
    coords[1] = midY + (((2.f / 3.f) + 1.f) / 2.f);
    //coords[2] = -1.f;
    coords[3] = coords[1] - (2.f / 3.f);
    coords[4] = (2.f / 3.f) - 1.f;
    coords[5] = coords[3];
    coords[6] = coords[4];
    coords[7] = coords[1];
    mFrame.setVertices(coords);

    // Initialize 'mPicture' using Facebook network as default
    mPicture.start(Textures::getInstance()->getIndex(Facebook::TEXTURE_ID));
    mPicture.setTexCoords(FULL_TEXCOORD_BUFFER);

    float ringThick = (RING_THICKNESS * (2.f / 3.f)) / RING_TEXTURE_SIZE;
    coords[0] += ringThick;
    coords[1] -= ringThick;
    coords[2] = coords[0];
    coords[3] += ringThick;
    coords[4] -= ringThick;
    coords[5] = coords[3];
    coords[6] = coords[4];
    coords[7] = coords[1];
    mPicture.setVertices(coords);

    mUserBack.start(game);

    coords[0] = g_TriangleVertices[0] + (2.f / 3.f) / 2.f;
    coords[1] = mPicture.getTop();
    coords[2] = (2.f / 3.f) / 2.f;
    coords[3] = coords[1] - TRIANGLE_HEIGHT;
    coords[4] = g_TriangleVertices[4] + (2.f / 3.f) / 2.f;
    coords[5] = coords[1];
    mUserBack.setVertices(coords);

    mUserText.start(L"User");
    mUserText.setColor(1.f, 0.f, 0.f);
    mUserText.scale(fontScale, fontScale);
    mUserText.position(coords[0] + ((FONT_WIDTH << 1) / static_cast<float>(game->getScreen()->width)) * fontScale,
            coords[1]);

    mNetworkPic.start(TEXTURE_ID_NETWORKS);
    mNetworkID = Network::FACEBOOK; // Using Facebook network as default
    setNetwork();
    mNetworkID = Network::NONE;

    coords[0] = 0.75f;
    coords[1] = midY - (((2.f / 3.f) + 1.f) / 2.f) + 0.5f + 0.25f;
    coords[2] = 0.75f;
    coords[3] = coords[1] - 0.25f;
    coords[4] = 1.f;
    coords[5] = coords[3];
    coords[6] = 1.f;
    coords[7] = coords[1];
    mNetworkPic.setVertices(coords);

    mShareBack.start(game);

    mShareBack.setRed(0xff / MAX_COLOR);
    mShareBack.setGreen(0xff / MAX_COLOR);
    //mShareBack.setBlue(0x00 / MAX_COLOR);

    mShareBack.translate(g_TriangleVertices[4] - 1.f, midY - (((2.f / 3.f) + 1.f) / 2.f) + 0.5f - g_TriangleVertices[1]);

    mShareConst = g_TriangleVertices[3] + mShareBack.getTransform()[Dynamic2D::TRANS_Y];
    mShareFactor = (g_TriangleVertices[1] - g_TriangleVertices[3]) / g_TriangleVertices[0];
    mShareBottom = midY - (((2.f / 3.f) + 1.f) / 2.f) + 0.5f;

    mCloseBack.start(game);

    float scale = 0.5f / TRIANGLE_HEIGHT;
    mCloseBack.scale(scale, scale);
    mCloseBack.translate(1.f - ((UNIT_TRIANGLE_WIDTH * scale) / 2.f), midY - (((2.f / 3.f) + 1.f) / 2.f) -
            (g_TriangleVertices[1] * scale));

    mCloseConst = (g_TriangleVertices[3] * scale) + mCloseBack.getTransform()[Dynamic2D::TRANS_Y];
    mCloseFactor = ((g_TriangleVertices[1] - g_TriangleVertices[3]) / g_TriangleVertices[0]) * scale;
    mCloseBottom = midY - (((2.f / 3.f) + 1.f) / 2.f);

    mLogBack.start(TEXTURE_ID_DISK);
    mLogBack.setTexCoords(FULL_TEXCOORD_BUFFER);

    //mLogBack.setRed(0xff / MAX_COLOR);
    mLogBack.setGreen(0x00 / MAX_COLOR);
    mLogBack.setBlue(0x00 / MAX_COLOR);

    coords[0] = -LOGOUT_RADIUS;
    coords[1] = midY - (((2.f / 3.f) + 1.f) / 2.f) + 0.5f;
    coords[2] = -LOGOUT_RADIUS;
    coords[3] = coords[1] - 0.5f;
    coords[4] = LOGOUT_RADIUS;
    coords[5] = coords[3];
    coords[6] = LOGOUT_RADIUS;
    coords[7] = coords[1];
    mLogBack.setVertices(coords);

    mLogoutX = game->getScreen()->width >> 1;
    mLogoutY = ((coords[1] - LOGOUT_RADIUS) - game->getScreen()->top) * (game->getScreen()->height /
            (-2.f * game->getScreen()->top));

    mLogoutText.start(L"Logout");
    mLogoutText.setColor(0.f, 0.f, 0.f);
    mLogoutText.scale(fontScale * 0.8f, fontScale * 0.8f);
    mLogoutText.position(mLogBack.getLeft(), mLogBack.getTop() - 0.25f + ((((FONT_HEIGHT << 1) /
            static_cast<float>(game->getScreen()->width)) * (fontScale * 0.8f)) / 2.f));

    mInfo.start(L"Birthday:\nGender:\nExternal profil URL:");
    mInfo.setColor(1.f, 1.f, 0.f);
    mInfo.scale(fontScale * PROFIL_INFO_SCALE, fontScale * PROFIL_INFO_SCALE);
    mInfo.position(PROFIL_INFO_LAG - (1.f / 3.f) - ringThick, midY + (((2.f / 3.f) + 1.f) / 2.f) - ((2.f / 3.f) / 2.f) -
            PROFIL_INFO_LAG);

    mBirthday.start(EMPTY_BIRTHDAY);
    mBirthday.scale(fontScale * PROFIL_INFO_SCALE, fontScale * PROFIL_INFO_SCALE);
    mBirthday.position(PROFIL_INFO_LAG - (1.f / 3.f) - ringThick + (((FONT_WIDTH << 1) /
            static_cast<float>(game->getScreen()->width)) * fontScale * PROFIL_INFO_SCALE * 9.f), midY + (((2.f / 3.f) +
            1.f) / 2.f) - ((2.f / 3.f) / 2.f) - PROFIL_INFO_LAG);
    // "Birthday:" contains 9.f lettres

    mGender.start(L","); // Will be replace by '_' character (see font texture)
    mGender.scale(fontScale * PROFIL_INFO_SCALE, fontScale * PROFIL_INFO_SCALE);
    mGender.position(PROFIL_INFO_LAG - (1.f / 3.f) - ringThick + (((FONT_WIDTH << 1) /
            static_cast<float>(game->getScreen()->width)) * fontScale * PROFIL_INFO_SCALE * 7.f), midY + (((2.f / 3.f) +
            1.f) / 2.f) - ((2.f / 3.f) / 2.f) - PROFIL_INFO_LAG - (((FONT_HEIGHT << 1) /
            static_cast<float>(game->getScreen()->width)) * fontScale * PROFIL_INFO_SCALE));
    // "Gender:" contains 7.f lettres
}
void Share::resume() {

    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    if (mStatus < NETWORKS_DISPLAYING)
        mPanel.resume(0xff, 0x00, 0x00);
    else
        mPanel.resume(0x00, 0x00, 0x00);
    mNetworks.resume(TEXTURE_ID_NETWORKS);

    mInfo.resume();
    mUserBack.resume();
    mUserText.resume();
    mShareBack.resume();
    mShareText.resume();
    mCloseBack.resume();
    mCloseText.resume();
    mLogBack.resume(TEXTURE_ID_DISK);
    mLogoutText.resume();

    mShareBack.setRed(0xff / MAX_COLOR);
    mShareBack.setGreen(0xff / MAX_COLOR);
    //mShareBack.setBlue(0x00 / MAX_COLOR);

    switch (mNetworkID) {

        case Network::NONE:
        case Network::FACEBOOK:
            mPicture.resume(Textures::getInstance()->getIndex(Facebook::TEXTURE_ID));
            break;

        case Network::TWITTER: mPicture.resume(Textures::getInstance()->getIndex(Twitter::TEXTURE_ID)); break;
        case Network::GOOGLE: mPicture.resume(Textures::getInstance()->getIndex(Google::TEXTURE_ID)); break;
    }
    if ((mNetworkID != Network::NONE) && (mSocial->_getSession(mNetworkID)) &&
            (mSocial->_getSession(mNetworkID)->getUserPic()) && (mDispPic[mNetworkID]))
        setPicture();

    mFrame.resume(TEXTURE_ID_RING);
    mNetworkPic.resume(TEXTURE_ID_NETWORKS);
    mGender.resume();
    mBirthday.resume();
    if (mURL)
        mURL->resume();
}

void Share::setNetwork() {

    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    assert(mNetworkID != Network::NONE);

#ifdef DEBUG
    mPicture.pause(); // Reset texture index to avoid assert when 'resume' method is called
#endif
    switch (mNetworkID) {

        case Network::FACEBOOK: {
            mPicture.resume(Textures::getInstance()->getIndex(Facebook::TEXTURE_ID));
            mPicture.setTexCoords(FULL_TEXCOORD_BUFFER);
            static const float texCoords[8] = { FACEBOOK_PIC_X0 / NETWORKS_TEX_WIDTH,
                    FACEBOOK_PIC_Y0 / NETWORKS_TEX_HEIGHT, FACEBOOK_PIC_X0 / NETWORKS_TEX_WIDTH,
                    FACEBOOK_PIC_Y2 / NETWORKS_TEX_HEIGHT, FACEBOOK_PIC_X2 / NETWORKS_TEX_WIDTH,
                    FACEBOOK_PIC_Y2 / NETWORKS_TEX_HEIGHT, FACEBOOK_PIC_X2 / NETWORKS_TEX_WIDTH,
                    FACEBOOK_PIC_Y0 / NETWORKS_TEX_HEIGHT };
            mNetworkPic.setTexCoords(texCoords);
            break;
        }
        case Network::TWITTER: {
            mPicture.resume(Textures::getInstance()->getIndex(Twitter::TEXTURE_ID));
            mPicture.setTexCoords(FULL_TEXCOORD_BUFFER);
            static const float texCoords[8] = { TWITTER_PIC_X0 / NETWORKS_TEX_WIDTH,
                    TWITTER_PIC_Y0 / NETWORKS_TEX_HEIGHT, TWITTER_PIC_X0 / NETWORKS_TEX_WIDTH,
                    TWITTER_PIC_Y2 / NETWORKS_TEX_HEIGHT, TWITTER_PIC_X2 / NETWORKS_TEX_WIDTH,
                    TWITTER_PIC_Y2 / NETWORKS_TEX_HEIGHT, TWITTER_PIC_X2 / NETWORKS_TEX_WIDTH,
                    TWITTER_PIC_Y0 / NETWORKS_TEX_HEIGHT };
            mNetworkPic.setTexCoords(texCoords);
            break;
        }
        case Network::GOOGLE: {
            mPicture.resume(Textures::getInstance()->getIndex(Google::TEXTURE_ID));
            mPicture.setTexCoords(FULL_TEXCOORD_BUFFER);
            static const float texCoords[8] = { GOOGLE_PIC_X0 / NETWORKS_TEX_WIDTH,
                    GOOGLE_PIC_Y0 / NETWORKS_TEX_HEIGHT, GOOGLE_PIC_X0 / NETWORKS_TEX_WIDTH,
                    GOOGLE_PIC_Y2 / NETWORKS_TEX_HEIGHT, GOOGLE_PIC_X2 / NETWORKS_TEX_WIDTH,
                    GOOGLE_PIC_Y2 / NETWORKS_TEX_HEIGHT, GOOGLE_PIC_X2 / NETWORKS_TEX_WIDTH,
                    GOOGLE_PIC_Y0 / NETWORKS_TEX_HEIGHT };
            mNetworkPic.setTexCoords(texCoords);
            break;
        }
    }
}
void Share::setPicture() {

    assert(mNetworkID != Network::NONE);

    Textures* textures = Textures::getInstance();
    unsigned char textureIdx = textures->addTexPic(mNetworkID);
    if (textureIdx != TEXTURE_IDX_INVALID) {

        float texCoords[8] = {0};
        texCoords[3] = static_cast<float>(mSocial->_getSession(mNetworkID)->getPicHeight()) /
                (*textures)[textureIdx]->height;
        texCoords[4] = static_cast<float>(mSocial->_getSession(mNetworkID)->getPicWidth()) /
                (*textures)[textureIdx]->width;
        texCoords[5] = texCoords[3];
        texCoords[6] = texCoords[4];
        mPicture.setTexCoords(texCoords);

        mDispPic[mNetworkID] = true;
    }
    //else // Picture buffer has changed (== NULL)
}
bool Share::select() {

    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    if (!mSocial->_getSession(mNetworkID))
        mSocial->addSession(mNetworkID, false);

    assert(mSocial->_getSession(mNetworkID));
    if (!mSocial->_getSession(mNetworkID)->isOpened())
        return mSocial->request(mNetworkID, Session::REQUEST_LOGIN, NULL);

    else {

        bool idEmpty = false;
        switch (mNetworkID) {

            case Network::FACEBOOK: idEmpty = mFacebookURL.empty(); break;
            case Network::TWITTER: idEmpty = mTwitterURL.empty(); break;
            case Network::GOOGLE: idEmpty = mGoogleURL.empty(); break;
        }
        if (idEmpty)
            return mSocial->request(mNetworkID, Session::REQUEST_INFO, NULL);

        mStatus = SHARE_DISPLAYING; // Display share dialog
    }
    return false;
}
void Share::displayInfo(const Game* game, float looseY) {

    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(" - g:%x"), __PRETTY_FUNCTION__, __LINE__, game);
    assert(mNetworkID != Network::NONE);
    assert(mSocial->_getSession(mNetworkID));
    assert(!mURL);

    // Gender
    switch (mSocial->_getSession(mNetworkID)->getUserGender()) {
        case GENDER_MALE: if (mGender.getText() != L"M") mGender.update(L"M"); break;
        case GENDER_FEMALE: if (mGender.getText() != L"F") mGender.update(L"F"); break;
    }
    mGender.setAlpha(1.f);

    // Birthday
    std::string userBirthday(mSocial->_getSession(mNetworkID)->getUserBirthday());
    if (userBirthday.length() > 9) { // MM/dd/yyyy

        std::wstring birthday(g_Month[strToNum<short>(userBirthday.substr(0, 2)) - 1]);
        birthday += L' ';
        birthday.append(numToWStr<unsigned char>(strToNum<short>(userBirthday.substr(3, 2))));

        if (mBirthday.getText() != birthday)
            mBirthday.update(birthday);
    }
    else if (mBirthday.getText() != EMPTY_BIRTHDAY)
        mBirthday.update(EMPTY_BIRTHDAY);
    mBirthday.setAlpha(1.f);

    // URL
    std::wstring url;
    switch (mNetworkID) {

        case Network::FACEBOOK: url.assign(mFacebookURL.begin(), mFacebookURL.end()); break;
        case Network::TWITTER: url.assign(mTwitterURL.begin(), mTwitterURL.end()); break;
        case Network::GOOGLE: url.assign(mGoogleURL.begin(), mGoogleURL.end()); break;
    }
    std::replace(url.begin(), url.end(), L'*', L'à');
    std::replace(url.begin(), url.end(), L'#', L'â');
    std::replace(url.begin(), url.end(), L'^', L'€');
    std::replace(url.begin(), url.end(), L'}', L'>');
    std::replace(url.begin(), url.end(), L']', L'}');
    std::replace(url.begin(), url.end(), L'{', L'<');
    std::replace(url.begin(), url.end(), L'[', L'{');
    std::replace(url.begin(), url.end(), L'~', L'\'');
    std::replace(url.begin(), url.end(), L'§', L'"');
    std::replace(url.begin(), url.end(), L'_', L',');

#ifdef DEBUG
    if (url.length() > 255) {
        LOGE(LOG_FORMAT(" - External profile URL too long"), __PRETTY_FUNCTION__, __LINE__);
        assert(NULL);
    }
#endif
    for (unsigned char i = (static_cast<unsigned char>(url.length()) / 20); i; --i)
        url.insert(20 * i, 1, '\n'); // "External profil URL:" contains 20 letters

    mURL = new Text2D;
    mURL->initialize(game2DVia(game));
    mURL->start(url);

    float scale = (game->getScreen()->width / FONT_SCALE_RATIO) * PROFIL_INFO_SCALE;
    mURL->scale(scale, scale);
    mURL->position(PROFIL_INFO_LAG - (1.f / 3.f) - ((RING_THICKNESS * (2.f / 3.f)) / RING_TEXTURE_SIZE), (looseY -
            ((1.f + TRIANGLE_HEIGHT) / 2.f)) + (((2.f / 3.f) + 1.f) / 2.f) - ((2.f / 3.f) / 2.f) - PROFIL_INFO_LAG -
            (((FONT_HEIGHT << 1) / static_cast<float>(game->getScreen()->width)) * scale * 3.f));
}

std::string Share::getUserURL() const {

    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    assert(mSocial->_getSession(mNetworkID));

    std::string user(WATCHOUT_PHP);
    user.append(encodeURL(encodeB64(numToHex<int>(static_cast<int>(mNetworkID)) +
            mSocial->_getSession(mNetworkID)->getUserID())));
    return user;
}
std::string Share::getRequestURL() const {

    LOGV(LOG_LEVEL_SHARE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    assert(mSocial->_getSession(mNetworkID));
    assert(mScore > 0);

    int score = static_cast<int>(mScore) * 10;
    switch (mSocial->_getSession(mNetworkID)->getUserGender()) {

        case GENDER_NONE:
        case GENDER_MALE: score += 1; //break;
        case GENDER_FEMALE: break;
        //case GENDER_NONE: {
        //
        //    LOGW(LOG_FORMAT(" - Failed to get user gender"), __PRETTY_FUNCTION__, __LINE__);
        //    assert(NULL);
        //    break;
        //}
    }
    score = 32766 - score;

    time_t now = time(NULL);
    struct tm* date = gmtime(&now);
    if (((date->tm_min + date->tm_hour) % ((date->tm_sec)? date->tm_sec:1)) & 0x01)
        score *= (date->tm_sec + date->tm_hour)? (date->tm_sec + date->tm_hour):1;
    else
        score *= (date->tm_min + date->tm_hour)? (date->tm_min + date->tm_hour):1;

    std::string url(getUserURL()); // user

    std::string data(numToHex<int>(std::rand()));
    data += '_';
    data.append(numToHex<int>(score));
    data += '_';
    std::string userID(mSocial->_getSession(mNetworkID)->getUserID());
    int userCheckSum = 0;
    for (unsigned char i = 0; i < static_cast<unsigned char>(userID.length()); ++i)
        userCheckSum += static_cast<unsigned char>(userID.at(i));
    userCheckSum *= 32766;
    if ((date->tm_min + date->tm_hour) > date->tm_sec)
        userCheckSum += 32;
    else
        userCheckSum -= 23;
    userCheckSum /= (date->tm_sec)? date->tm_sec:1;
    data.append(numToHex<int>(userCheckSum));
    data += '_';
    unsigned char randChar = static_cast<unsigned char>(std::rand() % 16); // 16 == 0xF
    data.append(numToHex<int>((randChar)? randChar:1));
    if (date->tm_sec < 16) data += '0';
    data.append(numToHex<int>(date->tm_sec));
    data.append(numToHex<int>(static_cast<unsigned char>(std::rand() % 16)));
    if (date->tm_hour < 16) data += '0';
    data.append(numToHex<int>(date->tm_hour));
    data.append(numToHex<int>(static_cast<unsigned char>(std::rand() % 16)));
    if (date->tm_min < 16) data += '0';
    data.append(numToHex<int>(date->tm_min));
    data.append(numToHex<int>(static_cast<unsigned char>(std::rand() % 16)));
    url.append(URL_PHP_DATA);
    url.append(encodeURL(encodeB64(data))); // data

    url.append(URL_PHP_NAME); // name
    std::wstring userName(mSocial->_getSession(mNetworkID)->getUserName());
    std::string name;
    for (std::wstring::const_iterator iter = userName.begin(); iter != userName.end(); ++iter) {
        name += '%';
        name.append(numToHex<unsigned int>(static_cast<unsigned int>(*iter)));
        if (name.length() > (DB_NAME_SIZE - 6)) {
            name += '.'; // ...
            break;
        }
    }
    if (name.empty())
        name += '.'; // ...
    url.append(encodeURL(encodeB64(name)));
    return url;
}

bool Share::update(const Game* game, float looseY) {

#ifdef DEBUG
    LOGV(LOG_LEVEL_SHARE, (*mLog % 100), LOG_FORMAT(" - g:%x; l:%f"), __PRETTY_FUNCTION__, __LINE__, game, looseY);
#endif
    static bool wait = false;
#ifndef __ANDROID__
    static short signInCount = 0;
#endif

    switch (mStatus) {
        case PANEL_DISPLAYING: {

            mPanel.setRed(0xff / MAX_COLOR);
            //mPanel.setGreen(0x00 / MAX_COLOR);
            //mPanel.setBlue(0x00 / MAX_COLOR);

            static const float verCoord[8] = { -1.f, looseY - 1.f, -1.f, looseY - 1.f - TRIANGLE_HEIGHT, 0.f,
                    looseY - 1.f - TRIANGLE_HEIGHT, 0.f, looseY - 1.f };
            mPanel.setVertices(verCoord);

            float fontScale = game->getScreen()->width / FONT_SCALE_RATIO;
            mShareText.position(((FONT_WIDTH << 1) / static_cast<float>(game->getScreen()->width)) * fontScale * -5.f,
                    looseY - 1.f - TRIANGLE_HEIGHT + ((FONT_HEIGHT << 1) / static_cast<float>(game->getScreen()->width)) *
                    fontScale);

            mStatus = PANEL_DISPLAYED;
            break;
        }
        case PANEL_DISPLAYED: {

            unsigned char touchCount = game->mTouchCount;
            while (touchCount--) {

                if (game->mTouchData[touchCount].Type != TouchInput::TOUCH_UP)
                    continue;

                // Check share
                if ((game->mTouchData[touchCount].X > mShareArea.left) &&
                        (game->mTouchData[touchCount].X < mShareArea.right) &&
                        (game->mTouchData[touchCount].Y > mShareArea.top) &&
                        (game->mTouchData[touchCount].Y < mShareArea.bottom)) {

                    mStatus = NETWORKS_DISPLAYING;
                    return true;
                }
            }
            break;
        }
        case NETWORKS_DISPLAYING: {

            mPanel.setRed(0x00 / MAX_COLOR);
            //mPanel.setGreen(0x00 / MAX_COLOR);
            //mPanel.setBlue(0x00 / MAX_COLOR);

            static const float verCoord[8] = { 0.5f, looseY - ((1.f + TRIANGLE_HEIGHT) / 2.f), 0.5f,
                    looseY - ((1.f + TRIANGLE_HEIGHT) / 2.f) - 0.5f, 1.f, looseY - ((1.f + TRIANGLE_HEIGHT) / 2.f) - 0.5f,
                    1.f, looseY - ((1.f + TRIANGLE_HEIGHT) / 2.f) };
            mPanel.setVertices(verCoord);

            float fontScale = game->getScreen()->width / FONT_SCALE_RATIO;
            mCloseText.position(1.f - ((FONT_WIDTH << 1) / static_cast<float>(game->getScreen()->width)) * fontScale * 5.f,
                    looseY - ((1.f + TRIANGLE_HEIGHT) / 2.f) - 0.5f + ((FONT_HEIGHT << 1) /
                    static_cast<float>(game->getScreen()->width)) * fontScale);

            if (mURL) {

                delete mURL;
                mURL = NULL;
            }
            mStatus = NETWORKS_DISPLAYED;
            break;
        }
        case NETWORKS_DISPLAYED: {

            if (wait) {

                switch (mSocial->_getSession(mNetworkID)->getResult()) {
                    case Request::RESULT_SUCCEEDED: {

                        LOGI(LOG_LEVEL_SHARE, 0, LOG_FORMAT(" - RESULT_SUCCEEDED"), __PRETTY_FUNCTION__, __LINE__);
                        switch (mSocial->_getSession(mNetworkID)->getRequest()) {

                            case Session::REQUEST_LOGIN: {
#ifndef __ANDROID__
                                signInCount = 0;
#endif
                                wait = mSocial->request(mNetworkID, Session::REQUEST_INFO, NULL);
                                if (!wait) mSocial->_getSession(mNetworkID)->close();
                                break;
                            }
                            case Session::REQUEST_INFO: {
                                switch (mNetworkID) {
                                    case Network::FACEBOOK: {

                                        mFacebookURL = LIBENG_FACEBOOK_URL;
                                        mFacebookURL.append(mSocial->_getSession(Network::FACEBOOK)->getUserID());
                                        break;
                                    }
                                    case Network::TWITTER: {

                                        mTwitterURL = LIBENG_TWITTER_URL;
                                        mTwitterURL.append(mSocial->_getSession(Network::TWITTER)->getUserID());
                                        break;
                                    }
                                    case Network::GOOGLE: {

                                        mGoogleURL = LIBENG_GOOGLE_URL;
                                        mGoogleURL.append(mSocial->_getSession(Network::GOOGLE)->getUserID());
                                        break;
                                    }
                                }
                                mStatus = SHARE_DISPLAYING; // Display share dialog
                                //break;
                            }
                            case Session::REQUEST_SHARE_LINK: {
                                wait = false;
                                break;
                            }
                        }
                        break;
                    }
                    case Request::RESULT_EXPIRED: {

                        LOGI(LOG_LEVEL_SHARE, 0, LOG_FORMAT(" - RESULT_EXPIRED"), __PRETTY_FUNCTION__, __LINE__);
                        mSocial->_getSession(mNetworkID)->close();
                        mDispPic[mNetworkID] = false;
                        switch (mNetworkID) {
                                
                            case Network::FACEBOOK: mFacebookURL.clear(); break;
                            case Network::TWITTER: mTwitterURL.clear(); break;
                            case Network::GOOGLE: mGoogleURL.clear(); break;
                        }
                        wait = mSocial->request(mNetworkID, Session::REQUEST_LOGIN, NULL);
                        break;
                    }
                    case Request::RESULT_WAITING: { // Wait

                        // Time out control for Goolge+ login request on iOS only coz there is no way to catch
                        // a cancel event when no Google+ application is installed (login managed in Safari)
                        // and user goes back to this application without logged
#ifndef __ANDROID__
                        if ((mNetworkID == Network::GOOGLE) &&
                            ((mSocial->_getSession(mNetworkID)->getRequest() == Session::REQUEST_LOGIN) ||
                             (mSocial->_getSession(mNetworkID)->getRequest() == Session::REQUEST_SHARE_LINK)) &&
                            (++signInCount > GOOGLE_REQ_TIMEOUT)) {

                            signInCount = 0;
                            mNetworkID = Network::NONE;
                            mStatus = ALL_HIDDEN;
                            wait = false;
                            return true;
                        }
#endif
                        break;
                    }
                    default: {
                    //case Request::RESULT_NONE: // Request sent error (!wait)
                    //case Request::RESULT_CANCELED:
                    //case Request::RESULT_FAILED: {

                        LOGI(LOG_LEVEL_SHARE, 0, LOG_FORMAT(" - RESULT_CANCELED/FAILED"), __PRETTY_FUNCTION__, __LINE__);
                        if ((mSocial->_getSession(mNetworkID)->getRequest() == Session::REQUEST_INFO) &&
                            (mSocial->_getSession(mNetworkID)->getResult() == Request::RESULT_FAILED))
                            mSocial->_getSession(mNetworkID)->close();

                        mNetworkID = Network::NONE;
                        wait = false;
#ifndef __ANDROID__
                        signInCount = 0;
#endif
                        break;
                    }
                }
                break; // Stop touch management
            }

            // Touch...
            unsigned char touchCount = game->mTouchCount;
            while (touchCount--) {

                if (game->mTouchData[touchCount].Type != TouchInput::TOUCH_UP)
                    continue;

                // Check close
                if ((game->mTouchData[touchCount].X > mCloseArea.left) &&
                        (game->mTouchData[touchCount].X < mCloseArea.right) &&
                        (game->mTouchData[touchCount].Y > mCloseArea.top) &&
                        (game->mTouchData[touchCount].Y < mCloseArea.bottom)) {

                    mNetworkID = Network::NONE;
                    mStatus = ALL_HIDDEN;
                    return true;
                }

                // Check Facebook
                if ((game->mTouchData[touchCount].X > mFacebookArea.left) &&
                        (game->mTouchData[touchCount].X < mFacebookArea.right) &&
                        (game->mTouchData[touchCount].Y > mFacebookArea.top) &&
                        (game->mTouchData[touchCount].Y < mFacebookArea.bottom)) {

                    LOGI(LOG_LEVEL_SHARE, 0, LOG_FORMAT(" - Facebook"), __PRETTY_FUNCTION__, __LINE__);
                    mNetworkID = Network::FACEBOOK;
                    wait = select();
                    break;
                }

                // Check Twitter
                float y = (game->getScreen()->height - (game->mTouchData[touchCount].Y << 1)) /
                        static_cast<float>(game->getScreen()->width);
                float x = ((game->mTouchData[touchCount].X << 1) / static_cast<float>(game->getScreen()->width)) - 1.f;
                // ...in unit

                if ((y < mTwitterTop) && (y > ((mTwitterFactor * x) + mTwitterConst)) &&
                        (y > (mTwitterConst - (mTwitterFactor * x)))) {

                    LOGI(LOG_LEVEL_SHARE, 0, LOG_FORMAT(" - Twitter"), __PRETTY_FUNCTION__, __LINE__);
                    mNetworkID = Network::TWITTER;
                    wait = select();
                    break;
                }

                // Check Google+
#ifdef __ANDROID__
                if (boost::math::tr1::hypotf(mGoogleX - game->mTouchData[touchCount].X,
                        mGoogleY - game->mTouchData[touchCount].Y) < static_cast<float>(game->getScreen()->width >> 3)) {
#else
                if (hypotf(mGoogleX - game->mTouchData[touchCount].X,
                        mGoogleY - game->mTouchData[touchCount].Y) < static_cast<float>(game->getScreen()->width >> 3)) {
#endif
                    LOGI(LOG_LEVEL_SHARE, 0, LOG_FORMAT(" - Google+"), __PRETTY_FUNCTION__, __LINE__);
                    mNetworkID = Network::GOOGLE;
                    wait = select();
                    break;
                }
            }
            break;
        }
        case SHARE_DISPLAYING: {

            float verCoord[8];
            verCoord[0] = ((RING_THICKNESS * (2.f / 3.f)) / -RING_TEXTURE_SIZE) - (1.f / 3.f);
            verCoord[1] = looseY - ((1.f + TRIANGLE_HEIGHT) / 2.f) + (((2.f / 3.f) + 1.f) / 2.f) - ((2.f / 3.f) / 2.f);
            verCoord[2] = verCoord[0];
            verCoord[3] = verCoord[1] - ((2.f / 3.f) / 2.f) - 0.5f;
            verCoord[4] = 1.f;
            verCoord[5] = verCoord[3];
            verCoord[6] = 1.f;
            verCoord[7] = verCoord[1];
            mPanel.setVertices(verCoord);

            float fontScale = game->getScreen()->width / FONT_SCALE_RATIO;
            float width = ((FONT_WIDTH << 1) / static_cast<float>(game->getScreen()->width)) * fontScale * 5.f;
            mCloseText.position(1.f - width - (((0.5f / TRIANGLE_HEIGHT) * (g_TriangleVertices[4] * 2.f)) - width) / 2.f,
                    looseY - ((1.f + TRIANGLE_HEIGHT) / 2.f) - (((2.f / 3.f) + 1.f) / 2.f) + ((FONT_HEIGHT << 1) /
                    static_cast<float>(game->getScreen()->width)) * fontScale);

            mShareText.position(((FONT_WIDTH << 1) / static_cast<float>(game->getScreen()->width)) * fontScale - 1.f,
                    looseY - ((1.f + TRIANGLE_HEIGHT) / 2.f) - (((2.f / 3.f) + 1.f) / 2.f) + 0.5f + ((FONT_HEIGHT << 1) /
                    static_cast<float>(game->getScreen()->width)) * fontScale);

            setNetwork();
#ifdef DEBUG
            LOGI(LOG_LEVEL_SHARE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
            assert(mSocial->_getSession(mNetworkID));
            switch (mNetworkID) {

                case Network::FACEBOOK: assert(!mFacebookURL.empty()); break;
                case Network::TWITTER: assert(!mTwitterURL.empty()); break;
                case Network::GOOGLE: assert(!mGoogleURL.empty()); break;
            }
#endif
            mSocial->setAdReqInfo(mNetworkID);

            mStatus = SHARE_DISPLAYED;
            if ((!mSocial->_getSession(mNetworkID)->getUserPic()) &&
                    ((mSocial->_getSession(mNetworkID)->getRequest() != Session::REQUEST_PICTURE) ||
                    (mSocial->_getSession(mNetworkID)->getResult() != Request::RESULT_WAITING)))
                mSocial->request(mNetworkID, Session::REQUEST_PICTURE, NULL);
            displayInfo(game, looseY);
            break;
        }
        case SHARE_DISPLAYED: {

            if (mSocial->_getSession(mNetworkID)->getUserPic()) //&& (!mDispPic[mNetworkID])) // ...for resume
                setPicture();

            static bool httpSent = false;
            if (httpSent) {
                if (!mInternet->waitHTTP()) {
                    if (mInternet->getHTTP().find("WOT-:)") != std::string::npos) {

                        ShareData* link = NULL;
                        switch (mNetworkID) {
                            case Network::FACEBOOK: {

                                link = new Facebook::LinkData;
                                static_cast<Facebook::LinkData*>(link)->name = WATCHOUT_TITLE;
                                static_cast<Facebook::LinkData*>(link)->caption = WATCHOUT_TITLE;
                                static_cast<Facebook::LinkData*>(link)->description = BEST_SCORE_CAPTION;
                                static_cast<Facebook::LinkData*>(link)->description.append(numToStr<short>(mScore));
                                static_cast<Facebook::LinkData*>(link)->description.append(BEST_SCORE_DESC);
                                static_cast<Facebook::LinkData*>(link)->link = STUDIO_ARTABAN_URL;
                                static_cast<Facebook::LinkData*>(link)->link.append(getUserURL());
                                static_cast<Facebook::LinkData*>(link)->picture = STUDIO_ARTABAN_URL;
                                static_cast<Facebook::LinkData*>(link)->picture.append(WATCHOUT_PICTURE);
                                break;
                            }
                            case Network::GOOGLE: {

                                link = new Google::LinkData;
                                static_cast<Google::LinkData*>(link)->url = STUDIO_ARTABAN_URL;
                                static_cast<Google::LinkData*>(link)->url.append(getUserURL());
                                break;
                            }
                            case Network::TWITTER:

                                link = new Twitter::LinkData;
                                static_cast<Twitter::LinkData*>(link)->tweet = BEST_SCORE_TWITTER;
                                static_cast<Twitter::LinkData*>(link)->tweet.append(numToStr<short>(mScore));
                                static_cast<Twitter::LinkData*>(link)->tweet.append(BEST_SCORE_DESC);
                                static_cast<Twitter::LinkData*>(link)->tweet += ' ';
                                static_cast<Twitter::LinkData*>(link)->tweet.append(STUDIO_ARTABAN_URL);
                                static_cast<Twitter::LinkData*>(link)->tweet.append(getUserURL());
                                break;
                        }
                        wait = mSocial->request(mNetworkID, Session::REQUEST_SHARE_LINK, link);
                        delete link;
                        mStatus = NETWORKS_DISPLAYING;
                    }
                    else {

                        LOGW(LOG_FORMAT(" - Failed to update score"), __PRETTY_FUNCTION__, __LINE__);
                        mSocial->request(mNetworkID, Session::REQUEST_SHARE_LINK, NULL); // Display alert message
                    }
                    httpSent = false;
                }
                break; // Stop touch management
            }

            unsigned char touchCount = game->mTouchCount;
            while (touchCount--) {

                if (game->mTouchData[touchCount].Type != TouchInput::TOUCH_UP)
                    continue;

                // Check close
                float y = (game->getScreen()->height - (game->mTouchData[touchCount].Y << 1)) /
                        static_cast<float>(game->getScreen()->width);
                float x = ((game->mTouchData[touchCount].X << 1) / static_cast<float>(game->getScreen()->width)) - 1.f -
                        mCloseBack.getTransform()[Dynamic2D::TRANS_X];
                // ...in unit

                if ((y > mCloseBottom) && (y < ((mCloseFactor * x) + mCloseConst)) &&
                        (y < (mCloseConst - (mCloseFactor * x)))) {

                    mNetworkID = Network::NONE;
                    mStatus = NETWORKS_DISPLAYING;
                    return false;
                }

                // Check logout
#ifdef __ANDROID__
                if (boost::math::tr1::hypotf(mLogoutX - game->mTouchData[touchCount].X,
                        mLogoutY - game->mTouchData[touchCount].Y) < static_cast<float>(game->getScreen()->width >> 3)) {
#else
                if (hypotf(mLogoutX - game->mTouchData[touchCount].X,
                        mLogoutY - game->mTouchData[touchCount].Y) < static_cast<float>(game->getScreen()->width >> 3)) {
#endif
                    LOGI(LOG_LEVEL_SHARE, 0, LOG_FORMAT(" - Logout"), __PRETTY_FUNCTION__, __LINE__);
                    mSocial->_getSession(mNetworkID)->close();
                    mDispPic[mNetworkID] = false;
                    switch (mNetworkID) {

                        case Network::FACEBOOK: mFacebookURL.clear(); break;
                        case Network::TWITTER: mTwitterURL.clear(); break;
                        case Network::GOOGLE: mGoogleURL.clear(); break;
                    }
                    mNetworkID = Network::NONE;
                    mStatus = NETWORKS_DISPLAYING;
                    break;
                }

                // Check share
                x = ((game->mTouchData[touchCount].X << 1) / static_cast<float>(game->getScreen()->width)) - 1.f -
                        mShareBack.getTransform()[Dynamic2D::TRANS_X];
                // ...in unit

                if ((y > mShareBottom) && (y < ((mShareFactor * x) + mShareConst)) &&
                        (y < (mShareConst - (mShareFactor * x)))) {

                    LOGI(LOG_LEVEL_SHARE, 0, LOG_FORMAT(" - Share"), __PRETTY_FUNCTION__, __LINE__);
                    if (!mInternet->sendHTTP(WATCHOUT_HOST, getRequestURL())) {

                        LOGW(LOG_FORMAT(" - Failed to send HTTP request"), __PRETTY_FUNCTION__, __LINE__);
                        mSocial->request(mNetworkID, Session::REQUEST_SHARE_LINK, NULL); // Display alert message
                        break;
                    }
                    httpSent = true;
                    break;
                }
            }
            break;
        }
    }
    return false;
}
void Share::render() const {

#ifdef DEBUG
    LOGV(LOG_LEVEL_SHARE, (*mLog % 100), LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
#endif
    switch (mStatus) {
        case PANEL_DISPLAYING:
        case PANEL_DISPLAYED: {

            glDisable(GL_BLEND);
            mPanel.render(false);
            glEnable(GL_BLEND);

            mShareText.render(true);
            break;
        }
        case NETWORKS_DISPLAYING:
        case NETWORKS_DISPLAYED: {

            glDisable(GL_BLEND);
            mPanel.render(false);
            glEnable(GL_BLEND);

            mCloseText.render(true);
            mNetworks.render(true);
            break;
        }
        case SHARE_DISPLAYING:
        case SHARE_DISPLAYED: {

            glDisable(GL_BLEND);
            mPicture.render(false);
            glEnable(GL_BLEND);

            mFrame.render(false);

            glDisable(GL_BLEND);
            mPanel.render(false);
            glEnable(GL_BLEND);

            mNetworkPic.render(false);

            mUserBack.render();
            mUserText.render(true);
            mInfo.render(true);
            mCloseBack.render();
            mCloseText.render(true);
            mShareBack.render();
            mShareText.render(true);
            mLogBack.render(true);
            mLogoutText.render(true);

            mGender.render(true);
            mBirthday.render(true);
            if (mURL)
                mURL->render(true);

            break;
        }
    }
}
