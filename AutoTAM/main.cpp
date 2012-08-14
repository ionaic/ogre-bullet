#include <GL/glew.h>
#include <GL/glfw.h>
#define ILUT_USE_OPENGL
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
using std::cout; using std::endl;

// global variables to store the scene geometry
GLuint vertexArrayID;
GLuint bufferIDs[3];
// GL texture ID
GLuint TexID = 0;
// shader program
GLuint p, f, v;

// initialize the scene geometry
void initGeometry() {
	static const GLfloat rawVertexData[18] = { -1.0f,-1.0f,0.0f,  1.0f,1.0f,0.0f,  1.0f,-1.0f,0.0f,  -1.0f,1.0f,0.0f, -1.0f,-1.0f,0.0f,  1.0f,1.0f,0.0f  };
	static const GLfloat rawTexCoordData[18] = {0.0f,0.0f,0.0f,  1.0f,1.0f,0.0f,  1.0f,0.0f,0.0f , 0.0f,1.0f,0.0f, 0.0f,0.0f,0.0f,  1.0f,1.0f,0.0f};
	// create a new renderable object and set it to be active
	glGenVertexArrays(1,&vertexArrayID);
	glBindVertexArray(vertexArrayID);
	// create buffers for associated data
	glGenBuffers(3,bufferIDs);
	
	// set a buffer to be active and shove vertex data into it
	glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[0]);
	glBufferData(GL_ARRAY_BUFFER, 18*sizeof(GLfloat), rawVertexData, GL_STATIC_DRAW);
	// bind that data to shader variable 0
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	// set a buffer to be active and shove color data into it
	glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[2]);
	glBufferData(GL_ARRAY_BUFFER, 18*sizeof(GLfloat), rawTexCoordData, GL_STATIC_DRAW);
	// bind that data to shader variable 1
	glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);
}

// loadFile - loads text file into char* fname
// allocates memory - so need to delete after use
// size of file returned in fSize
char* loadFile(const char *fname, GLint &fSize) {
	std::ifstream::pos_type size;
	char * memblock;

	// file read based on example in cplusplus.com tutorial
	std::ifstream file (fname, std::ios::in|std::ios::binary|std::ios::ate);
	if (file.is_open()) {
		size = file.tellg();
		fSize = (GLuint) size;
		memblock = new char [size];
		file.seekg (0, std::ios::beg);
		file.read (memblock, size);
		file.close();
	} else {
		std::cout << "Unable to open " << fname << std::endl;
		exit(1);
	}
	cout << fname << " loaded" << endl;
	return memblock;
}

GLuint compileShader(const char *fname, GLuint type) {
	GLuint shader;
	GLint length;
	// create a shader ID
	shader = glCreateShader(type);
	// load the file into memory and try to compile it
	char *source = loadFile(fname,length);
	glShaderSource(shader, 1, (const GLchar**)&source,&length);
	GLint compiled;
	glCompileShader(shader);
	// print out errors if they're there
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		std::cout << fname << " failed to compile" << std::endl;
		// find the error length
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		if (length>0) {
			// print the errors
			char *error = new GLchar[length];
			GLint written;
			glGetShaderInfoLog(shader,length, &written, error);
			std::cout << "Error log:\n" << error << std::endl;
			delete[] error;
		}
	}
	delete[] source;
	return shader;
}

void initShaders() {
	// get the shader code
	v = compileShader("minimal.vert",GL_VERTEX_SHADER);
	f = compileShader("minimal.frag",GL_FRAGMENT_SHADER);	
	// the GLSL program links shaders together to form the render pipeline
	p = glCreateProgram();
	// assign numerical IDs to the variables that we pass to the shaders 
	glBindAttribLocation(p,0, "in_Position");
	glBindAttribLocation(p,1, "in_Color");
	glBindAttribLocation(p,2, "in_ColorTexCoords");
	// bind our shaders to this program
	glAttachShader(p,v);
	glAttachShader(p,f);
	// link things together and activate the shader
	glLinkProgram(p);
	glUseProgram(p);
	glUniform1i(glGetUniformLocation(p,"ColorTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,TexID);
}

// Callback for when the window is resized
void GLFWCALL windowResize( int width, int height ) {
	glViewport(0,0,(GLsizei)width,(GLsizei)height);
}
void HandleDevILErrors () {
	ILenum error = ilGetError ();
	if (error != IL_NO_ERROR) {
		do {
			printf ("\n\n%s\n", iluErrorString (error));	
		} while ((error = ilGetError ()));
		exit (1);
	}
}
int main( void ) {
	// init DevIL
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION ||
		iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION ||
		ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION) {
		cout << "DevIL library is out of date! Please upgrade"<<endl;
		return 2;
	}
	// Needed to initialize DevIL.
	ilInit ();
	iluInit();
	// GL cannot use palettes anyway, so convert early.
	ilEnable (IL_CONV_PAL);
	// Gets rid of dithering on some nVidia-based cards.
	ilutEnable (ILUT_OPENGL_CONV);
	
	// Initialize GLFW
	if( !glfwInit() ) {
		exit( EXIT_FAILURE );
	}
	// Open an OpenGL window
	if( !glfwOpenWindow( 300,300, 0,0,0,0,0,0, GLFW_WINDOW ) ) {
		glfwTerminate();
		exit( EXIT_FAILURE );
	}
	
	glfwSetWindowTitle("Triangle");
	glfwSetWindowSizeCallback( windowResize );
	glfwSwapInterval( 1 );
	windowResize(300,300);
	glewInit();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	// load a texture
	{
		// IL image ID
		ILuint ImgId = 0;
		
		// Generate the main image name to use.
		ilGenImages (1, &ImgId);
		
		// Bind this image name.
		ilBindImage (ImgId);

		// Loads the image specified by File into the ImgId image.
		if (!ilLoadImage ("cloth.png")) {
			HandleDevILErrors ();
		}
		glEnable(GL_TEXTURE_2D);
		// Lets ILUT know to use its OpenGL functions.
		ilutRenderer (ILUT_OPENGL);
		// Goes through all steps of sending the image to OpenGL.
		TexID = ilutGLBindTexImage();
		// We're done with our image, so we go ahead and delete it.
		ilDeleteImages(1, &ImgId);
	}
	ilShutDown();
	// last of the init
	initGeometry();
	initShaders();
	GLint texUnits;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS,&texUnits);
	cout << "# of tex units: " << texUnits << endl;
	// Main loop
	int running = GL_TRUE;
	while( running ) {
		// OpenGL rendering goes here...
		glClear( GL_COLOR_BUFFER_BIT );
		// draw the triangle
		glBindVertexArray(vertexArrayID);
		glDrawArrays(GL_TRIANGLES,0,6);
		// Swap front and back rendering buffers
		glfwSwapBuffers();
		// Check if ESC key was pressed or window was closed
		running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );
	}
	// delete the texture
	glDeleteTextures(1, &TexID);
	// delete the buffers
	glDeleteVertexArrays(1,&vertexArrayID);
	glDeleteBuffers(3,bufferIDs);
	// delete the shader
	glDetachShader(p,f);
	glDeleteShader(f);
	glDetachShader(p,v);
	glDeleteShader(v);
	glDeleteProgram(p);
	// Close window and terminate GLFW
	glfwTerminate();
	// Exit program
	exit( EXIT_SUCCESS );
}
