package com.graphics.sg.opengles_tiger;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.renderscript.Matrix4f;

import java.text.DecimalFormat;
import java.util.Vector;

import javax.microedition.khronos.opengles.GL10;

public class GLES20Renderer implements GLSurfaceView.Renderer{
    private Context mContext;

    private Axis mAxis;
    private Floor mFloor,mWall;
    private Tiger mTiger;
    private Dragon mDragon;
    private Optimus mOptimus;
    private Circle mCircle;

    public float fovy = 45.0f;
    private float ratio = 1.0f;

    // Animation frame index
    final static int N_TIGER_FRAMES = 12;
    int cur_frame_tiger = 0;
    int rotation_angle_tiger = 0;

    public float[] mMVPMatrix = new float[16];
    public float[] mProjectionMatrix = new float[16];
    public float[] mModelViewMatrix = new float[16];
    public float[] mModelMatrix = new float[16];
    public float[] mViewMatrix = new float[16];
    public float[] mModelViewInvTrans = new float[16];
    public float[] mCameraPos = new float[4];
    public float[] mCameraVAxis = new float[4];
    public float[] mCarPos = {200,20,200,1};
    public float[] mCarDir = {1,0,0,0};
    public float[] mCarTurn = {1,0,0,0,
            0,1,0,0,
            0,0,1,0,
            0,0,0,1};
    public float mCarTurnAngle=0;
    public float mCarRollAngle=0;
    // Texture
    final static int TEXTURE_ID_FLOOR = 0;
    final static int TEXTURE_ID_TIGER = 1;

    // OpenGL Handles
    private PhongShadingProgram mPhongShadingProgram;
    private SimpleProgram mSimpleProgram;

    public GLES20Renderer(Context context) {
        mContext = context;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, javax.microedition.khronos.egl.EGLConfig config) {
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        GLES20.glEnable(GLES20.GL_DEPTH_TEST);

        // matrix, offset, eye xyz, center xyz, top xyz.
        // View Matrix 설정. 고정 된 카메라이기 때문에 한 번만 호출한다.
        mCameraPos[0] = 300.0f;
        mCameraPos[1] = 300.0f;
        mCameraPos[2] = 300.0f;
        mCameraPos[3] = 0;
        mCameraVAxis[0] = -1.0f;
        mCameraVAxis[1] = 1.0f;
        mCameraVAxis[2] = -1.0f;
        mCameraVAxis[3] = 0.0f;

        Matrix.setLookAtM(mViewMatrix, 0, mCameraPos[0], mCameraPos[1], mCameraPos[2], 0f, 0f, -250f, mCameraVAxis[0], mCameraVAxis[1], mCameraVAxis[2]);

        // GL Program을 생성.
        // phong shading을 수행하는 프로그램.
        mPhongShadingProgram = new PhongShadingProgram(
                AssetReader.readFromFile("phong.vert", mContext),
                AssetReader.readFromFile("phong.frag", mContext));
        mPhongShadingProgram.prepare();
        mPhongShadingProgram.initLightsAndMaterial();
        mPhongShadingProgram.initFlags();
        mPhongShadingProgram.set_up_scene_lights(mViewMatrix);

        // 축을 그릴 때 사용하는 프로그램.
        mSimpleProgram = new SimpleProgram(
                AssetReader.readFromFile("simple.vert", mContext),
                AssetReader.readFromFile("simple.frag", mContext));
        mSimpleProgram.prepare();

        mAxis = new Axis();
        mAxis.prepare();

        // 바닥 오브젝트 로드.
        mFloor = new Floor();
        mFloor.prepare();
        mFloor.setTexture(AssetReader.getBitmapFromFile("grass_tex.jpg", mContext), TEXTURE_ID_FLOOR);
        mWall = new Floor();
        mWall.prepare();
        mWall.setTexture(AssetReader.getBitmapFromFile("wall.jpg", mContext), TEXTURE_ID_FLOOR);

        // Tiger 오브젝트 로드.
        int nBytesPerVertex = 8 * 4;        // 3 for vertex, 3 for normal, 2 for texcoord, 4 is sizeof(float)
        int nBytesPerTriangles = nBytesPerVertex * 3;

        mTiger = new Tiger();
        for (int i = 0; i < N_TIGER_FRAMES; i++) {
            DecimalFormat dfm = new DecimalFormat("00");
            String num = dfm.format(i);
            String filename = "Tiger_" + num + "_triangles_vnt.geom";
            mTiger.addGeometry(AssetReader.readGeometry(filename, nBytesPerTriangles, mContext));
        }
        mTiger.prepare();
        mTiger.setTexture(AssetReader.getBitmapFromFile("tiger_tex.jpg", mContext), TEXTURE_ID_TIGER);

        mDragon = new Dragon();
        String filename = "dragon_vnt.geom";
        mDragon.addGeometry(AssetReader.readGeometry(filename, nBytesPerTriangles, mContext));
        mDragon.prepare();
        mDragon.setTexture(AssetReader.getBitmapFromFile("sky.jpg", mContext), TEXTURE_ID_TIGER);

        mOptimus = new Optimus();
        filename = "optimus_vnt.geom";
        mOptimus.addGeometry(AssetReader.readGeometry(filename, nBytesPerTriangles, mContext));
        mOptimus.prepare();
        mOptimus.setTexture(AssetReader.getBitmapFromFile("dragon.jpg", mContext), TEXTURE_ID_TIGER);
        mCircle = new Circle();
        mCircle.prepare();
    }

    int init_move =0;
    double mov_dir[][] = new double[3][3];
    float rot_dir[][] = new float[3][4];
    float rot_mat[][] = new float[2][16];
    @Override
    public void onDrawFrame(GL10 gl) {
        int pId;
        int timestamp = getTimeStamp();
        float light_dir[] = new float[4];

        cur_frame_tiger = timestamp % N_TIGER_FRAMES;
        rotation_angle_tiger = timestamp % 360;

        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);

        mPhongShadingProgram.set_lights1();
        //Matrix.perspectiveM(mProjectionMatrix, 0, 45.0f, ratio, 0.1f, 2000.0f);
        perspectiveM(mProjectionMatrix, 0, fovy, ratio, 0.1f, 2000.0f);

        // Simple Program을 이용해서 축을 그린다.
        mSimpleProgram.use();
        pId = mSimpleProgram.getProgramID();
         // Axis 그리기 영역.
        Matrix.setIdentityM(mModelMatrix, 0);
        Matrix.scaleM(mModelMatrix, 0, 50f, 50f, 50f);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);

        mAxis.draw(pId, mMVPMatrix);

        // 생성한 glProgram을 이용해서 오브젝트들을 화면에 그린다.
        mPhongShadingProgram.use();
        pId = mPhongShadingProgram.getProgramID();

        GLES20.glUniform1i(mPhongShadingProgram.locFlagFog, mPhongShadingProgram.mFlagFog);
        GLES20.glUniform1i(mPhongShadingProgram.locFlagTextureMapping, mPhongShadingProgram.mFlagTextureMapping);

        // Model Matrix 설정.
        Matrix.setIdentityM(mModelMatrix, 0);
        Matrix.translateM(mModelMatrix, 0, -250f, 0f, 250f);
        Matrix.scaleM(mModelMatrix, 0, 1000f, 1000f, 1000f);
        Matrix.rotateM(mModelMatrix, 0, -90.0f, 1f, 0f, 0f);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        Matrix.invertM(mModelViewInvTrans, 0, mModelViewMatrix, 0);
        Matrix.transposeM(mModelViewInvTrans, 0, mModelViewInvTrans, 0);

        GLES20.glUniformMatrix4fv(mPhongShadingProgram.locModelViewProjectionMatrix, 1, false, mMVPMatrix, 0);
        GLES20.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrix, 1, false, mModelViewMatrix, 0);
        GLES20.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrixInvTrans, 1, false, mModelViewInvTrans, 0);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mFloor.mTexId[0]);
        GLES20.glUniform1i(mPhongShadingProgram.locTexture, 1);

        mPhongShadingProgram.setUpMaterialFloor();
        mFloor.draw(pId);



        Matrix.setIdentityM(mModelMatrix, 0);
        Matrix.translateM(mModelMatrix, 0, -250f, 0f, -250f);
        Matrix.scaleM(mModelMatrix, 0, 500f, 500f, 500f);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        Matrix.invertM(mModelViewInvTrans, 0, mModelViewMatrix, 0);
        Matrix.transposeM(mModelViewInvTrans, 0, mModelViewInvTrans, 0);

        GLES20.glUniformMatrix4fv(mPhongShadingProgram.locModelViewProjectionMatrix, 1, false, mMVPMatrix, 0);
        GLES20.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrix, 1, false, mModelViewMatrix, 0);
        GLES20.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrixInvTrans, 1, false, mModelViewInvTrans, 0);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mWall.mTexId[0]);
        GLES20.glUniform1i(mPhongShadingProgram.locTexture, 1);

        mPhongShadingProgram.setUpMaterialFloor();
        mWall.draw(pId);

        GLES20.glEnable(GLES20.GL_BLEND);
        GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA,GLES20.GL_ONE_MINUS_CONSTANT_ALPHA);

        // Tiger 그리기 영역.
        Matrix.setIdentityM(mModelMatrix, 0);
        Matrix.rotateM(mModelMatrix, 0, -rotation_angle_tiger, 0f, 1f, 0f);
        Matrix.translateM(mModelMatrix, 0, 200.0f, 0.0f, 0.0f);
        Matrix.rotateM(mModelMatrix, 0, -90.0f, 1.0f, 0.0f, 0.0f);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        Matrix.transposeM(mModelViewInvTrans, 0, mModelViewMatrix, 0);
        Matrix.invertM(mModelViewInvTrans, 0, mModelViewInvTrans, 0);

        GLES20.glUniformMatrix4fv(mPhongShadingProgram.locModelViewProjectionMatrix, 1, false, mMVPMatrix, 0);
        GLES20.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrix, 1, false, mModelViewMatrix, 0);
        GLES20.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrixInvTrans, 1, false, mModelViewInvTrans, 0);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTiger.mTexId[0]);
        GLES20.glUniform1i(mPhongShadingProgram.locFlagTextureMapping, 1);


        mPhongShadingProgram.setUpMaterialTiger();
        mTiger.draw(pId, cur_frame_tiger);
        GLES20.glDisable(GLES20.GL_BLEND);

        float positionEC[] = new float[4];
        Matrix.translateM(mMVPMatrix, 0, 0.0f, 200.0f, 0.0f);
        Matrix.multiplyMV(positionEC,0,mModelMatrix,0,mPhongShadingProgram.light[3].position,0);
        GLES20.glUniform4fv(mPhongShadingProgram.locLight[3].position, 1, BufferConverter.floatArrayToBuffer(positionEC));




        //Dragon 그리기 영역
        Matrix.setIdentityM(mModelMatrix, 0);
        Matrix.scaleM(mModelMatrix, 0, 10f, 10f, 10f);
        Matrix.rotateM(mModelMatrix, 0, -90, 1f, 0f, 0f);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        Matrix.transposeM(mModelViewInvTrans, 0, mModelViewMatrix, 0);
        Matrix.invertM(mModelViewInvTrans, 0, mModelViewInvTrans, 0);

        GLES20.glUniformMatrix4fv(mPhongShadingProgram.locModelViewProjectionMatrix, 1, false, mMVPMatrix, 0);
        GLES20.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrix, 1, false, mModelViewMatrix, 0);
        GLES20.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrixInvTrans, 1, false, mModelViewInvTrans, 0);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mDragon.mTexId[0]);
        GLES20.glUniform1i(mPhongShadingProgram.locFlagTextureMapping, 1);
        mDragon.draw(pId,0);

        //optimus 그리기 영역
        Matrix.setIdentityM(mModelMatrix, 0);
        Matrix.scaleM(mModelMatrix, 0, 0.3f, 0.3f, 0.3f);
        Matrix.rotateM(mModelMatrix, 0, -90, 1f, 0f, 0f);
        Matrix.translateM(mModelMatrix, 0, 200.0f, 2000.0f, 0.0f);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        Matrix.transposeM(mModelViewInvTrans, 0, mModelViewMatrix, 0);
        Matrix.invertM(mModelViewInvTrans, 0, mModelViewInvTrans, 0);

        GLES20.glUniformMatrix4fv(mPhongShadingProgram.locModelViewProjectionMatrix, 1, false, mMVPMatrix, 0);
        GLES20.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrix, 1, false, mModelViewMatrix, 0);
        GLES20.glUniformMatrix4fv(mPhongShadingProgram.locModelViewMatrixInvTrans, 1, false, mModelViewInvTrans, 0);

        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mOptimus.mTexId[0]);
        GLES20.glUniform1i(mPhongShadingProgram.locFlagTextureMapping, 1);
        mOptimus.draw(pId,0);



        if (init_move == 0) {
            for (int i = 0; i < 3; i++) {
                mov_dir[i][0] = (float)i+1.0f; mov_dir[i][1] = 0; mov_dir[i][2] = 1.0f;
                rot_dir[i][0] = (float)i+1.0f; rot_dir[i][1] = 1.0f; rot_dir[i][2] = 1.0f; rot_dir[i][3] = 0.0f;
            }
            init_move = 1;
        }

        int i;
        for (i = 1; i <= 2; i++) {
            if (500 - mPhongShadingProgram.light[i].position[0] <5) { mov_dir[i - 1][0] *= -1;}
            else if (mPhongShadingProgram.light[i].position[0] < -500) { mov_dir[i - 1][0] *= -1; }
            mPhongShadingProgram.light[i].position[0] += mov_dir[i - 1][0];

            if (500 - mPhongShadingProgram.light[i].position[2] <5) { mov_dir[i - 1][2] *= -1; }
            else if (mPhongShadingProgram.light[i].position[2] < -500) { mov_dir[i - 1][2] *= -1; }
            mPhongShadingProgram.light[i].position[2] += mov_dir[i - 1][2];
            float position_EC[] = new float[4];
            Matrix.multiplyMV(position_EC,0,mViewMatrix,0,mPhongShadingProgram.light[i].position,0);
            GLES20.glUniform4fv(mPhongShadingProgram.locLight[i].position, 1, BufferConverter.floatArrayToBuffer(position_EC));

        }

        Matrix.setIdentityM(rot_mat[0], 0);
        Matrix.setIdentityM(rot_mat[1], 0);
        for ( i = 1; i <= 2; i++) {
            light_dir[0]= mPhongShadingProgram.light[i].spot_direction[0];  light_dir[1]= mPhongShadingProgram.light[i].spot_direction[1];
            light_dir[2]= mPhongShadingProgram.light[2].spot_direction[2];  light_dir[3]= 0.0f;
            Matrix.rotateM(rot_mat[i-1], 0, 1.7f, rot_dir[i][0], rot_dir[i][1], rot_dir[i][2]);
            Matrix.multiplyMV(light_dir,0,rot_mat[i-1],0,light_dir,0);
            float direction_EC[] = new float[4];
            Matrix.multiplyMV(direction_EC,0,mViewMatrix,0,light_dir,0);
            float dirEC[] = new float[3];
            dirEC[0]=direction_EC[0];   dirEC[1]=direction_EC[1];   dirEC[2]=direction_EC[2];
            GLES20.glUniform3fv(mPhongShadingProgram.locLight[i].spot_direction, 1, BufferConverter.floatArrayToBuffer(dirEC));
            mPhongShadingProgram.light[i].spot_direction[0]=light_dir[0];   mPhongShadingProgram.light[i].spot_direction[1]=light_dir[1];
            mPhongShadingProgram.light[i].spot_direction[2]=light_dir[2];
         }




        // Axis 그리기 영역.

        Matrix.scaleM(mMVPMatrix, 0, 20f, 20f, 20f);
        mAxis.draw(pId, mMVPMatrix);
        // Hierarchy
        float temp[] = {1,0,0,0};

        Matrix.rotateM(mCarTurn,0,30*0.017f,0,0,1);

        Matrix.multiplyMV(mCarDir,0,mCarTurn,0,temp,0);
        mCarPos[0]+=mCarDir[0]; mCarPos[2]+=mCarDir[2];

        Matrix.setIdentityM(mModelMatrix, 0);
        Matrix.translateM(mModelMatrix,0,mCarPos[0]+20,20,mCarPos[1]+20);
        Matrix.rotateM(mModelMatrix,0,mCarPos[0]+mCarPos[2],0,0,-1);
        Matrix.scaleM(mModelMatrix, 0, 5f, 5f, 5f);
        Matrix.rotateM(mModelMatrix,0,90,0,1,0);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        mCircle.draw(pId, mMVPMatrix);

        Matrix.setIdentityM(mModelMatrix, 0);
        Matrix.translateM(mModelMatrix,0,mCarPos[0]+20,20,mCarPos[1]-20);
        Matrix.rotateM(mModelMatrix,0,mCarPos[0]+mCarPos[2],0,0,-1);
        Matrix.scaleM(mModelMatrix, 0, 5f, 5f, 5f);
        Matrix.rotateM(mModelMatrix,0,90,0,1,0);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        mCircle.draw(pId, mMVPMatrix);

        Matrix.setIdentityM(mModelMatrix, 0);
        Matrix.translateM(mModelMatrix,0,mCarPos[0]-20,20,mCarPos[1]+20);
        Matrix.rotateM(mModelMatrix,0,mCarPos[0]+mCarPos[2],0,0,-1);
        Matrix.scaleM(mModelMatrix, 0, 5f, 5f, 5f);
        Matrix.rotateM(mModelMatrix,0,90,0,1,0);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        mCircle.draw(pId, mMVPMatrix);

        Matrix.setIdentityM(mModelMatrix, 0);
        Matrix.translateM(mModelMatrix,0,mCarPos[0]-20,20,mCarPos[1]-20);
        Matrix.rotateM(mModelMatrix,0,mCarPos[0]+mCarPos[2],0,0,-1);
        Matrix.scaleM(mModelMatrix, 0, 5f, 5f, 5f);
        Matrix.rotateM(mModelMatrix,0,90,0,1,0);
        Matrix.multiplyMM(mModelViewMatrix, 0, mViewMatrix, 0, mModelMatrix, 0);
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mModelViewMatrix, 0);
        mCircle.draw(pId, mMVPMatrix);

    }


    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        GLES20.glViewport(0, 0, width, height);

        ratio = (float) width / height;

        //Matrix.perspectiveM(mProjectionMatrix, 0, 45.0f, ratio, 0.1f, 2000.0f);
        perspectiveM(mProjectionMatrix, 0, fovy, ratio, 0.1f, 2000.0f);
    }

    /**
     * Defines a projection matrix in terms of a field of view angle, an
     * aspect ratio, and z clip planes.
     *
     * @param m the float array that holds the perspective matrix
     * @param offset the offset into float array m where the perspective
     *        matrix data is written
     * @param fovy field of view in y direction, in degrees
     * @param aspect width to height aspect ratio of the viewport
     * @param zNear
     * @param zFar
     */
    public static void perspectiveM(float[] m, int offset,
                                    float fovy, float aspect, float zNear, float zFar) {
        float f = 1.0f / (float) Math.tan(fovy * (Math.PI / 360.0));
        float rangeReciprocal = 1.0f / (zNear - zFar);

        m[offset + 0] = f / aspect;
        m[offset + 1] = 0.0f;
        m[offset + 2] = 0.0f;
        m[offset + 3] = 0.0f;

        m[offset + 4] = 0.0f;
        m[offset + 5] = f;
        m[offset + 6] = 0.0f;
        m[offset + 7] = 0.0f;

        m[offset + 8] = 0.0f;
        m[offset + 9] = 0.0f;
        m[offset + 10] = (zFar + zNear) * rangeReciprocal;
        m[offset + 11] = -1.0f;

        m[offset + 12] = 0.0f;
        m[offset + 13] = 0.0f;
        m[offset + 14] = 2.0f * zFar * zNear * rangeReciprocal;
        m[offset + 15] = 0.0f;
    }

    static int prevTimeStamp = 0;
    static int currTimeStamp = 0;
    static int totalTimeStamp = 0;

    private int getTimeStamp() {
        Long tsLong = System.currentTimeMillis() / 100;

        currTimeStamp = tsLong.intValue();
        if(prevTimeStamp != 0) {
            totalTimeStamp += (currTimeStamp - prevTimeStamp);
        }
        prevTimeStamp = currTimeStamp;




        return totalTimeStamp;
    }


    // CheckBox 조작 관련 함수.
    public void toggleTexture(int type, boolean onTexture) {
        int toggleVal;
        if(onTexture) toggleVal = 1;
        else          toggleVal = 0;

        if(type == 0)           // Texture
            mPhongShadingProgram.mFlagTextureMapping = toggleVal;
        else                    // Fog
            mPhongShadingProgram.mFlagFog = toggleVal;
    }

    public void setLight1() {
        mPhongShadingProgram.light[1].light_on = 1 - mPhongShadingProgram.light[1].light_on;
    }

    public void setView(float in_fovy) {
        fovy = in_fovy;
    }
}