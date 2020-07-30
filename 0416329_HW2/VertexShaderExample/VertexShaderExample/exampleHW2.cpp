#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#define PI 3.14159265358
using namespace std;

int currT, oldT;
int windowSize[2] = { 600, 600 };
void display();
void idle();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);

GLuint program;
GLuint vboName;

//Initialize the shaders 
void shaderInit();
void textureInit();
//Number of vertices that will be sent to shader
int stacks = 180, slices = 360;
long numberVertices = 130684;//65342;
GLfloat hour = 0.0f;
GLuint texture;

vector<GLuint>indices;
//Storing vertex datas that will be sent to shader
GLuint m_vao, m_vboIndex;
/*class VertexAttribute {
public:
	GLfloat position[3];
	void setPosition(float x, float y, float z) {
		position[0] = x;
		position[1] = y;
		position[2] = z;
	};
};*/

struct VertexAttribute
{
	GLfloat position[3];
	GLfloat texcoord[2];
};

//Collecting vertex datas
VertexAttribute *drawTriangle();
VertexAttribute *drawSphere();

int main(int argc, char** argv) {	
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("ComputerGraphicsDemo");

	glewInit();
	shaderInit();
	textureInit();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

void shaderInit() {
	GLuint vert = createShader("Shaders/example.vert", "vertex");
	GLuint frag = createShader("Shaders/example.frag", "fragment");
	program = createProgram(vert, frag);

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
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));
	glEnableVertexAttribArray(1);
	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

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

void display() {   
	//1.Clear the buffer

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 2.viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f,// eye
		0.0f, 0.0f, 0.0f,// center
		0.0f, 1.0f, 0.0f);// up

	// 3.projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);

	// 4.viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);
	//glEnable(GL_CULL_FACE);
	// 5.sphere rotation
	glMatrixMode(GL_MODELVIEW);
	glRotatef(hour, 0.0f, 1.0f, 0.0f);
	//glRotatef(360.0 * hour / 24.0, 1.0f, 0.0f, 0.0f);
	//6.Get the projection and modelview matrix and store in pmtx and mmtx
	GLfloat pmtx[16];
	GLfloat mmtx[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	//7. get the location of uniform variable
 	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");
	GLint texLoc = glGetUniformLocation(program, "ourTexture");

	//8.install the program object
	glUseProgram(program);

	//texture stuff
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texLoc, 0);
	//9.Pass proj and modelv matrix
	//input the modelview matrix into vertex shader
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	//input the rotation matrix into vertex shader
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	//10.render primities
	glBindVertexArray(m_vao);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndex);
	glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT,NULL);

	//11.set the current rendering state to NULL
	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(0);
	glPopMatrix();
	glutSwapBuffers();



	//hour += 0.05f;
	//hour = hour - ((int)(hour / 24)) * 24;
}

VertexAttribute* drawSphere() {
	VertexAttribute* vertices;
	
	vertices = new VertexAttribute[numberVertices];
	int vertexIndex = 0;
	float r = 2.0f,x,y,z;
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
			
			s = (PI+atan2f(x, z)) / (2.0f * PI) + 0.5f;
			t = asinf(y) / PI + 0.5f;
			
			vertices[vertexIndex].position[0] = x;
			vertices[vertexIndex].position[1] = y;
			vertices[vertexIndex].position[2] = z;
			vertices[vertexIndex].texcoord[0] = s;
			vertices[vertexIndex].texcoord[1] = t;
			indices.push_back(vertexIndex);
			vertexIndex++;

			x = sin(j * stackStep) * cos((i+1) * sliceStep);
			y = cos(j * stackStep);
			z = sin(j * stackStep) * sin((i+1) * sliceStep);
			s2 = (PI+atan2f(x, z) )/ (2.0f * PI) + 0.5f;
			t2 = asinf(y) / PI + 0.5f;
			if (s2 < 0.75f && s > 0.75f)
			{
				s2 += 1.0f;
			}
			else if (s2 > 0.75 && s < 0.75)
			{
				s2 -= 1.0f;
			}
			/*if (t2 < 0.75f && t > 0.75f)
			{
				t2 += 1.0f;
			}
			else if (t2 > 0.75 && t < 0.75)
			{
				t2 -= 1.0f;
			}*/
			vertices[vertexIndex].position[0] = x;
			vertices[vertexIndex].position[1] = y;
			vertices[vertexIndex].position[2] = z;
			vertices[vertexIndex].texcoord[0] = s2;
			vertices[vertexIndex].texcoord[1] = t2;
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

}

void idle() {
	currT = glutGet(GLUT_ELAPSED_TIME);
	float diff = (currT - oldT) / 1000.0f;
	oldT = currT;

	hour = fmod(hour + 20.0f * diff, 360);
	glutPostRedisplay();
}