#include <cstdlib>
#include <cstdio>
#include "graphics.hpp"
#include "adventure.hpp"

GLfloat	origin_z = -10.0f;
GLfloat	rotation_x = 0.0f;
GLfloat	rotation_y = 0.0f;
GLfloat	rotation_z = 0.0f;
GLfloat speed_x = 0.3f;
GLfloat speed_y = 0.2f;
GLfloat speed_z = 0.4f;
GLint position_x = 0.0f;
GLint position_y = 0.0f;
GLint position_z = 0.0f;
GLfloat apothem = 0.5f;
GLfloat side = 1.0f;
GLint position_x_max = 0;
GLint position_y_max = 0;
GLint position_z_max = 0;
GLint position_x_min = 0;
GLint position_y_min = 0;
GLint position_z_min = 0;

GLint door[6];
GLint find_monster = 0;
GLint find_princess = 0;
GLint success = 0;

GLuint texture[5];
GLuint rot = 0;
GLuint light = 0;
GLuint blend = 0;

GLfloat LightAmbient[]= { 0.75f, 0.75f, 0.75f, 1.0f }; 
GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[]={ 0.0f, 0.0f, 3.0f, 1.0f };

extern Castle* castle_ptr;

void ErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_RELEASE)
		switch(key) {
			case GLFW_KEY_UP   : rotation_x -= 5.0f; break;
			case GLFW_KEY_DOWN : rotation_x += 5.0f; break;
			case GLFW_KEY_LEFT : rotation_y -= 5.0f; break;
			case GLFW_KEY_RIGHT: rotation_y += 5.0f; break;
			case GLFW_KEY_KP_8 : speed_x+=0.1f; break;
			case GLFW_KEY_KP_2 : speed_x-=0.1f; break;
			case GLFW_KEY_KP_4 : speed_y-=0.1f; break;
			case GLFW_KEY_KP_6 : speed_y+=0.1f; break;

		}
	if (action == GLFW_PRESS)
		switch(key)  {
			case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE);
			//case GLFW_KEY_L: light = light ? (glDisable(GL_LIGHTING), 0) : (glEnable(GL_LIGHTING), 1); break;
			case GLFW_KEY_R: rot = 1 - rot; break;
			//case GLFW_KEY_B: blend = blend ? (glDisable(GL_BLEND), glEnable(GL_DEPTH_TEST), 0) : (glEnable(GL_BLEND), glDisable(GL_DEPTH_TEST), 1); break;
			case GLFW_KEY_W: if(door[0]) castle_ptr->hero_.Move(0);position_y++; break;
			case GLFW_KEY_S: if(door[1]) castle_ptr->hero_.Move(1);position_y--; break;
			case GLFW_KEY_A: if(door[2]) castle_ptr->hero_.Move(2);position_x--; break;
			case GLFW_KEY_D: if(door[3]) castle_ptr->hero_.Move(3);position_x++; break;
			case GLFW_KEY_E: if(door[4]) castle_ptr->hero_.Move(4);position_z++; break;
			case GLFW_KEY_Q: if(door[5]) castle_ptr->hero_.Move(5);position_z--; break;
			case GLFW_KEY_SPACE: rotation_x = rotation_y = rotation_z = 0.0; rot = 0; break;
		}
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	origin_z += yoffset;
}

void ResizeWindow(GLFWwindow* window, int width, int height)
{
	float ratio;

	ratio = width / (float) height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);
	//gluLookAt(eye_x, eye_y, eye_z, 0, 0, 0, 0, 1, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int LoadBMP(const char * imagepath){

//	printf("Reading image %s\n", imagepath);

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	// Actual RGB data
	unsigned char * data;

	unsigned int texture;

	// Open the file
	FILE * file = fopen(imagepath,"rb");
	if (!file)
		{printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0;}

	// Read the header, i.e. the 54 first bytes

	// If less than 54 bytes are read, problem
	int depress_warning = fread(header, 1, 54, file);
	if ( depress_warning!=54 ){ 
		printf("Not a correct BMP file\n");
		return 0;
	}
	// A BMP files always begins with "BM"
	if ( header[0]!='B' || header[1]!='M' ){
		printf("Not a correct BMP file\n");
		return 0;
	}
	// Make sure this is a 24bpp file
	if ( *(int*)&(header[0x1E])!=0  )
		{printf("Not a correct BMP file\n");    return 0;}
	if ( *(int*)&(header[0x1C])!=24 )
		{printf("Not a correct BMP file\n");    return 0;}

	// Read the information about the image
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char [imageSize];

	// Read the actual data from the file into the buffer
	depress_warning = fread(data,1,imageSize,file);

	// Everything is in memory now, the file wan be closed
	fclose (file);

	// Create one OpenGL texture
	glGenTextures(1, &texture);
	
	// Create Linear Filtered Texture
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	delete [] data;

	return texture;
}

void LoadTexture() {
	texture[0] = LoadBMP("data/red.bmp");
	texture[1] = LoadBMP("data/green.bmp");
	texture[2] = LoadBMP("data/princess.bmp");
	texture[3] = LoadBMP("data/monster.bmp");
	texture[4] = LoadBMP("data/success.bmp");
}

void InitGL() {
	LoadTexture();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
//	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//	glEnable(GL_MULTISAMPLE_ARB);

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);
	glEnable(GL_LIGHT1);
}

void Render(GLFWwindow* window) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, origin_z);

	if(rot) { 
		rotation_x+=speed_x;
		rotation_y+=speed_y;
		rotation_z+=speed_z;
	}

	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef( 45.0f, 0.0f, 0.0f, 1.0f);
	glRotatef( 45.0f, 1.0f,-1.0f, 0.0f);

	glRotatef(rotation_x, 1.0f,-1.0f, 0.0f);
	glRotatef(rotation_y, 0.0f, 0.0f, 1.0f);
	glRotatef(rotation_z, 0.0f, 0.0f, 1.0f);

	glLineStipple(1, 0x1111);
	glEnable(GL_LINE_STIPPLE);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	glColor3f(0.25f, 0.25f, 0.25f);
	for(int i = position_x_min; i <= position_x_max; i++)
		for(int j = position_y_min; j <= position_y_max; j++) {
			glBegin(GL_LINE_LOOP);
				glNormal3f(0.f, 0.f, 1.f);
				glVertex3f(i - apothem, j - apothem, position_z_max + apothem);
				glVertex3f(i - apothem, j + apothem, position_z_max + apothem);
				glVertex3f(i + apothem, j + apothem, position_z_max + apothem);
				glVertex3f(i + apothem, j - apothem, position_z_max + apothem);
			glEnd();
/*			glBegin(GL_LINE_LOOP);
				glNormal3f(0.f, 0.f, -1.f);
				glVertex3f(position_x - apothem, position_y - apothem, position_z - apothem);
				glVertex3f(position_x - apothem, position_y + apothem, position_z - apothem);
				glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
				glVertex3f(position_x + apothem, position_y - apothem, position_z - apothem);
			glEnd();
			glBegin(GL_LINE_LOOP);
				glNormal3f(0.f, 1.f, 0.f);
				glVertex3f(position_x - apothem, position_y + apothem, position_z - apothem);
				glVertex3f(position_x - apothem, position_y + apothem, position_z + apothem);
				glVertex3f(position_x + apothem, position_y + apothem, position_z + apothem);
				glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
			glEnd();
			glBegin(GL_LINE_LOOP);
				glNormal3f(0.f, -1.f, 0.f);
				glVertex3f(position_x - apothem, position_y - apothem, position_z - apothem);
				glVertex3f(position_x - apothem, position_y - apothem, position_z + apothem);
				glVertex3f(position_x + apothem, position_y - apothem, position_z + apothem);
				glVertex3f(position_x + apothem, position_y - apothem, position_z - apothem);
			glEnd();
			glBegin(GL_LINE_LOOP);
				glNormal3f(1.f, 0.f, 0.f);
				glVertex3f(position_x + apothem, position_y - apothem, position_z - apothem);
				glVertex3f(position_x + apothem, position_y - apothem, position_z + apothem);
				glVertex3f(position_x + apothem, position_y + apothem, position_z + apothem);
				glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
			glEnd();
			glBegin(GL_LINE_LOOP);
				glNormal3f(-1.f, 0.f, 0.f);
				glVertex3f(position_x - apothem, position_y - apothem, position_z - apothem);
				glVertex3f(position_x - apothem, position_y - apothem, position_z + apothem);
				glVertex3f(position_x - apothem, position_y + apothem, position_z + apothem);
				glVertex3f(position_x - apothem, position_y + apothem, position_z - apothem);
			glEnd();*/
		}
	for(int i = position_x_min; i <= position_x_max; i++)
		for(int j = position_z_min; j <= position_z_max; j++) {
/*			glBegin(GL_LINE_LOOP);
				glNormal3f(0.f, 0.f, 1.f);
				glVertex3f(i - apothem, j - apothem, position_z_max + apothem);
				glVertex3f(i - apothem, j + apothem, position_z_max + apothem);
				glVertex3f(i + apothem, j + apothem, position_z_max + apothem);
				glVertex3f(i + apothem, j - apothem, position_z_max + apothem);
			glEnd();
			glBegin(GL_LINE_LOOP);
				glNormal3f(0.f, 0.f, -1.f);
				glVertex3f(position_x - apothem, position_y - apothem, position_z - apothem);
				glVertex3f(position_x - apothem, position_y + apothem, position_z - apothem);
				glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
				glVertex3f(position_x + apothem, position_y - apothem, position_z - apothem);
			glEnd();
			glBegin(GL_LINE_LOOP);
				glNormal3f(0.f, 1.f, 0.f);
				glVertex3f(position_x - apothem, position_y + apothem, position_z - apothem);
				glVertex3f(position_x - apothem, position_y + apothem, position_z + apothem);
				glVertex3f(position_x + apothem, position_y + apothem, position_z + apothem);
				glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
			glEnd();*/
			glBegin(GL_LINE_LOOP);
				glNormal3f(0.f, -1.f, 0.f);
				glVertex3f(i - apothem, position_y_min - apothem, j - apothem);
				glVertex3f(i - apothem, position_y_min - apothem, j + apothem);
				glVertex3f(i + apothem, position_y_min - apothem, j + apothem);
				glVertex3f(i + apothem, position_y_min - apothem, j - apothem);
			glEnd();
/*			glBegin(GL_LINE_LOOP);
				glNormal3f(1.f, 0.f, 0.f);
				glVertex3f(position_x + apothem, position_y - apothem, position_z - apothem);
				glVertex3f(position_x + apothem, position_y - apothem, position_z + apothem);
				glVertex3f(position_x + apothem, position_y + apothem, position_z + apothem);
				glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
			glEnd();
			glBegin(GL_LINE_LOOP);
				glNormal3f(-1.f, 0.f, 0.f);
				glVertex3f(position_x - apothem, position_y - apothem, position_z - apothem);
				glVertex3f(position_x - apothem, position_y - apothem, position_z + apothem);
				glVertex3f(position_x - apothem, position_y + apothem, position_z + apothem);
				glVertex3f(position_x - apothem, position_y + apothem, position_z - apothem);
			glEnd();*/
		}
	for(int i = position_z_min; i <= position_z_max; i++)
		for(int j = position_y_min; j <= position_y_max; j++) {
/*			glBegin(GL_LINE_LOOP);
				glNormal3f(0.f, 0.f, 1.f);
				glVertex3f(i - apothem, j - apothem, position_z_max + apothem);
				glVertex3f(i - apothem, j + apothem, position_z_max + apothem);
				glVertex3f(i + apothem, j + apothem, position_z_max + apothem);
				glVertex3f(i + apothem, j - apothem, position_z_max + apothem);
			glEnd();
			glBegin(GL_LINE_LOOP);
				glNormal3f(0.f, 0.f, -1.f);
				glVertex3f(position_x - apothem, position_y - apothem, position_z - apothem);
				glVertex3f(position_x - apothem, position_y + apothem, position_z - apothem);
				glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
				glVertex3f(position_x + apothem, position_y - apothem, position_z - apothem);
			glEnd();
			glBegin(GL_LINE_LOOP);
				glNormal3f(0.f, 1.f, 0.f);
				glVertex3f(position_x - apothem, position_y + apothem, position_z - apothem);
				glVertex3f(position_x - apothem, position_y + apothem, position_z + apothem);
				glVertex3f(position_x + apothem, position_y + apothem, position_z + apothem);
				glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
			glEnd();
			glBegin(GL_LINE_LOOP);
				glNormal3f(0.f, -1.f, 0.f);
				glVertex3f(i - apothem, position_y_min - apothem, j - apothem);
				glVertex3f(i - apothem, position_y_min - apothem, j + apothem);
				glVertex3f(i + apothem, position_y_min - apothem, j + apothem);
				glVertex3f(i + apothem, position_y_min - apothem, j - apothem);
			glEnd();
			glBegin(GL_LINE_LOOP);
				glNormal3f(1.f, 0.f, 0.f);
				glVertex3f(position_x + apothem, position_y - apothem, position_z - apothem);
				glVertex3f(position_x + apothem, position_y - apothem, position_z + apothem);
				glVertex3f(position_x + apothem, position_y + apothem, position_z + apothem);
				glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
			glEnd();*/
			glBegin(GL_LINE_LOOP);
				glNormal3f(-1.f, 0.f, 0.f);
				glVertex3f(position_x_min - apothem, j - apothem, i - apothem);
				glVertex3f(position_x_min - apothem, j - apothem, i + apothem);
				glVertex3f(position_x_min - apothem, j + apothem, i + apothem);
				glVertex3f(position_x_min - apothem, j + apothem, i - apothem);
			glEnd();
		}
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);

	glDisable(GL_LINE_STIPPLE);
	
	glColor4f(1.0f,1.0f,1.0f,0.5f);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);

	glBindTexture(GL_TEXTURE_2D, texture[!!door[4]]);
	glBegin(GL_QUADS);
		// Front Face
		glNormal3f(0.f, 0.f, 1.f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(position_x - apothem, position_y - apothem, position_z + apothem);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(position_x - apothem, position_y + apothem, position_z + apothem);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(position_x + apothem, position_y + apothem, position_z + apothem);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(position_x + apothem, position_y - apothem, position_z + apothem);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[!!door[5]]);
	glBegin(GL_QUADS);
		// Back Face
		glNormal3f(0.f, 0.f, -1.f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(position_x - apothem, position_y - apothem, position_z - apothem);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(position_x - apothem, position_y + apothem, position_z - apothem);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(position_x + apothem, position_y - apothem, position_z - apothem);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[!!door[0]]);
	glBegin(GL_QUADS);
		// Top Face
		glNormal3f(0.f, 1.f, 0.f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(position_x - apothem, position_y + apothem, position_z - apothem);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(position_x - apothem, position_y + apothem, position_z + apothem);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(position_x + apothem, position_y + apothem, position_z + apothem);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[!!door[1]]);
	glBegin(GL_QUADS);
		// Bottom Face
		glNormal3f(0.f, -1.f, 0.f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(position_x - apothem, position_y - apothem, position_z - apothem);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(position_x - apothem, position_y - apothem, position_z + apothem);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(position_x + apothem, position_y - apothem, position_z + apothem);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(position_x + apothem, position_y - apothem, position_z - apothem);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[!!door[3]]);
	glBegin(GL_QUADS);
		// Right face
		glNormal3f(1.f, 0.f, 0.f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(position_x + apothem, position_y - apothem, position_z - apothem);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(position_x + apothem, position_y - apothem, position_z + apothem);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(position_x + apothem, position_y + apothem, position_z + apothem);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[!!door[2]]);
	glBegin(GL_QUADS);
		// Left Face
		glNormal3f(-1.f, 0.f, 0.f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(position_x - apothem, position_y - apothem, position_z - apothem);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(position_x - apothem, position_y - apothem, position_z + apothem);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(position_x - apothem, position_y + apothem, position_z + apothem);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(position_x - apothem, position_y + apothem, position_z - apothem);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glColor3f(0.25f, 0.25f, 0.25f);
	glBegin(GL_LINE_LOOP);
		glNormal3f(0.f, 0.f, 1.f);
		glVertex3f(position_x - apothem, position_y - apothem, position_z_max + apothem);
		glVertex3f(position_x - apothem, position_y + apothem, position_z_max + apothem);
		glVertex3f(position_x + apothem, position_y + apothem, position_z_max + apothem);
		glVertex3f(position_x + apothem, position_y - apothem, position_z_max + apothem);
	glEnd();
	glBegin(GL_LINE_LOOP);
		glNormal3f(0.f, 0.f, -1.f);
		glVertex3f(position_x - apothem, position_y - apothem, position_z - apothem);
		glVertex3f(position_x - apothem, position_y + apothem, position_z - apothem);
		glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
		glVertex3f(position_x + apothem, position_y - apothem, position_z - apothem);
	glEnd();
	glBegin(GL_LINE_LOOP);
		glNormal3f(0.f, 1.f, 0.f);
		glVertex3f(position_x - apothem, position_y + apothem, position_z - apothem);
		glVertex3f(position_x - apothem, position_y + apothem, position_z + apothem);
		glVertex3f(position_x + apothem, position_y + apothem, position_z + apothem);
		glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
	glEnd();
	glBegin(GL_LINE_LOOP);
		glNormal3f(0.f, -1.f, 0.f);
		glVertex3f(position_x - apothem, position_y - apothem, position_z - apothem);
		glVertex3f(position_x - apothem, position_y - apothem, position_z + apothem);
		glVertex3f(position_x + apothem, position_y - apothem, position_z + apothem);
		glVertex3f(position_x + apothem, position_y - apothem, position_z - apothem);
	glEnd();
	glBegin(GL_LINE_LOOP);
		glNormal3f(1.f, 0.f, 0.f);
		glVertex3f(position_x + apothem, position_y - apothem, position_z - apothem);
		glVertex3f(position_x + apothem, position_y - apothem, position_z + apothem);
		glVertex3f(position_x + apothem, position_y + apothem, position_z + apothem);
		glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
	glEnd();
	glBegin(GL_LINE_LOOP);
		glNormal3f(-1.f, 0.f, 0.f);
		glVertex3f(position_x - apothem, position_y - apothem, position_z - apothem);
		glVertex3f(position_x - apothem, position_y - apothem, position_z + apothem);
		glVertex3f(position_x - apothem, position_y + apothem, position_z + apothem);
		glVertex3f(position_x - apothem, position_y + apothem, position_z - apothem);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINE_LOOP);
		glNormal3f(0.f, 0.f, 1.f);
		glVertex3f(position_x - apothem, position_y - apothem, position_z_max + apothem);
		glVertex3f(position_x - apothem, position_y + apothem, position_z_max + apothem);
		glVertex3f(position_x + apothem, position_y + apothem, position_z_max + apothem);
		glVertex3f(position_x + apothem, position_y - apothem, position_z_max + apothem);
	glEnd();
/*	glBegin(GL_LINE_LOOP);
		glNormal3f(0.f, 0.f, -1.f);
		glVertex3f(position_x - apothem, position_y - apothem, position_z - apothem);
		glVertex3f(position_x - apothem, position_y + apothem, position_z - apothem);
		glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
		glVertex3f(position_x + apothem, position_y - apothem, position_z - apothem);
	glEnd();*/
/*	glBegin(GL_LINE_LOOP);
		glNormal3f(0.f, 1.f, 0.f);
		glVertex3f(position_x - apothem, position_y + apothem, position_z - apothem);
		glVertex3f(position_x - apothem, position_y + apothem, position_z + apothem);
		glVertex3f(position_x + apothem, position_y + apothem, position_z + apothem);
		glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
	glEnd();*/
	glBegin(GL_LINE_LOOP);
		glNormal3f(0.f, -1.f, 0.f);
		glVertex3f(position_x - apothem, position_y_min - apothem, position_z - apothem);
		glVertex3f(position_x - apothem, position_y_min - apothem, position_z + apothem);
		glVertex3f(position_x + apothem, position_y_min - apothem, position_z + apothem);
		glVertex3f(position_x + apothem, position_y_min - apothem, position_z - apothem);
	glEnd();
/*	glBegin(GL_LINE_LOOP);
		glNormal3f(1.f, 0.f, 0.f);
		glVertex3f(position_x + apothem, position_y - apothem, position_z - apothem);
		glVertex3f(position_x + apothem, position_y - apothem, position_z + apothem);
		glVertex3f(position_x + apothem, position_y + apothem, position_z + apothem);
		glVertex3f(position_x + apothem, position_y + apothem, position_z - apothem);
	glEnd();*/
	glBegin(GL_LINE_LOOP);
		glNormal3f(-1.f, 0.f, 0.f);
		glVertex3f(position_x_min - apothem, position_y - apothem, position_z - apothem);
		glVertex3f(position_x_min - apothem, position_y - apothem, position_z + apothem);
		glVertex3f(position_x_min - apothem, position_y + apothem, position_z + apothem);
		glVertex3f(position_x_min - apothem, position_y + apothem, position_z - apothem);
	glEnd();
	glEnable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);

	glLoadIdentity();
	if(find_princess) {
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		GLfloat x = 2.2f, y = 1.5f, z = -5.0f;
		glBegin(GL_QUADS);
			glNormal3f(-1.f, 0.f, 0.f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(x - apothem, y - apothem, z);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(x + apothem, y - apothem, z);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(x + apothem, y + apothem, z);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(x - apothem, y + apothem, z);
		glEnd();
	}
	if(find_monster) {
		glBindTexture(GL_TEXTURE_2D, texture[3]);
		GLfloat x = 1.2f, y = 1.5f, z = -5.0f;
		glBegin(GL_QUADS);
			glNormal3f(-1.f, 0.f, 0.f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(x - apothem, y - apothem, z);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(x + apothem, y - apothem, z);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(x + apothem, y + apothem, z);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(x - apothem, y + apothem, z);
		glEnd();
		find_monster = 0;
		find_princess = 0;
	}
	if(success) {
		glBindTexture(GL_TEXTURE_2D, texture[4]);
		GLfloat x = 2.2f, y = 0.5f, z = -5.0f;
		glBegin(GL_QUADS);
			glNormal3f(-1.f, 0.f, 0.f);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(x - apothem, y - apothem, z);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(x + apothem, y - apothem, z);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(x + apothem, y + apothem, z);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(x - apothem, y + apothem, z);
		glEnd();
	}
}
