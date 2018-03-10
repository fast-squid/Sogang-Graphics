uniform mat4 ModelViewProjectionMatrix;
uniform vec3 primitive_color;

attribute vec4 v_position;

varying vec4 color;

void main(void) {
 	color = vec4(primitive_color, 1.0);
    gl_Position =  ModelViewProjectionMatrix * v_position;
}
