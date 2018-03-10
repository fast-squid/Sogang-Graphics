package com.graphics.sg.opengles_tiger;

import android.opengl.GLSurfaceView;
import android.renderscript.Matrix3f;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.MotionEvent;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.opengl.Matrix;

import java.util.Vector;

public class MainActivity extends AppCompatActivity{

    GLES20Renderer mRenderer;

    CheckBox mCheckBoxTexture;
    CheckBox mCheckBoxFog;
    static final int TYPE_TEXTURE = 0;
    static final int TYPE_FOG     = 1;

    private boolean touchAvailable = false;
    private boolean toogleLight = false;
    private float[] previousX = {0, 0, 0};
    private float[] currentX = {0, 0, 0};
    private float[] previousY = {0, 0, 0};
    private float[] currentY = {0, 0, 0};
    private float fovy = 45.0f;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);

        mCheckBoxTexture = (CheckBox) findViewById(R.id.checkbox_texture);
        mCheckBoxTexture.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                mRenderer.toggleTexture(TYPE_TEXTURE, isChecked);
            }
        });
        mCheckBoxFog = (CheckBox) findViewById(R.id.checkbox_fog);
        mCheckBoxFog.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                mRenderer.toggleTexture(TYPE_FOG, isChecked);
            }
        });

        // layout에 있는 GLSurfaceView 불러오기.
        GLSurfaceView glSurfaceView = (GLSurfaceView) findViewById(R.id.surface_view);
        // Renderer를 설정해주기 전에 클라이언트 버전을 먼저 설정해준다.
        glSurfaceView.setEGLContextClientVersion(2);
        // 커스텀 렌더러를 설정.
        mRenderer = new GLES20Renderer(this);
        glSurfaceView.setRenderer(mRenderer);
    }

    @Override
    public boolean onTouchEvent(MotionEvent e) {
        int touchCount = e.getPointerCount();
        if (touchCount < 1 || touchCount > 3) return false;

        switch (e.getAction() & MotionEvent.ACTION_MASK) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN:
                touchAvailable = true;
                toogleLight = true;
                for (int i = 0; i < e.getPointerCount(); i++) {
                    currentX[i] = e.getX(i);
                    currentY[i] = e.getY(i);
                    previousX[i] = currentX[i];
                    previousY[i] = currentY[i];
                }
                break;
            case MotionEvent.ACTION_MOVE:
                if (!touchAvailable) return false;
                for (int i = 0; i < e.getPointerCount(); i++) {
                    previousX[i] = currentX[i];
                    previousY[i] = currentY[i];
                    currentX[i] = e.getX(i);
                    currentY[i] = e.getY(i);
                }
                if (touchCount == 1) {
                    toogleLight = false;
                    float distX = (currentX[0] - previousX[0])*0.5f;
                    float distY = (currentY[0] - previousY[0])*0.5f;
                    mRenderer.mCameraPos[0]+=distX+distY;
                    mRenderer.mCameraPos[2]+=-distX+distY;

                    Matrix.translateM(mRenderer.mViewMatrix, 0, distX, 0f, 0f);
                    Matrix.translateM(mRenderer.mViewMatrix, 0, 0, 0, distY);
                    //Matrix.multiplyMM(mRenderer.mMVPMatrix,0,mRenderer.mProjectionMatrix,0,mRenderer.mViewMatrix,0);
                }
                else if (touchCount == 2) {
                    toogleLight = false;
                    float dist = currentX[0] - currentX[1];
                     mRenderer.fovy+=(dist)*0.001f;
                }
                else if (touchCount == 3) {
                    toogleLight = false;
                    float dist = currentX[2] - previousX[2];
                    float crossvec[] = new float[4];
                    float mCameraRotate[] = new float[16];
                    float mtemp[] = new float[16];

                    Matrix.rotateM(mRenderer.mViewMatrix,0,dist,0,1,0);
                }
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
                touchAvailable = false;
                if (toogleLight) {
                    mRenderer.setLight1();
                    toogleLight = false;
                }
                break;
        }
        return true;
    }
}