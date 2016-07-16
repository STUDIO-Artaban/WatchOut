package com.studio.artaban.watchout.data;

import com.studio.artaban.watchout.EngActivity;
import com.studio.artaban.watchout.EngAdvertising;
import com.studio.artaban.watchout.EngLibrary;
import com.studio.artaban.watchout.EngResources;

import android.util.DisplayMetrics;
import android.util.Log;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.Animation.AnimationListener;
import android.view.animation.AnimationUtils;
import android.widget.RelativeLayout;

import com.studio.artaban.watchout.R;

public class EngData {

    ////// JNI
    static public final String PROJECT_NAME_LIB = "WatchOut";

    ////// Languages
    static private final short LANG_EN = 0;
    @SuppressWarnings("unused") static private final short LANG_FR = 1;
    @SuppressWarnings("unused") static private final short LANG_DE = 2;
    @SuppressWarnings("unused") static private final short LANG_ES = 3;
    @SuppressWarnings("unused") static private final short LANG_IT = 4;
    @SuppressWarnings("unused") static private final short LANG_PT = 5;

    @SuppressWarnings("unused") static private final short mLanguage = LANG_EN;

    ////// Permissions
    static public final boolean USES_PERMISSION_CAMERA = false; // android.permission.CAMERA
    static public final boolean USES_PERMISSION_MIC = false; // android.permission.RECORD_AUDIO
    static public final boolean USES_PERMISSION_INTERNET = true; // android.permission.INTERNET & ACCESS_NETWORK_STATE
    static public final boolean USES_PERMISSION_STORAGE = false; // android.permission.WRITE_INTERNAL_STORAGE & WRITE_EXTERNAL_STORAGE
    static public final boolean USES_PERMISSION_BLUETOOTH = false; // android.permission.BLUETOOTH & BLUETOOTH_ADMIN

    ////// Font
    static public final boolean FONT_TEXTURE_GRAYSCALE = true;

    ////// Textures
    static private final short TEXTURE_ID_AVAILABLE = 2; // EngActivity.TEXTURE_ID_FONT + 1
    static private final short TEXTURE_ID_CLOSE = 3;
    static private final short TEXTURE_ID_PUMPKIN = 4;
    static private final short TEXTURE_ID_BACK = 5;
    static private final short TEXTURE_ID_MOON = 6;
    static private final short TEXTURE_ID_SCENE = 7;
    static private final short TEXTURE_ID_TITLE_FWSP = 8;
    static private final short TEXTURE_ID_STORES_FWSP = 9;
    static private final short TEXTURE_ID_FACE = 10;

    static private final short TEXTURE_ID_WITCH0 = 11;
    static private final short TEXTURE_ID_WITCH1 = 12;
    static private final short TEXTURE_ID_WITCH2 = 13;
    static private final short TEXTURE_ID_WITCH3 = 14;
    static private final short TEXTURE_ID_WITCH4 = 15;
    static private final short TEXTURE_ID_WITCH5 = 16;
    static private final short TEXTURE_ID_WITCH6 = 17;
    static private final short TEXTURE_ID_WITCH7 = 18;

    static private final short TEXTURE_ID_TITLE = 19;
    static private final short TEXTURE_ID_DISK = 20;
    static private final short TEXTURE_ID_RING = 21;
    static private final short TEXTURE_ID_MEDAL = 22;
    static private final short TEXTURE_ID_LOOSE = 23;
    static private final short TEXTURE_ID_STAR = 24;
    static private final short TEXTURE_ID_NETWORKS = 25;

    static public short loadTexture(EngResources resources, short id) {

        switch (id) {
	        case TEXTURE_ID_AVAILABLE: {
	            EngResources.BmpInfo bmpInfo = resources.getBufferPNG("available.png");
	            return EngLibrary.loadTexture(TEXTURE_ID_AVAILABLE, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
	        }
            case TEXTURE_ID_CLOSE: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("close.png");
                return EngLibrary.loadTexture(TEXTURE_ID_CLOSE, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_PUMPKIN: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("pumpkin.png");
                return EngLibrary.loadTexture(TEXTURE_ID_PUMPKIN, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }
            case TEXTURE_ID_BACK: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("back.png");
                return EngLibrary.loadTexture(TEXTURE_ID_BACK, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_MOON: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("moon.png");
                return EngLibrary.loadTexture(TEXTURE_ID_MOON, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_SCENE: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("scene.png");
                return EngLibrary.loadTexture(TEXTURE_ID_SCENE, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }
            case TEXTURE_ID_TITLE_FWSP: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("titleFWSP.png");
                return EngLibrary.loadTexture(TEXTURE_ID_TITLE_FWSP, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }
            case TEXTURE_ID_STORES_FWSP: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("storesFWSP.png");
                return EngLibrary.loadTexture(TEXTURE_ID_STORES_FWSP, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }
            case TEXTURE_ID_FACE: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("face.png");
                return EngLibrary.loadTexture(TEXTURE_ID_FACE, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }
            case TEXTURE_ID_WITCH0:
            case TEXTURE_ID_WITCH1:
            case TEXTURE_ID_WITCH2:
            case TEXTURE_ID_WITCH3:
            case TEXTURE_ID_WITCH4:
            case TEXTURE_ID_WITCH5:
            case TEXTURE_ID_WITCH6:
            case TEXTURE_ID_WITCH7: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("witch-" + (id - TEXTURE_ID_WITCH0) + ".png");
                return EngLibrary.loadTexture(id, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }

            case TEXTURE_ID_TITLE: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("title.png");
                return EngLibrary.loadTexture(TEXTURE_ID_TITLE, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_DISK: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("disk.png");
                return EngLibrary.loadTexture(TEXTURE_ID_DISK, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_RING: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("ring.png");
                return EngLibrary.loadTexture(TEXTURE_ID_RING, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_MEDAL: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("medal.png");
                return EngLibrary.loadTexture(TEXTURE_ID_MEDAL, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_LOOSE: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("loose.png");
                return EngLibrary.loadTexture(TEXTURE_ID_LOOSE, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_STAR: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("star.png");
                return EngLibrary.loadTexture(TEXTURE_ID_STAR, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, true);
            }
            case TEXTURE_ID_NETWORKS: {
                EngResources.BmpInfo bmpInfo = resources.getBufferPNG("networks.png");
                return EngLibrary.loadTexture(TEXTURE_ID_NETWORKS, bmpInfo.width, bmpInfo.height, bmpInfo.buffer, false);
            }

            default: {
                Log.e("EngData", "Failed to load PNG ID: " + Integer.toString(id));
                return EngActivity.NO_TEXTURE_LOADED;
            }
        }
    }

    ////// Sounds
    static private final short SOUND_ID_FLAP = 1; // EngActivity.SOUND_ID_LOGO + 1

    static private final short SOUND_ID_COLLISION = 2;
    static private final short SOUND_ID_START = 3;
    static private final short SOUND_ID_LOOSE = 4;
    static private final short SOUND_ID_BONUS = 5;

    static public void loadOgg(EngResources resources, short id) {

        switch (id) {
	        case SOUND_ID_FLAP: {
	            EngLibrary.loadOgg(SOUND_ID_FLAP, resources.getBufferOGG("flap.ogg"), false);
	            break;
	        }
            case SOUND_ID_COLLISION: {
                EngLibrary.loadOgg(SOUND_ID_COLLISION, resources.getBufferOGG("collision.ogg"), false);
                break;
            }
            case SOUND_ID_START: {
                EngLibrary.loadOgg(SOUND_ID_START, resources.getBufferOGG("start.ogg"), true);
                break;
            }
            case SOUND_ID_LOOSE: {
                EngLibrary.loadOgg(SOUND_ID_LOOSE, resources.getBufferOGG("loose.ogg"), true);
                break;
            }
            case SOUND_ID_BONUS: {
                EngLibrary.loadOgg(SOUND_ID_BONUS, resources.getBufferOGG("bonus.ogg"), false);
                break;
            }

            default: {
                Log.e("EngData", "Failed to load OGG ID: " + Integer.toString(id));
                break;
            }
        }
    }

    ////// Advertising
    static public final boolean INTERSTITIAL_AD = false; // TRUE: InterstitialAd; FALSE: AdView
    static private final boolean TEST_ADVERTISING = false; // Set to 'false' in release mode
    static private final String ADV_UNIT_ID = "ca-app-pub-1474300545363558/9895582627";

    static private class AdAnimListener implements AnimationListener {

		public void onAnimationEnd(Animation animation) { EngAdvertising.mStatus = EngAdvertising.AD_STATUS_DISPLAYED; }
		public void onAnimationRepeat(Animation animation) { }
		public void onAnimationStart(Animation animation) { }
    };

    static public void loadAd(EngActivity activity) {

    	if (!USES_PERMISSION_INTERNET)
    		Log.e("EngData", "Missing INTERNET & ACCESS_NETWORK_STATE permissions");

    	//
    	DisplayMetrics metrics = new DisplayMetrics();
    	activity.getWindowManager().getDefaultDisplay().getMetrics(metrics);
        if (((activity.getWindowManager().getDefaultDisplay().getWidth() * 160) / metrics.xdpi) > 468)
        	EngAdvertising.setBanner(EngAdvertising.BANNER_ID_FULL_BANNER);
        else
    		EngAdvertising.setBanner(EngAdvertising.BANNER_ID_BANNER);

		EngAdvertising.setUnitID(ADV_UNIT_ID);
		if (TEST_ADVERTISING)
			EngAdvertising.load(activity.getContentResolver());
		else
			EngAdvertising.load(null);
    }
    static public void displayAd(short id, EngActivity activity) {

		if (activity.mSurfaceLayout.getChildCount() == 2)
			activity.mSurfaceLayout.removeView(EngAdvertising.getView());
		EngAdvertising.getView().setVisibility(View.VISIBLE);

        RelativeLayout.LayoutParams adParams = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.WRAP_CONTENT,
        																	RelativeLayout.LayoutParams.WRAP_CONTENT);
        adParams.addRule(RelativeLayout.ALIGN_PARENT_TOP);
        adParams.addRule(RelativeLayout.CENTER_HORIZONTAL);

        activity.mSurfaceLayout.addView(EngAdvertising.getView(), adParams);

        Animation animAds = (Animation)AnimationUtils.loadAnimation(activity, R.anim.disp_ad);
        animAds.setAnimationListener(new AdAnimListener());
        EngAdvertising.getView().startAnimation(animAds);

        EngAdvertising.mStatus = EngAdvertising.AD_STATUS_DISPLAYING;
    }
    static public void hideAd(short id, EngActivity activity) {

		EngAdvertising.getView().clearAnimation();

        EngAdvertising.mStatus = EngAdvertising.AD_STATUS_LOADED;
		EngAdvertising.getView().setVisibility(View.INVISIBLE);
		// -> Always set invisible when hidden (see 'onAdLoaded' method in 'EngAdvertising')
    }

    ////// Social
    static public final boolean INFO_FIELD_FACEBOOK_BIRTHDAY = true;
    static public final boolean INFO_FIELD_FACEBOOK_LOCATION = true;
    // NB: INFO_FIELD_FACEBOOK_NAME & INFO_FIELD_FACEBOOK_GENDER are always 'true'

}
