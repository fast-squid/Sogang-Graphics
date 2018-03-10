package com.graphics.sg.opengles_tiger;

import android.opengl.GLES20;

public class Floor extends Object {
    /**
     * 여러개의 Vertex Buffer Object를 이용해서 하나의 물체를 그리는 방식의 오브젝트.
     */
    float mVertices[] = {
            0.0f, 0.0f, 0.0f,   // top left
            1.0f, 0.0f, 0.0f,   // bottom left
            1.0f, 1.0f, 0.0f,   // bottom right
            0.0f, 1.0f, 0.0f }; // top right
    float mNormals[] = {
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f };
    float mTexCoords[] = {
            0.0f, 0.0f,
            4.0f, 0.0f,
            4.0f, 4.0f,
            0.0f, 4.0f };
    short mIndices[] = { 0, 1, 2, 0, 2, 3 };

    public void prepare() {
        GLES20.glGenBuffers(4, mVBO, 0);

        // 정점의 좌표 데이터를 glBuffer로 생성.
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVBO[0]);
        GLES20.glBufferData(
                GLES20.GL_ARRAY_BUFFER,
                mVertices.length * BufferConverter.SIZE_OF_FLOAT,
                BufferConverter.floatArrayToBuffer(mVertices),
                GLES20.GL_STATIC_DRAW);

        // 정점의 법선 데이터를 glBuffer로 생성.
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVBO[1]);
        GLES20.glBufferData(
                GLES20.GL_ARRAY_BUFFER,
                mNormals.length * BufferConverter.SIZE_OF_FLOAT,
                BufferConverter.floatArrayToBuffer(mNormals),
                GLES20.GL_STATIC_DRAW);

        // 정점의 텍스쳐 좌표값을 glBuffer로 생성.
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVBO[2]);
        GLES20.glBufferData(
                GLES20.GL_ARRAY_BUFFER,
                mTexCoords.length * BufferConverter.SIZE_OF_FLOAT,
                BufferConverter.floatArrayToBuffer(mTexCoords),
                GLES20.GL_STATIC_DRAW);

        // 정점의 출력 인덱스 데이터를 glBuffer로 생성.
        GLES20.glBindBuffer(GLES20.GL_ELEMENT_ARRAY_BUFFER, mVBO[3]);
        GLES20.glBufferData(
                GLES20.GL_ELEMENT_ARRAY_BUFFER,
                mIndices.length * BufferConverter.SIZE_OF_SHORT,
                BufferConverter.shortArrayToBuffer(mIndices),
                GLES20.GL_STATIC_DRAW);


    }

    void draw(int pId) {

        int loc_v_position = GLES20.glGetAttribLocation(pId, "v_position");
        int loc_v_normal = GLES20.glGetAttribLocation(pId, "v_normal");
        int loc_v_texcoord = GLES20.glGetAttribLocation(pId, "v_tex_coord");

        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVBO[0]);
        GLES20.glVertexAttribPointer(loc_v_position, VERTEX_POS_SIZE, GLES20.GL_FLOAT, false, VERTEX_POS_SIZE * BufferConverter.SIZE_OF_FLOAT, 0);
        GLES20.glEnableVertexAttribArray(loc_v_position);
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVBO[1]);
        GLES20.glVertexAttribPointer(loc_v_normal, VERTEX_NORM_SIZE, GLES20.GL_FLOAT, false, VERTEX_NORM_SIZE * BufferConverter.SIZE_OF_FLOAT, 0);
        GLES20.glEnableVertexAttribArray(loc_v_normal);
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, mVBO[2]);
        GLES20.glVertexAttribPointer(loc_v_texcoord, VERTEX_TEX_SIZE, GLES20.GL_FLOAT, false, VERTEX_TEX_SIZE * BufferConverter.SIZE_OF_FLOAT, 0);
        GLES20.glEnableVertexAttribArray(loc_v_texcoord);
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);
        GLES20.glDrawElements(GLES20.GL_TRIANGLES, mIndices.length, GLES20.GL_UNSIGNED_SHORT, 0);
        /**/
    }
}
