#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))
#define INDEX_VERTEX_POSITION 0
#define LOC_VERTEX 0
#define LOC_NORMAL 1
#define LOC_TEXCOORD 2
#define PI 3.1415926

GLuint axes_VBO, axes_VAO;
GLfloat axes_vertices[6][3] = {
	{ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }
};
GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };

void prepare_axes(void) { // Draw coordinate axes.
	// Initialize vertex buffer object.
	glGenBuffers(1, &axes_VBO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes_vertices), &axes_vertices[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &axes_VAO);
	glBindVertexArray(axes_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, axes_VBO);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_axes(void) {
	glBindVertexArray(axes_VAO);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
}

void free_axes(void) {
	glDeleteVertexArrays(1, &axes_VAO);
	glDeleteBuffers(1, &axes_VBO);
}

GLuint path_VBO, path_VAO;
GLfloat *path_vertices;
int path_n_vertices;

int read_path_file(GLfloat **object, char *filename) {
	int i, n_vertices;
	float *flt_ptr;
	FILE *fp;

	fprintf(stdout, "Reading path from the path file %s...\n", filename);
	fp = fopen(filename, "r");
	if (fp == NULL){
		fprintf(stderr, "Cannot open the path file %s ...", filename);
		return -1;
	}

	fscanf(fp, "%d", &n_vertices);
	*object = (float *)malloc(n_vertices * 3 * sizeof(float));
	if (*object == NULL){
		fprintf(stderr, "Cannot allocate memory for the path file %s ...", filename);
		return -1;
	}

	flt_ptr = *object;
	for (i = 0; i < n_vertices; i++) {
		fscanf(fp, "%f %f %f", flt_ptr, flt_ptr + 1, flt_ptr + 2);
		flt_ptr += 3;
	}
	fclose(fp);

	fprintf(stdout, "Read %d vertices successfully.\n\n", n_vertices);

	return n_vertices;
}

void prepare_path(void) { // Draw path.
	//	return;
	path_n_vertices = read_path_file(&path_vertices, "Data/path.txt");
	printf("%d %f\n", path_n_vertices, path_vertices[(path_n_vertices - 1)]);
	// Initialize vertex array object.
	glGenVertexArrays(1, &path_VAO);
	glBindVertexArray(path_VAO);
	glGenBuffers(1, &path_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, path_VBO);
	glBufferData(GL_ARRAY_BUFFER, path_n_vertices * 3 * sizeof(float), path_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_path(void) {
	glBindVertexArray(path_VAO);
	glUniform3f(loc_primitive_color, 1.000f, 1.000f, 0.000f); // color name: Yellow
	glDrawArrays(GL_LINE_STRIP, 0, path_n_vertices);
}

void free_path(void) {
	glDeleteVertexArrays(1, &path_VAO);
	glDeleteBuffers(1, &path_VBO);
}

int read_triangular_mesh(GLfloat **object, int bytes_per_primitive, char *filename) {
	int n_triangles;
	FILE *fp;

	fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the object file %s ...", filename);
		return -1;
	}
	fread(&n_triangles, sizeof(int), 1, fp);
	*object = (float *)malloc(n_triangles*bytes_per_primitive);
	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...", filename);
		return -1;
	}

	fread(*object, bytes_per_primitive, n_triangles, fp);
	fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}

#define N_OBJECTS 6
#define OBJECT_SQUARE16 0
#define OBJECT_TIGER 1
#define OBJECT_COW 2
#define OBJECT_AIRPLANE 3
#define OBJECT_DRAGON 4
#define OBJECT_OPTIMUS 5

GLuint object_VBO[N_OBJECTS], object_VAO[N_OBJECTS];
int object_n_triangles[N_OBJECTS];
GLfloat *object_vertices[N_OBJECTS];

void set_up_object(int object_ID, char *filename, int n_bytes_per_vertex) {
	object_n_triangles[object_ID] = read_triangular_mesh(&object_vertices[object_ID],
		3 * n_bytes_per_vertex, filename);
	// Error checking is needed here.

	// Initialize vertex buffer object.
	glGenBuffers(1, &object_VBO[object_ID]);

	glBindBuffer(GL_ARRAY_BUFFER, object_VBO[object_ID]);
	glBufferData(GL_ARRAY_BUFFER, object_n_triangles[object_ID] * 3 * n_bytes_per_vertex,
		object_vertices[object_ID], GL_STATIC_DRAW);

	// As the geometry data exists now in graphics memory, ...
	free(object_vertices[object_ID]);

	// Initialize vertex array object.
	glGenVertexArrays(1, &object_VAO[object_ID]);
	glBindVertexArray(object_VAO[object_ID]);

	glBindBuffer(GL_ARRAY_BUFFER, object_VBO[object_ID]);
	glVertexAttribPointer(INDEX_VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(INDEX_VERTEX_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void prepare_square(void) {
	// 8 = 3 for vertex, 3 for normal, and 2 for texcoord
	set_up_object(OBJECT_SQUARE16, "Data/Square16_triangles_vnt.geom", 8 * sizeof(float));
}

void prepare_tiger(void) {
	// 8 = 3 for vertex, 3 for normal, and 2 for texcoord
	set_up_object(OBJECT_TIGER, "Data/Tiger_00_triangles_vnt.geom", 8 * sizeof(float));
}

void prepare_airplane(void) {
	set_up_object(OBJECT_AIRPLANE, "Data/airplane.geom", 8 * sizeof(float));
}

void prepare_dragon(void) {
	set_up_object(OBJECT_DRAGON, "Data/dragon.geom", 8 * sizeof(float));
}

void prepare_optimus(void) {
	set_up_object(OBJECT_OPTIMUS, "Data/optimus.geom", 8 * sizeof(float));
}

void draw_object(int object_ID, float r, float g, float b) {
	glUniform3f(loc_primitive_color, r, g, b);
	glBindVertexArray(object_VAO[object_ID]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * object_n_triangles[object_ID]);
	glBindVertexArray(0);
}

#define N_GEOMETRY_OBJECTS 6
#define GEOM_OBJ_ID_CAR_BODY 0
#define GEOM_OBJ_ID_CAR_WHEEL 1
#define GEOM_OBJ_ID_CAR_NUT 2
#define GEOM_OBJ_ID_COW 3
#define GEOM_OBJ_ID_TEAPOT 4
#define GEOM_OBJ_ID_BOX 5

GLuint geom_obj_VBO[N_GEOMETRY_OBJECTS];
GLuint geom_obj_VAO[N_GEOMETRY_OBJECTS];

int geom_obj_n_triangles[N_GEOMETRY_OBJECTS];
GLfloat *geom_obj_vertices[N_GEOMETRY_OBJECTS];

// codes for the 'general' triangular-mesh object
typedef enum _GEOM_OBJ_TYPE { GEOM_OBJ_TYPE_V = 0, GEOM_OBJ_TYPE_VN, GEOM_OBJ_TYPE_VNT } GEOM_OBJ_TYPE;
// GEOM_OBJ_TYPE_V: (x, y, z)
// GEOM_OBJ_TYPE_VN: (x, y, z, nx, ny, nz)
// GEOM_OBJ_TYPE_VNT: (x, y, z, nx, ny, nz, s, t)
int GEOM_OBJ_ELEMENTS_PER_VERTEX[3] = { 3, 6, 8 };

int read_geometry_file(GLfloat **object, char *filename, GEOM_OBJ_TYPE geom_obj_type) {
	int i, n_triangles;
	float *flt_ptr;
	FILE *fp;

	fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "r");
	if (fp == NULL){
		fprintf(stderr, "Cannot open the geometry file %s ...", filename);
		return -1;
	}

	fscanf(fp, "%d", &n_triangles);
	*object = (float *)malloc(3 * n_triangles*GEOM_OBJ_ELEMENTS_PER_VERTEX[geom_obj_type] * sizeof(float));
	if (*object == NULL){
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...", filename);
		return -1;
	}

	flt_ptr = *object;
	for (i = 0; i < 3 * n_triangles * GEOM_OBJ_ELEMENTS_PER_VERTEX[geom_obj_type]; i++)
		fscanf(fp, "%f", flt_ptr++);
	fclose(fp);

	fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);

	return n_triangles;
}

void prepare_geom_obj(int geom_obj_ID, char *filename, GEOM_OBJ_TYPE geom_obj_type) {
	int n_bytes_per_vertex;

	n_bytes_per_vertex = GEOM_OBJ_ELEMENTS_PER_VERTEX[geom_obj_type] * sizeof(float);
	geom_obj_n_triangles[geom_obj_ID] = read_geometry_file(&geom_obj_vertices[geom_obj_ID], filename, geom_obj_type);

	// Initialize vertex array object.
	glGenVertexArrays(1, &geom_obj_VAO[geom_obj_ID]);
	glBindVertexArray(geom_obj_VAO[geom_obj_ID]);
	glGenBuffers(1, &geom_obj_VBO[geom_obj_ID]);
	glBindBuffer(GL_ARRAY_BUFFER, geom_obj_VBO[geom_obj_ID]);
	glBufferData(GL_ARRAY_BUFFER, 3 * geom_obj_n_triangles[geom_obj_ID] * n_bytes_per_vertex,
		geom_obj_vertices[geom_obj_ID], GL_STATIC_DRAW);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	if (geom_obj_type >= GEOM_OBJ_TYPE_VN) {
		glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
	if (geom_obj_type >= GEOM_OBJ_TYPE_VNT) {
		glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	free(geom_obj_vertices[geom_obj_ID]);
}

void draw_geom_obj(int geom_obj_ID) {
	glBindVertexArray(geom_obj_VAO[geom_obj_ID]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * geom_obj_n_triangles[geom_obj_ID]);
	glBindVertexArray(0);
}

void free_geom_obj(int geom_obj_ID) {
	glDeleteVertexArrays(1, &geom_obj_VAO[geom_obj_ID]);
	glDeleteBuffers(1, &geom_obj_VBO[geom_obj_ID]);
}

GLfloat cir[40][21][3];
GLfloat cir_center[1][3];
GLfloat cir_color[2][3] = { { 0.0f, 0.0f, 0.0f },{ 255 / 255.0f,   0 / 255.0f,   0 / 255.0f } };
GLuint VBO_cir, VAO_cir;

void prepare_cir(void) { // Draw circle.
	GLsizeiptr buffer_size = sizeof(cir) + sizeof(cir_center);

	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory
	
	for (int i = 0; i < 21; i++) {
		for (int j = 0; j < 40; j++) {
			double theta;
			double phi;
			theta = j * 2 * PI / 40;
			phi = i * PI / 21 - (PI / 2);
			cir[j][i][0] = 4 * cos(theta)*cos(phi);
			cir[j][i][1] = 4 * sin(theta)*cos(phi);
			cir[j][i][2] = 4 * sin(phi);
		}
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
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
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
	
	for (int i = 0; i < 840; i += 21){
		glDrawArrays(GL_LINES, i, 21);
		glDrawArrays(GL_LINES, i+1, 21);
	}
	glBindVertexArray(0);
}

GLfloat WC_top[4][3] = { { 0,0,0 },{ 30,0,0 },{ 30,30,0 },{ 0,30,0 } };
GLfloat WC_bottom[4][3] = { { 0,0,30 },{ 30,0,30 },{ 30,30,30 },{ 0,30,30 } };
GLfloat WC_1[4][3] = { { 0,0,0 },{ 0,0,30 },{ 0,30,30 },{ 0,30,0 } };
GLfloat WC_2[4][3] = { { 0,0,0 },{ 0,0,30 },{ 30,0,30 },{ 30,0,0 } };
GLfloat WC_3[4][3] = { { 30,0,0 },{ 30,0,30 },{ 30,30,30 },{ 30,30,0 } };
GLfloat WC_4[4][3] = { { 0,30,30 },{ 30,30,30 },{ 30,30,0 },{ 0,30,0 } };

GLuint VBO_WC, VAO_WC;

void prepare_WC(void) { // Draw WC.
	GLsizeiptr buffer_size = sizeof(WC_top)+ sizeof(WC_bottom)+ sizeof(WC_1)+ sizeof(WC_2)+ sizeof(WC_3)+ sizeof(WC_4);

	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

																	  // Initialize vertex buffer object.
	glGenBuffers(1, &VBO_WC);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_WC);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(WC_top), WC_top);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(WC_top), sizeof(WC_bottom), WC_bottom);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(WC_top)+ sizeof(WC_bottom), sizeof(WC_1), WC_1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(WC_top) + sizeof(WC_bottom)+ sizeof(WC_1), sizeof(WC_2), WC_2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(WC_top) + sizeof(WC_bottom)+ sizeof(WC_1)+ sizeof(WC_2), sizeof(WC_3), WC_3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(WC_top) + sizeof(WC_bottom) + sizeof(WC_1) + sizeof(WC_2)+ sizeof(WC_3), sizeof(WC_4), WC_4);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_WC);
	glBindVertexArray(VAO_WC);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_WC);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(LOC_VERTEX);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
//
//void update_WC(void) {
//	glBindBuffer(GL_ARRAY_BUFFER, VBO_WC);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(WC), WC, GL_STATIC_DRAW);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//}

void draw_WC(void) {
	glBindVertexArray(VAO_WC);
	glUniform3fv(loc_primitive_color, 1, cir_color[0]);
	
	glDrawArrays(GL_POLYGON, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindVertexArray(0);
}
