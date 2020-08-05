package com.mrap.compass;

import android.app.NativeActivity;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;

public class CompassNativeActivity extends NativeActivity {

    float[] accel = new float[3];
    float[] mag = new float[3];
    float[] gyro = new float[3];

    long start = 0;
    long sec = 0;
    long prevSec = 0;

    SensorEventListener sel = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent sensorEvent) {
            if (sensorEvent.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
                accel = sensorEvent.values;
            }
            if (sensorEvent.sensor.getType() == Sensor.TYPE_GYROSCOPE) {
                gyro = sensorEvent.values;
            }
            if (sensorEvent.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD) {
                mag = sensorEvent.values;
            }

            if (start == 0) {
                start = System.currentTimeMillis();
            }

            long diff = System.currentTimeMillis() - start;
            sec = diff / 1000;
            if (sec != prevSec) {
                StringBuilder values = new StringBuilder();
                float[][] tmpf = new float[][] {accel, mag, gyro};
                for (float[] tmpfel : tmpf) {
                    for (int i = 0; i < tmpfel.length; i++) {
                        values.append(tmpfel[i] + "");
                        if (i < tmpfel.length - 1) {
                            values.append(",");
                        }
                    }
                    values.append(" :: ");
                }
                System.out.println("onSensorChanged " + values.toString());
            }

            prevSec = sec;
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int i) {

        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        System.out.println("onCreate");
        read(new float[3], new float[3], new float[3]);

        //SensorManager sm = (SensorManager)getSystemService(SENSOR_SERVICE);
        //sm.registerListener(sel, sm.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), SensorManager.SENSOR_DELAY_NORMAL);
        //sm.registerListener(sel, sm.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD), SensorManager.SENSOR_DELAY_NORMAL);
        //sm.registerListener(sel, sm.getDefaultSensor(Sensor.TYPE_GYROSCOPE), SensorManager.SENSOR_DELAY_NORMAL);
    }

    public void read(float[] accel, float[] mag, float[] orientation) {
        float[] r = new float[9], i = new float[9];
        boolean success = SensorManager.getRotationMatrix(r, i, accel, mag);
        if (success) {
            SensorManager.getOrientation(r, orientation);
        }
        if (prevSec != sec) {
            System.out.println("java " + success);
        }
    }
}
