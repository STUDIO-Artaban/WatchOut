#include "Triangle.h"

#ifndef __ANDROID__
#include "OpenGLES/ES2/gl.h"
#endif

static const unsigned short g_TriangleIndices[] = { 0, 1, 2 };

//////
Triangle::Triangle() : Object2D(), Dynamic2D() {

    LOGV(LOG_LEVEL_TRIANGLE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    mTransform = new float[TRANSFORM_BUFFER_SIZE];
    std::memset(mTransform, 0, sizeof(float) * TRANSFORM_BUFFER_SIZE);

    mVertices = new float[6];
    std::memset(mVertices, 0, sizeof(float) * 6);
}
Triangle::~Triangle() {

    LOGV(LOG_LEVEL_TRIANGLE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    delete [] mVertices;
}

void Triangle::start(const Game* game) {

    LOGV(LOG_LEVEL_TRIANGLE, 0, LOG_FORMAT(" g:%x"), __PRETTY_FUNCTION__, __LINE__, game);
    Object2D::start(0x00, 0x00, 0x00);

    std::memcpy(mTransform, Object2D::TransformBuffer, sizeof(float) * TRANSFORM_BUFFER_SIZE);
    std::memcpy(mVertices, g_TriangleVertices, sizeof(float) * 6);

    mBounds = new Bounds*[1];
    mBounds[0] = new Triangle2D(g_TriangleVertices[0], g_TriangleVertices[1], g_TriangleVertices[2],
            g_TriangleVertices[3], g_TriangleVertices[4], g_TriangleVertices[5]);
#ifdef DEBUG
    mBounds[0]->setLog(game->getLog());
#endif
    mBoundCount = 1;

    mVelocities = new float[2];
    mVelocities[0] = 0.f;
    mVelocities[1] = 0.f;
}

void Triangle::resume() {

    LOGV(LOG_LEVEL_TRIANGLE, 0, LOG_FORMAT(), __PRETTY_FUNCTION__, __LINE__);
    Object2D::resume(0x00, 0x00, 0x00);
}

void Triangle::render() const {

#ifdef DEBUG
    LOGV(LOG_LEVEL_TRIANGLE, (*mLog % 100), LOG_FORMAT(" - (u:%s)"), __PRETTY_FUNCTION__, __LINE__,
            (mUpdated)? "true":"false");
    Dynamic2D::transform(getShader2D(), mLog);
#else
    Dynamic2D::transform(getShader2D());
#endif
    Object2D::render(false);
    glDrawElements(GL_TRIANGLES, sizeof(g_TriangleIndices) / sizeof(short), GL_UNSIGNED_SHORT, g_TriangleIndices);
}
