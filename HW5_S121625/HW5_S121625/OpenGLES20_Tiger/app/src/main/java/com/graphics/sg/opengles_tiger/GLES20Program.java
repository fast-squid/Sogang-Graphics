package com.graphics.sg.opengles_tiger;

import android.opengl.GLES20;
import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;

public class GLES20Program {

    protected int mId;
    public int getProgramID(){ return mId; }

    /**
     * Vertex Shader와 Fragment Shader만을 사용하는 기초적인 glProgram.
     * @param vertexShaderCode String 타입의 vertex shader 코드.
     * @param fragmentShaderCode String 타입의 fragment shader 코드.
     */
    public GLES20Program(String vertexShaderCode, String fragmentShaderCode) {

        int vsHandle = loadShader(GLES20.GL_VERTEX_SHADER, vertexShaderCode);
        int fsHandle = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentShaderCode);

        mId = GLES20.glCreateProgram();
        GLES20.glAttachShader(mId, vsHandle);
        GLES20.glAttachShader(mId, fsHandle);
        GLES20.glLinkProgram(mId);

        int linked[] = new int[1];
        GLES20.glGetProgramiv(mId, GLES20.GL_LINK_STATUS, linked, 0);
        if(linked[0] == 0) {
            String infoLog = GLES20.glGetProgramInfoLog(mId);
            Log.e("Program", infoLog);
            GLES20.glDeleteProgram(mId);
        }
    }

    public int use() {
        GLES20.glUseProgram(mId);
        return mId;
    }

    private int loadShader(int type, String shaderCode) {
        // 빈 쉐이더를 생성하고 그 인덱스를 할당.
        int shader = GLES20.glCreateShader(type);

        // 컴파일 결과를 받을 공간을 생성.
        IntBuffer compiled = ByteBuffer.allocateDirect(4).order(ByteOrder.nativeOrder()).asIntBuffer();
        String shaderType;

        // 컴파일 결과를 출력하기 위해 쉐이더를 구분.
        if (type == GLES20.GL_VERTEX_SHADER)
            shaderType = "Vertex";
        else if (type == GLES20.GL_FRAGMENT_SHADER)
            shaderType = "Fragment";
        else
            shaderType = "Unknown";

        // 빈 쉐이더에 소스코드를 할당.
        GLES20.glShaderSource(shader, shaderCode);
        // 쉐이더에 저장 된 소스코드를 컴파일
        GLES20.glCompileShader(shader);

        // 컴파일 결과 오류가 발생했는지를 확인.
        GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS, compiled);
        // 컴파일 에러가 발생했을 경우 이를 출력.
        if (compiled.get(0) == 0) {
            GLES20.glGetShaderiv(shader, GLES20.GL_INFO_LOG_LENGTH, compiled);
            if (compiled.get(0) > 1) {
                Log.e("Shader", shaderType + " shader: " + GLES20.glGetShaderInfoLog(shader));
            }
            GLES20.glDeleteShader(shader);
            Log.e("Shader", shaderType + " shader compile error.");
        }

        // 완성된 쉐이더의 인덱스를 리턴.
        return shader;
    }
}
