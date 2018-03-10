package com.graphics.sg.opengles_tiger;

import android.opengl.GLES20;

public class Circle extends Object {
    float mVertices[] = new float[840*3];
    float mColors[][] = {
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f }
    };

    public void prepare(){
        int i,j;
        for (i = 0; i < 21; i++) {
            for (j = 0; j < 40; j++) {
                double theta;
                double phi;
                theta = j * 2 * 3.141f / 40;
                phi = i * 3.141f / 21 - (3.141f / 2);
                mVertices[j*3+i*40*3+0] = 4 * (float)Math.cos(theta)*(float)Math.cos(phi);
                mVertices[j*3+i*40*3+1] = 4 * (float)Math.sin(theta)*(float)Math.cos(phi);
                mVertices[j*3+i*40*3+2] = 4 * (float)Math.sin(phi);
            }
        }
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
        int i;

        GLES20.glLineWidth(3.0f);


        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVBO[0]);
        GLES20.glVertexAttribPointer(loc_v_position, 3, GLES20.GL_FLOAT, false, 0, 0);
        GLES20.glEnableVertexAttribArray(loc_v_position);

        GLES20.glUniform3fv(loc_primitive_color, 1, BufferConverter.floatArrayToBuffer(mColors[0]));
        for(i=0; i<840;i+=21) {
            GLES20.glDrawArrays(GLES20.GL_LINES, i, 21);
            GLES20.glDrawArrays(GLES20.GL_LINES, i+1, 21);
        }
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);

        GLES20.glLineWidth(1.0f);
    }
}
