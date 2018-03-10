#define _CRT_SECURE_NO_WARNINGS

#define USE_set_ViewMatrix_for_driver2

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <time.h>
#define TO_RAD 0.0001f  
#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> //inverse, affineInverse, etc.

glm::mat4 ModelMatrix_CAR_BODY, ModelMatrix_CAR_WHEEL, ModelMatrix_CAR_NUT, ModelMatrix_CAR_DRIVER;
// The following two variables are computed only once in initialize_camera().
glm::mat4 ModelMatrix_CAR_BODY_to_DRIVER; 
glm::mat4 ModelMatrix_CAR_BODY_to_DRIVER_INVERSE; // used for set_ViewMatrix_for_driver2()
glm::mat4 ModelMatrix_CAR_BODY_INVERSE; // used for set_ViewMatrix_for_driver2()

#include "Camera.h"
#include "Geometry.h"
#include "Car.h"

glm::vec3 cir_dir,cir_pos;
float cir_speed[8],sp=0;
int r_sp = 0;
glm::vec3 tracer_dir[8], tracer_pos[8];


int win_width = 0, win_height = 0;
int m_posx, m_posy;
int init=0;
int key_state;

void draw_objects_in_world_2(void) {
	int i;
	float angle;
	glm::mat4 ModelMatrix_OBJECT;
	

	for (i = 0, angle = 0.0f; i < 3; i++, angle += 10.0f) {
		ModelMatrix_OBJECT = glm::rotate(glm::mat4(1.0f), TO_RADIAN*angle, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelMatrix_OBJECT = glm::translate(ModelMatrix_OBJECT, glm::vec3(60.0f, 0.0f, 0.0f));
		ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, -TO_RADIAN*90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		ModelViewProjectionMatrix = ViewProjectionMatrix[camera_selected] * ModelMatrix_OBJECT;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

		if (i % 9 == 0) {
			float grey = 1.0f - i / 27.0f;

			ModelViewProjectionMatrix = ViewProjectionMatrix[camera_selected] * ModelMatrix_OBJECT;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			glUniform3f(loc_primitive_color, grey, grey, grey);
			draw_geom_obj(GEOM_OBJ_ID_BOX); // draw box
		}
		else {
			if (i % 3 == 0)
				glUniform3f(loc_primitive_color, 1.0f - i / 35.0f, 1.0f, 0.0f);
			else  if (i % 3 == 1)
				glUniform3f(loc_primitive_color, 1.0f, 1.0 - i / 35.0f, 0.0f);
			else
				glUniform3f(loc_primitive_color, 0.0f, 1.0 - i / 35.0f, 1.0f);

			ModelViewProjectionMatrix = ViewProjectionMatrix[camera_selected] * ModelMatrix_OBJECT;
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			draw_geom_obj(GEOM_OBJ_ID_TEAPOT); // draw teapot
		}
	}


	
	ModelMatrix_OBJECT = glm::mat4(1.0f);
	ModelViewProjectionMatrix = ViewProjectionMatrix[camera_selected] * ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_WC();
	

	
}
/*********************************  START: callbacks *********************************/
void display(void) {
	glm::mat4 ModelMatrix_OBJECT;
	glm::mat4 Matrix_TIGER_tmp;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	ModelViewProjectionMatrix = ViewProjectionMatrix[camera_selected] * ModelMatrix_CAR_BODY;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_car();

	ModelViewProjectionMatrix = glm::scale(ViewProjectionMatrix[camera_selected], glm::vec3(8.0f, 8.0f, 8.0f));
	
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(2.0f);
 	draw_axes();
	glLineWidth(1.0f);
	draw_objects_in_world_2();

	ModelMatrix_OBJECT = glm::mat4(1.0f);
	if (!init) {
		cir_dir = { rand() % 8,rand() % 8,rand() % 8 };
		cir_dir = glm::normalize(cir_dir);
		cir_dir *= 0.1;
		cir_pos = { 5, 7, 6 };
		init = 1;
		for (int i = 0; i < 8; i++)
			tracer_pos[i] = { 0 ,0,30 };
		H_OBJ_POS = glm::vec3(0, 0, 0);
		H_OBJ_B = glm::mat4(1);
		H_OBJ_W1 = H_OBJ_W2 = H_OBJ_W3 = H_OBJ_W4 = glm::mat4(1);


	}
 
	ModelViewProjectionMatrix = ViewProjectionMatrix[camera_selected] * H_OBJ_B;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_oo();
	
	/////////////////////////////////////////////////////////////////////////////////////////////
	cir_pos += cir_dir*sp;
	ModelMatrix_OBJECT = glm::translate(ModelMatrix_OBJECT, cir_pos);
	ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, r_sp*TO_RADIAN, glm::vec3(1,1,1));
	ModelViewProjectionMatrix = ViewProjectionMatrix[camera_selected] * ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_cir();
	/////////////////////////////////////////////////////////////////////////////////////////////
	glm::mat4 temp;
	cir_speed[0] = 0.04; cir_speed[1] = 0.06; cir_speed[2] = 0.08; cir_speed[3] = 0.1;
	cir_speed[4] = 0.12; cir_speed[5] = 0.14; cir_speed[6] = 0.16; cir_speed[7] = 0.18;
	for (int i = 0; i <8; i++) {
		ModelMatrix_OBJECT = glm::mat4(1.0f);
		
		ModelMatrix_OBJECT = glm::translate(ModelMatrix_OBJECT, glm::vec3(tracer_pos[i].x + tracer_dir[i].x*cir_speed[i], tracer_pos[i].y + tracer_dir[i].y*cir_speed[i], 0));

		if ((m_posx - win_width / 2 - tracer_pos[i].x)*(m_posx - win_width / 2 - tracer_pos[i].x) +
			(win_height / 2 - m_posy - tracer_pos[i].y)*(win_height / 2 - m_posy - tracer_pos[i].y) <4) {
			tracer_pos[i].x = tracer_pos[i].x; tracer_pos[i].y = tracer_pos[i].y; tracer_pos[i].z = tracer_pos[i].z;
		}
		else {
			tracer_pos[i].x = tracer_pos[i].x + tracer_dir[i].x*cir_speed[i];
			tracer_pos[i].y = tracer_pos[i].y + tracer_dir[i].y*cir_speed[i];
			tracer_pos[i].z = tracer_pos[i].z + tracer_dir[i].z*cir_speed[i];
		}
		if (!(tracer_dir[i].x || tracer_dir[i].y)) temp = glm::mat4(1.0f);
		else
			temp = { { tracer_dir[i].x , tracer_dir[i].y,0,0 },
			{ -tracer_dir[i].y , tracer_dir[i].x,0,0 },
			{ 0,0,1,0 } ,
			{ 0,0,0,1 }
		};
		ModelMatrix_OBJECT *= temp;
		ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, (float)r_sp, tracer_dir[i]);
		ModelMatrix_OBJECT = glm::scale(ModelMatrix_OBJECT, glm::vec3(0.3, 0.3, 0.3));
		ModelMatrix_OBJECT = glm::rotate(ModelMatrix_OBJECT, (float)r_sp*TO_RADIAN, tracer_dir[i]);
		ModelViewProjectionMatrix = ViewProjectionMatrix[camera_selected] * ModelMatrix_OBJECT;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_cir();
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////
	
	ModelMatrix_OBJECT = glm::mat4(1.0f);
	ModelViewProjectionMatrix = ViewProjectionMatrix[camera_selected] * ModelMatrix_OBJECT;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_cir();
	//////////////////////
	ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix[camera_selected], glm::vec3(-30.0f, -5, -30));
	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix,
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	draw_object(OBJECT_SQUARE16, 0.5, 0.5, 0.5); //  draw the floor.
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// set up this matrix only once outside this callback function if it is static.
	Matrix_TIGER_tmp = glm::rotate(glm::mat4(1.0f), -90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	Matrix_TIGER_tmp = glm::scale(Matrix_TIGER_tmp, glm::vec3(0.05f,0.05f, 0.05f));

	ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix[camera_selected], glm::vec3(-20.0f, 0, -30.5f));
	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 30.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(1.5f, 1.5f, 1.5f));
	ModelViewProjectionMatrix *= Matrix_TIGER_tmp;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_object(OBJECT_AIRPLANE, 255 / 255.0f, 255 / 255.0f, 255 / 255.0f); // White

	ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix[camera_selected], glm::vec3(0.0f, 0.0f, -30.5f));
	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(5.0f, 5.0f, 5.0f));
	ModelViewProjectionMatrix *= Matrix_TIGER_tmp;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_object(OBJECT_DRAGON, 0 / 255.0f, 0 / 255.0f, 255 / 255.0f); // Blue

	ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix[camera_selected], glm::vec3(0.0f, 0.0f, 40.25f));
	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 180.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
	ModelViewProjectionMatrix *= Matrix_TIGER_tmp;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_object(OBJECT_OPTIMUS, 0 / 255.0f, 255 / 255.0f, 255 / 255.0f); // Cyan

	ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix[camera_selected], glm::vec3(-30.0f, 0.0f, 20.0f));
	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, 90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix *= Matrix_TIGER_tmp;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_object(OBJECT_TIGER, 255 / 255.0f, 0 / 255.0f, 255 / 255.0f); // Magenta

	ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix[camera_selected], glm::vec3(20.0f, 0.0f, -30.0f));
	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -45.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix *= Matrix_TIGER_tmp;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_object(OBJECT_TIGER, 0 / 255.0f, 255 / 255.0f, 0 / 255.0f); // Green

	ModelViewProjectionMatrix = glm::translate(ViewProjectionMatrix[camera_selected], glm::vec3(3.0f, 0.0f, 20.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glPointSize(5.0f);
	//draw_points(255 / 255.0f, 0 / 255.0f, 0 / 255.0f);
	glPointSize(1.0f);

	ModelViewProjectionMatrix = glm::rotate(ModelViewProjectionMatrix, -135.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewProjectionMatrix *= Matrix_TIGER_tmp;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_object(OBJECT_TIGER, 255 / 255.0f, 0 / 255.0f, 0 / 255.0f); // Red

	glutSwapBuffers();

}

int prevx, prevy;
void motion(int x, int y) {
	float a,b;
	glm::mat3 rot;
	rot[0].x = camera_wv[camera_selected].uaxis.x; rot[0].y = camera_wv[camera_selected].vaxis.x; rot[0].z = camera_wv[camera_selected].naxis.x;
	rot[1].x = camera_wv[camera_selected].uaxis.x; rot[1].y = camera_wv[camera_selected].vaxis.x; rot[1].z = camera_wv[camera_selected].naxis.x;
	rot[2].x = camera_wv[camera_selected].uaxis.x; rot[2].y = camera_wv[camera_selected].vaxis.x; rot[2].z = camera_wv[camera_selected].naxis.x;

	sp += (float)(y - prevy)*0.005;
	if (camera_wv[camera_selected].flag_move_mode==2 && (camera_type == CAMERA_DRIVER)) {
		//car_status.body_yaw_angle += BODY_YAW_SENSITIVITY*(x - prevx);
		//update_car_body_transformation();

/*
#ifdef USE_set_ViewMatrix_for_driver2
			set_ViewMatrix_for_driver2();
#else
			
#endif
*/

	
//			renew_camera_wv_frame_rotation_around_axis((float)(x - prevx));
		set_ViewMatrix_for_driver();
		if (camera_wv[camera_selected].flag_move_mode == 2)
			renew_cam_orientation_rotation_around_axis(camera_selected, (float)(x - prevx));
		ProjectionMatrix[camera_selected] = glm::perspective(TO_RADIAN*camera_wv[camera_selected].fovy, camera_wv[camera_selected].aspect_ratio, camera_wv[camera_selected].near_c, camera_wv[camera_selected].far_c);
		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];
		glutPostRedisplay();
	}
	else if (camera_wv[camera_selected].flag_move_mode && (camera_type == CAMERA_WORLD_VIEWER)) {
		// The world camera moves only when the scene is viewed from the world camera.
		if (key_state == 't') {
			for (int i = 0; i < 8; i++) {

				tracer_dir[i] = { (x - win_width / 2) - tracer_pos[i].x, (win_height / 2 - y) - tracer_pos[i].y,0 };
				
				if (tracer_dir[i].x == 0 && tracer_dir[i].y == 0) {
					tracer_dir[i].x = 1; tracer_dir[i].y = 1;  tracer_dir[i].z = 1;
				}
				/* Á¤±ÔÈ­ */
				tracer_dir[i] = glm::normalize(tracer_dir[i]);
			}
			m_posx = x; m_posy = y;
		}
		else if(camera_wv[camera_selected].flag_move_mode==1) renew_camera_wv_frame_rotation_around_axis((float)(x - prevx));
		else if (camera_wv[camera_selected].flag_move_mode == 2) renew_cam_orientation_rotation_around_axis(camera_selected, (float)(x - prevx));

		
		set_ViewMatrix_for_world_viewer();
		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];
		glutPostRedisplay();
	}
	else if (camera_wv[camera_selected].flag_move_mode && (camera_type == CAMERA_OBSERVER)) {
		// The world camera moves only when the scene is viewed from the world camera.
		

		renew_camera_observe((float)(x - prevx));
		//set_ViewMatrix_for_observer();
		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];
		glutPostRedisplay();
	} 
	prevx = x; prevy = y;
	
}

void move_world_cam(int dir ,glm::vec3 axis) {
	set_ViewMatrix_for_world_viewer();
	camera_wv[0].pos += CAM_TRANSLATION_SPEED * 10* dir * (-axis);
	ProjectionMatrix[camera_selected] = glm::perspective(TO_RADIAN*camera_wv[0].fovy, camera_wv[0].aspect_ratio, camera_wv[0].near_c, camera_wv[0].far_c);
	ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
	glutPostRedisplay();
}

void move_obs_cam(int dir, glm::vec3 axis) {
	camera_wv[2].pos += CAM_TRANSLATION_SPEED * 1 * dir * (glm::vec3(0, 0, 0) - camera_wv[2].pos);
	set_ViewMatrix_for_observer();
	//ProjectionMatrix[camera_selected] = glm::perspective(TO_RADIAN*camera_wv[2].fovy, camera_wv[2].aspect_ratio, camera_wv[2].near_c, camera_wv[2].far_c);
	//ViewProjectionMatrix[2] = ProjectionMatrix[2] * ViewMatrix[2];
	//glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case '1':
		key_state = 'x';
		camera_selected = CAMERA_DRIVER;
		camera_type = CAMERA_DRIVER;
		set_ViewMatrix_for_driver();
		ProjectionMatrix[camera_selected] = glm::perspective(TO_RADIAN*camera_wv[camera_selected].fovy, camera_wv[camera_selected].aspect_ratio, camera_wv[camera_selected].near_c, camera_wv[camera_selected].far_c);
		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];
		glutPostRedisplay();
		break;
	case '0':
		key_state = 'x';
		camera_selected = CAMERA_WORLD_VIEWER;
		camera_type = CAMERA_WORLD_VIEWER;
		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];
		glutPostRedisplay();
		break;
	case '2':
		key_state = 'x';
		camera_selected = CAMERA_OBSERVER;
		camera_type = CAMERA_OBSERVER; 
		
		ProjectionMatrix[camera_selected] = glm::perspective(TO_RADIAN*camera_wv[camera_selected].fovy, camera_wv[camera_selected].aspect_ratio, camera_wv[camera_selected].near_c, camera_wv[camera_selected].far_c);
		ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];
		glutPostRedisplay();
		break;
	case 'w':
		if (camera_selected == CAMERA_WORLD_VIEWER)move_world_cam(-1, -camera_wv[0].naxis);
		else if (camera_selected == CAMERA_OBSERVER)
			if (glm::distance(camera_wv[camera_selected].pos, glm::vec3(0)) > 5) {
				camera_wv[2].pos += CAM_TRANSLATION_SPEED * 1 * 1 * (glm::vec3(0, 0, 0) - camera_wv[2].pos);
				renew_camera_observe(0);
			}
		break;
	case 's':
		if (camera_selected == CAMERA_WORLD_VIEWER)move_world_cam(1, -camera_wv[0].naxis);
		else if (camera_selected == CAMERA_OBSERVER) {
			camera_wv[2].pos += CAM_TRANSLATION_SPEED * 1 * 1 * (glm::vec3(0, 0, 0) + camera_wv[2].pos);
			renew_camera_observe(0);
		}
		break;
	case 'a':
		if (camera_selected == CAMERA_WORLD_VIEWER)move_world_cam(-1, -camera_wv[0].uaxis);
		break;
	case 'd':
		if (camera_selected == CAMERA_WORLD_VIEWER)move_world_cam(1, -camera_wv[0].uaxis);
		break;
	case 'e':
		if (camera_selected == CAMERA_WORLD_VIEWER)move_world_cam(-1, -camera_wv[0].vaxis);
		break;
	case 'q':
		if (camera_selected == CAMERA_WORLD_VIEWER)move_world_cam(1, -camera_wv[0].vaxis);
		break;
	case 'x': rot_state = 'x'; break;
	case 'y': rot_state = 'y'; break;
	case 'z': rot_state = 'z'; break;
	case 't': key_state = 't'; camera_type = CAMERA_WORLD_VIEWER; camera_selected = 0; break;
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}
float car_del;
void special(int key, int x, int y) {
	float speed = 0;
	static float delx=0, delz=0;

	switch (key) {
	case GLUT_KEY_UP:
		car_status.front_wheel_roll_angle += FRONT_WHEEL_ROLL_ANGLE_DELTA;
		speed+=car_status.front_wheel_roll_angle*0.1;
		delx = car_status.dir.x*TO_RADIAN*FRONT_WHEEL_ROLL_ANGLE_DELTA;
		delz = car_status.dir.z*TO_RADIAN*FRONT_WHEEL_ROLL_ANGLE_DELTA;
		ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, TO_RAD*car_status.front_wheel_turn_angle, glm::vec3(0, 1, 0));
		ModelMatrix_CAR_BODY = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(car_status.pos.x - delx, 0, car_status.pos.z - delz));
		H_OBJ_B = glm::translate(H_OBJ_B, glm::vec3(H_OBJ_POS.x - delx, 0, H_OBJ_POS.z - delz));

		break;
	case GLUT_KEY_DOWN:
		car_status.front_wheel_roll_angle -= FRONT_WHEEL_ROLL_ANGLE_DELTA;
		speed-=car_status.front_wheel_roll_angle*0.1;
		delx = -car_status.dir.x*TO_RADIAN*FRONT_WHEEL_ROLL_ANGLE_DELTA;
		delz = -car_status.dir.z*TO_RADIAN*FRONT_WHEEL_ROLL_ANGLE_DELTA;
		ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, TO_RAD*car_status.front_wheel_turn_angle, glm::vec3(0, -1, 0));
		ModelMatrix_CAR_BODY = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(car_status.pos.x - delx, 0, car_status.pos.z - delz));
		H_OBJ_B = glm::translate(H_OBJ_B, glm::vec3(H_OBJ_POS.x - delx, 0, H_OBJ_POS.z - delz));

		break;
	case GLUT_KEY_LEFT:
		car_status.front_wheel_turn_angle += FRONT_WHEEL_TURN_ANGLE_DELTA;
		if (car_status.front_wheel_turn_angle > FRONT_WHEEL_TURN_ANGLE_MAX)
			car_status.front_wheel_turn_angle = FRONT_WHEEL_TURN_ANGLE_MAX;
		break;
	case GLUT_KEY_RIGHT:
		car_status.front_wheel_turn_angle -= FRONT_WHEEL_TURN_ANGLE_DELTA;
		if (car_status.front_wheel_turn_angle < -FRONT_WHEEL_TURN_ANGLE_MAX)
			car_status.front_wheel_turn_angle = -FRONT_WHEEL_TURN_ANGLE_MAX;
		break;
	}
	car_status.dir = car_status.Matrix_CAR_WHEEL_ROT*glm::vec4(1,0,0,0);
	glm::normalize(car_status.dir);
	/* */
	set_ViewMatrix_for_driver();
	ProjectionMatrix[1] = glm::perspective(TO_RADIAN*camera_wv[1].fovy, camera_wv[1].aspect_ratio, camera_wv[1].near_c, camera_wv[1].far_c);
	ViewProjectionMatrix[1] = ProjectionMatrix[1] * ViewMatrix[1];

	update_car_front_wheel_rotate_matrix();
	update_HOBJ_front_wheel_rotate_matrix();

	glutPostRedisplay();
}


void mousescroll(int button, int dir, int x, int y) {
	if (camera_selected == 1)set_ViewMatrix_for_driver();

	if (dir > 0) camera_wv[camera_selected].fovy--;
		//camera_wv[camera_selected].pos += CAM_TRANSLATION_SPEED * 10 * (-camera_wv[camera_selected].naxis);
	else if (dir <0) camera_wv[camera_selected].fovy++;
		//camera_wv[camera_selected].pos -= CAM_TRANSLATION_SPEED * 10 * (-camera_wv[camera_selected].naxis);
	
	ProjectionMatrix[camera_selected] = glm::perspective(TO_RADIAN*camera_wv[camera_selected].fovy, camera_wv[camera_selected].aspect_ratio, camera_wv[camera_selected].near_c, camera_wv[camera_selected].far_c);

	ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];
	//glutSwapBuffers();
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON)) {
		if (state == GLUT_DOWN) {
			camera_wv[camera_selected].flag_move_mode = 2;
			//car_status.flag_body_yaw_mode = 1; 
			prevx = x; prevy = y;
		}
		else if (state == GLUT_UP) 
			camera_wv[camera_selected].flag_move_mode = 0;
			//car_status.flag_body_yaw_mode = 0;
	}
	else if ((button == GLUT_RIGHT_BUTTON)) {
		if (state == GLUT_DOWN) {
			camera_wv[camera_selected].flag_move_mode = 1;
			prevx = x; prevy = y;
		}
		else if (state == GLUT_UP)
			camera_wv[camera_selected].flag_move_mode = 0;
	}
}

void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	win_width = width; win_height = height;
	camera_wv[camera_selected].aspect_ratio = (float)width / height;

	ProjectionMatrix[camera_selected] = glm::perspective(TO_RADIAN*camera_wv[camera_selected].fovy, camera_wv[camera_selected].aspect_ratio, camera_wv[camera_selected].near_c, camera_wv[camera_selected].far_c);
	ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];

	glutPostRedisplay();
}

void cleanup(void) {
	free_axes();
	free_path();

	free_geom_obj(GEOM_OBJ_ID_CAR_BODY);
	free_geom_obj(GEOM_OBJ_ID_CAR_WHEEL);
	free_geom_obj(GEOM_OBJ_ID_CAR_NUT);
	free_geom_obj(GEOM_OBJ_ID_CAR_BODY);
//	free_geom_obj(GEOM_OBJ_ID_COW);
	free_geom_obj(GEOM_OBJ_ID_TEAPOT);
	free_geom_obj(GEOM_OBJ_ID_BOX);
}
/*********************************  END: callbacks *********************************/
void timer(int value) {

	cir_pos += cir_dir;
	if (cir_pos.x > 25) { cir_dir.x *= -1; }
	else if (cir_pos.x < 5) { cir_dir.x *= -1;}
	if (cir_pos.y > 25) { cir_dir.y *= -1; }
	else if (cir_pos.y < 5) { cir_dir.y *= -1;}
	if (cir_pos.z > 25) { cir_dir.z *= -1;}
	else if (cir_pos.z < 5) { cir_dir.z *= -1; }
	r_sp++;
	r_sp %= 720;
	glutPostRedisplay();
	glutTimerFunc(10, timer, 0);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutMouseWheelFunc(mousescroll);
	//glutPassiveMotionFunc(motion);
	
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutTimerFunc(10, timer, 0);
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void prepare_geometry(void) {
	prepare_axes(); 
	prepare_path();

	prepare_geom_obj(GEOM_OBJ_ID_CAR_BODY, "Data/car_body_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_CAR_WHEEL, "Data/car_wheel_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_CAR_NUT, "Data/car_nut_triangles_v.txt", GEOM_OBJ_TYPE_V);
//	prepare_geom_obj(GEOM_OBJ_ID_COW, "Data/cow_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_TEAPOT, "Data/teapot_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_BOX, "Data/box_triangles_v.txt", GEOM_OBJ_TYPE_V);
	prepare_cir();
	prepare_WC();
//	prepare_scene();
}

void initialize_scene(void) {
	prepare_geometry();
	initialize_camera();
	initialize_car();
	prepare_axes();
	prepare_square();
	prepare_tiger();
	prepare_airplane();
	prepare_dragon();
	prepare_optimus();
}

void initialize_OpenGL(void) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);

	glClearColor(11.0f / 255.0f, 83.0f / 255.0f, 69.0f / 255.0f, 1.0f);

	ViewMatrix[camera_selected] = glm::mat4(1.0f);
	ProjectionMatrix[camera_selected] = glm::mat4(1.0f);
	ViewProjectionMatrix[camera_selected] = ProjectionMatrix[camera_selected] * ViewMatrix[camera_selected];

	ModelMatrix_CAR_BODY = glm::translate(glm::mat4(1.0f),car_status.pos);
	ModelMatrix_CAR_WHEEL = glm::mat4(1.0f);
	ModelMatrix_CAR_NUT = glm::mat4(1.0f);

	ModelMatrix_CAR_BODY_INVERSE = glm::mat4(1.0f);
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	initialize_scene();
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

void print_message(const char * m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. i. & Eng., Sogang University.\n\n");
																									
	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 2
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 4.7.3.Hier_Car_Model_Driver_GLSL";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: 'f', l', 'd', 'w', four arrows, 'ESC'",
											"    - Mouse used: L/R-Click and move" };
	srand(time(0));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();
	glutMainLoop();
}
