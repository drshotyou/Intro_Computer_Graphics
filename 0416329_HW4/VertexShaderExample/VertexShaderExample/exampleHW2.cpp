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
int windowSize[2] = { 1080, 1080 };
void display();
void idle();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);

GLuint program;


//Initialize the shaders 
void shaderInit();
void textureInit();

//Number of vertices that will be sent to shader
int stacks = 180, slices = 360, mode = 0;
long numberVertices = 130684;//65342;
GLfloat hour = 0.0f;
GLuint texture;
vector<GLuint>indices;
//Storing vertex datas that will be sent to shader
GLuint m_vao, m_vboIndex, vboName;
GLuint m_vao2, vboName2;
GLuint m_vao3, vboName3;
GLfloat time,alpha=1.0f,particleAlpha=0.0f;


struct VertexAttribute
{
	GLfloat position[3];
	GLfloat texcoord[2];
};

struct ParticleAttribute
{
	GLfloat position[3] = {};
	GLfloat speed[3];
	GLfloat fade;
	GLfloat life;
};
ParticleAttribute* gParticles;
ParticleAttribute* gParticles2;
//Collecting vertex datas
VertexAttribute *drawTriangle();
VertexAttribute *drawSphere();
ParticleAttribute* initParticlesPosition();
ParticleAttribute* initParticlesPosition2();

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
	
	glGenVertexArrays(1, &m_vao2);
	glBindVertexArray(m_vao2);
	glGenBuffers(1, &vboName2);
	glBindBuffer(GL_ARRAY_BUFFER, vboName2);
	gParticles = initParticlesPosition();
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 20000, gParticles, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));
	glEnableVertexAttribArray(2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 20000, gParticles, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, life)));
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_vao3);
	glBindVertexArray(m_vao3);
	glGenBuffers(1, &vboName3);
	glBindBuffer(GL_ARRAY_BUFFER, vboName3);
	gParticles2 = initParticlesPosition2();
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 80000, gParticles2, GL_STATIC_DRAW);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));
	glEnableVertexAttribArray(4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 80000, gParticles2, GL_STATIC_DRAW);
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, life)));
	glEnableVertexAttribArray(5);
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
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
	glClearDepth(1.0f);
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);
	//glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 10.0f,// eye
		0.0f, 0.0f, 0.0f,// center
		0.0f, 1.0f, 0.0f);// up
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);
	glViewport(0, 0, windowSize[0], windowSize[1]);
	//glEnable(GL_CULL_FACE);
	glMatrixMode(GL_MODELVIEW);
	GLfloat rmtx[16];
	GLint rotLoc = glGetUniformLocation(program, "rotation");
	glGetFloatv(GL_PROJECTION, rmtx);
	glUniformMatrix4fv(rotLoc, 1, GL_FALSE, rmtx);
	glRotatef(hour, 0.0f, 1.0f, 0.0f);
	GLfloat pmtx[16];
	GLfloat mmtx[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	mode = 0;
 	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");
	GLint texLoc = glGetUniformLocation(program, "ourTexture");
	GLint timeLoc = glGetUniformLocation(program, "time");
	GLint alphaLoc = glGetUniformLocation(program, "alpha");
	GLint modeLoc = glGetUniformLocation(program, "mode");
	GLint particleAlphaLoc = glGetUniformLocation(program, "particleAlpha");
	glUseProgram(program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texLoc, 0);
	glUniform1i(modeLoc, mode);
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);
	if (time >= 15000)
	{
		if (alpha > 0.01)
		{
			alpha -= 0.0075;
		}
		if (particleAlpha < 1)
		{
			particleAlpha += 0.01;
		}
		
	//std:printf("alpha:%f\n", alpha);
	}
	glUniform1f(timeLoc, time);
	glUniform1f(alphaLoc, alpha);
	glUniform1f(particleAlphaLoc, particleAlpha);
	//10.render primities
	glBindVertexArray(m_vao);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(GL_PRIMITIVE_RESTART_FIXED_INDEX);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndex);
	glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT,NULL);
	
	//11.set the current rendering state to NULL
	glBindTexture(GL_TEXTURE_2D, NULL);


	glUseProgram(0);
	glUseProgram(program);
	//GLint pmatLoc = glGetUniformLocation(program, "Projection");
	//GLint mmatLoc = glGetUniformLocation(program, "ModelView");
	mode = 1;
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);
	glUniform1i(modeLoc, mode);
	glUniform1f(particleAlphaLoc, particleAlpha);
	glBindVertexArray(m_vao2);
	
	if (alpha < 0.5)
	{
		glDrawArrays(GL_POINTS, 0, 20000);
	}
	glUseProgram(0);
	glUseProgram(program);
	mode = 2;
	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);
	glUniform1i(modeLoc, mode);
	glUniform1f(particleAlphaLoc, particleAlpha);
	glBindVertexArray(m_vao3);

	if (alpha < 0.5)
	{
		glDrawArrays(GL_POINTS, 0, 80000);
	}
	glUseProgram(0);
	/*if (alpha < 0.1)
	{
		GLUquadric* quad = gluNewQuadric();
		glColor4f(0.98f, 0.83f, 0.25f, particleAlpha);
		//glColor3f();
		glScalef(1.0f, 1.0f, 1.0f);
		gluSphere(quad, 1, 20, 10);
	}*/
	
	glutSwapBuffers();
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

ParticleAttribute* initParticlesPosition() {
	ParticleAttribute* particles;
	particles = new ParticleAttribute[20000];

	for (int i = 0; i < 20000; i++)
	{
		particles[i].position[0] = 0.0;
		particles[i].position[1] = 0.0;
		particles[i].position[2] = 0.0;
		float stepi = 2 * PI / 180;
		int randomi = rand() % 180;
		float stepj = 2 * PI / 180;
		int randomj = rand() % 180;
		particles[i].speed[0] = sin(randomj * stepj) * cos(randomi * stepi) * 100.0;
		particles[i].speed[1] = cos(randomj * stepj) * 100.0;
		particles[i].speed[2] = sin(randomj * stepj) * sin(randomi * stepi) * 100.0;
		if (i < 5000)
		{
		//	particles[i].speed[0] = sin(randomj * stepj) * cos(randomi * stepi) * 500.0;
		}
		particles[i].life = 2.5f;
		particles[i].fade = GLfloat(rand() % 100) / 1000.0f + 0.003f;
	}
	return particles;
}

ParticleAttribute* initParticlesPosition2() {
	ParticleAttribute* particles;
	particles = new ParticleAttribute[80000];

	for (int i = 0; i < 80000; i++)
	{
		particles[i].position[0] = 0.0;
		particles[i].position[1] = 0.0;
		particles[i].position[2] = 0.0;
		float stepi = 2 * PI / 180;
		int randomi = rand() % 180;
		float stepj = 2 * PI / 180;
		int randomj = rand() % 180;
		particles[i].speed[0] = sin(randomj * stepj) * cos(randomi * stepi) * 50.0;
		particles[i].speed[1] = cos(randomj * stepj) * 50.0;
		particles[i].speed[2] = sin(randomj * stepj) * sin(randomi * stepi) * 50.0;
		if (i < 5000)
		{
			//	particles[i].speed[0] = sin(randomj * stepj) * cos(randomi * stepi) * 500.0;
		}
		particles[i].life = 2.0f;
		particles[i].fade = GLfloat(rand() % 100) / 1000.0f + 0.003f;
	}
	return particles;

}

void reshape(GLsizei w, GLsizei h) {
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y) {

}

void idle() {
	currT = glutGet(GLUT_ELAPSED_TIME);
	time = currT;
	//std::printf("time:%f\n",time);
	float diff = (currT - oldT) / 1000.0f;
	oldT = currT;

	hour = fmod(hour + 20.0f * diff, 360);
	if (alpha < 0.95)
	{
		for (int i = 0; i < 20000; i++)
		{
			gParticles[i].position[0] = gParticles[i].position[0] + gParticles[i].speed[0] / 10000.0;
			gParticles[i].position[1] = gParticles[i].position[1] + gParticles[i].speed[1] / 10000.0;
			gParticles[i].position[2] = gParticles[i].position[2] + gParticles[i].speed[2] / 10000.0;

			gParticles[i].life -= gParticles[i].fade;
			//if (gParticles[i].life < 0.0)
			//{
			//	gParticles[i].life = 1.0f;
			//	gParticles[i].fade = float(rand() % 100) / 1000.0f + 0.003f;
			//	gParticles[i].position[0] = 0.0;
			//	gParticles[i].position[1] = 0.0;
			//	gParticles[i].position[2] = 0.0;
			//}
		}
		for (int i = 0; i < 80000; i++)
		{
			gParticles2[i].position[0] = gParticles2[i].position[0] + gParticles2[i].speed[0] / 10000.0;
			gParticles2[i].position[1] = gParticles2[i].position[1] + gParticles2[i].speed[1] / 10000.0;
			gParticles2[i].position[2] = gParticles2[i].position[2] + gParticles2[i].speed[2] / 10000.0;

			gParticles2[i].life -= gParticles2[i].fade;
			if (gParticles2[i].life < 0.0)
			{
				gParticles2[i].life = 1.0f;
				gParticles2[i].fade = float(rand() % 100) / 1000.0f + 0.003f;
				gParticles2[i].position[0] = 0.0;
				gParticles2[i].position[1] = 0.0;
				gParticles2[i].position[2] = 0.0;
			}
		}
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, vboName2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 20000, gParticles, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));
	glEnableVertexAttribArray(2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 20000, gParticles, GL_STATIC_DRAW);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, life)));
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vboName3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 80000, gParticles2, GL_STATIC_DRAW);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, position)));
	glEnableVertexAttribArray(4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleAttribute) * 80000, gParticles2, GL_STATIC_DRAW);
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleAttribute), (void*)(offsetof(ParticleAttribute, life)));
	glEnableVertexAttribArray(5);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	glutPostRedisplay();
}