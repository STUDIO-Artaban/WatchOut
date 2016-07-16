#ifndef GLOBAL_H_
#define GLOBAL_H_

#ifdef __ANDROID__
////// DEBUG | RELEASE

// Debug
//#ifndef DEBUG
//#define DEBUG
//#endif
//#undef NDEBUG

// Relase
#ifndef NDEBUG
#define NDEBUG
#endif
#undef DEBUG

#define GOOGLE_PLAY_PACKAGE         "com.android.vending"
#define GOOGLE_PLAY_ACTIVITY        "com.google.android.finsky.activities.LaunchUrlHandlerActivity"

#endif // __ANDROID__

//#ifdef DEBUG
#define DEMO_VERSION
//#endif

#define DISPLAY_DELAY               50
#define MAX_COLOR                   255.f
#define MAX_AD_HEIGHT               60 // In pixel

#define FONT_TEXTURE_WIDTH          1024.f
#define FONT_TEXTURE_HEIGHT         256.f
#define FONT_WIDTH                  35
#define FONT_HEIGHT                 46

#define FONT_SCALE_RATIO            680.f

#define UNIT_TRIANGLE_WIDTH         (2.f / 3.f) // = Disk diameter & = Square size
#define TRIANGLE_HEIGHT             0.577350269f

// Coordinate of the triangle center: x = (x0 + x1 + x2) / 3; y = (y0 + y1 + y2) / 3
static const float g_TriangleVertices[] = { -UNIT_TRIANGLE_WIDTH / 2.f, -TRIANGLE_HEIGHT / 3.f, 0.f,
        (2.f * TRIANGLE_HEIGHT) / 3.f, UNIT_TRIANGLE_WIDTH / 2.f, -TRIANGLE_HEIGHT / 3.f };

// Texture IDs
#define TEXTURE_ID_DISK             20

// Log levels (< 5 to log)
#define LOG_LEVEL_WATCHOUT          5
#define LOG_LEVEL_WITCH             4
#define LOG_LEVEL_WITCHSPET         4
#define LOG_LEVEL_TRIANGLE          5
#define LOG_LEVEL_TITLELEVEL        5
#define LOG_LEVEL_SCORES            4
#define LOG_LEVEL_GAMELEVEL         4
#define LOG_LEVEL_SHARE             4

typedef struct {

    short left;
    short right;
    short top;
    short bottom;

} TouchArea;

#endif // GLOBAL_H_
