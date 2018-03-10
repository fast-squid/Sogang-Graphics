#define NUMBER_OF_CAMERAS 3
#define CAM_TRANSLATION_SPEED 0.025f
#define CAM_ROTATION_SPEED 0.1f

enum _CameraType { CAMERA_WORLD_VIEWER, CAMERA_DRIVER , CAMERA_OBSERVER } camera_type;

typedef struct _Camera {
	glm::vec3 pos;
	glm::vec3 uaxis, vaxis, naxis;
	glm::mat4 ROT_MAT;
	float fovy, aspect_ratio, near_c, far_c;
	int flag_move_mode;
} Camera;
Camera camera_wv[NUMBER_OF_CAMERAS];
int camera_selected;
int rot_state;

glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewProjectionMatrix[NUMBER_OF_CAMERAS], ViewMatrix[NUMBER_OF_CAMERAS], ProjectionMatrix[NUMBER_OF_CAMERAS];

void set_ViewMatrix_for_world_viewer(void) {
	ViewMatrix[0] = glm::mat4(camera_wv[0].uaxis.x, camera_wv[0].vaxis.x, camera_wv[0].naxis.x, 0.0f,
		camera_wv[0].uaxis.y, camera_wv[0].vaxis.y, camera_wv[0].naxis.y, 0.0f,
		camera_wv[0].uaxis.z, camera_wv[0].vaxis.z, camera_wv[0].naxis.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	ViewMatrix[0] = glm::translate(ViewMatrix[0], -camera_wv[0].pos);
}

void set_ViewMatrix_for_driver(void) {
	ViewMatrix[1] = glm::affineInverse(ModelMatrix_CAR_BODY * ModelMatrix_CAR_BODY_to_DRIVER*camera_wv[camera_selected].ROT_MAT);
}
void set_ViewMatrix_for_observer(void) {
	ViewMatrix[2] = glm::mat4(camera_wv[2].uaxis.x, camera_wv[2].vaxis.x, camera_wv[2].naxis.x, 0.0f,
		camera_wv[2].uaxis.y, camera_wv[2].vaxis.y, camera_wv[2].naxis.y, 0.0f,
		camera_wv[2].uaxis.z, camera_wv[2].vaxis.z, camera_wv[2].naxis.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	ViewMatrix[2] = glm::translate(ViewMatrix[2],-camera_wv[2].pos);
}
void renew_cam_orientation_rotation_around_axis(int cam_index, float angle) {
	glm::mat3 RotationMatrix;
	glm::vec3 axis;
	if (rot_state == 'x') axis = camera_wv[cam_index].uaxis;
	else if (rot_state == 'y') axis = camera_wv[cam_index].vaxis;
	else if (rot_state == 'z') axis = -camera_wv[cam_index].naxis;
	RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0f), CAM_ROTATION_SPEED*TO_RADIAN*angle, axis));
	camera_wv[cam_index].ROT_MAT = glm::rotate(camera_wv[cam_index].ROT_MAT, CAM_ROTATION_SPEED*TO_RADIAN*angle, axis);

	camera_wv[cam_index].uaxis = RotationMatrix * camera_wv[cam_index].uaxis;
	camera_wv[cam_index].vaxis = RotationMatrix * camera_wv[cam_index].vaxis;
	camera_wv[cam_index].naxis = RotationMatrix * camera_wv[cam_index].naxis;

	//else
}

void set_ViewMatrix_from_camera_frame(int cam_index) {
	ViewMatrix[cam_index][0] = glm::vec4(camera_wv[cam_index].uaxis.x, camera_wv[cam_index].vaxis.x, camera_wv[cam_index].naxis.x, 0.0f);
	ViewMatrix[cam_index][1] = glm::vec4(camera_wv[cam_index].uaxis.y, camera_wv[cam_index].vaxis.y, camera_wv[cam_index].naxis.y, 0.0f);
	ViewMatrix[cam_index][2] = glm::vec4(camera_wv[cam_index].uaxis.z, camera_wv[cam_index].vaxis.z, camera_wv[cam_index].naxis.z, 0.0f);
	ViewMatrix[cam_index][3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	ViewMatrix[cam_index] = glm::translate(ViewMatrix[cam_index], -camera_wv[cam_index].pos);
}

#define CAMER_ROTATION_RADIUS		50.0f
void initialize_camera(void) {
	camera_type = CAMERA_WORLD_VIEWER;
	rot_state = 'y';
	ViewMatrix[0] = glm::lookAt(glm::vec3(0.0f, 10.0f, CAMER_ROTATION_RADIUS), glm::vec3(0.0f, 0.0f, 0.0f),
							 glm::vec3(0.0f, 1.0f, 0.0f));
	
	for (int i = 0; i < 3; i++) {
		camera_wv[i].uaxis = glm::vec3(ViewMatrix[0][0].x, ViewMatrix[0][1].x, ViewMatrix[0][2].x);
		camera_wv[i].vaxis = glm::vec3(ViewMatrix[0][0].y, ViewMatrix[0][1].y, ViewMatrix[0][2].y);
		camera_wv[i].naxis = glm::vec3(ViewMatrix[0][0].z, ViewMatrix[0][1].z, ViewMatrix[0][2].z);
		camera_wv[i].pos = -(ViewMatrix[0][3].x*camera_wv[0].uaxis + ViewMatrix[0][3].y*camera_wv[0].vaxis + ViewMatrix[0][3].z*camera_wv[0].naxis);
	
		camera_wv[i].flag_move_mode = 0;
		camera_wv[i].fovy = 30.0f, camera_wv[i].aspect_ratio = 1.0f; camera_wv[i].near_c = 5.0f; camera_wv[i].far_c = 500.0f;
	}
	camera_wv[2].uaxis = glm::vec3(0,0, 1);
	camera_wv[2].vaxis = glm::vec3(1, 0,0);
	camera_wv[2].naxis = glm::vec3(0, 1, 0);
	camera_wv[2].pos = { 0,40,0 };
	
	set_ViewMatrix_for_observer();
	ProjectionMatrix[2] = glm::perspective(TO_RADIAN*camera_wv[2].fovy, camera_wv[2].aspect_ratio, camera_wv[2].near_c, camera_wv[2].far_c);
	ViewProjectionMatrix[2] = ProjectionMatrix[2] * ViewMatrix[2];
	
	ModelMatrix_CAR_BODY_to_DRIVER = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.5f, 2.5f));
	ModelMatrix_CAR_BODY_to_DRIVER = glm::rotate(ModelMatrix_CAR_BODY_to_DRIVER,
		TO_RADIAN*90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	ModelMatrix_CAR_BODY_to_DRIVER_INVERSE = glm::rotate(glm::mat4(1.0f), -TO_RADIAN*90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_CAR_BODY_to_DRIVER_INVERSE = glm::translate(ModelMatrix_CAR_BODY_to_DRIVER_INVERSE, 
																			-glm::vec3(-3.0f, 0.5f, 2.5f));
} 
#define CAMERA_ROTATION_SENSITIVITY			0.25f
void renew_camera_wv_frame_rotation_around_axis(float angle) {
	glm::mat3 RotationMatrix;
	glm::vec3 axis;
	
	if (rot_state == 'x') axis=glm::vec3(1.0f,0.0f,0.0f);
	else if (rot_state == 'y') axis = glm::vec3(0.0f, 1.0f, 0.0f);
	else if (rot_state == 'z') axis = glm::vec3(0.0f, 0.0f, 1.0f);

	RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0), 
		CAMERA_ROTATION_SENSITIVITY*TO_RADIAN*angle, axis));  
	
	camera_wv[camera_selected].pos = RotationMatrix*camera_wv[camera_selected].pos;

	camera_wv[camera_selected].uaxis = RotationMatrix*camera_wv[camera_selected].uaxis;
	camera_wv[camera_selected].vaxis = RotationMatrix*camera_wv[camera_selected].vaxis;
	camera_wv[camera_selected].naxis = RotationMatrix*camera_wv[camera_selected].naxis;
}
void renew_camera_observe(float angle) {
	glm::mat4 mat4_tmp=glm::mat4(1);
	glm::vec3 aaa;

	
	if (rot_state == 'y'|| rot_state == 'z') aaa = glm::cross(camera_wv[2].vaxis, glm::vec3(0, 0, 0) - camera_wv[2].pos);
	else if (rot_state == 'x') aaa = glm::cross(camera_wv[2].uaxis, glm::vec3(0, 0, 0) - camera_wv[2].pos);

	mat4_tmp = glm::rotate(mat4_tmp, CAMERA_ROTATION_SENSITIVITY*angle*TO_RADIAN, aaa);
//	mat4_tmp = glm::translate(mat4_tmp, glm::vec3(0,0,0));

	camera_wv[2].pos = glm::vec3(mat4_tmp*glm::vec4(camera_wv[2].pos, 1));
	camera_wv[2].vaxis = glm::vec3(mat4_tmp*glm::vec4(camera_wv[2].vaxis, 0));
	
	ViewMatrix[2] = glm::lookAt(camera_wv[2].pos, glm::vec3(0,0,0),camera_wv[2].vaxis);

	ViewProjectionMatrix[2] = ProjectionMatrix[2] * ViewMatrix[2];
	camera_wv[2].naxis = glm::vec3(0, 0, 0) - camera_wv[2].pos;
	//camera_wv[2].uaxis = glm::cross(camera_wv[2].naxis, camera_wv[2].vaxis);
	glutPostRedisplay();
	
}

