package com.graphics.sg.opengles_tiger;

import android.opengl.GLES20;
import android.opengl.Matrix;

class LightParameters {
        int light_on;
        float position[] = new float[4];
        float ambient_color[] = new float[4];
        float diffuse_color[] = new float[4];
        float specular_color[] = new float[4];
        float spot_direction[] = new float[3];
        float spot_exponent;
        float spot_cutoff_angle;
        float blind_width;
};

class MaterialParameters {
    float ambient_color[] = new float[4];
    float diffuse_color[] = new float[4];
    float specular_color[] = new float[4];
    float emissive_color[] = new float[4];
    float specular_exponent;
}

class LocLightParameter {
    int light_on;
    int position;
    int ambient_color, diffuse_color, specular_color;
    int spot_direction;
    int spot_exponent;
    int spot_cutoff_angle;
    int light_attenuation_factors;
    int blind_width;
}

class LocMaterialParameter {
    int ambient_color, diffuse_color, specular_color, emissive_color;
    int specular_exponent;
}

public class PhongShadingProgram extends GLES20Program {

    final static int NUMBER_OF_LIGHT_SUPPORTED = 4;

    int locModelViewProjectionMatrix;
    int locModelViewMatrix;
    int locModelViewMatrixInvTrans;

    int locGlobalAmbientColor;
    LocLightParameter locLight[];
    LocMaterialParameter locMaterial = new LocMaterialParameter();
    int locTexture;
    int locFlagTextureMapping;
    int locFlagFog;

    int mFlagFog;
    int mFlagTextureMapping;

    LightParameters light[];
    MaterialParameters materialFloor = new MaterialParameters();
    MaterialParameters materialTiger = new MaterialParameters();

    public PhongShadingProgram(String vertexShaderCode, String fragmentShaderCode) {
        super(vertexShaderCode, fragmentShaderCode);
    }

    /**
     * GLProgram에 결합 된 Shader 내 변수들의 location 인덱스를 설정하는 함수.
     */
    public void prepare() {
        locLight = new LocLightParameter[NUMBER_OF_LIGHT_SUPPORTED];
        for(int i=0 ; i<NUMBER_OF_LIGHT_SUPPORTED ; i++)
            locLight[i] = new LocLightParameter();

        locModelViewProjectionMatrix = GLES20.glGetUniformLocation(mId, "ModelViewProjectionMatrix");
        locModelViewMatrix = GLES20.glGetUniformLocation(mId, "ModelViewMatrix");
        locModelViewMatrixInvTrans = GLES20.glGetUniformLocation(mId, "ModelViewMatrixInvTrans");

        locTexture = GLES20.glGetUniformLocation(mId, "base_texture");

        locFlagTextureMapping = GLES20.glGetUniformLocation(mId, "flag_texture_mapping");
        locFlagFog = GLES20.glGetUniformLocation(mId, "flag_fog");

        locGlobalAmbientColor = GLES20.glGetUniformLocation(mId, "global_ambient_color");
        for (int i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
            String lightNumStr = "light[" + i + "]";
            locLight[i].light_on = GLES20.glGetUniformLocation(mId, lightNumStr + ".light_on");
            locLight[i].position = GLES20.glGetUniformLocation(mId, lightNumStr + ".position");
            locLight[i].ambient_color = GLES20.glGetUniformLocation(mId, lightNumStr + ".ambient_color");
            locLight[i].diffuse_color = GLES20.glGetUniformLocation(mId, lightNumStr + ".diffuse_color");
            locLight[i].specular_color = GLES20.glGetUniformLocation(mId, lightNumStr + ".specular_color");
            locLight[i].spot_direction = GLES20.glGetUniformLocation(mId, lightNumStr + ".spot_direction");
            locLight[i].spot_exponent = GLES20.glGetUniformLocation(mId, lightNumStr + ".spot_exponent");
            locLight[i].spot_cutoff_angle = GLES20.glGetUniformLocation(mId, lightNumStr + ".spot_cutoff_angle");
            locLight[i].light_attenuation_factors = GLES20.glGetUniformLocation(mId, lightNumStr + ".light_attenuation_factors");
            locLight[i].blind_width = GLES20.glGetUniformLocation(mId, lightNumStr + ".blind_width");
        }

        locMaterial.ambient_color = GLES20.glGetUniformLocation(mId, "material.ambient_color");
        locMaterial.diffuse_color = GLES20.glGetUniformLocation(mId, "material.diffuse_color");
        locMaterial.specular_color = GLES20.glGetUniformLocation(mId, "material.specular_color");
        locMaterial.emissive_color = GLES20.glGetUniformLocation(mId, "material.emissive_color");
        locMaterial.specular_exponent = GLES20.glGetUniformLocation(mId, "material.specular_exponent");
    }

    /**
     * Light와 Material의 값을 설정하는 함수.
     */
    public void initLightsAndMaterial() {
        GLES20.glUseProgram(mId);

        GLES20.glUniform4f(locGlobalAmbientColor, 0.115f, 0.115f, 0.115f, 1.0f);
        for (int i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
            GLES20.glUniform1i(locLight[i].light_on, 0); // turn off all lights initially
            GLES20.glUniform4f(locLight[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
            GLES20.glUniform4f(locLight[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
            if (i == 0) {
                GLES20.glUniform4f(locLight[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
                GLES20.glUniform4f(locLight[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
            }
            else {
                GLES20.glUniform4f(locLight[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
                GLES20.glUniform4f(locLight[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
            }
            GLES20.glUniform3f(locLight[i].spot_direction, 0.0f, 0.0f, -1.0f);
            GLES20.glUniform1f(locLight[i].spot_exponent, 0.0f); // [0.0, 128.0]
            GLES20.glUniform1f(locLight[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
            GLES20.glUniform4f(locLight[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
        }

        GLES20.glUniform4f(locMaterial.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
        GLES20.glUniform4f(locMaterial.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
        GLES20.glUniform4f(locMaterial.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
        GLES20.glUniform4f(locMaterial.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
        GLES20.glUniform1f(locMaterial.specular_exponent, 0.0f); // [0.0, 128.0]

        GLES20.glUseProgram(0);


        // Material 설정.
        materialFloor.ambient_color[0] = 0.0f;
        materialFloor.ambient_color[1] = 0.05f;
        materialFloor.ambient_color[2] = 0.0f;
        materialFloor.ambient_color[3] = 1.0f;

        materialFloor.diffuse_color[0] = 0.2f;
        materialFloor.diffuse_color[1] = 0.5f;
        materialFloor.diffuse_color[2] = 0.2f;
        materialFloor.diffuse_color[3] = 1.0f;

        materialFloor.specular_color[0] = 0.24f;
        materialFloor.specular_color[1] = 0.5f;
        materialFloor.specular_color[2] = 0.24f;
        materialFloor.specular_color[3] = 1.0f;

        materialFloor.specular_exponent = 2.5f;

        materialFloor.emissive_color[0] = 0.0f;
        materialFloor.emissive_color[1] = 0.0f;
        materialFloor.emissive_color[2] = 0.0f;
        materialFloor.emissive_color[3] = 1.0f;


        materialTiger.ambient_color[0] = 0.24725f;
        materialTiger.ambient_color[1] = 0.1995f;
        materialTiger.ambient_color[2] = 0.0745f;
        materialTiger.ambient_color[3] = 1.0f;

        materialTiger.diffuse_color[0] = 0.75164f;
        materialTiger.diffuse_color[1] = 0.60648f;
        materialTiger.diffuse_color[2] = 0.22648f;
        materialTiger.diffuse_color[3] = 1.0f;

        materialTiger.specular_color[0] = 0.728281f;
        materialTiger.specular_color[1] = 0.655802f;
        materialTiger.specular_color[2] = 0.466065f;
        materialTiger.specular_color[3] = 1.0f;

        materialTiger.specular_exponent = 51.2f;

        materialTiger.emissive_color[0] = 0.1f;
        materialTiger.emissive_color[1] = 0.1f;
        materialTiger.emissive_color[2] = 0.0f;
        materialTiger.emissive_color[3] = 1.0f;
    }

    public void initFlags() {

        mFlagFog = 0;
        mFlagTextureMapping = 1;

        GLES20.glUseProgram(mId);
        GLES20.glUniform1i(locFlagFog, mFlagFog);
        GLES20.glUniform1i(locFlagTextureMapping, mFlagTextureMapping);
        GLES20.glUseProgram(0);
    }

    /**
     * 퐁 쉐이딩을 위한 각종 light 관련 값을 그래픽 메모리에 전달하는 함수.
     * @param viewMatrix 현재 상태의 view matrix.
     */
    public void set_up_scene_lights(float[] viewMatrix) {
        light = new LightParameters[NUMBER_OF_LIGHT_SUPPORTED];
        for(int i=0 ; i<NUMBER_OF_LIGHT_SUPPORTED ; i++)
            light[i] = new LightParameters();

        // point_light_EC: use light 0
        light[0].light_on = 1  ;
        light[0].position[0] = 0.0f; light[0].position[1] = 100.0f; 	// point light position in EC
        light[0].position[2] = 0.0f; light[0].position[3] = 1.0f;

        light[0].ambient_color[0] = 0.13f; light[0].ambient_color[1] = 0.13f;
        light[0].ambient_color[2] = 0.13f; light[0].ambient_color[3] = 1.0f;

        light[0].diffuse_color[0] = 0.5f; light[0].diffuse_color[1] = 0.5f;
        light[0].diffuse_color[2] = 0.5f; light[0].diffuse_color[3] = 1.5f;

        light[0].specular_color[0] = 0.8f; light[0].specular_color[1] = 0.8f;
        light[0].specular_color[2] = 0.8f; light[0].specular_color[3] = 1.0f;

        // spot_light_WC: use light 1
        light[1].light_on = 1;
        light[1].position[0] = -200.0f; light[1].position[1] = 300.0f; // spot light position in WC
        light[1].position[2] = -200.0f; light[1].position[3] = 1.0f;

        light[1].ambient_color[0] = 0.152f; light[1].ambient_color[1] = 0.152f;
        light[1].ambient_color[2] = 0.152f; light[1].ambient_color[3] = 1.0f;

        light[1].diffuse_color[0] = 0.0f; light[1].diffuse_color[1] = 0.0f;
        light[1].diffuse_color[2] = 1.0f; light[1].diffuse_color[3] = 1.0f;

        light[1].specular_color[0] = 0.0f; light[1].specular_color[1] = 0.0f;
        light[1].specular_color[2] = 1.0f; light[1].specular_color[3] = 1.0f;

        light[1].spot_direction[0] = 0.0f; light[1].spot_direction[1] = -1.0f; // spot light direction in WC
        light[1].spot_direction[2] = 0.0f;
        light[1].spot_cutoff_angle = 20.0f;
        light[1].spot_exponent = 8.0f;

        // spot_light_MC: use light 1
        light[2].light_on = 1;
        light[2].position[0] = 0.0f; light[2].position[1] = 300.0f; // spot light position in WC
        light[2].position[2] = 0.0f; light[2].position[3] = 1.0f;

        light[2].ambient_color[0] = 0.152f; light[2].ambient_color[1] = 0.152f;
        light[2].ambient_color[2] = 0.152f; light[2].ambient_color[3] = 1.0f;

        light[2].diffuse_color[0] = 1.0f; light[2].diffuse_color[1] = 0.0f;
        light[2].diffuse_color[2] = 0.0f; light[2].diffuse_color[3] = 1.0f;

        light[2].specular_color[0] = 1.0f; light[2].specular_color[1] = 0.0f;
        light[2].specular_color[2] = 0.0f; light[2].specular_color[3] = 1.0f;

        light[2].spot_direction[0] = 0.0f; light[2].spot_direction[1] = -1.0f; // spot light direction in WC
        light[2].spot_direction[2] = 0.0f;
        light[2].spot_cutoff_angle = 20.0f;
        light[2].spot_exponent = 8.0f;

        light[3].light_on = 1;
        light[3].position[0] = 500.0f; light[3].position[1] = 0.0f; // spot light position in WC
        light[3].position[2] =500.0f; light[3].position[3] = 1.0f;

        light[3].ambient_color[0] = 0.152f; light[3].ambient_color[1] = 0.152f;
        light[3].ambient_color[2] = 0.152f; light[3].ambient_color[3] = 1.0f;

        light[3].diffuse_color[0] = 0.0f; light[3].diffuse_color[1] = 1.0f;
        light[3].diffuse_color[2] = 0.0f; light[3].diffuse_color[3] = 1.0f;

        light[3].specular_color[0] = 0.0f; light[3].specular_color[1] = 1.0f;
        light[3].specular_color[2] = 0.0f; light[3].specular_color[3] = 1.0f;

        light[3].spot_direction[0] = 0.0f; light[3].spot_direction[1] = -1.0f; // spot light direction in WC
        light[3].spot_direction[2] = 0.0f;
        light[3].spot_cutoff_angle = 20.0f;
        light[3].spot_exponent = 8.0f;
        GLES20.glUseProgram(mId);
        GLES20.glUniform1i(locLight[0].light_on, light[0].light_on);
        GLES20.glUniform4fv(locLight[0].position, 1, BufferConverter.floatArrayToBuffer(light[0].position));
        GLES20.glUniform4fv(locLight[0].ambient_color, 1, BufferConverter.floatArrayToBuffer(light[0].ambient_color));
        GLES20.glUniform4fv(locLight[0].diffuse_color, 1, BufferConverter.floatArrayToBuffer(light[0].diffuse_color));
        GLES20.glUniform4fv(locLight[0].specular_color, 1, BufferConverter.floatArrayToBuffer(light[0].specular_color));


        GLES20.glUniform1i(locLight[1].light_on, light[1].light_on);
        // need to supply position in EC for shading
        float[] positionEC = new float[4];
        Matrix.multiplyMV(positionEC, 0, viewMatrix, 0, light[1].position, 0);

        GLES20.glUniform4fv(locLight[1].position, 1, BufferConverter.floatArrayToBuffer(positionEC));
        GLES20.glUniform4fv(locLight[1].ambient_color, 1, BufferConverter.floatArrayToBuffer(light[1].ambient_color));
        GLES20.glUniform4fv(locLight[1].diffuse_color, 1, BufferConverter.floatArrayToBuffer(light[1].diffuse_color));
        GLES20.glUniform4fv(locLight[1].specular_color, 1, BufferConverter.floatArrayToBuffer(light[1].specular_color));
        GLES20.glUniform1f(locLight[1].blind_width, light[1].blind_width);

        float[] spot_direction = {
                light[1].spot_direction[0], light[1].spot_direction[1], light[1].spot_direction[2], 0.0f
        };

        float[] directionEC = new float[4];
        Matrix.multiplyMV(directionEC, 0, viewMatrix, 0, spot_direction, 0);

        GLES20.glUniform3fv(locLight[1].spot_direction, 1, BufferConverter.floatArrayToBuffer(directionEC));
        GLES20.glUniform1f(locLight[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
        GLES20.glUniform1f(locLight[1].spot_exponent, light[1].spot_exponent);

        GLES20.glUniform1i(locLight[2].light_on, light[2].light_on);
        // need to supply position in EC for shading
        Matrix.multiplyMV(positionEC, 0, viewMatrix, 0, light[2].position, 0);

        GLES20.glUniform4fv(locLight[2].position, 1, BufferConverter.floatArrayToBuffer(positionEC));
        GLES20.glUniform4fv(locLight[2].ambient_color, 1, BufferConverter.floatArrayToBuffer(light[2].ambient_color));
        GLES20.glUniform4fv(locLight[2].diffuse_color, 1, BufferConverter.floatArrayToBuffer(light[2].diffuse_color));
        GLES20.glUniform4fv(locLight[2].specular_color, 1, BufferConverter.floatArrayToBuffer(light[2].specular_color));
        GLES20.glUniform1f(locLight[2].blind_width, light[2].blind_width);

        spot_direction[0] = light[2].spot_direction[0]; spot_direction[1] = light[2].spot_direction[1];
        spot_direction[2] = light[2].spot_direction[2]; spot_direction[3] = 0.0f;

        Matrix.multiplyMV(directionEC, 0, viewMatrix, 0, spot_direction, 0);

        GLES20.glUniform3fv(locLight[2].spot_direction, 1, BufferConverter.floatArrayToBuffer(directionEC));
        GLES20.glUniform1f(locLight[2].spot_cutoff_angle, light[2].spot_cutoff_angle);
        GLES20.glUniform1f(locLight[2].spot_exponent, light[2].spot_exponent);

        GLES20.glUniform1i(locLight[3].light_on, light[3].light_on);
        // need to supply position in EC for shading
        Matrix.multiplyMV(positionEC, 0, viewMatrix, 0, light[3].position, 0);

        GLES20.glUniform4fv(locLight[3].position, 1, BufferConverter.floatArrayToBuffer(positionEC));
        GLES20.glUniform4fv(locLight[3].ambient_color, 1, BufferConverter.floatArrayToBuffer(light[3].ambient_color));
        GLES20.glUniform4fv(locLight[3].diffuse_color, 1, BufferConverter.floatArrayToBuffer(light[3].diffuse_color));
        GLES20.glUniform4fv(locLight[3].specular_color, 1, BufferConverter.floatArrayToBuffer(light[3].specular_color));
        GLES20.glUniform1f(locLight[3].blind_width, light[3].blind_width);

        spot_direction[0] = light[3].spot_direction[0]; spot_direction[1] = light[3].spot_direction[1];
        spot_direction[2] = light[3].spot_direction[2]; spot_direction[3] = 0.0f;

        Matrix.multiplyMV(directionEC, 0, viewMatrix, 0, spot_direction, 0);

        GLES20.glUniform3fv(locLight[3].spot_direction, 1, BufferConverter.floatArrayToBuffer(directionEC));
        GLES20.glUniform1f(locLight[3].spot_cutoff_angle, light[3].spot_cutoff_angle);
        GLES20.glUniform1f(locLight[3].spot_exponent, light[3].spot_exponent);


        GLES20.glUseProgram(0);
    }

    public void setUpMaterialFloor() {
        GLES20.glUniform4fv(locMaterial.ambient_color, 1, BufferConverter.floatArrayToBuffer(materialFloor.ambient_color));
        GLES20.glUniform4fv(locMaterial.diffuse_color, 1, BufferConverter.floatArrayToBuffer(materialFloor.diffuse_color));
        GLES20.glUniform4fv(locMaterial.specular_color, 1, BufferConverter.floatArrayToBuffer(materialFloor.specular_color));
        GLES20.glUniform1f(locMaterial.specular_exponent, materialFloor.specular_exponent);
        GLES20.glUniform4fv(locMaterial.emissive_color, 1, BufferConverter.floatArrayToBuffer(materialFloor.emissive_color));
    }

    public void setUpMaterialTiger() {
        GLES20.glUniform4fv(locMaterial.ambient_color, 1, BufferConverter.floatArrayToBuffer(materialTiger.ambient_color));
        GLES20.glUniform4fv(locMaterial.diffuse_color, 1, BufferConverter.floatArrayToBuffer(materialTiger.diffuse_color));
        GLES20.glUniform4fv(locMaterial.specular_color, 1, BufferConverter.floatArrayToBuffer(materialTiger.specular_color));
        GLES20.glUniform1f(locMaterial.specular_exponent, materialTiger.specular_exponent);
        GLES20.glUniform4fv(locMaterial.emissive_color, 1, BufferConverter.floatArrayToBuffer(materialTiger.emissive_color));
    }

    public void set_lights1() {
        GLES20.glUseProgram(mId);
        GLES20.glUniform1i(locLight[1].light_on, light[1].light_on);
        GLES20.glUseProgram(0);
    }
}
