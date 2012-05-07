/*	Final Project by Peter Barnett

	Angle the mirror with 'Q' and 'W'
	Stop and Start the rotation of the objects with 'R'
*/

#include <GL/glew.h>
#include <GL/freeglut.h>

//glm library
#include <glm/glm.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "textfile.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include <iostream>

using namespace glm;



/* vertex array data consisting of normal and position coordinates for a unit cube centered at the origin */
const GLfloat varray[] = {
	//SIDE 1  front
	0.0f, 0.0f, 1.0f, /* normal */
	-.5f, -0.5f, 0.5f,1.0f,      

	0.0f, 0.0f, 1.0f, /* normal */
	-0.5f, 0.5f,0.5f,1.0f,         

	0.0f, 0.0f, 1.0f, /* normal */
	0.5f, 0.5f, 0.5f,1.0f,        

	0.0f, 0.0f, 1.0f, /* normal */
	0.5f, -.5f, 0.5f,1.0f ,       

	//SIDE 2 back
	0.0f, 0.0f, -1.0f, /* normal */
	0.5f, -.5f, -0.5f,1.0f ,       

	0.0f, 0.0f, -1.0f, /* normal */
	0.5f, 0.5f, -0.5f,1.0f, 

	0.0f, 0.0f, -1.0f, /* normal */
	-0.50f, 0.5f,-0.5f,1.0f,        

	0.0f, 0.0f, -1.0f, /* normal */
	-0.50f, -0.50f, -0.5f,1.0f ,    

	//SIDE 3 left
	1.0f, 0.0f, .0f, /* normal */
	0.5f, 0.5f,0.5f,1.0f,        


	1.0f, 0.0f, .0f, /* normal */
	0.5f, -0.5f, 0.5f,1.0f,        

	1.0f, 0.0f, .0f, /* normal */
	0.50f, -0.50f, -0.5f,1.0f ,     

	1.0f, 0.0f, .0f, /* normal */
	0.50f, 0.50f, -0.5f,1.0f ,     

	//SIDE 4 right
	-1.0f, 0.0f, .0f, /* normal */
	-0.5f, 0.5f,0.5f,1.0f,        


	-1.0f, 0.0f, .0f, /* normal */
	-0.5f, -0.5f, 0.5f,1.0f,         

	- 1.0f, 0.0f, .0f, /* normal */
	-0.50f, -0.50f, -0.5f,1.0f ,    

	-1.0f, 0.0f, .0f, /* normal */
	-0.50f, 0.50f, -0.5f,1.0f ,     

	//SIDE 5 bottom
	0.0f, -1.0f, .0f, /* normal */
	0.5f, -0.5f,0.5f,1.0f,         

	.0f, -1.0f, .0f, /* normal */
	-0.5f, -0.5f, 0.5f,1.0f,        

	.0f, -1.0f, .0f, /* normal */
	-0.50f, -0.50f, -0.5f,1.0f ,    

	.0f, -1.0f, .0f, /* normal */
	0.50f, -0.50f, -0.5f,1.0f,     

	//SIDE 6 top
	0.0f, 1.0f, .0f, /* normal */
	0.5f, 0.5f,0.5f,1.0f,        

	.0f, 1.0f, .0f, /* normal */
	-0.5f, 0.5f, 0.5f,1.0f,         

	.0f, 1.0f, .0f, /* normal */
	-0.50f, 0.50f, -0.5f,1.0f ,    

	.0f, 1.0f, .0f, /* normal */
	0.50f, 0.50f, -0.5f,1.0f     

};

GLuint projectionMatrixIndex;
GLuint modelViewMatrixIndex;
GLuint normalIndex;
GLuint vertexIndex;
GLuint lightPositionIndex;

glm::mat4 projectionMatrix;
glm::mat4 modelviewMatrix;
glm::mat4 viewMatrix;
glm::vec4 lightPosition = vec4(0.0f,0.0f,5.0f, 1.0f);
float camX = 0.0f;
float camY = 0.0f;
float camZ = -5.0f;

enum {
	numNormalComponents = 3, // there are 3 floats to define a normal
	numVertexComponents = 4, // there are 2 floats to define the position of a vertex
	stride = sizeof(GLfloat) * (numNormalComponents + numVertexComponents), // the space between all the information for a vertex
	numElements = sizeof(varray) / stride // number of vertices in the array
};

GLuint vertexBufferName;

bool spinning = true;
float my_angle = 0.0;
float user_angle = 0.0;

void checkError(const char *functionName)
{
	GLenum error;
	while (( error = glGetError() ) != GL_NO_ERROR) {
		fprintf (stderr, "GL error 0x%X detected in %s\n", error, functionName);
	}
}

void initBuffer(void)
{
	glGenBuffers (1, &vertexBufferName);
	glBindBuffer (GL_ARRAY_BUFFER, vertexBufferName);
	glBufferData (GL_ARRAY_BUFFER, sizeof(varray), varray, GL_STATIC_DRAW);
	checkError ("initBuffer");
}

void compileAndCheck(GLuint shader)
{
	GLint status;
	glCompileShader (shader);
	glGetShaderiv (shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		char *infoLog;
		glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		infoLog = (char*) malloc (infoLogLength);
		glGetShaderInfoLog (shader, infoLogLength, NULL, infoLog);
		fprintf (stderr, "compile log: %s\n", infoLog);
		free (infoLog);
	}
}

GLuint compileShaderSource(GLenum type, GLsizei count, const char **string)
{
	GLuint shader = glCreateShader (type);
	glShaderSource (shader, count,  string, NULL);
	compileAndCheck (shader);
	return shader;
}

void linkAndCheck(GLuint program)
{
	GLint status;
	glLinkProgram (program);
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		char *infoLog;
		glGetProgramiv (program, GL_INFO_LOG_LENGTH, &infoLogLength);
		infoLog = (char*) malloc (infoLogLength);
		glGetProgramInfoLog (program, infoLogLength, NULL, infoLog);
		fprintf (stderr, "link log: %s\n", infoLog);
		free (infoLog);
	}
}

GLuint createProgram(GLuint vertexShader, GLuint fragmentShader)
{
	GLuint program = glCreateProgram ();
	if (vertexShader != 0) {
		glAttachShader (program, vertexShader);
	}
	if (fragmentShader != 0) {
		glAttachShader (program, fragmentShader);
	}
	linkAndCheck (program);
	return program;
}



void initShader(void)
{
	char * vertexShaderSource = textFileRead("./shaders/phong.vert");
	GLuint vertexShader =
		compileShaderSource (GL_VERTEX_SHADER, 1, (const char **) &vertexShaderSource);

	char * fragmentShaderSource = textFileRead("./shaders/phong.frag");
	GLuint fragmentShader =
		compileShaderSource (GL_FRAGMENT_SHADER, 1,(const char **) &fragmentShaderSource);

	GLuint program = createProgram (vertexShader, fragmentShader);

	glUseProgram (program);
	//jpq
	projectionMatrixIndex = glGetUniformLocation(program, "Projection");
	modelViewMatrixIndex = glGetUniformLocation(program, "ModelView");
	lightPositionIndex = glGetUniformLocation(program, "lightPosition");

	normalIndex = glGetAttribLocation(program, "vertexNormal");
	glEnableVertexAttribArray (normalIndex);

	vertexIndex = glGetAttribLocation(program, "vertexPosition");
	glEnableVertexAttribArray (vertexIndex);
	checkError ("initShader");
}

void initRendering(void)
{
	glClearColor (0.1, 0.1, 0.6, 0.0);

	checkError ("initRendering");
}

void init(void) 
{
	initBuffer ();
	initShader ();
	initRendering ();
}

void dumpInfo(void)
{
	
	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
	checkError ("dumpInfo");
	printf ("Graphics Final Project by Peter Barnett\n\n");
	printf ("*- Angle the mirror with 'Q' and 'W'\n");
	printf ("*- Stop and Start the rotation of the objects with 'R'\n");
}

const GLvoid *bufferObjectPtr (GLsizei index)
{
	;
	return (const GLvoid *) (((char *) NULL) + index);
}


// draw a cube
void renderCube(){
	glUniformMatrix4fv (projectionMatrixIndex, 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv (modelViewMatrixIndex, 1, GL_FALSE, &modelviewMatrix[0][0]);
	glUniform4fv(lightPositionIndex,1,&lightPosition[0]);
	glBindBuffer (GL_ARRAY_BUFFER, vertexBufferName);
	glVertexAttribPointer (normalIndex, numNormalComponents, GL_FLOAT, GL_FALSE,
		stride, bufferObjectPtr (0));
	glVertexAttribPointer (vertexIndex, numVertexComponents, GL_FLOAT, GL_FALSE,
		stride, bufferObjectPtr (sizeof(GLfloat) * numNormalComponents));
	glDrawArrays(GL_QUADS, 0, numElements);
	checkError ("renderCube");
}

// draw a flattened cube
void renderFlat(){
	modelviewMatrix *= scale(3.0f,0.01f,5.0f);
	renderCube();
}

void display(void)
{
	glClear (GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	viewMatrix = glm::lookAt(vec3(camX,camY,camZ),vec3(0.0,0.0,camZ + 5.0f),vec3(0.0,1.0,0.0));// you can think of this matrix as the view transform of the camera
	// Eye is the pos of the camera, Center is where the camera points, Up describes the direction of up
	viewMatrix *= translate(0.0f, 0.0f, -5.0f) * translate(0.0f, 0.0f, 5.0f);

	// Begin stencil buffer stuff
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glEnable(GL_STENCIL_TEST);	
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	//Draw the stencil shape (same as the reflecting surface)
	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);
	modelviewMatrix = viewMatrix * translate(0.0f,-0.99f,0.0f);
	modelviewMatrix *= rotate(user_angle,0.0f,0.0f,1.0f);
	renderFlat();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glStencilMask(0x00);

	// draw where stencil is 1
	glStencilFunc(GL_EQUAL, 1, 0xFF);

	// Draw the REFLECTIONS (reflected object)
	// MID REFLECTION
	modelviewMatrix = viewMatrix * translate(0.0f,-1.5f,1.0f);
	modelviewMatrix *= translate(0.0f, 0.75f, 0.0f) * rotate(user_angle*2,0.0f,0.0f,1.0f) * translate(0.0f, -0.75f, 0.0f);
	modelviewMatrix *= rotate(my_angle, 0.0f, 1.0f, 0.0f);
	renderCube();
	// LEFT REFLECTION
	modelviewMatrix = viewMatrix * translate(2.0f, -1.5f, 1.0f);
	modelviewMatrix *= translate(-2.0f, 0.75f, 0.0f) * rotate(user_angle*2,0.0f,0.0f,1.0f) * translate(2.0f, -0.75f, 0.0f);
	modelviewMatrix *= rotate(my_angle, 0.0f, 1.0f, 0.0f);
	renderCube();
	// RIGHT REFLECTION
	modelviewMatrix = viewMatrix * translate(-2.0f, -1.5f, 1.0f);
	modelviewMatrix *= translate(2.0f, 0.75f, 0.0f) * rotate(user_angle*2,0.0f,0.0f,1.0f) * translate(-2.0f, -0.75f, 0.0f);
	modelviewMatrix *= rotate(my_angle, 0.0f, 1.0f, 0.0f);
	renderCube();
	
	// draw where stencil is 0
	//glStencilFunc(GL_EQUAL, 0, 0xFF);

	glDisable(GL_STENCIL_TEST); //disable the stencil testing

	// Draw the OBJECTS
	// MID OBJECT
	modelviewMatrix = viewMatrix * translate(0.0f,0.0f,1.0f);
	modelviewMatrix *= rotate(my_angle, 0.0f, 1.0f, 0.0f);
	renderCube();
	// LEFT OBJECT
	modelviewMatrix = viewMatrix * translate(2.0f, 0.0f, 1.0f);
	modelviewMatrix *= rotate(my_angle, 0.0f, 1.0f, 0.0f);
	renderCube();
	// RIGHT OBJECT
	modelviewMatrix = viewMatrix * translate(-2.0f, 0.0f, 1.0f);
	modelviewMatrix *= rotate(my_angle, 0.0f, 1.0f, 0.0f);
	renderCube();

	//glDisable(GL_STENCIL_TEST); //disable the stencil testing

	// Draw the MIRROR with alpha blending
	glEnable(GL_BLEND); //enable alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //set the blending function
	modelviewMatrix = viewMatrix * translate(0.0f,-0.99f,0.0f);
	modelviewMatrix *= rotate(user_angle,0.0f,0.0f,1.0f);
	renderFlat();
	glDisable(GL_BLEND); //disable alpha blending	

	glutSwapBuffers ();
	if( spinning )
		my_angle++;
	checkError ("display");
}

void idle(){
	glutPostRedisplay();
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);

	projectionMatrix = glm::perspective(45.0f, (float)w/(float)h,.01f,20.0f);
	
	checkError ("reshape");
}

void keyboard( unsigned char key, int x, int y)
{
	switch(key) {
		case 27:
			exit(0);
			break;
		case 'q':
			user_angle += 1.0;
			break;
		case 'w':
			user_angle -= 1.0;
			break;
		case 'r':
			if( spinning )
				spinning = false;
			else
				spinning = true;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize (500, 500); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow (argv[0]);
	glewInit();
	dumpInfo ();
	init ();
	glutDisplayFunc(display); 
	glutIdleFunc(idle); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc (keyboard);
	glEnable(GL_DEPTH_TEST);

	glutMainLoop();
	return 0;
}