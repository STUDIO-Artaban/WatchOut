#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "Global.h"

#include <libeng/Game/2D/Game2D.h>
#include <libeng/Graphic/Object/2D/Object2D.h>
#include <libeng/Graphic/Object/2D/Dynamic2D.h>
#include <libeng/Graphic/Bounds/2D/Overlap2D.h>

// Bound IDs
#define BOUND_ID_SCREEN_LEFT        0
#define BOUND_ID_SCREEN_TOP         1
#define BOUND_ID_SCREEN_RIGHT       2
#define BOUND_ID_SCREEN_BOTTOM      3

// Object IDs
#define OBJECT_ID_CHIP              0

using namespace eng;

class Triangle : public Object2D, public Dynamic2D {

public:
    Triangle();
    virtual ~Triangle();

    inline void setVertices(const float coords[6]) {

        mVertices[0] = coords[0];
        mVertices[1] = coords[1];
        mVertices[2] = coords[2];
        mVertices[3] = coords[3];
        mVertices[4] = coords[4];
        mVertices[5] = coords[5];
    };

    //
    void start(const Game* game);
    void resume();

    inline void update(const Game* game, const Level* level) { assert(NULL); }
    inline bool checkCollision(const Bounds* bound, unsigned char boundId) {

#ifdef DEBUG
        LOGV(LOG_LEVEL_TRIANGLE, (*mLog % 100), LOG_FORMAT(" - b:%x; i:%d"), __PRETTY_FUNCTION__, __LINE__, bound, boundId);
#endif
        switch (boundId) {

            case BOUND_ID_SCREEN_LEFT: return trian2DVia(mBounds[0])->overlap<BorderLeft>(bLeftVia(bound));
            case BOUND_ID_SCREEN_TOP: return trian2DVia(mBounds[0])->overlap<BorderTop>(bTopVia(bound));
            case BOUND_ID_SCREEN_RIGHT: return trian2DVia(mBounds[0])->overlap<BorderRight>(bRightVia(bound));
            case BOUND_ID_SCREEN_BOTTOM: return trian2DVia(mBounds[0])->overlap<BorderBottom>(bBotVia(bound));
        }
        LOGE(LOG_FORMAT(" - Unknown bound ID %d"), __PRETTY_FUNCTION__, __LINE__, boundId);
        assert(NULL);
        return false;
    }
    inline bool checkCollision(const Object* object, unsigned char objectId) {

#ifdef DEBUG
        LOGV(LOG_LEVEL_TRIANGLE, (*mLog % 100), LOG_FORMAT(" - o:%x; i:%d"), __PRETTY_FUNCTION__, __LINE__, object, objectId);
#endif
        switch (objectId) {

        case OBJECT_ID_CHIP: return trian2DVia(mBounds[0])->overlap<Circle2D>(circ2DVia(static_cast<const Element2D*>(object)->getBounds(0)));
        }
        LOGE(LOG_FORMAT(" - Unknown object ID %d"), __PRETTY_FUNCTION__, __LINE__, objectId);
        assert(NULL);
        return false;
    }

    void render() const;

};

#endif // TRIANGLE_H_
