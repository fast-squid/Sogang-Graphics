#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0

int win_width = 0, win_height = 0;
float centerx = 0.0f, centery = 0.0f, rotate_angle = 0.0f;

/* added global variable*//////////////////////////////////////
int m_prevx=0, m_prevy =0,m_posx=0,m_posy=0;
char key_state='f';
float air_dragx=0,air_dragy=0;
float car_delx, car_dely;
int rev = 1;
int refx = 1;

glm::vec3 cock_dir[8] = {  { 0,0,0},{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 } };
float cock_posx[8] = { 0 }, cock_posy[8] = { 0 };
float cock_speed[8] = { 0 };

float colis_posx[4] = { 0 }, colis_posy[4] = { 0 }, colis_posz[4] = { 0 }, colis_speed[4];
glm::vec3 colis_dir[4];
int init = 0;
///////////////////////////////////////////////////////////////

GLfloat axes[4][2];
GLfloat axes_color[3] = { 0.0f, 0.0f, 0.0f };
GLuint VBO_axes, VAO_axes;

void prepare_axes(void) { // Draw axes in their MC.
	axes[0][0] = -win_width / 2.5f; axes[0][1] = 0.0f;
	axes[1][0] = win_width / 2.5f;	axes[1][1] = 0.0f;
	axes[2][0] = 0.0f;				axes[2][1] = -win_height / 2.5f;
	axes[3][0] = 0.0f;				axes[3][1] = win_height / 2.5f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_axes(void) {
	axes[0][0] = -win_width / 2.25f; axes[1][0] = win_width / 2.25f;
	axes[2][1] = -win_height / 2.25f;
	axes[3][1] = win_height / 2.25f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_axes(void) {
	glUniform3fv(loc_primitive_color, 1, axes_color);
	glBindVertexArray(VAO_axes);
	glDrawArrays(GL_LINES, 0, 4);
	glBindVertexArray(0);
}

GLfloat line[2][2];
GLfloat line_color[3] = { 1.0f, 0.0f, 0.0f };
GLuint VBO_line, VAO_line;

void prepare_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height;
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f;
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_line);
	glBindVertexArray(VAO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height;
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f;
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_line(void) { // Draw line in its MC.
					   // y = x - win_height/4
	glUniform3fv(loc_primitive_color, 1, line_color);
	glBindVertexArray(VAO_line);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

#define AIRPLANE_BIG_WING 0
#define AIRPLANE_SMALL_WING 1
#define AIRPLANE_BODY 2
#define AIRPLANE_BACK 3
#define AIRPLANE_SIDEWINDER1 4
#define AIRPLANE_SIDEWINDER2 5
#define AIRPLANE_CENTER 6
GLfloat big_wing[6][2] = { { 0.0, 0.0 },{ -20.0, 15.0 },{ -20.0, 20.0 },{ 0.0, 23.0 },{ 20.0, 20.0 },{ 20.0, 15.0 } };
GLfloat small_wing[6][2] = { { 0.0, -18.0 },{ -11.0, -12.0 },{ -12.0, -7.0 },{ 0.0, -10.0 },{ 12.0, -7.0 },{ 11.0, -12.0 } };
GLfloat body[5][2] = { { 0.0, -25.0 },{ -6.0, 0.0 },{ -6.0, 22.0 },{ 6.0, 22.0 },{ 6.0, 0.0 } };
GLfloat back[5][2] = { { 0.0, 25.0 },{ -7.0, 24.0 },{ -7.0, 21.0 },{ 7.0, 21.0 },{ 7.0, 24.0 } };
GLfloat sidewinder1[5][2] = { { -20.0, 10.0 },{ -18.0, 3.0 },{ -16.0, 10.0 },{ -18.0, 20.0 },{ -20.0, 20.0 } };
GLfloat sidewinder2[5][2] = { { 20.0, 10.0 },{ 18.0, 3.0 },{ 16.0, 10.0 },{ 18.0, 20.0 },{ 20.0, 20.0 } };
GLfloat center[1][2] = { { 0.0, 0.0 } };
GLfloat airplane_color[7][3] = {
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // big_wing
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // small_wing
	{ 111 / 255.0f,  85 / 255.0f, 157 / 255.0f },  // body
	{ 150 / 255.0f, 129 / 255.0f, 183 / 255.0f },  // back
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder1
	{ 245 / 255.0f, 211 / 255.0f,   0 / 255.0f },  // sidewinder2
	{ 255 / 255.0f,   0 / 255.0f,   0 / 255.0f }   // center
};

GLuint VBO_airplane, VAO_airplane;

int airplane_clock = 0;
int car_clock = 0;
int car_clock2 = 0;

float airplane_s_factor = 1.0f;

void prepare_airplane() {
	GLsizeiptr buffer_size = sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1) + sizeof(sidewinder2) + sizeof(center);
	
	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(big_wing), big_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing), sizeof(small_wing), small_wing);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing), sizeof(body), body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body), sizeof(back), back);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back),
		sizeof(sidewinder1), sidewinder1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1), sizeof(sidewinder2), sidewinder2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(big_wing) + sizeof(small_wing) + sizeof(body) + sizeof(back)
		+ sizeof(sidewinder1) + sizeof(sidewinder2), sizeof(center), center);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_airplane);
	glBindVertexArray(VAO_airplane);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_airplane);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_airplane() { // Draw airplane in its MC.
	glBindVertexArray(VAO_airplane);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BIG_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SMALL_WING]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_BACK]);
	glDrawArrays(GL_TRIANGLE_FAN, 17, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER1]);
	glDrawArrays(GL_TRIANGLE_FAN, 22, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_SIDEWINDER2]);
	glDrawArrays(GL_TRIANGLE_FAN, 27, 5);

	glUniform3fv(loc_primitive_color, 1, airplane_color[AIRPLANE_CENTER]);
	glPointSize(5.0);
	glDrawArrays(GL_POINTS, 32, 1);
	glPointSize(1.0);
	glBindVertexArray(0);
}

//shirt
#define SHIRT_LEFT_BODY 0
#define SHIRT_RIGHT_BODY 1
#define SHIRT_LEFT_COLLAR 2
#define SHIRT_RIGHT_COLLAR 3
#define SHIRT_FRONT_POCKET 4
#define SHIRT_BUTTON1 5
#define SHIRT_BUTTON2 6
#define SHIRT_BUTTON3 7
#define SHIRT_BUTTON4 8
GLfloat left_body[6][2] = { { 0.0, -9.0 },{ -8.0, -9.0 },{ -11.0, 8.0 },{ -6.0, 10.0 },{ -3.0, 7.0 },{ 0.0, 9.0 } };
GLfloat right_body[6][2] = { { 0.0, -9.0 },{ 0.0, 9.0 },{ 3.0, 7.0 },{ 6.0, 10.0 },{ 11.0, 8.0 },{ 8.0, -9.0 } };
GLfloat left_collar[4][2] = { { 0.0, 9.0 },{ -3.0, 7.0 },{ -6.0, 10.0 },{ -4.0, 11.0 } };
GLfloat right_collar[4][2] = { { 0.0, 9.0 },{ 4.0, 11.0 },{ 6.0, 10.0 },{ 3.0, 7.0 } };
GLfloat front_pocket[6][2] = { { 5.0, 0.0 },{ 4.0, 1.0 },{ 4.0, 3.0 },{ 7.0, 3.0 },{ 7.0, 1.0 },{ 6.0, 0.0 } };
GLfloat button1[3][2] = { { -1.0, 6.0 },{ 1.0, 6.0 },{ 0.0, 5.0 } };
GLfloat button2[3][2] = { { -1.0, 3.0 },{ 1.0, 3.0 },{ 0.0, 2.0 } };
GLfloat button3[3][2] = { { -1.0, 0.0 },{ 1.0, 0.0 },{ 0.0, -1.0 } };
GLfloat button4[3][2] = { { -1.0, -3.0 },{ 1.0, -3.0 },{ 0.0, -4.0 } };

GLfloat shirt_color[9][3] = {
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f }
};

GLuint VBO_shirt, VAO_shirt;
void prepare_shirt() {
	GLsizeiptr buffer_size = sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2) + sizeof(button3) + sizeof(button4);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(left_body), left_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body), sizeof(right_body), right_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body), sizeof(left_collar), left_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar), sizeof(right_collar), right_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar),
		sizeof(front_pocket), front_pocket);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket), sizeof(button1), button1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1), sizeof(button2), button2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2), sizeof(button3), button3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body) + sizeof(right_body) + sizeof(left_collar) + sizeof(right_collar)
		+ sizeof(front_pocket) + sizeof(button1) + sizeof(button2) + sizeof(button3), sizeof(button4), button4);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_shirt);
	glBindVertexArray(VAO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_shirt() {
	glBindVertexArray(VAO_shirt);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_LEFT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_RIGHT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_LEFT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_RIGHT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_FRONT_POCKET]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON1]);
	glDrawArrays(GL_TRIANGLE_FAN, 26, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON2]);
	glDrawArrays(GL_TRIANGLE_FAN, 29, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON3]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON4]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);
	glBindVertexArray(0);
}

//house
#define HOUSE_ROOF 0
#define HOUSE_BODY 1
#define HOUSE_CHIMNEY 2
#define HOUSE_DOOR 3
#define HOUSE_WINDOW 4

GLfloat roof[3][2] = { { -12.0, 0.0 },{ 0.0, 12.0 },{ 12.0, 0.0 } };
GLfloat house_body[4][2] = { { -12.0, -14.0 },{ -12.0, 0.0 },{ 12.0, 0.0 },{ 12.0, -14.0 } };
GLfloat chimney[4][2] = { { 6.0, 6.0 },{ 6.0, 14.0 },{ 10.0, 14.0 },{ 10.0, 2.0 } };
GLfloat door[4][2] = { { -8.0, -14.0 },{ -8.0, -8.0 },{ -4.0, -8.0 },{ -4.0, -14.0 } };
GLfloat window[4][2] = { { 4.0, -6.0 },{ 4.0, -2.0 },{ 8.0, -2.0 },{ 8.0, -6.0 } };

GLfloat house_color[5][3] = {
	{ 200 / 255.0f, 39 / 255.0f, 42 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 233 / 255.0f, 113 / 255.0f, 23 / 255.0f },
	{ 44 / 255.0f, 180 / 255.0f, 49 / 255.0f }
};

GLuint VBO_house, VAO_house;
void prepare_house() {
	GLsizeiptr buffer_size = sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door)
		+ sizeof(window);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(roof), roof);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof), sizeof(house_body), house_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body), sizeof(chimney), chimney);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney), sizeof(door), door);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof) + sizeof(house_body) + sizeof(chimney) + sizeof(door),
		sizeof(window), window);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_house);
	glBindVertexArray(VAO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_house() {
	glBindVertexArray(VAO_house);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_ROOF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 3, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_CHIMNEY]);
	glDrawArrays(GL_TRIANGLE_FAN, 7, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_DOOR]);
	glDrawArrays(GL_TRIANGLE_FAN, 11, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glBindVertexArray(0);
}

//car
#define CAR_BODY 0
#define CAR_FRAME 1
#define CAR_WINDOW 2
#define CAR_LEFT_LIGHT 3
#define CAR_RIGHT_LIGHT 4
#define CAR_LEFT_WHEEL 5
#define CAR_RIGHT_WHEEL 6

GLfloat car_body[4][2] = { { -16.0, -8.0 },{ -16.0, 0.0 },{ 16.0, 0.0 },{ 16.0, -8.0 } };
GLfloat car_frame[4][2] = { { -10.0, 0.0 },{ -10.0, 10.0 },{ 10.0, 10.0 },{ 10.0, 0.0 } };
GLfloat car_window[4][2] = { { -8.0, 0.0 },{ -8.0, 8.0 },{ 8.0, 8.0 },{ 8.0, 0.0 } };
GLfloat car_left_light[4][2] = { { -9.0, -6.0 },{ -10.0, -5.0 },{ -9.0, -4.0 },{ -8.0, -5.0 } };
GLfloat car_right_light[4][2] = { { 9.0, -6.0 },{ 8.0, -5.0 },{ 9.0, -4.0 },{ 10.0, -5.0 } };
GLfloat car_left_wheel[4][2] = { { -10.0, -12.0 },{ -10.0, -8.0 },{ -6.0, -8.0 },{ -6.0, -12.0 } };
GLfloat car_right_wheel[4][2] = { { 6.0, -12.0 },{ 6.0, -8.0 },{ 10.0, -8.0 },{ 10.0, -12.0 } };

GLfloat car_color[7][3] = {
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 216 / 255.0f, 208 / 255.0f, 174 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f }
};

GLuint VBO_car, VAO_car;
void prepare_car() {
	GLsizeiptr buffer_size = sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light) + sizeof(car_left_wheel) + sizeof(car_right_wheel);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car_body), car_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body), sizeof(car_frame), car_frame);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame), sizeof(car_window), car_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window), sizeof(car_left_light), car_left_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light),
		sizeof(car_right_light), car_right_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light), sizeof(car_left_wheel), car_left_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body) + sizeof(car_frame) + sizeof(car_window) + sizeof(car_left_light)
		+ sizeof(car_right_light) + sizeof(car_left_wheel), sizeof(car_right_wheel), car_right_wheel);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car);
	glBindVertexArray(VAO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car() {
	glBindVertexArray(VAO_car);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_FRAME]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
	
	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);

	glBindVertexArray(0);
}

//draw cocktail
#define COCKTAIL_NECK 0
#define COCKTAIL_LIQUID 1
#define COCKTAIL_REMAIN 2
#define COCKTAIL_STRAW 3
#define COCKTAIL_DECO 4

GLfloat neck[6][2] = { { -6.0, -12.0 },{ -6.0, -11.0 },{ -1.0, 0.0 },{ 1.0, 0.0 },{ 6.0, -11.0 },{ 6.0, -12.0 } };
GLfloat liquid[6][2] = { { -1.0, 0.0 },{ -9.0, 4.0 },{ -12.0, 7.0 },{ 12.0, 7.0 },{ 9.0, 4.0 },{ 1.0, 0.0 } };
GLfloat remain[4][2] = { { -12.0, 7.0 },{ -12.0, 10.0 },{ 12.0, 10.0 },{ 12.0, 7.0 } };
GLfloat straw[4][2] = { { 7.0, 7.0 },{ 12.0, 12.0 },{ 14.0, 12.0 },{ 9.0, 7.0 } };
GLfloat deco[8][2] = { { 12.0, 12.0 },{ 10.0, 14.0 },{ 10.0, 16.0 },{ 12.0, 18.0 },{ 14.0, 18.0 },{ 16.0, 16.0 },{ 16.0, 14.0 },{ 14.0, 12.0 } };

GLfloat cocktail_color[5][3] = {
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 0 / 255.0f, 63 / 255.0f, 122 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 191 / 255.0f, 255 / 255.0f, 0 / 255.0f },
	{ 218 / 255.0f, 165 / 255.0f, 32 / 255.0f }
};
float cock_clock = 0;
GLuint VBO_cocktail, VAO_cocktail;
void prepare_cocktail() {
	GLsizeiptr buffer_size = sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw)
		+ sizeof(deco);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_cocktail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cocktail);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(neck), neck);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck), sizeof(liquid), liquid);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid), sizeof(remain), remain);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain), sizeof(straw), straw);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(neck) + sizeof(liquid) + sizeof(remain) + sizeof(straw),
		sizeof(deco), deco);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_cocktail);
	glBindVertexArray(VAO_cocktail);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cocktail);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_cocktail() {
	glBindVertexArray(VAO_cocktail);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_NECK]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_LIQUID]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_REMAIN]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_STRAW]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, cocktail_color[COCKTAIL_DECO]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 8);

	glBindVertexArray(0);
}

//draw car2
#define CAR2_BODY 0
#define CAR2_FRONT_WINDOW 1
#define CAR2_BACK_WINDOW 2
#define CAR2_FRONT_WHEEL 3
#define CAR2_BACK_WHEEL 4
#define CAR2_LIGHT1 5
#define CAR2_LIGHT2 6

GLfloat car2_body[8][2] = { { -18.0, -7.0 },{ -18.0, 0.0 },{ -13.0, 0.0 },{ -10.0, 8.0 },{ 10.0, 8.0 },{ 13.0, 0.0 },{ 18.0, 0.0 },{ 18.0, -7.0 } };
GLfloat car2_front_window[4][2] = { { -10.0, 0.0 },{ -8.0, 6.0 },{ -2.0, 6.0 },{ -2.0, 0.0 } };
GLfloat car2_back_window[4][2] = { { 0.0, 0.0 },{ 0.0, 6.0 },{ 8.0, 6.0 },{ 10.0, 0.0 } };
GLfloat car2_front_wheel[8][2] = { { -11.0, -11.0 },{ -13.0, -8.0 },{ -13.0, -7.0 },{ -11.0, -4.0 },{ -7.0, -4.0 },{ -5.0, -7.0 },{ -5.0, -8.0 },{ -7.0, -11.0 } };
GLfloat car2_back_wheel[8][2] = { { 7.0, -11.0 },{ 5.0, -8.0 },{ 5.0, -7.0 },{ 7.0, -4.0 },{ 11.0, -4.0 },{ 13.0, -7.0 },{ 13.0, -8.0 },{ 11.0, -11.0 } };
GLfloat car2_light1[3][2] = { { -18.0, -1.0 },{ -17.0, -2.0 },{ -18.0, -3.0 } };
GLfloat car2_light2[3][2] = { { -18.0, -4.0 },{ -17.0, -5.0 },{ -18.0, -6.0 } };

GLfloat car2_color[7][3] = {
	{ 100 / 255.0f, 141 / 255.0f, 159 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f }
};

GLuint VBO_car2, VAO_car2;
void prepare_car2() {
	GLsizeiptr buffer_size = sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel) + sizeof(car2_light1) + sizeof(car2_light2);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car2_body), car2_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body), sizeof(car2_front_window), car2_front_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window), sizeof(car2_back_window), car2_back_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window), sizeof(car2_front_wheel), car2_front_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel),
		sizeof(car2_back_wheel), car2_back_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel), sizeof(car2_light1), car2_light1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body) + sizeof(car2_front_window) + sizeof(car2_back_window) + sizeof(car2_front_wheel)
		+ sizeof(car2_back_wheel) + sizeof(car2_light1), sizeof(car2_light2), car2_light2);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car2);
	glBindVertexArray(VAO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car2() {
	glBindVertexArray(VAO_car2);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT1]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT2]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);

	glBindVertexArray(0);
}
GLfloat cir[30][2];
GLfloat cir_center[1][2] = { { 0.0, 0.0 } };
GLfloat cir_color[2][3] = { { 0.0f, 0.0f, 0.0f },{ 255 / 255.0f,   0 / 255.0f,   0 / 255.0f } };
GLuint VBO_cir, VAO_cir;

void prepare_cir(void) { // Draw circle.
	GLsizeiptr buffer_size = sizeof(cir) + sizeof(cir_center);

	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory
	for (int i = 0; i < 30; i++) {
		double angle;
		angle = i*3.141592 / 15;
		cir[i][0] = 20*cos(angle); 
		cir[i][1] = 20*sin(angle);
	}
	
	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_cir);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cir);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cir), cir);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cir), sizeof(cir_center), cir_center);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_cir);
	glBindVertexArray(VAO_cir);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cir);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_cir(void) {
	glBindBuffer(GL_ARRAY_BUFFER, VBO_cir);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cir), cir, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_cir(void) {
	glBindVertexArray(VAO_cir);
	glUniform3fv(loc_primitive_color, 1, cir_color[0]);

	glDrawArrays(GL_LINES, 0, 30);
	glBindVertexArray(0);
}

/* new 2d object*/
#define QWERTY_EYE 0
#define QWERTY_EYE_IN 1
#define QWERTY_FACE 2

GLfloat qwerty_face[50][2];
GLfloat qwerty_eye1[30][2];
GLfloat qwerty_eye_in1[15][2];
GLfloat qwerty_eye2[30][2];
GLfloat qwerty_eye_in2[15][2];
GLfloat qwerty_ear1[3][2] = { {-5,-2},{ 5,0 },{ 0,4.7 } };
GLfloat qwerty_ear2[3][2] = { { -5,0 },{ 5,-2 },{ 0,4.7 } };
GLfloat qwerty_nose[3][2] = { { -3,-2 },{ 3,-2 },{ 0,-4.7 } };
GLfloat qwerty_line1[2][2] = { { 0,-4.7 },{ 3,-5.3 }};
GLfloat qwerty_line2[2][2] = { { 0,-4.7 },{ -3,-5.3 } };
//GLfloat qwerty_center[1][2] = { { 0.0, 0.0 } };
GLfloat qwerty_color[3][3] = {
	{ 0 / 255.0f,   0 / 255.0f,   0 / 255.0f },
	{ 1.0f, 0.0f, 0.0f },
	{ 200 / 255.0f,   200 / 255.0f,   200 / 255.0f }
};
GLuint VBO_qwerty, VAO_qwerty;

void prepare_qwerty(void) { // Draw qwerty.
	GLsizeiptr buffer_size = sizeof(qwerty_face)+sizeof(qwerty_eye1) + sizeof(qwerty_eye_in1)+ sizeof(qwerty_eye2) + sizeof(qwerty_eye_in2)
		+sizeof(qwerty_ear1)+sizeof(qwerty_ear2)+ sizeof(qwerty_nose) +sizeof(qwerty_line1)+ sizeof(qwerty_line2);

	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory
	for (int i = 0; i < 3; i++) {
		qwerty_ear1[i][0] += 8;
		qwerty_ear1[i][1] += 14;
		qwerty_ear2[i][0] += -8;
		qwerty_ear2[i][1] += 14;
	}
	for (int i = 0; i < 50; i++) {
		double angle;
		angle = i*3.141592 / 25;
		qwerty_face[i][0] = 18 * cos(angle) ;
		qwerty_face[i][1] = 18 * sin(angle) ;
	}
	for (int i = 0; i < 30; i++) {
		double angle;
		angle = i*3.141592 / 15;
		qwerty_eye1[i][0] = 8 * cos(angle) - 13;
		qwerty_eye1[i][1] = 8 * sin(angle) - 1;
		qwerty_eye2[i][0] = 8 * cos(angle) + 13;
		qwerty_eye2[i][1] = 8 * sin(angle) - 1;
	}
	for (int i = 0; i < 15; i++) {
		double angle;
		angle = i*3.141592 / 8;
		qwerty_eye_in1[i][0] = 6 * cos(angle) - 13;
		qwerty_eye_in1[i][1] = 2 * sin(angle) - 1;
		qwerty_eye_in2[i][0] = 6 * cos(angle) + 13;
		qwerty_eye_in2[i][1] = 2 * sin(angle) - 1;
	}
	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_qwerty);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_qwerty);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(qwerty_face), qwerty_face);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(qwerty_face), sizeof(qwerty_eye1), qwerty_eye1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(qwerty_face)+sizeof(qwerty_eye1), sizeof(qwerty_eye_in1), qwerty_eye_in1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(qwerty_face)+sizeof(qwerty_eye1)+ sizeof(qwerty_eye_in1), sizeof(qwerty_eye2), qwerty_eye2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(qwerty_face)+sizeof(qwerty_eye1) + sizeof(qwerty_eye_in1)+ sizeof(qwerty_eye2), sizeof(qwerty_eye_in2), qwerty_eye_in2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(qwerty_face) + sizeof(qwerty_eye1) + sizeof(qwerty_eye_in1) + sizeof(qwerty_eye2)+sizeof(qwerty_eye_in2), sizeof(qwerty_ear1), qwerty_ear1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(qwerty_face) + sizeof(qwerty_eye1) + sizeof(qwerty_eye_in1) + sizeof(qwerty_eye2) + sizeof(qwerty_eye_in2)+ sizeof(qwerty_ear1), sizeof(qwerty_ear2), qwerty_ear2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(qwerty_face) + sizeof(qwerty_eye1) + sizeof(qwerty_eye_in1) + sizeof(qwerty_eye2) + sizeof(qwerty_eye_in2) + sizeof(qwerty_ear1)+ sizeof(qwerty_ear2),
		sizeof(qwerty_nose), qwerty_nose);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(qwerty_face) + sizeof(qwerty_eye1) + sizeof(qwerty_eye_in1) + sizeof(qwerty_eye2) + sizeof(qwerty_eye_in2) + sizeof(qwerty_ear1) + sizeof(qwerty_ear2)+
		sizeof(qwerty_nose),sizeof(qwerty_line1), qwerty_line1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(qwerty_face) + sizeof(qwerty_eye1) + sizeof(qwerty_eye_in1) + sizeof(qwerty_eye2) + sizeof(qwerty_eye_in2) + sizeof(qwerty_ear1) + sizeof(qwerty_ear2)+
		sizeof(qwerty_nose)+ sizeof(qwerty_line1), sizeof(qwerty_line2),qwerty_line2);
	
	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_qwerty);
	glBindVertexArray(VAO_qwerty);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_qwerty);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_qwerty(void) {
	glBindVertexArray(VAO_qwerty);

	glUniform3fv(loc_primitive_color, 1, qwerty_color[QWERTY_FACE]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 50);

	glUniform3fv(loc_primitive_color, 1,qwerty_color[QWERTY_EYE]);
	glDrawArrays(GL_TRIANGLE_FAN, 50, 30);

	glUniform3fv(loc_primitive_color, 1, qwerty_color[QWERTY_EYE_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 80, 15);
	
	glUniform3fv(loc_primitive_color, 1, qwerty_color[QWERTY_EYE]);
	glDrawArrays(GL_TRIANGLE_FAN, 95, 30);

	glUniform3fv(loc_primitive_color, 1, qwerty_color[QWERTY_EYE_IN]);
	glDrawArrays(GL_TRIANGLE_FAN, 125, 15);

	glUniform3fv(loc_primitive_color, 1, qwerty_color[QWERTY_EYE]);
	glDrawArrays(GL_TRIANGLE_FAN, 140, 3);

	glUniform3fv(loc_primitive_color, 1, qwerty_color[QWERTY_EYE]);
	glDrawArrays(GL_TRIANGLE_FAN, 143, 3);

	glUniform3fv(loc_primitive_color, 1, qwerty_color[QWERTY_EYE]);
	glDrawArrays(GL_TRIANGLE_FAN, 146, 3);

	glUniform3fv(loc_primitive_color, 1, qwerty_color[QWERTY_EYE]);
	glDrawArrays(GL_LINES, 149, 2);

	glUniform3fv(loc_primitive_color, 1, qwerty_color[QWERTY_EYE]);
	glDrawArrays(GL_LINES, 151, 2);

/*
	glUniform3fv(loc_primitive_color, 1, qwerty_color[QWERTY_EYE]);
	glDrawArrays(GL_TRIANGLE_FAN, 153, 2);
*/
	glBindVertexArray(0);
}



void display(void) {
	int i,j,k;
	float x, r, s, delx, delr, dels;
	static float car1_posx = 0, car1_posy = 0;

	glm::mat4 ModelMatrix, temp;
	glm::vec3 AirStart;
	glm::vec3 slide;
	glm::vec3 tempvec;
	glClear(GL_COLOR_BUFFER_BIT);

	ModelMatrix = glm::mat4(1.0f);
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes();

	for (i = 0; i < 8; i++) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		switch (i) {
		case 0:AirStart = glm::vec3(200.0f, 0.0f, 0.0f); break;						//
		case 1:AirStart = glm::vec3(141.42f, 141.42f, 0.0f); break;					// rotate
		case 2:AirStart = glm::vec3(0.0f, 200.0f, 0.0f); break;						// 45 degree
		case 3:AirStart = glm::vec3(-141.42f, 141.42f, 0.0f); break;				// 
		case 4:AirStart = glm::vec3(-200.0f, 0.0f, 0.0f); break;					// 
		case 5:AirStart = glm::vec3(-141.42f, -141.42f, 0.0f); break;				//
		case 6:AirStart = glm::vec3(0.0f, -200.0f, 0.0f); break;					//
		case 7:AirStart = glm::vec3(141.42f, -141.42f, 0.0f); break;				//
		}
		
		if (key_state == 'a') {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(air_dragx,air_dragy, 0.0f));
		}
		
		
		if (airplane_clock <= 360) {
			ModelMatrix = glm::translate(ModelMatrix, AirStart);
			temp = { { 1.5*cos(airplane_clock*TO_RADIAN) , sin(airplane_clock*TO_RADIAN),0,0 },
			{ 1.5* -sin(airplane_clock*TO_RADIAN) , cos(airplane_clock*TO_RADIAN),0,0 },
			{ 0,0,1,0 } ,
			{ 0,0,0,1 }
			};
			ModelMatrix *= temp;
			ModelMatrix = glm::translate(ModelMatrix, -AirStart);
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(airplane_clock*0.0072 * 1.0f, airplane_clock*0.0072 * 1.0f, 0.1f));
		}
		else if (airplane_clock <= 720) {
			ModelMatrix = glm::translate(ModelMatrix, -AirStart);
			temp = { { 1.5* cos(airplane_clock*TO_RADIAN) ,sin(airplane_clock*TO_RADIAN ),0,0 },
			{ 1.5*-sin(airplane_clock*TO_RADIAN) , cos(airplane_clock*TO_RADIAN ),0,0 },
			{ 0,0,1,0 } ,
			{ 0,0,0,1 }
			};
			ModelMatrix *= temp;
			ModelMatrix = glm::translate(ModelMatrix, AirStart);
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3((720 - airplane_clock)*0.0072 * 1.0f, (720 - airplane_clock)*0.0072 * 1.0f, 0.1f));
			ModelMatrix = glm::rotate(ModelMatrix, 180 * TO_RADIAN, glm::vec3(0.0f, 0.0f, 0.1f));
		}
		ModelMatrix = glm::rotate(ModelMatrix, 45 * i * TO_RADIAN, glm::vec3(0.0f, 0.0f, 0.1f));

		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		if(key_state=='a' || key_state == 'f')draw_airplane();
	}

	/* rotating car *//////////////////////////////////////////////////////////////////////////////////////	
	for (i = 0; i < 8; i++) {
		if (key_state == 'b') {
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-car_delx, car_dely, 0.0f)); // translate 
		}
		else ModelMatrix = glm::mat4(1.0f);
		ModelMatrix = glm::rotate(ModelMatrix, (rev*car_clock2*2)*TO_RADIAN, glm::vec3(0.0f, 0.0f, 0.1f));
		switch (i) {
		case 0: ModelMatrix = glm::translate(ModelMatrix, glm::vec3(car_clock2*0.5, 0.0f, 0.0f)); break;
		case 1: ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-car_clock2*0.5, 0.0f, 0.0f)); break;
		case 2: ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, car_clock2*0.5, 0.0f)); break;
		case 3: ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -car_clock2*0.5, 0.0f)); break;
		case 4: ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.7071f*car_clock2*0.5, 0.7071f*car_clock2*0.5, 0.0f)); break;
		case 5: ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-0.7071f*car_clock2*0.5, 0.7071f*car_clock2*0.5, 0.0f)); break;
		case 6: ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-0.7071f*car_clock2*0.5, -0.7071f*car_clock2*0.5, 0.0f)); break;
		case 7: ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.7071f*car_clock2*0.5, -0.7071f*car_clock2*0.5, 0.0f)); break;
		}
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));

		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		if (key_state == 'b' || key_state == 'f') draw_car2();
	}

	/* sine wave car *//////////////////////////////////////////////////////////////////
	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(400 , -200.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-car_clock*0.6, -90*cos(car_clock*TO_RADIAN), 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, -sin((car_clock%360)*TO_RADIAN+0.0f), glm::vec3(0.0f, 0.0f, 0.1f)); 
	
	car1_posx = 400 - car_clock*0.6;
	car1_posy = -200 - 40 * cos(car_clock*TO_RADIAN);
	if (((win_width/2- car1_posx)*(win_width/2 - car1_posx) < 200) || ((-win_width / 2 - car1_posx)*(-win_width/2 - car1_posx)<200))
		refx *=-1;
	
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;	
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	if(key_state == 'c' || key_state == 'f') draw_car();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(400, -200.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-car_clock*0.6, 90 * cos(car_clock*TO_RADIAN), 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, -sin((car_clock % 360+180)*TO_RADIAN + 0.0f), glm::vec3(0.0f, 0.0f, 0.1f));
	
	car1_posx = 400 - car_clock*0.6;
	car1_posy = -200 - 40 * cos(car_clock*TO_RADIAN);

	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	if (key_state == 'c' || key_state == 'f') draw_car();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-400, -200.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(car_clock*0.6,-	90 * cos(car_clock*TO_RADIAN), 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, sin((car_clock % 360)*TO_RADIAN + 0.0f), glm::vec3(0.0f, 0.0f, 0.1f));
	
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	if (key_state == 'c' || key_state == 'f') draw_car();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(-400, -200.0f, 0.0f));
	ModelMatrix = glm::translate(ModelMatrix, glm::vec3(car_clock*0.6, 90 * cos(car_clock*TO_RADIAN), 0.0f));
	ModelMatrix = glm::rotate(ModelMatrix, sin((car_clock % 360+180)*TO_RADIAN + 0.0f), glm::vec3(0.0f, 0.0f, 0.1f));
	
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	if (key_state == 'c' || key_state == 'f') draw_car();
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	cock_speed[0] = 1; cock_speed[1] = 1.2; cock_speed[2] = 1.4; cock_speed[3] = 1.6;
	cock_speed[4] = 1.8; cock_speed[5] = 2; cock_speed[6] = 2.2; cock_speed[7] = 2.5;
	for (i = 0; i <8; i++) {
		ModelMatrix = glm::mat4(1.0f);
 		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(cock_posx[i] + cock_dir[i].x*cock_speed[i], cock_posy[i] + cock_dir[i].y*cock_speed[i], 0));
	
		if ((m_posx - win_width / 2 - cock_posx[i])*(m_posx - win_width / 2 - cock_posx[i]) +
			(win_height / 2 - m_posy - cock_posy[i])*(win_height / 2 - m_posy - cock_posy[i]) <100) {
			cock_posx[i] = cock_posx[i]; cock_posy[i] = cock_posy[i];
		}
		else {
			cock_posx[i] = cock_posx[i] + cock_dir[i].x*cock_speed[i];
			cock_posy[i] = cock_posy[i] + cock_dir[i].y*cock_speed[i];
		}
		if (!(cock_dir[i].x || cock_dir[i].y)) temp = glm::mat4(1.0f);
		else 
		temp = { { cock_dir[i].x , cock_dir[i].y,0,0 },
		{-cock_dir[i].y , cock_dir[i].x,0,0 },
		{ 0,0,1,0 } ,
		{ 0,0,0,1 }
		};
		ModelMatrix *= temp;
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		if (key_state == 'd' || key_state == 'f') draw_cocktail();
	}
	/* 충돌체 */
	float tempx;

	if (!init) {	//초기화
		colis_speed[0] = 0.7; colis_speed[1] = 0.8; colis_speed[2] = 0.9; colis_speed[3] =1;
		for (i = 0; i < 4; i++) {
			colis_dir[i] = { rand() % 11 - 5,rand() % 11 - 5,0 };
			glm::normalize(colis_dir[i]);
			/*
			tempx = colis_dir[i].x;
			colis_dir[i].x /= sqrt(colis_dir[i].x*colis_dir[i].x + colis_dir[i].y*colis_dir[i].y);  
			colis_dir[i].x *= colis_speed[i];
			colis_dir[i].y /= sqrt(tempx*tempx + colis_dir[i].y*colis_dir[i].y);
			colis_dir[i].y *= colis_speed[i];
			*/
		}
		colis_posx[0] = 100; colis_posy[0] = 100; colis_posz[0] = 0;
		colis_posx[1] = 100; colis_posy[1] = -100; colis_posz[1] = 0;
		colis_posx[2] = -100; colis_posy[2] = 100; colis_posz[2] = 0;
		colis_posx[3] = -100; colis_posy[3] = -100; colis_posz[3] = 0;
		init = 1;	
	}
	
	int colis_flag = 0;
	glm::vec3 n_colis,v_colis,d_colis,sep,acc;
	float dist;
	float d;
	float vn;
	float jj;
	
	/* window 충돌 */
	for (i = 0; i < 4; i++) {
		ModelMatrix = glm::mat4(1.0f);
		for (j = i + 1; j < 4; j++) {
			if ((colis_posx[i] - colis_posx[j])*(colis_posx[i] - colis_posx[j]) +
				(colis_posy[i] - colis_posy[j])*(colis_posy[i] - colis_posy[j]) < 6400) {
				slide.x = colis_posx[j] - colis_posx[i]; slide.y = colis_posy[j] - colis_posy[i]; slide.z = 0;
				glm::normalize(slide);
				/*
				
				tempx = slide.x;
				slide.x /= (slide.x*slide.x + slide.y*slide.y);
				slide.y /= (tempx*tempx + slide.y*slide.y);
				*/
				d = glm::dot(slide, slide);
				dist = sqrt(d);

				n_colis.x = slide.x / dist;	n_colis.y = slide.y / dist; n_colis.z = slide.z / dist;
				d_colis.x = 1 - d;			d_colis.y = 1 - d;	d_colis.z = 1 - d;

				sep.x = n_colis.x*(d_colis.x);
				sep.y = n_colis.y*(d_colis.y);
				sep.z = n_colis.z*(d_colis.z);

				colis_posx[i] -= sep.x;
				colis_posy[i] -= sep.y;
				colis_posz[i] -= sep.z;

				colis_posx[j] += sep.x;
				colis_posy[j] += sep.y;
				colis_posz[j] += sep.z;

				v_colis.x = (colis_dir[j].x - colis_dir[i].x);
				v_colis.y = (colis_dir[j].y - colis_dir[i].y);
				v_colis.z = (colis_dir[j].z - colis_dir[i].z);

				vn = glm::dot(n_colis, v_colis);
				jj = -(2)*(vn) / 2;
				acc.x = jj*n_colis.x;
				acc.y = jj*n_colis.y;
				acc.z = jj*n_colis.z;

				/* collision response */
				colis_dir[i].x -= acc.x;
				colis_dir[i].y -= acc.y;
				colis_dir[i].z -= acc.z;

				colis_dir[j].x += acc.x;
				colis_dir[j].y += acc.y;
				colis_dir[j].z += acc.z;
				/* 정규화 */

				glm::normalize(colis_dir[i]);
				glm::normalize(colis_dir[j]);
			}
		}

		colis_posx[i] += colis_dir[i].x * colis_speed[i]; 
		colis_posy[i] += colis_dir[i].y * colis_speed[i];
		colis_posz[i] += colis_dir[i].z * colis_speed[i];

		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(colis_posx[i], colis_posy[i], colis_posz[i]));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2, 2, 0));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		if (key_state == 'e' || key_state == 'f') {/* draw_qwerty();*/ draw_cir(); }
	}
	for (i = 0; i < 4; i++) {
		ModelMatrix = glm::mat4(1.0f);

		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(colis_posx[i], colis_posy[i], 0));
		ModelMatrix = glm::rotate(ModelMatrix, (rev*car_clock2+i)*TO_RADIAN, glm::vec3(0.0f, 0.0f, 0.1f));
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(40, 40, 0));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		if (key_state == 'e' || key_state == 'f') { draw_qwerty(); }
	}
	glFlush();
}

void mousepress(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON)) {
		if (state == GLUT_DOWN) {
			m_prevx = x; m_prevy = y;
		}
		else if (state == GLUT_UP) {
		}
	}
	printf("%d %d\n", x,y);
}

void motion(int x, int y) { 
	int i,sum;

	if (key_state == 'a') { 
		air_dragx = (-(float)(m_prevx - x));
		air_dragy= ((float)(m_prevy - y));
	}
	else if (key_state == 'b') { car_delx = (float)(m_prevx - x) * 1; car_dely = (float)(m_prevy - y) * 1; }
	else if (key_state == 'd') {
		for (i = 0; i < 8; i++) {
			cock_dir[i] = { (x - win_width / 2) - cock_posx[i], (win_height/2 - y) - cock_posy[i],0 };
			if (cock_dir[i].x == 0 && cock_dir[i].y == 0) {
				cock_dir[i].x = 1; cock_dir[i].y = 1;
			}
			/* 정규화 */
			cock_dir[i] = { cock_dir[i].x / sqrt(cock_dir[i].x* cock_dir[i].x + cock_dir[i].y * cock_dir[i].y),
				cock_dir[i].y / sqrt(cock_dir[i].x* cock_dir[i].x + cock_dir[i].y * cock_dir[i].y), 0.0f };
		}
		m_posx = x; m_posy = y;
	}
}

void timer(int value) {
	int i;
	airplane_clock = (airplane_clock + 1) % 720;

	if (refx == -1)
	{
		car_clock += -3;
	}
	else if (refx == 1)
	{
		car_clock += 3;
	}

	car_clock2 = (car_clock2 + rev);
	if (car_clock2 % 360 == 0)
		rev *= -1;
	cock_clock = 1;
	glutPostRedisplay();
	glutTimerFunc(10, timer, 0);
	
	for (i = 0; i < 4; i++) {
		if (win_width / 2 - colis_posx[i] < 40) colis_dir[i].x *= -1;
		else if (colis_posx[i] < -win_width / 2 + 40) colis_dir[i].x *= -1;
		if (win_height / 2 - colis_posy[i] < 40) colis_dir[i].y *= -1;
		else if (colis_posy[i] < -win_height / 2 + 40) colis_dir[i].y *= -1;
	}
}


void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 'a': key_state = 'a'; break;
	case 'b': key_state = 'b'; break;
	case 'c': key_state = 'c'; break;
	case 'd': key_state = 'd'; break;
	case 'e': key_state = 'e'; break;
	case 'f': key_state = 'f'; break;
	}
}

void reshape(int width, int height) {
	win_width = width, win_height = height;

	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0,
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	update_axes();
	update_line();
	update_cir();

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_line);
	glDeleteBuffers(1, &VBO_line);

	glDeleteVertexArrays(1, &VAO_airplane);
	glDeleteBuffers(1, &VBO_airplane);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mousepress);
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

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(250 / 255.0f, 128 / 255.0f, 114 / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_line();
	prepare_airplane();
	prepare_shirt();
	prepare_house();
	prepare_car();
	prepare_cocktail();
	prepare_car2();
	prepare_cir();
	prepare_qwerty();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
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
	char program_name[64] = "Sogang CSE4170 2DObjects_GLSL_3.1";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC' "
	};
	srand(time(0));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 1200);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}


