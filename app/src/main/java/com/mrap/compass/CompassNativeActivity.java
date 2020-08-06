package com.mrap.compass;

import android.app.Activity;
import android.app.NativeActivity;
import android.graphics.Color;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.widget.PopupWindow;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class CompassNativeActivity extends NativeActivity {

    RelativeLayout layout;
    TextView txtDegree;
    PopupWindow popupWindow = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE);
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

                ViewGroup.MarginLayoutParams mlp = new ViewGroup.MarginLayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
                mlp.setMargins(0, 0, 0, 0);

                that.setContentView(layout, mlp);



                txtDegree = new TextView(that);
                txtDegree.setText("0000");
                txtDegree.setTextColor(Color.parseColor("#ffffffff"));

                popupWindow = new PopupWindow(txtDegree, ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
                popupWindow.showAtLocation(layout, Gravity.TOP | Gravity.START, 100, 100);
            }
        });
    }

    public void read(float[] accel, float[] mag, float[] orientation) {
        float[] r = new float[9], i = new float[9];
        boolean success = SensorManager.getRotationMatrix(r, i, accel, mag);
        if (success) {
            SensorManager.getOrientation(r, orientation);
            final float deg = (float)(orientation[0] * 180 / Math.PI);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    txtDegree.setText(String.format("%.2f", deg));
                }
            });
        }
    }
}
