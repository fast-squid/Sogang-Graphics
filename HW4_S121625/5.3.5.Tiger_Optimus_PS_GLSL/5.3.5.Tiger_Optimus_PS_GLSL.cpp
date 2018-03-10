#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <time.h>
#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram_simple, h_ShaderProgram_PS; // handles to shader programs

#include "Shading.h"
// for simple shader
GLint loc_ModelViewProjectionMatrix_simple, loc_primitive_color;

// for Phone Shading shader
#define NUMBER_OF_LIGHT_SUPPORTED 4 
GLint loc_global_ambient_color;
loc_light_Parameters loc_light[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material;
int loc_blind_effect;
GLint loc_ModelViewProjectionMatrix_PS, loc_ModelViewMatrix_PS, loc_ModelViewMatrixInvTrans_PS;


// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.
//glm::mat4 ModelViewProjectionMatrix, ModelViewMatrix;
glm::mat3 ModelViewMatrixInvTrans;
//glm::mat4 ViewMatrix, ProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f



// lights in scene
Light_Parameters light[NUMBER_OF_LIGHT_SUPPORTED];

ShaderInfo shader_info_simple[3] = {
	{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
	{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
	{ GL_NONE, NULL }
};
ShaderInfo shader_info_PS[3] = {
	{ GL_VERTEX_SHADER, "Shaders/Phong.vert" },
	{ GL_FRAGMENT_SHADER, "Shaders/Phong.frag" },
	{ GL_NONE, NULL }
};

// for tiger animation
int cur_frame_tiger = 0;
float rotation_angle_tiger = 0.0f;
int move_flag=0,color_flag=0;
int win_flag = 1, loc_win_flag = 0, loc_club_flag = 0, loc_cir_flag = 0, club_flag = 0;
float light_strength=1.0f,loc_light_strength;

#include "Object.h"
#include "Camera.h"
// callbacks
int flag_draw_objects = 1;

int init = 0;
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(h_ShaderProgram_PS);

	if (flag_draw_objects) {
		// Currently, there is only one object.
		set_material_object(OBJECT_OPTIMUS);
		ModelViewMatrix[camera_selected] = glm::scale(ViewMatrix[camera_selected], glm::vec3(0.2f, 0.2f, 0.2f));
		ModelViewMatrix[camera_selected] = glm::rotate(ModelViewMatrix[camera_selected], rotation_angle_tiger, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewMatrix[camera_selected] = glm::rotate(ModelViewMatrix[camera_selected], -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = ProjectionMatrix[camera_selected] * ModelViewMatrix[camera_selected];
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix[camera_selected]));

		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[camera_selected][0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		draw_object(OBJECT_OPTIMUS, 1.0f, 1.0f, 1.0f, GL_CCW);
		
		glUseProgram(h_ShaderProgram_simple);
		ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glLineWidth(3.0f);
		draw_axes();
		glLineWidth(1.0f);
	}

	glUseProgram(h_ShaderProgram_PS);
	set_material_floor();
	ModelViewMatrix[camera_selected] = glm::translate(ViewMatrix[camera_selected], glm::vec3(-250.0f, 0.0f, 250.0f));
	ModelViewMatrix[camera_selected] = glm::scale(ModelViewMatrix[camera_selected], glm::vec3(500.0f, 500.0f, 500.0f));
	ModelViewMatrix[camera_selected] = glm::rotate(ModelViewMatrix[camera_selected], -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix[camera_selected] * ModelViewMatrix[camera_selected];
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix[camera_selected]));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[camera_selected][0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_floor();

	ModelViewMatrix[camera_selected] = glm::translate(ViewMatrix[camera_selected], glm::vec3(-10.0f, 30.0f, -10.0f));
	ModelViewMatrix[camera_selected] = glm::scale(ModelViewMatrix[camera_selected], glm::vec3(10.0f, 10.0f, 10.0f));
	ModelViewProjectionMatrix = ProjectionMatrix[camera_selected] * ModelViewMatrix[camera_selected];
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix[camera_selected]));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[camera_selected][0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);	
	
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	glUniform1i(loc_win_flag, win_flag);
	set_material_window();
	ModelViewMatrix[camera_selected] = glm::translate(ViewMatrix[camera_selected], glm::vec3(0, 0, 0));
	ModelViewProjectionMatrix = ProjectionMatrix[camera_selected] * ModelViewMatrix[camera_selected];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[camera_selected][0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_window();
	//
	//ModelViewMatrix[camera_selected] = glm::translate(ViewMatrix[camera_selected], glm::vec3(0, 250, 250));
	//ModelViewMatrix[camera_selected] = glm::rotate(ModelViewMatrix[camera_selected], 180.0f*TO_RADIAN, glm::vec3(0.0f, -1.0f, 1.0f));
	//
	//ModelViewProjectionMatrix = ProjectionMatrix[camera_selected] * ModelViewMatrix[camera_selected];
	//glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	//glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[camera_selected][0][0]);
	//glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	//draw_window();
	glUniform1i(loc_win_flag, 0);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	glUniform1i(loc_cir_flag, win_flag);
	set_material_cir();
	if (init == 0) {
		cir_pos = { 100,100,0 };
		cir_dir = { rand() % 8,rand() % 8 ,rand() % 8 };
		glm::normalize(cir_dir);
		init = 1;
	}
	cir_pos += cir_dir;
	ModelViewMatrix[camera_selected] = glm::translate(ViewMatrix[camera_selected], cir_pos);
	ModelViewMatrix[camera_selected] = glm::scale(ModelViewMatrix[camera_selected], glm::vec3(10.0f, 10.0f, 10.0f));
	ModelViewMatrix[camera_selected] = glm::rotate(ModelViewMatrix[camera_selected], TO_RADIAN*cir_pos.z, {1,1,1});

	ModelViewProjectionMatrix = ProjectionMatrix[camera_selected] * ModelViewMatrix[camera_selected];
	
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[camera_selected][0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	
	draw_cir();
	glUniform1i(loc_cir_flag, 0);
	////////////////////////////////////////////////////////////////////////////////////////////////////

	set_material_floor();
	ModelViewMatrix[camera_selected] = glm::translate(ViewMatrix[camera_selected], glm::vec3(250, 0, -250));
	ModelViewMatrix[camera_selected] = glm::scale(ModelViewMatrix[camera_selected], glm::vec3(500.0f, 500.0f, 500.0f));
	ModelViewMatrix[camera_selected] = glm::rotate(ModelViewMatrix[camera_selected], 90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewProjectionMatrix = ProjectionMatrix[camera_selected] * ModelViewMatrix[camera_selected];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[camera_selected][0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_floor();

	set_material_floor();
	ModelViewMatrix[camera_selected] = glm::translate(ViewMatrix[camera_selected], glm::vec3(-250, 0, -250));
	ModelViewMatrix[camera_selected] = glm::scale(ModelViewMatrix[camera_selected], glm::vec3(500.0f, 500.0f, 500.0f));
	ModelViewMatrix[camera_selected] = glm::rotate(ModelViewMatrix[camera_selected], -90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix[camera_selected] * ModelViewMatrix[camera_selected];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[camera_selected][0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_floor();

	set_material_tiger();
	ModelViewMatrix[camera_selected] = glm::rotate(ViewMatrix[camera_selected], -rotation_angle_tiger, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix[camera_selected] = glm::translate(ModelViewMatrix[camera_selected], glm::vec3(200.0f, 0.0f, 0.0f));
	ModelViewMatrix[camera_selected] = glm::rotate(ModelViewMatrix[camera_selected], -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix[camera_selected] * ModelViewMatrix[camera_selected];
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix[camera_selected]));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[camera_selected][0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_tiger();

	glUseProgram(h_ShaderProgram_simple);
	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(20.0f, 20.0f, 20.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(3.0f);
	draw_axes();
	glLineWidth(1.0f);

	glUseProgram(h_ShaderProgram_simple);
	ModelViewMatrix[camera_selected] = glm::scale(ViewMatrix[camera_selected], glm::vec3(100.0f, 100.0f, 100.0f));
	ModelViewProjectionMatrix = ProjectionMatrix[camera_selected] * ModelViewMatrix[camera_selected];
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(2.0f);
	draw_axes();
	glLineWidth(1.0f);

	glUseProgram(0);

	glutSwapBuffers();
}
int init_move = 0;
glm::vec3 mov_dir[3];
glm::vec3 rot_dir[3];
glm::mat4 rot_mat;

void move_light() {

	glm::vec3 light_dir;
	if (init_move == 0) {
		for (int i = 0; i < 3; i++) {
			mov_dir[i] = { rand() % 8, 0 ,rand() % 8 };
			rot_dir[i] = { rand() % 8,rand() % 8,rand() % 8 };
			
		}
		init_move = 1;
	}
	glUseProgram(h_ShaderProgram_PS);
	int i = 1;
	for (i = 1; i <= 3; i++) {
		if (250-light[i].position[0] <5) { mov_dir[i - 1].x *= -1;}
		else if (light[i].position[0] < -250) { mov_dir[i - 1].x *= -1; }
		light[i].position[0] += mov_dir[i - 1].x;
		
		if (250 - light[i].position[2] <5) { mov_dir[i - 1].z *= -1; }
		else if (light[i].position[2] < -250) { mov_dir[i - 1].z *= -1; }
		light[i].position[2] += mov_dir[i - 1].z;
		glm::vec4 position_EC = ViewMatrix[1] * glm::vec4(light[i].position[0], light[i].position[1],
			light[i].position[2], light[i].position[3]);
		glUniform4fv(loc_light[i].position, 1, &position_EC[0]);
	}

	for ( i = 1; i <= 3; i++) {
		light_dir.x = light[i].spot_direction[0];
		light_dir.y = light[i].spot_direction[1];
		light_dir.z = light[i].spot_direction[2];
		rot_mat = glm::rotate(glm::mat4(1.0f), TO_RADIAN*2, rot_dir[i]);
		light_dir = glm::vec3(rot_mat*glm::vec4(light_dir,1.0f));
		light[i].spot_direction[0] = light_dir.x;
		light[i].spot_direction[1] = light_dir.y;
		light[i].spot_direction[2] = light_dir.z;
		glm::vec3 direction_EC = glm::mat3(ViewMatrix[1]) * glm::vec3(light[i].spot_direction[0], light[i].spot_direction[1],
			light[i].spot_direction[2]);


		glUniform3fv(loc_light[i].spot_direction, 1, &direction_EC[0]);
	}
	glUniform1i(loc_club_flag, club_flag);
	glUseProgram(0);
	
}

int shading_mode,loc_shading_mode;
void prepare_shader_program(void) {
	int i;
	char string[256];
	
	h_ShaderProgram_simple = LoadShaders(shader_info_simple);
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");
	loc_ModelViewProjectionMatrix_simple = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");

	h_ShaderProgram_PS = LoadShaders(shader_info_PS);
	loc_ModelViewProjectionMatrix_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_PS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].blind_width", i);
		loc_light[i].blind_width = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_PS, string);
	}
	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_PS, "u_material.specular_exponent");
	loc_win_flag = glGetUniformLocation(h_ShaderProgram_PS, "u_win_flag");
	loc_cir_flag = glGetUniformLocation(h_ShaderProgram_PS, "u_cir_flag");
	loc_club_flag = glGetUniformLocation(h_ShaderProgram_PS, "u_club_flag");

	loc_blind_effect = glGetUniformLocation(h_ShaderProgram_PS, "u_blind_effect");
	loc_shading_mode = glGetUniformLocation(h_ShaderProgram_PS, "u_shading_mode");
	loc_light_strength = glGetUniformLocation(h_ShaderProgram_PS, "u_light_strength");
}

void initialize_lights_and_material(void) { // follow OpenGL conventions for initialization
	int i;

	glUseProgram(h_ShaderProgram_PS);
	
	glUniform4f(loc_global_ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform1f(loc_light[i].blind_width, 1.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light[i].light_attenuation_factors, 0.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material.specular_color, 0.0f, 1.0f, 0.0f, 1.0f);
	glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material.specular_exponent, 50.0f); // [0.0, 128.0]

	glUniform1i(loc_blind_effect,0);

	glUseProgram(0);
}

void initialize_OpenGL(void) {
  	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	ViewMatrix[0] = glm::lookAt(glm::vec3(400.0f,500.0f, 400.0f), glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	ViewMatrix[1] = ViewMatrix[0];
	ProjectionMatrix[camera_selected] = glm::perspective(TO_RADIAN*camera[camera_selected].fovy, camera[camera_selected].aspect_ratio, 1.0f, 1400.0f);

	initialize_lights_and_material();
}

void set_up_scene_lights(void) {
	shading_mode = 0;
	// point_light_EC: use light 0
	light[0].light_on = 1;
	light[0].position[0] = 0.0f; light[0].position[1] = 10.0f; 	// point light position in EC
	light[0].position[2] = 0.0f; light[0].position[3] = 1.0f;

	light[0].ambient_color[0] = 0.3f; light[0].ambient_color[1] = 0.3f;
	light[0].ambient_color[2] = 0.3f; light[0].ambient_color[3] = 1.0f;

	light[0].diffuse_color[0] = 0.7f; light[0].diffuse_color[1] = 0.7f;
	light[0].diffuse_color[2] = 0.7f; light[0].diffuse_color[3] = 1.0f;

	light[0].specular_color[0] = 0.9f; light[0].specular_color[1] = 0.9f;
	light[0].specular_color[2] = 0.9f; light[0].specular_color[3] = 1.0f;
	////////////////////////////////////////////////////////////////////////////////////////////////
	light[1].position[0] = -50.0f; light[1].position[1] = 100.0f; // spot light position in WC
	light[1].position[2] = -50.0f; light[1].position[3] = 1.0f;

	light[1].ambient_color[0] = 0.2f; light[1].ambient_color[1] = 0.2f;
	light[1].ambient_color[2] = 0.2f; light[1].ambient_color[3] = 1.0f;

	light[1].diffuse_color[0] = 0.82f; light[1].diffuse_color[1] = 0.82f;
	light[1].diffuse_color[2] = 0.82f; light[1].diffuse_color[3] = 1.0f;

	light[1].specular_color[0] = 0.82f; light[1].specular_color[1] = 0.82f;
	light[1].specular_color[2] = 0.82f; light[1].specular_color[3] = 1.0f;
	light[1].spot_direction[0] = 0.0f; light[1].spot_direction[1] = -1.0f; // spot light direction in WC
	light[1].spot_direction[2] = 0.0f;
	light[1].spot_cutoff_angle = 20.0f;
	light[1].spot_exponent = 27.0f;
	///////////////////////////////////////////////////////////////////////////////////////////////
	light[2].position[0] = 100.0f; light[2].position[1] = 100.0f; // spot light position in WC
	light[2].position[2] = 0.0f; light[2].position[3] = 1.0f;

	light[2].ambient_color[0] = 0.2f; light[2].ambient_color[1] = 0.2f;
	light[2].ambient_color[2] = 0.2f; light[2].ambient_color[3] = 1.0f;

	light[2].diffuse_color[0] = 0.82f; light[2].diffuse_color[1] = 0.82f;
	light[2].diffuse_color[2] = 0.82f; light[2].diffuse_color[3] = 1.0f;

	light[2].specular_color[0] = 0.82f; light[2].specular_color[1] = 0.82f;
	light[2].specular_color[2] = 0.82f; light[2].specular_color[3] = 1.0f;

	light[2].spot_direction[0] = 0.0f; light[2].spot_direction[1] = -1.0f; // spot light direction in WC
	light[2].spot_direction[2] = 0.0f;
	light[2].spot_cutoff_angle = 20.0f;
	light[2].spot_exponent = 10.0f;
	///////////////////////////////////////////////////////////////////////////////////////////////

	light[3].position[0] = 0.0f; light[3].position[1] = 100.0f; // spot light position in WC
	light[3].position[2] = 100.0f; light[3].position[3] = 1.0f;

	light[3].ambient_color[0] = 0.2f; light[3].ambient_color[1] = 0.2f;
	light[3].ambient_color[2] = 0.2f; light[3].ambient_color[3] = 1.0f;

	light[3].diffuse_color[0] = 0.82f; light[3].diffuse_color[1] = 0.82f;
	light[3].diffuse_color[2] = 0.82f; light[3].diffuse_color[3] = 1.0f;

	light[3].specular_color[0] = 0.82f; light[3].specular_color[1] = 0.82f;
	light[3].specular_color[2] = 0.82f; light[3].specular_color[3] = 1.0f;

	light[3].spot_direction[0] = 0.0f; light[3].spot_direction[1] = -1.0f; // spot light direction in WC
	light[3].spot_direction[2] = 0.0f;
	light[3].spot_cutoff_angle = 20.0f;
	light[3].spot_exponent = 7.0f;
	///////////////////////////////////////////////////////////////////////////////////////////////
	light[1].blind_width = 3;
	light[2].blind_width = 3;
	light[3].blind_width = 1;

	glUseProgram(h_ShaderProgram_PS);
	glUniform1i(loc_light[0].light_on, light[0].light_on);
	glUniform4fv(loc_light[0].position, 1, light[0].position);
	glUniform4fv(loc_light[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light[0].specular_color, 1, light[0].specular_color);
	//////////////////////////////////////////////////////////////////////////////////
	glUniform1i(loc_light[1].light_on, light[1].light_on);
	glm::vec4 position_EC = ViewMatrix[1]*  glm::vec4(light[1].position[0], light[1].position[1],
												light[1].position[2], light[1].position[3]);
	glUniform4fv(loc_light[1].position, 1, &position_EC[0]); 
	glUniform4fv(loc_light[1].ambient_color, 1, light[1].ambient_color);
	glUniform4fv(loc_light[1].diffuse_color, 1, light[1].diffuse_color);
	glUniform4fv(loc_light[1].specular_color, 1, light[1].specular_color);
	glm::vec3 direction_EC = glm::mat3(ViewMatrix[1]) * glm::vec3(light[1].spot_direction[0], light[1].spot_direction[1], 
																light[1].spot_direction[2]);
	glUniform3fv(loc_light[1].spot_direction, 1, &direction_EC[0]); 
	glUniform1f(loc_light[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
	glUniform1f(loc_light[1].spot_exponent, light[1].spot_exponent);
	////////////////////////////////////////////////////////////////////////////////////
	glUniform1i(loc_light[2].light_on, light[2].light_on);
	position_EC = ViewMatrix[1] * glm::vec4(light[2].position[0], light[2].position[1],
		light[2].position[2], light[2].position[3]);
	glUniform4fv(loc_light[2].position, 1, &position_EC[0]);
	glUniform4fv(loc_light[2].ambient_color, 1, light[2].ambient_color);
	glUniform4fv(loc_light[2].diffuse_color, 1, light[2].diffuse_color);
	glUniform4fv(loc_light[2].specular_color, 1, light[2].specular_color);
	direction_EC = glm::mat3(ViewMatrix[1]) * glm::vec3(light[2].spot_direction[0], light[2].spot_direction[1],
		light[2].spot_direction[2]);
	glUniform3fv(loc_light[2].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light[2].spot_cutoff_angle, light[2].spot_cutoff_angle);
	glUniform1f(loc_light[2].spot_exponent, light[2].spot_exponent);
	//////////////////////////////////////////////////////////////////////////////////////
	position_EC = ViewMatrix[1] * glm::vec4(light[3].position[0], light[3].position[1],
		light[3].position[2], light[3].position[3]);
	glUniform4fv(loc_light[3].position, 1, &position_EC[0]);
	glUniform4fv(loc_light[3].ambient_color, 1, light[3].ambient_color);
	glUniform4fv(loc_light[3].diffuse_color, 1, light[3].diffuse_color);
	glUniform4fv(loc_light[3].specular_color, 1, light[3].specular_color);
	direction_EC = glm::mat3(ViewMatrix[1]) * glm::vec3(light[3].spot_direction[0], light[3].spot_direction[1],
		light[3].spot_direction[2]);
	glUniform3fv(loc_light[3].spot_direction, 1, &direction_EC[0]);
	glUniform1f(loc_light[3].spot_cutoff_angle, light[3].spot_cutoff_angle);
	glUniform1f(loc_light[3].spot_exponent, light[3].spot_exponent);
	glUseProgram(0);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_window();
	prepare_floor();
	prepare_tiger();
	prepare_OPTIMUS();
	prepare_cir();
}
int prevx;
void motion(int x, int y)
{
	light_strength += (float)(x - prevx)*0.001;
	glUseProgram(h_ShaderProgram_PS);
	glUniform1f(loc_light_strength, light_strength); 
	glUseProgram(0);
	prevx = x;
}

void mouse(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON)) {
		if (state == GLUT_DOWN) {
		}
		else if (state == GLUT_UP);
	}
	else if ((button == GLUT_RIGHT_BUTTON)) {
		if (state == GLUT_DOWN) {
			shader_info_PS[0] = { GL_VERTEX_SHADER, "Shaders/Gouraud.vert" };
			shader_info_PS[1] = { GL_FRAGMENT_SHADER, "Shaders/Gouraud.frag" };
		}
		else if (state == GLUT_UP) {
			shader_info_PS[0] = { GL_VERTEX_SHADER, "Shaders/Phong.vert" };
			shader_info_PS[1] = { GL_FRAGMENT_SHADER, "Shaders/Phong.frag" };
		}
		prepare_shader_program();
		initialize_lights_and_material();
		set_up_scene_lights();
	}
}

void mousescroll(int button, int dir, int x, int y) {
	if (dir > 0) camera[camera_selected].fovy-=1.0f;
	else if (dir <0) camera[camera_selected].fovy+= 1.0f;
	
	ProjectionMatrix[camera_selected] = glm::perspective(TO_RADIAN*camera[camera_selected].fovy, camera[camera_selected].aspect_ratio, 1.0f, 1400.0f);
	glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0;
	static int flag_polygon_mode = 1;
	static int flag_blind_effect = 0;
	glm::vec4 position_EC;
	glUniform4fv(loc_light[2].position, 1, &position_EC[0]);
	
	glUseProgram(h_ShaderProgram_PS);

	if ((key >= '0') && (key <= '0' + NUMBER_OF_LIGHT_SUPPORTED - 1)) {
		int light_ID = (int)(key - '0');

		light[light_ID].light_on = 1 - light[light_ID].light_on;
		glUniform1i(loc_light[light_ID].light_on, light[light_ID].light_on);
		
		return;
	}

	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanupsbreak;
		break;
	case 'n':
		if (shading_mode == 0) { shading_mode = 1; glUniform1i(loc_shading_mode, shading_mode); }
		else { shading_mode = 0; glUniform1i(loc_shading_mode, shading_mode); }
	    break;
	case 'g':
		if (color_flag == 0) {
			color_flag = 1;
			light[1].diffuse_color[0] = 1; light[1].diffuse_color[1] = 0;
			light[1].diffuse_color[2] = 0; light[1].diffuse_color[3] = 1.0f;
			light[1].specular_color[0] = 1; light[1].specular_color[1] = 0;
			light[1].specular_color[2] = 0; light[1].specular_color[3] = 1.0f;

			light[2].diffuse_color[0] = 0; light[2].diffuse_color[1] = 1;
			light[2].diffuse_color[2] = 0; light[2].diffuse_color[3] = 1.0f;
			light[2].specular_color[0] = 0; light[2].specular_color[1] = 1;
			light[2].specular_color[2] = 0; light[2].specular_color[3] = 1.0f;

			light[3].diffuse_color[0] = 0; light[3].diffuse_color[1] = 0;
			light[3].diffuse_color[2] = 1; light[3].diffuse_color[3] = 1.0f;
			light[3].specular_color[0] = 0; light[3].specular_color[1] = 0;
			light[3].specular_color[2] = 1; light[3].specular_color[3] = 1.0f;
		}
		else {
			color_flag = 0;
			for (int i = 1; i <= 3; i++) {
				light[i].diffuse_color[0] = 0.82f; light[i].diffuse_color[1] = 0.82f;
				light[i].diffuse_color[2] = 0.82f; light[i].diffuse_color[3] = 1.0f;
				light[i].specular_color[0] = 0.82f; light[i].specular_color[1] = 0.82f;
				light[i].specular_color[2] = 0.82f; light[i].specular_color[3] = 1.0f;
			}
		}
		for (int i = 1; i <= 3; i++) {
			glUniform4fv(loc_light[i].diffuse_color, 1, light[i].diffuse_color);
			glUniform4fv(loc_light[i].specular_color, 1, light[i].specular_color);
		}
		break;
	case 'p':
		flag_polygon_mode = 1 - flag_polygon_mode;
		if (flag_polygon_mode)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'b':
		flag_blind_effect = 1 - flag_blind_effect;
		glUniform1i(loc_blind_effect, flag_blind_effect);
		break;
	case 'm': 
		if(move_flag==0) move_flag = 1;
		else move_flag = 0;
		break;
	case 'o':
		flag_draw_objects = 1 - flag_draw_objects;
		glutPostRedisplay();
		break;
	case '=':
		if (win_flag < 50) {
			win_flag++;
		}
		break;
	case '-':
		if (1 < win_flag) {
			win_flag--;
		}
		break;
	case '.':
		if(material_cir.specular_exponent<127)material_cir.specular_exponent++;
		glUniform1f(loc_material.specular_exponent, material_cir.specular_exponent);
		break;
	case ',':
		if (material_cir.specular_exponent>0)material_cir.specular_exponent--;
		glUniform1f(loc_material.specular_exponent, material_cir.specular_exponent);
		break;
	}
	glUseProgram(0);
	glutPostRedisplay();
	
}


void special(int key, int x, int y) {
	float speed = 0;
	static float delx = 0, delz = 0;

	glUseProgram(h_ShaderProgram_PS);
	switch (key) {
	case GLUT_KEY_UP:
		if(light[1].spot_cutoff_angle<89) light[1].spot_cutoff_angle++;
		glUniform1f(loc_light[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
		if (light[2].spot_cutoff_angle<89) light[2].spot_cutoff_angle++;
		glUniform1f(loc_light[2].spot_cutoff_angle, light[2].spot_cutoff_angle);
		if (light[3].spot_cutoff_angle<89) light[3].spot_cutoff_angle++;
		glUniform1f(loc_light[3].spot_cutoff_angle, light[3].spot_cutoff_angle);
		break;
	case GLUT_KEY_DOWN:
		if (light[1].spot_cutoff_angle>5) light[1].spot_cutoff_angle--;
		glUniform1f(loc_light[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
		if (light[2].spot_cutoff_angle>5) light[2].spot_cutoff_angle--;
		glUniform1f(loc_light[2].spot_cutoff_angle, light[2].spot_cutoff_angle);
		if (light[3].spot_cutoff_angle>5) light[3].spot_cutoff_angle--;
		glUniform1f(loc_light[3].spot_cutoff_angle, light[3].spot_cutoff_angle);
		break;
	case GLUT_KEY_LEFT:
		if(light[1].blind_width<5) light[1].blind_width += 0.01;
		glUniform1f(loc_light[1].blind_width, light[1].blind_width);
		if (light[2].blind_width<5) light[2].blind_width += 0.01;
		glUniform1f(loc_light[2].blind_width, light[2].blind_width);
		if (light[3].blind_width<5) light[3].blind_width += 0.0035;
		glUniform1f(loc_light[3].blind_width, light[3].blind_width);
		break;
	case GLUT_KEY_RIGHT:
		if (light[1].blind_width>0) light[1].blind_width -= 0.01;
		glUniform1f(loc_light[1].blind_width, light[1].blind_width);
		if (light[2].blind_width>0) light[2].blind_width -= 0.01;
		glUniform1f(loc_light[2].blind_width, light[2].blind_width);
		if (light[3].blind_width>0) light[3].blind_width -= 0.0035;
		glUniform1f(loc_light[3].blind_width, light[3].blind_width);
		break;
	}
	//glUniform1i(loc_blind_effect, 1);
	glUseProgram(0);

	glutPostRedisplay();
}

void reshape(int width, int height) {
	float aspect_ratio;
	glViewport(0, 0, width, height);

	aspect_ratio = camera[camera_selected].aspect_ratio;
	ProjectionMatrix[camera_selected] = glm::perspective(45.0f*camera[camera_selected].fovy, aspect_ratio, 1.0f, 1400.0f);

	glutPostRedisplay();
}

void timer_scene(int timestamp_scene) {
	cur_frame_tiger = timestamp_scene % N_TIGER_FRAMES;
	rotation_angle_tiger = (timestamp_scene % 360)*TO_RADIAN;
	
	if (cir_pos.x > 250) { cir_dir.x *= -1; cir_pos.x += -8; }
	else if (cir_pos.x < -210) { cir_dir.x *= -1; cir_pos.x += 8;}
	if (cir_pos.y > 450) { cir_dir.y *= -1; cir_pos.y += -8; }
	else if (cir_pos.y < 40) { cir_dir.y *= -1; cir_pos.y +=8;}
	if (cir_pos.z > 250) { cir_dir.z *= -1; cir_pos.z += -8;}
	else if (cir_pos.z <-210) { cir_dir.z *= -1; cir_pos.z += 8;}

	if (move_flag == 1) {
		move_light();
		club_flag += 1;
		club_flag %= 360;
	}
	glutPostRedisplay();	
	glutTimerFunc(100, timer_scene, (timestamp_scene + 1) % INT_MAX);

}

void cleanup(void) {
	glDeleteVertexArrays(1, &axes_VAO);
	glDeleteBuffers(1, &axes_VBO);

	glDeleteVertexArrays(1, &tiger_VAO);
	glDeleteBuffers(1, &tiger_VBO);

	glDeleteVertexArrays(1, &object_VAO[OBJECT_OPTIMUS]);
	glDeleteBuffers(1, &object_VBO[OBJECT_OPTIMUS]);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMouseWheelFunc(mousescroll);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(motion);
	glutSpecialFunc(special);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup);
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_camera();
	initialize_OpenGL();
	prepare_scene();
	set_up_scene_lights();

}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) {
	// Phong Shading
	char program_name[64] = "Sogang CSE4170 5.3.5.Tiger_Optimus_PS_GLSL";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: '0', '1', 'c', 'p', 'b', 'o', 'ESC'"  };
	srand(time(0));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(800, 800);
	glutInitContextVersion(3, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}