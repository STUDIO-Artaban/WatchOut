#ifndef WITCH_H_
#define WITCH_H_

#include "Global.h"

#include <libeng/Graphic/Object/2D/Static2D.h>
#include <libeng/Graphic/Object/2D/Dynamic2D.h>

#include <libeng/Graphic/Bounds/2D/Polygone2D.h>
#include <libeng/Graphic/Bounds/BorderBottom.h>
#include <libeng/Graphic/Bounds/2D/Overlap2D.h>
#include <libeng/Graphic/Bounds/Overlap.h>

#define ANIMS_COUNT             8

using namespace eng;

//////
class Witch : public Static2D, public Dynamic2D {

private:
    unsigned char mAnims[ANIMS_COUNT];

public:
    Witch();
    virtual ~Witch();

    unsigned char mAnimIdx;

    //////
    void load(unsigned char step);
    void update(const Game* game, const Level* level);

    inline bool checkCollision(const Bounds* bounds, unsigned char boundsId) { return false; }
    inline bool checkCollision(const Object* object, unsigned char objectId) { assert(NULL); return false; }

    void render(bool resetUniform) const;

};

#endif // WITCH_H_
