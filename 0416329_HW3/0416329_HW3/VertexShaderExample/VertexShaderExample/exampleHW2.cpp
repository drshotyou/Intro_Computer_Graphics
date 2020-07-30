#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/type_ptr.hpp"
//#include "glm/vec3.hpp"
//#include "glm/vec4.hpp"
//#include "glm/ext/matrix_transform.hpp"
//#include "glm/ext/matrix_clip_space.hpp"
#define PI 3.14159265358
using namespace std;

int currT, oldT;
int windowSize[2] = { 600, 600 };
void display();
void idle();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);
GLuint shaderOption = 0;
GLuint program;
GLuint vboName;

//Initialize the shaders 
void shaderInit();
void textureInit();
//Number of vertices that will be sent to shader
int stacks = 10, slices = 20;
long numberVertices = 130684;
glm::vec3 lightVect(1.1, 1.0, 1.3);
GLfloat hour = 0.0f,Kd=0,Ks=0;


vector<GLuint>indices;
//Storing vertex datas that will be sent to shader
GLuint m_vao, m_vboIndex;


struct VertexAttribute
{
	GLfloat position[3];
	GLfloat normals[3];
};

//Collecting vertex datas

VertexAttribute *drawSphere();

int main(int argc, char** argv) {	
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("ComputerGraphicsDemo");

	glewInit();
	shaderInit();
	

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

void shaderInit() {
	GLuint vert1,vert2;
	GLuint frag1,frag2;
	if (shaderOption == 0)
	{
		vert1 = createShader("Shaders/gourard.vert", "vertex");
		frag1 = createShader("Shaders/gourard.frag", "fragment");
		program = createProgram(vert1, frag1);
	}
	else
	{
		vert2 = createShader("Shaders/phong.vert", "vertex");
		frag2 = createShader("Shaders/phong.frag", "fragment");
		program = createProgram(vert2, frag2);
	}
	

	//Generate a new buffer object
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);
	//Copy vertex data to the buffer object
	VertexAttribute *vertices;
	vertices = drawSphere();
	//cout << "vertices" <<  endl;
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * numberVertices, vertices, GL_STATIC_DRAW);
	//6.1
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glEnableVertexAttribArray(0);
	//6.2
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normals)));
	glEnableVertexAttribArray(1);
	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
/*
void textureInit()
{
	//char  fileName = ;
	const char* fileNameP = "earth_texture_map.jpg";
	//1. Enable Texture

	glEnable(GL_TEXTURE_2D);
	//2. Load texture
	FIBITMAP* pImage = FreeImage_Load(FreeImage_GetFileType(fileNameP,0),fileNameP);
	FIBITMAP* p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	int iWidth = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);
	//3. Generate texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	//4. Set the texture warping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//5. Specify texture environment
	glTexEnvf(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//6. Generate a two dimensional texture image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));
	//set filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	//glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	FreeImage_Unload(p32BitsImage);
	FreeImage_Unload(pImage);


}
*/
void display() {   
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 5.6f,// eye
		0.0f, 0.0f, 0.0f,// center
		0.0f, 1.0f, 0.0f);// up

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);

	glViewport(0, 0, windowSize[0], windowSize[1]);
	//1.Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_CULL_FACE);
	GLfloat lmtx[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(1.1f, 1.0f, 1.3f);
	GLUquadric* quad = gluNewQuadric();
	glColor3f(0.4f, 0.5f, 0.0f);
	glScalef(0.05f, 0.05f, 0.05f);
	gluSphere(quad, 1, 20, 10);
	//glTranslatef(-1.1f, -1.0f, -1.3f);
	glPopMatrix();

	glPushMatrix();
	glGetFloatv(GL_MODELVIEW_MATRIX, lmtx);
	glRotatef(hour, 0.0f, 1.0f, 0.0f);
	
	GLfloat pmtx[16];
	GLfloat mmtx[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	
	
	GLint mmatLoc = glGetUniformLocation(program, "modelview");
	GLint pmatLoc = glGetUniformLocation(program, "projection");
	GLint lmatLoc = glGetUniformLocation(program, "lightm");
	//glm::mat3 normalMat = glm::transpose(glm::inverse(modelView));
	GLint uniNormalMat = glGetUniformLocation(program, "normalMat");
	GLint uniLightPos = glGetUniformLocation(program, "lightPos");
	GLint uniKa = glGetUniformLocation(program, "Ka");
	GLint uniKd = glGetUniformLocation(program, "Kd");
	GLint uniKs = glGetUniformLocation(program, "Ks");
	GLint uniShininess = glGetUniformLocation(program, "shininessVal");
	GLint ambientLight = glGetUniformLocation(program, "ambientLight");
	GLint diffuseLight = glGetUniformLocation(program, "diffuseLight");
	GLint specularLight = glGetUniformLocation(program, "specularLight");
	GLfloat shineVal = 3.6,ka=0.5;
	
	
	GLfloat lightPos[3] = { 1.1f,1.0f,1.3f };
	glm::mat4 modelView(mmtx[0], mmtx[1], mmtx[2], mmtx[3], mmtx[4], mmtx[5], mmtx[6], mmtx[7], mmtx[8], mmtx[9], mmtx[10], mmtx[11], mmtx[12], mmtx[13], mmtx[14], mmtx[15]);
	glm::mat3 normMatrix;
	normMatrix = glm::transpose(glm::inverse((glm::mat3(modelView))));

	glUseProgram(program);
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);
	glUniformMatrix4fv(lmatLoc, 1, GL_FALSE, lmtx);
	glUniformMatrix3fv(uniNormalMat, 1, GL_FALSE,glm::value_ptr(normMatrix));
	glUniform3fv(uniLightPos,1,&lightPos[0]);
	glUniform1f(uniShininess,shineVal);
	glUniform1f(uniKa,ka);
	glUniform1f(uniKd,Kd);
	glUniform1f(uniKs,Ks);

	glBindVertexArray(m_vao);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndex);
	glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT,NULL);

	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(0);
	glPopMatrix();
	glutSwapBuffers();

}

VertexAttribute* drawSphere() {
	VertexAttribute* vertices;
	
	vertices = new VertexAttribute[numberVertices];
	int vertexIndex = 0;
	float r = 1.0f,x,y,z;
	float sliceStep = 2 * PI / slices;
	float stackStep = PI / stacks;
	float s, t,s2,t2;
	for (int i = 0; i <= slices; i++)
	{
		
		for (int j = 0; j <= stacks;j++)
		{
			
			x = sin(j * stackStep) * cos(i * sliceStep);
			y = cos(j * stackStep);
			z = sin(j * stackStep) * sin(i * sliceStep);
			
			vertices[vertexIndex].position[0] = x;
			vertices[vertexIndex].position[1] = y;
			vertices[vertexIndex].position[2] = z;
			vertices[vertexIndex].normals[0] = x;
			vertices[vertexIndex].normals[1] = y;
			vertices[vertexIndex].normals[2] = z;
			indices.push_back(vertexIndex);
			vertexIndex++;

			x = sin(j * stackStep) * cos((i+1) * sliceStep);
			y = cos(j * stackStep);
			z = sin(j * stackStep) * sin((i+1) * sliceStep);
			vertices[vertexIndex].position[0] = x;
			vertices[vertexIndex].position[1] = y;
			vertices[vertexIndex].position[2] = z;
			vertices[vertexIndex].normals[0] = x;
			vertices[vertexIndex].normals[1] = y;
			vertices[vertexIndex].normals[2] = z;
			indices.push_back(vertexIndex);
			vertexIndex++;
			

		}
		indices.push_back(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	}
	//cout << vertexIndex << endl;

	glGenBuffers(1, &m_vboIndex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	return vertices;
}
void reshape(GLsizei w, GLsizei h) {
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case '1': 
		printf("Ks:%f\n", Ks);
		if (Ks >= 0.1)
		{
			Ks -= 0.1;
			break;
		}
		else
			break;
	case '2':
		printf("Ks:%f\n", Ks);
		if (Ks < 1)
		{ 
			Ks += 0.1;
			break;
		}
		else
			break;
	case '3':
		printf("Kd:%f\n", Kd);
		if (Kd >=0.1)
		{
			Kd -= 0.1;
			break;
		}
		else
			break;
	case '4':
		printf("Kd:%f\n", Kd);
		if (Kd < 1)
		{
			Kd += 0.1;
			break;
		}
		else
			break;
	case 'b':
		printf("Changing to shader mode:%d\n",shaderOption);
		if (shaderOption == 0)
		{
			shaderOption = 1;
			shaderInit();
		}
		else 
		{
			shaderOption = 0;
			shaderInit();
		}
	}
}

void idle() {
	currT = glutGet(GLUT_ELAPSED_TIME);
	float diff = (currT - oldT) / 1000.0f;
	oldT = currT;

	hour = fmod(hour + 20.0f * diff, 360);
	glutPostRedisplay();
}