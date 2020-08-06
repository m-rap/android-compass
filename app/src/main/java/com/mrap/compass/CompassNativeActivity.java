package com.mrap.compass;

import android.app.Activity;
import android.app.NativeActivity;
import android.graphics.Color;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.PopupWindow;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class CompassNativeActivity extends NativeActivity {

    RelativeLayout layout = null;
    TextView txtDegree;
    PopupWindow popupWindow = null;
    float degree = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE);
        //getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_VISIBLE);
    }

    public void showUi() {
        if (popupWindow != null) {
            return;
        }

        final Activity that = this;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                layout = new RelativeLayout(that);

                //ViewGroup.MarginLayoutParams mlp = new ViewGroup.MarginLayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
                ViewGroup.MarginLayoutParams mlp = new ViewGroup.MarginLayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
                //mlp.setMargins(0, 0, 0, 0);

                that.setContentView(layout, mlp);

                //layout.invalidate();
                //DisplayMetrics displayMetrics = new DisplayMetrics();
                //getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
                //int height = displayMetrics.heightPixels;
                //int actBarH = height - layout.getHeight();
                //
                //mlp.setMargins(0, -actBarH, 0, 0);

                //layout.post(new Runnable() {
                //    @Override
                //    public void run() {
                //        //DisplayMetrics displayMetrics = new DisplayMetrics();
                //        //getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
                //        //System.out.println("layout height " + layout.getHeight() + " " + displayMetrics.heightPixels);
                //    }
                //});

                txtDegree = new TextView(that);
                txtDegree.setTextSize(TypedValue.COMPLEX_UNIT_PT, 20);
                txtDegree.setText("0");
                txtDegree.setTextColor(Color.parseColor("#ffffffff"));
                txtDegree.invalidate();



                popupWindow = new PopupWindow(txtDegree, ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
                //popupWindow.showAtLocation(layout, Gravity.TOP | Gravity.START, 100, 100);
                popupWindow.showAtLocation(layout, Gravity.CENTER, 0, 0);
            }
        });
    }

    public void setDegree(float degree1) {
        this.degree = degree1;
        if (txtDegree != null) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    //txtDegree.setText(String.format("%d", Math.round(degree)));
                    txtDegree.setText(String.format("%.1f", degree));
                }
            });
        }
    }

    public float getLayoutHeight() {
        if (layout == null)
            return 0;
        return layout.getHeight();
    }

    public void read(float[] accel, float[] mag, float[] orientation) {
        float[] r = new float[9], i = new float[9];
        boolean success = SensorManager.getRotationMatrix(r, i, accel, mag);
        if (success) {
            SensorManager.getOrientation(r, orientation);
        }
    }
}
