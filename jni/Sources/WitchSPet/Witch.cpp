#include "Witch.h"
#include <stdlib.h>

#define MAX_DELAY               6

// Texture IDs
#define TEXTURE_ID_WITCH0       11
//#define TEXTURE_ID_WITCH1      12
//#define TEXTURE_ID_WITCH2      13
//#define TEXTURE_ID_WITCH3      14
//#define TEXTURE_ID_WITCH4      15
//#define TEXTURE_ID_WITCH5      16
//#define TEXTURE_ID_WITCH6      17
//#define TEXTURE_ID_WITCH7      18

//////
Witch::Witch() : Static2D(false), Dynamic2D(), mAnimIdx(0) {

    LOGV(LOG_LEVEL_WITCH, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    std::memset(mAnims, 0, ANIMS_COUNT);

    mVertices[0] = -0.5f;
    mVertices[1] = 0.5f;
    mVertices[2] = -0.5f;
    mVertices[3] = -0.5f;
    mVertices[4] = 0.5f;
    mVertices[5] = -0.5f;
    mVertices[6] = 0.5f;
    mVertices[7] = 0.5f;

    mTransform = new float[TRANSFORM_BUFFER_SIZE];
    std::memcpy(mTransform, Object2D::TransformBuffer, sizeof(float) * TRANSFORM_BUFFER_SIZE);
}
Witch::~Witch() {

    LOGV(LOG_LEVEL_WITCH, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    for (unsigned char i = 0; i < ANIMS_COUNT; ++i)
        if (mAnims[i]) mTextures->delTexture(mAnims[i]);
}

void Witch::load(unsigned char step) {

    LOGV(LOG_LEVEL_WITCH, 0, LOG_FORMAT(" - s:%d"), __PRETTY_FUNCTION__, __LINE__, step);
    assert((step > -1) && (step < 8));

    mAnims[step] = Textures::loadTexture(TEXTURE_ID_WITCH0 + step);
    mTextures->genTexture(mAnims[step], false);
    if (!step)
        mTextureIdx = mAnims[0];
}

void Witch::update(const Game* game, const Level* level) {

    static unsigned char count = 0;
    if (++count == MAX_DELAY) {

        count = 0;
        if (++mAnimIdx == ANIMS_COUNT)
            mAnimIdx = 0;
        mTextureIdx = mAnims[mAnimIdx];
    }
}

void Witch::render(bool resetUniform) const {

#ifdef DEBUG
    LOGV(LOG_LEVEL_WITCH, (*mLog % 100), LOG_FORMAT(" - r:%s (u:%s)"), __PRETTY_FUNCTION__, __LINE__,
            (resetUniform)? "true":"false", (mUpdated)? "true":"false");
    if ((resetUniform) || (mUpdated))
        Dynamic2D::transform(getShader2D(), mLog);
#else
    if ((resetUniform) || (mUpdated))
        Dynamic2D::transform(getShader2D());
#endif
    Panel2D::render(false);
}
