// Vertex Shader - file "minimal.vert"

#version 330 core

in vec3 in_Position;
in vec3 in_Color;
in vec3 in_ColorTexCoords;
out vec3 ex_Color;
out vec3 ColorTexCoords;

void main(void) {
	ex_Color = in_Color;
	ColorTexCoords = in_ColorTexCoords;
	gl_Position = vec4(in_Position, 1.0);
}
