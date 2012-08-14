// Fragment Shader - file "minimal.frag"

#version 330 core

precision highp float; // needed only for version 1.30

uniform sampler2D ColorTex;
in vec3 ex_Color;
in vec3 ColorTexCoords;
out vec4 out_Color;

void main(void) {
	out_Color = texture2D(ColorTex,ColorTexCoords.xy);//vec4(ex_Color,1.0);//
	
	// Try replacing the above with the following:
	//vec3 tmp_Color;
	//tmp_Color = ex_Color.rrr;	
	//out_Color = vec4(tmp_Color,1.0);
}
