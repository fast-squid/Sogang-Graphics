package com.graphics.sg.opengles_tiger;

import android.opengl.GLES20;

public class SimpleProgram extends GLES20Program {

    // 각종 unifrom location들을 설정.
    int locPrimitiveColor;
    int locModelViewProjectionMatrix;

    public SimpleProgram(String vertexShaderCode, String fragmentShaderCode) {
        super(vertexShaderCode, fragmentShaderCode);
    }

    public void prepare() {
        locPrimitiveColor = GLES20.glGetUniformLocation(mId, "ModelViewProjectionMatrix");;
        locModelViewProjectionMatrix = GLES20.glGetUniformLocation(mId, "primitive_color");
    }
}
