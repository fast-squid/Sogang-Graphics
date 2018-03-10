#define NUMBER_OF_CAMERAS 3
#define CAM_TRANSLATION_SPEED 0.25f
#define CAM_ROTATION_SPEED 0.1f
#define CAMER_ROTATION_RADIUS		50.0f

enum _CameraType { WORLD_CAM, CAM2 } camera_type;

typedef struct _Camera {
	glm::vec3 pos;
	glm::vec3 uaxis, vaxis, naxis;
	float fovy, aspect_ratio, near_c, far_c;
	int flag_move_mode;
} Camera;
Camera camera[NUMBER_OF_CAMERAS];

int camera_selected;

glm::mat4 ModelViewProjectionMatrix, ModelViewProjectionMatrix_LI;
glm::mat4 ViewProjectionMatrix[NUMBER_OF_CAMERAS], ViewMatrix[NUMBER_OF_CAMERAS], ProjectionMatrix[NUMBER_OF_CAMERAS];
glm::mat4 ModelViewMatrix[NUMBER_OF_CAMERAS];

void initialize_camera(void) {
	camera_selected = WORLD_CAM;

	//ViewMatrix[0] = glm::lookAt(glm::vec3(300.0f, 400.0f, 300.0f), glm::vec3(0.0f, 0.0f, 0.0f),
	//	glm::vec3(0.0f, 1.0f, 0.0f));

	for (int i = 0; i < 3; i++) {
		camera[i].uaxis = glm::vec3(ViewMatrix[0][0].x, ViewMatrix[0][1].x, ViewMatrix[0][2].x);
		camera[i].vaxis = glm::vec3(ViewMatrix[0][0].y, ViewMatrix[0][1].y, ViewMatrix[0][2].y);
		camera[i].naxis = glm::vec3(ViewMatrix[0][0].z, ViewMatrix[0][1].z, ViewMatrix[0][2].z);
		camera[i].pos = glm::vec3(400, 500, 400);

		camera[i].flag_move_mode = 0;
		camera[i].fovy = 50.0f, camera[i].aspect_ratio = 1.0f; camera[i].near_c = 1.0f; camera[i].far_c =1400.0f;
		ProjectionMatrix[camera_selected] = glm::perspective(TO_RADIAN*camera[camera_selected].fovy, camera[camera_selected].aspect_ratio, 1.0f, 1400.0f);
	}

}
