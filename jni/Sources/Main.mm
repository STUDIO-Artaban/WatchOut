#include "Main.h"

////// Languages
typedef enum {

    LANG_EN = 0,
    LANG_FR,
    LANG_DE,
    LANG_ES,
    LANG_IT,
    LANG_PT

} Language;
static const Language g_MainLang = LANG_EN;

////// Textures
#define NO_TEXTURE_LOADED       0xFF

BOOL engGetFontGrayscale() { return YES; }

#define TEXTURE_ID_AVAILABLE        2 // EngActivity.TEXTURE_ID_FONT + 1
#define TEXTURE_ID_CLOSE            3
#define TEXTURE_ID_PUMPKIN          4
#define TEXTURE_ID_BACK             5
#define TEXTURE_ID_MOON             6
#define TEXTURE_ID_SCENE            7
#define TEXTURE_ID_TITLE_FWSP       8
#define TEXTURE_ID_STORES_FWSP      9
#define TEXTURE_ID_FACE             10

#define TEXTURE_ID_WITCH0           11
#define TEXTURE_ID_WITCH1           12
#define TEXTURE_ID_WITCH2           13
#define TEXTURE_ID_WITCH3           14
#define TEXTURE_ID_WITCH4           15
#define TEXTURE_ID_WITCH5           16
#define TEXTURE_ID_WITCH6           17
#define TEXTURE_ID_WITCH7           18

#define TEXTURE_ID_TITLE            19
#define TEXTURE_ID_DISK             20
#define TEXTURE_ID_RING             21
#define TEXTURE_ID_MEDAL            22
#define TEXTURE_ID_LOOSE            23
#define TEXTURE_ID_STAR             24
#define TEXTURE_ID_NETWORKS         25

unsigned char engLoadTexture(EngResources* resources, unsigned char Id) {
    switch (Id) {

        case TEXTURE_ID_AVAILABLE: {

            unsigned char* data = [resources getBufferPNG:@"available" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_AVAILABLE, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_CLOSE: {

            unsigned char* data = [resources getBufferPNG:@"close" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_CLOSE, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_PUMPKIN: {

            unsigned char* data = [resources getBufferPNG:@"pumpkin" inGrayScale:NO];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_PUMPKIN, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }
        case TEXTURE_ID_BACK: {
            
            unsigned char* data = [resources getBufferPNG:@"back" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_BACK, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_MOON: {

            unsigned char* data = [resources getBufferPNG:@"moon" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_MOON, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_SCENE: {
            
            unsigned char* data = [resources getBufferPNG:@"scene" inGrayScale:NO];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_SCENE, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }
        case TEXTURE_ID_TITLE_FWSP: {

            unsigned char* data = [resources getBufferPNG:@"titleFWSP" inGrayScale:NO];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_TITLE_FWSP, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }
        case TEXTURE_ID_STORES_FWSP: {
            
            unsigned char* data = [resources getBufferPNG:@"storesFWSP" inGrayScale:NO];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_STORES_FWSP, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }
        case TEXTURE_ID_FACE: {
            
            unsigned char* data = [resources getBufferPNG:@"face" inGrayScale:NO];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_FACE, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }
        case TEXTURE_ID_WITCH0:
        case TEXTURE_ID_WITCH1:
        case TEXTURE_ID_WITCH2:
        case TEXTURE_ID_WITCH3:
        case TEXTURE_ID_WITCH4:
        case TEXTURE_ID_WITCH5:
        case TEXTURE_ID_WITCH6:
        case TEXTURE_ID_WITCH7: {
            
            NSString* animImg = [[NSString alloc] initWithFormat:@"%@%d", @"witch-", (Id - TEXTURE_ID_WITCH0)];
            unsigned char* data = [resources getBufferPNG:animImg inGrayScale:NO];
            [animImg release];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(Id, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }

        case TEXTURE_ID_TITLE: {

            unsigned char* data = [resources getBufferPNG:@"title" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_TITLE, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_DISK: {

            unsigned char* data = [resources getBufferPNG:@"disk" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_DISK, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_RING: {

            unsigned char* data = [resources getBufferPNG:@"ring" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_RING, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_MEDAL: {

            unsigned char* data = [resources getBufferPNG:@"medal" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_MEDAL, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_LOOSE: {

            unsigned char* data = [resources getBufferPNG:@"loose" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_LOOSE, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_STAR: {

            unsigned char* data = [resources getBufferPNG:@"star" inGrayScale:YES];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_STAR, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data, true);
        }
        case TEXTURE_ID_NETWORKS: {

            unsigned char* data = [resources getBufferPNG:@"networks" inGrayScale:NO];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get PNG buffer (line:%d)", __LINE__);
                break;
            }
            return platformLoadTexture(TEXTURE_ID_NETWORKS, static_cast<short>(resources.pngWidth),
                                       static_cast<short>(resources.pngHeight), data);
        }

        default: {
            
            NSLog(@"ERROR: Failed to load PNG ID %d", Id);
            break;
        }
    }
    return NO_TEXTURE_LOADED;
}

////// Sounds
#define SOUND_ID_FLAP       1 // EngActivity.SOUND_ID_LOGO + 1

#define SOUND_ID_COLLISION  2
#define SOUND_ID_START      3
#define SOUND_ID_LOOSE      4
#define SOUND_ID_BONUS      5

void engLoadSound(EngResources* resources, unsigned char Id) {
    switch (Id) {

        case SOUND_ID_FLAP: {

            unsigned char* data = [resources getBufferOGG:@"flap"];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get OGG buffer (line:%d)", __LINE__);
                break;
            }
            platformLoadOgg(SOUND_ID_FLAP, resources.oggLength, data, false);
            break;
        }
        case SOUND_ID_COLLISION: {

            unsigned char* data = [resources getBufferOGG:@"collision"];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get OGG buffer (line:%d)", __LINE__);
                break;
            }
            platformLoadOgg(SOUND_ID_COLLISION, resources.oggLength, data, false);
            break;
        }
        case SOUND_ID_START: {

            unsigned char* data = [resources getBufferOGG:@"start"];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get OGG buffer (line:%d)", __LINE__);
                break;
            }
            platformLoadOgg(SOUND_ID_START, resources.oggLength, data, true);
            break;
        }
        case SOUND_ID_LOOSE: {

            unsigned char* data = [resources getBufferOGG:@"loose"];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get OGG buffer (line:%d)", __LINE__);
                break;
            }
            platformLoadOgg(SOUND_ID_LOOSE, resources.oggLength, data, true);
            break;
        }
        case SOUND_ID_BONUS: {

            unsigned char* data = [resources getBufferOGG:@"bonus"];
            if (data == NULL) {
                NSLog(@"ERROR: Failed to get OGG buffer (line:%d)", __LINE__);
                break;
            }
            platformLoadOgg(SOUND_ID_BONUS, resources.oggLength, data, false);
            break;
        }

        default: {

            NSLog(@"ERROR: Failed to load OGG ID %d", Id);
            break;
        }
    }
}

////// Advertising
#ifdef LIBENG_ENABLE_ADVERTISING

static NSString* ADV_UNIT_ID = @"ca-app-pub-1474300545363558/8154067024";
#ifdef DEBUG
static const NSString* IPAD_DEVICE_UID = @"655799b1c803de3417cbb36833b6c40c";
static const NSString* IPHONE_YACIN_UID = @"10053bb6983c6568b88812fbcfd7ab89";
#endif

BOOL engGetAdType() { return FALSE; } // TRUE: Interstitial; FALSE: Banner
void engLoadAd(EngAdvertising* ad, GADRequest* request) {

    static bool init = false;
    if (!init) {
        if ([[UIScreen mainScreen] bounds].size.width > 468)
            [ad getBanner].adSize = kGADAdSizeFullBanner;
        else
            [ad getBanner].adSize = kGADAdSizeBanner;
        [ad getBanner].adUnitID = ADV_UNIT_ID;
        init = true;
    }
#ifdef DEBUG
    request.testDevices = [NSArray arrayWithObjects: GAD_SIMULATOR_ID, IPAD_DEVICE_UID, IPHONE_YACIN_UID, nil];
#endif
    [[ad getBanner] loadRequest:request];
}
void engDisplayAd(EngAdvertising* ad, unsigned char Id) {

    CGFloat xPos = ([[UIScreen mainScreen] bounds].size.width - [ad getBanner].frame.size.width) / 2.0;
    [[ad getBanner] setHidden:NO];
    [ad getBanner].frame =  CGRectMake(xPos, -[ad getBanner].frame.size.height,
                                       [ad getBanner].frame.size.width, [ad getBanner].frame.size.height);
    [UIView animateWithDuration:1.5 animations:^{
        [ad getBanner].frame =  CGRectMake(xPos, 0, [ad getBanner].frame.size.width,
                                           [ad getBanner].frame.size.height);
    } completion:^(BOOL finished) {
        if (finished)
            ad.status = static_cast<unsigned char>(Advertising::STATUS_DISPLAYED);
    }];
}
void engHideAd(EngAdvertising* ad, unsigned char Id) { [[ad getBanner] setHidden:YES]; }
#endif

////// Social
#ifdef LIBENG_ENABLE_SOCIAL
BOOL engReqInfoField(SocialField field, unsigned char socialID) {

    switch (socialID) {
        case Network::FACEBOOK: {

            switch (field) {
                //case FIELD_NAME: return YES;
                //case FIELD_GENDER: return YES;
                case FIELD_BIRTHDAY: return YES;
                case FIELD_LOCATION: return YES;
                default: return YES; // FIELD_NAME & FIELD_GENDER are always retrieved
            }
            break;
        }
        case Network::GOOGLE:
            return YES; // All fields are always retrieved (if any)

        default: {

            NSLog(@"ERROR: Wrong social identifier %d", socialID);
            break;
        }
    }
    return NO;
}
#endif
