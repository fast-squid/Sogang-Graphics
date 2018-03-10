package com.graphics.sg.opengles_tiger;

import android.opengl.GLES20;

public class Axis extends Object {
    float mVertices[] = {
            // X 축.
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            // Y 축.
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            // Z 축.
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f
    };
    float mColors[][] = {
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f }
    };

    public void prepare(){
        GLES20.glGenBuffers(1, mVBO, 0);

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVBO[0]);
        GLES20.glBufferData(
                GLES20.GL_ARRAY_BUFFER,
                mVertices.length * BufferConverter.SIZE_OF_FLOAT,
                BufferConverter.floatArrayToBuffer(mVertices),
                GLES20.GL_STATIC_DRAW);
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);

    }

    public void draw(int pId, float[] mvpMatrix) {
        int loc_mvp_matrix = GLES20.glGetUniformLocation(pId, "ModelViewProjectionMatrix");
        GLES20.glUniformMatrix4fv(loc_mvp_matrix, 1, false, mvpMatrix, 0);
        int loc_primitive_color = GLES20.glGetUniformLocation(pId, "primitive_color");
        int loc_v_position = GLES20.glGetAttribLocation(pId, "v_position");


        GLES20.glLineWidth(3.0f);


        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVBO[0]);
        GLES20.glVertexAttribPointer(loc_v_position, 3, GLES20.GL_FLOAT, false, 0, 0);
        GLES20.glEnableVertexAttribArray(loc_v_position);

        GLES20.glUniform3fv(loc_primitive_color, 1, BufferConverter.floatArrayToBuffer(mColors[0]));
        GLES20.glDrawArrays(GLES20.GL_LINES, 0, 2);
        GLES20.glUniform3fv(loc_primitive_color, 1, BufferConverter.floatArrayToBuffer(mColors[1]));
        GLES20.glDrawArrays(GLES20.GL_LINES, 2, 2);
        GLES20.glUniform3fv(loc_primitive_color, 1, BufferConverter.floatArrayToBuffer(mColors[2]));
        GLES20.glDrawArrays(GLES20.GL_LINES, 4, 2);
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);
        /**/

        GLES20.glLineWidth(1.0f);
    }
}
