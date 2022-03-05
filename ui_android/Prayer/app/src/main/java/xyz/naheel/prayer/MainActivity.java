package xyz.naheel.prayer;

import android.annotation.SuppressLint;
import android.app.ActionBar;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Build;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.text.Html;
import android.view.Display;
import android.view.Surface;
import android.view.View;
import android.view.WindowInsetsController;
import android.view.WindowManager;
import android.webkit.GeolocationPermissions;
import android.webkit.JavascriptInterface;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.ImageView;
import android.widget.Toast;

public class MainActivity extends Activity implements SensorEventListener {

    private final String PREFS_KEY = "prayer-config";

    private SharedPreferences prefs;
    private SharedPreferences.Editor editor;

    private SensorManager mSensorManager;
    private WebView wv;

    private String theme;

    @SuppressLint({"AddJavascriptInterface", "SetJavaScriptEnabled"}) // be extra careful!!!
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        prefs = PreferenceManager.getDefaultSharedPreferences(this);
        editor = prefs.edit();

        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);

        ImageView splash = findViewById(R.id.splash);
        wv = findViewById(R.id.wv);

        wv.setBackgroundColor(Color.TRANSPARENT);
        wv.getSettings().setJavaScriptEnabled(true);
        wv.getSettings().setGeolocationEnabled(true);
        wv.getSettings().setGeolocationDatabasePath(getFilesDir().getPath());
        wv.addJavascriptInterface(new WebAppInterface(), "droid");
        wv.loadUrl("file:///android_asset/index.html");
        wv.setWebChromeClient(new WebChromeClient() {
            public void onGeolocationPermissionsShowPrompt(String origin, GeolocationPermissions.Callback callback) {
                callback.invoke(origin, true, false);
            }
        });
        wv.setWebViewClient(new WebViewClient() {
            @Override
            public void onPageCommitVisible(WebView view, String url) {
                splash.setVisibility(View.GONE);
            }
        });

        int nightModeFlags = getResources().getConfiguration().uiMode &
                Configuration.UI_MODE_NIGHT_MASK;
        if (nightModeFlags == Configuration.UI_MODE_NIGHT_NO) {
            theme = "light";
        } else {
            theme = "dark";
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ORIENTATION),
                SensorManager.SENSOR_DELAY_GAME);
    }

    @Override
    protected void onPause() {
        super.onPause();
        mSensorManager.unregisterListener(this);
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        float degree = event.values[0];
        Display display = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        switch (display.getRotation()) {
            case Surface.ROTATION_90:
                degree += 90;
                break;
            case Surface.ROTATION_180:
                degree += 180;
                break;
            case Surface.ROTATION_270:
                degree += 270;
                break;
            case Surface.ROTATION_0:
                break;
        }
        degree = 360 - degree;
        // TODO: low pass?
        wv.loadUrl("javascript: set_north(" + degree + ")");
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // not in use
    }

    void theme(boolean light, String bg, String fg) {
        int background = Color.parseColor(bg);
        int foreground = Color.parseColor(fg);
        // setting status bar and navigation bar colors
        getWindow().setStatusBarColor(background);
        getWindow().setNavigationBarColor(background);
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                WindowInsetsController controller = getWindow().getDecorView().getWindowInsetsController();
                controller.setSystemBarsAppearance(0, WindowInsetsController.APPEARANCE_LIGHT_NAVIGATION_BARS);
                controller.setSystemBarsAppearance(0, WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS);
                if (light) {
                    controller.setSystemBarsAppearance(WindowInsetsController.APPEARANCE_LIGHT_NAVIGATION_BARS,
                            WindowInsetsController.APPEARANCE_LIGHT_NAVIGATION_BARS);
                    controller.setSystemBarsAppearance(WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS,
                            WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS);
                }
            } else {
                throw new Exception();
            }
        } catch (NoSuchMethodError | Exception ignored) {
            if (light) {
                getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR |
                        View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);
            } else {
                getWindow().getDecorView().setSystemUiVisibility(0);
            }
        }
        // setting action bar colors
        ActionBar actionBar = getActionBar();
        CharSequence title = getString(R.string.app_name);
        actionBar.setBackgroundDrawable(new ColorDrawable(background));
        actionBar.setTitle(Html.fromHtml("<font color='" + foreground + "'>" + title + "</font>"));
        getWindow().getDecorView().setBackgroundColor(background);
    }

    public class WebAppInterface {
        @JavascriptInterface
        public void toast(String text) {
            Toast.makeText(MainActivity.this, text, Toast.LENGTH_SHORT).show();
        }

        @JavascriptInterface
        public void dialog(String title, String text) {
            new AlertDialog.Builder(MainActivity.this)
                    .setTitle(title)
                    .setMessage(text)
                    .show();
        }

        @JavascriptInterface
        public void theme(boolean light, String bg, String fg) {
            runOnUiThread(() -> MainActivity.this.theme(light, bg, fg));
        }

        @JavascriptInterface
        public String prefsLoad() {
            return prefs.getString(PREFS_KEY, "{}");
        }

        @JavascriptInterface
        public boolean prefsSave(String json) {
            return editor.putString(PREFS_KEY, json).commit();
        }

        @JavascriptInterface
        public String getTheme() {
            return theme;
        }
    }

}
