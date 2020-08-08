package com.mrap.compass;

import android.app.Activity;
import android.app.NativeActivity;
import android.graphics.Color;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.PopupWindow;
import android.widget.RelativeLayout;
import android.widget.TextView;

public class CompassNativeActivity extends NativeActivity {

    RelativeLayout layout = null;
    RelativeLayout innerLayout = null;
    RelativeLayout compassLayout = null;
    TextView txtDegree;
    PopupWindow popupWindow = null;
    float degree = 0;
    float drawnDegree = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE);
        //getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_VISIBLE);
    }

    public void showUi() {
        if (layout != null) {
            return;
        }

        final Activity that = this;
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                layout = new RelativeLayout(that);

                ViewGroup.LayoutParams lp = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
                that.setContentView(layout, lp);

                txtDegree = new TextView(that);
                txtDegree.setTextSize(TypedValue.COMPLEX_UNIT_DIP, 20);
                txtDegree.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
                txtDegree.setText("0");
                txtDegree.setTextColor(Color.parseColor("#ffffffff"));
                final RelativeLayout.LayoutParams txtLp = new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
                txtLp.addRule(RelativeLayout.CENTER_IN_PARENT);
                txtDegree.setLayoutParams(txtLp);

                innerLayout = new RelativeLayout(that);
                innerLayout.setLayoutParams(new RelativeLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
                innerLayout.addView(txtDegree);
                //innerLayout.setBackgroundColor(Color.parseColor("#AAFF0000"));

                //innerLayout.post(new Runnable() {
                //    @Override
                //    public void run() {
                //        System.out.println("innerlayout "+ innerLayout.getHeight() + " " + layout.getHeight() + " " + txtLp.getMarginStart() + " " + txtLp.topMargin);
                //    }
                //});

                DisplayMetrics displayMetrics = new DisplayMetrics();
                getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);

                LayoutInflater layoutInflater = (LayoutInflater)getBaseContext().getSystemService(LAYOUT_INFLATER_SERVICE);


                compassLayout = (RelativeLayout)layoutInflater.inflate(R.layout.compass_text_widget, innerLayout, false);
                RelativeLayout.LayoutParams clp = ((RelativeLayout.LayoutParams)compassLayout.getLayoutParams());
                clp.addRule(RelativeLayout.CENTER_IN_PARENT);
                clp.width = (int)(0.7 * displayMetrics.widthPixels);
                clp.height = (int)(0.7 * displayMetrics.widthPixels);
                //compassLayout.setBackgroundColor(Color.parseColor("#aa00ff00"));
                //final RelativeLayout compassInnerLayout = (RelativeLayout)compassLayout.getChildAt(0);
                //compassInnerLayout.setBackgroundColor(Color.parseColor("#aaff0000"));
                innerLayout.addView(compassLayout);

                popupWindow = new PopupWindow(innerLayout, ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
                //popupWindow.showAtLocation(layout, Gravity.CENTER, 0, 0);
                popupWindow.showAtLocation(layout, Gravity.TOP | Gravity.START, 0, 0);
            }
        });
    }

    public void setDegree(final float degree1Smooth, final float degree1) {
        this.degree = degree1Smooth;
        if (txtDegree != null) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    drawnDegree = degree1;
                    compassLayout.getChildAt(0).setRotation(-degree1);
                    txtDegree.setText(String.format("%.1f", degree));
                }
            });
        }
    }

    public float getDrawnDegree() {
        return drawnDegree;
    }

    public void getLayoutDimension(int[] dimensions) {
        //System.out.println(dimensions.length);
        if (layout == null) {
            System.out.println("layout null");
            for (int i = 0; i < dimensions.length; i++) {
                dimensions[i] = 0;
            }
            return;
        }
        layout.getLocationOnScreen(dimensions);
        dimensions[2] = layout.getWidth();
        dimensions[3] = layout.getHeight();
        //System.out.println(String.format("java layout %d %d %d %d",
        //        dimensions[0],
        //        dimensions[1],
        //        dimensions[2],
        //        dimensions[3]));
    }
}
