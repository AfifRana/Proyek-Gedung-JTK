#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
//#include "to_display.cpp"

#define FPS 60
#define TO_RADIANS 3.14/180.0

//width and height of the window ( Aspect ratio 16:9 )
const int width = 16*90;
const int height = 9*90;

float pitch = 0.0, yaw= 0.0;
float camX=0.0, camY=0.0, camZ=4.0;

/* Texture */

GLuint texture[3];

struct Image {
	unsigned long sizeX;
	unsigned long sizeY;
	char *data;
};

typedef struct Image Image;

#define checkImageWidth 64
#define checkImageHeight 64

GLubyte checkImage[checkImageWidth][checkImageHeight][3];

// Module
void display();
void reshape(int w,int h);
void timer(int);
void passive_motion(int,int);
void camera();
void keyboard(unsigned char key,int x,int y);
void keyboard_up(unsigned char key,int x,int y);

// texture
Image * loadTexture(const char * filename);
void makeCheckImage(void);
int ImageLoad(const char *filename, Image *image);

//to draw
void make_ledder(int x, int y, int z, int height, int width, bool reverse);
void make_pillar(int x, int y, int z);
void build_pillar(int src_x, int src_y, int src_z, int to_x, int to_y, int to_z);
void make_roof_of_pillar(int src_x, int src_y, int src_z, int to_x, int to_y, int to_z);
void drawBox(int x, int y, int z, int sizeX, int sizeY, int sizeZ, int texFront, int texBack, int texTop, int texBottom, int texLeft, int texRight, int tex_size);

struct Motion
{
    bool Forward,Backward,Left,Right,Up,Down;
};

Motion motion = {false,false,false,false,false,false};

void init()
{
//    glutSetCursor(GLUT_CURSOR_NONE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glutWarpPointer(width/2,height/2);
    
    Image *image1 = loadTexture("texture-data/genteng.bmp");
	Image *image2 = loadTexture("texture-data/coklat.bmp");
	Image *image3 = loadTexture("texture-data/lantai.bmp");
	Image *image4 = loadTexture("texture-data/paving-block.bmp");
	Image *image5 = loadTexture("texture-data/rumput.bmp");
	Image *image6 = loadTexture("texture-data/semen.bmp");
	
	if(image1 == NULL || image2 == NULL || image3 == NULL || image4 == NULL || image5 == NULL || image6 == NULL)
	{
        printf("Image was not returned from loadTexture\n");
        exit(0);
    }
    
    makeCheckImage();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Create Texture
    glGenTextures(8, texture);
    
    //Load Texture 1
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image1->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    //Load Texture 2
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image2->sizeX, image2->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image2->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    //Load Texture 3
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image3->sizeX, image3->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image3->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    //Load Texture 4
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image4->sizeX, image4->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image4->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    //Load Texture 5
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image5->sizeX, image5->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image5->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    //Load Texture 6
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image6->sizeX, image6->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image6->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, checkImageWidth,
    checkImageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,&checkImage[0][0][0]);
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_FLAT);
}

int main(int argc,char**argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Projectile Motion - 3D Simulation");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutPassiveMotionFunc(passive_motion);
    glutTimerFunc(0,timer,0);    //more info about this is given below at definition of timer()
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboard_up);

    glutMainLoop();
    return 0;
}

/* Texture Module */
void makeCheckImage(void) {
	int i, j, c;
	for(i=0; i < checkImageWidth; i++) {
		for(j=0; j < checkImageHeight; j++) {
			c = (((i&0x8)==0)^((j&0x8)==0))*255;
			checkImage[i][j][0] = (GLubyte) c;
			checkImage[i][j][1] = (GLubyte) c;
			checkImage[i][j][2] = (GLubyte) c;
		}
	}
}

int ImageLoad(const char *filename, Image *image) {
	FILE *file;
	unsigned long size; // size of image in bytes
	unsigned long i; // standard counter
	unsigned short int planes; // number of planes in image (must be 1)
	unsigned short int bpp; // number of bits per pixel (must be 24)
	char temp; // temporary color storage for bgr-rgb conversion
	
	// make sure the file is there
	if ((file = fopen(filename, "rb"))==NULL) {
		printf ("File not found : %s\n", filename);
		return 0;
	}
	
	// seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);

    // read the width
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) 
	{
        printf("Error reading width from %s.\n", filename);
        return 0;
    }

    //printf("Width of %s: %lu\n", filename, image->sizeX);
    // read the height
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) 
	{
        printf("Error reading height from %s.\n", filename);
        return 0;
    }

    //printf("Height of %s: %lu\n", filename, image->sizeY);
    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;

    // read the planes
    if ((fread(&planes, 2, 1, file)) != 1) 
	{
        printf("Error reading planes from %s.\n", filename);
        return 0;
    }

    if (planes != 1) 
	{
        printf("Planes from %s is not 1: %u\n", filename, planes);
        return 0;
    }

    // read the bitsperpixel
    if ((i = fread(&bpp, 2, 1, file)) != 1) 
	{
        printf("Error reading bpp from %s.\n", filename);
        return 0;
    }

    if (bpp != 24) 
	{
        printf("Bpp from %s is not 24: %u\n", filename, bpp);
        return 0;
    }

    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);

    // read the data.
    image->data = (char *) malloc(size);
    if (image->data == NULL) 
	{
        printf("Error allocating memory for color-corrected image data");
        return 0;
    }

    if ((i = fread(image->data, size, 1, file)) != 1) 
	{
        printf("Error reading image data from %s.\n", filename);
        return 0;
    }

    for (i=0;i<size;i+=3) 
	{ // reverse all of the colors. (bgr -> rgb)
        temp = image->data[i];
        image->data[i] = image->data[i+2];
        image->data[i+2] = temp;
    }

    // we're done.
    return 1;
}

Image * loadTexture(const char * filename)
{
    Image *image1;
    // allocate space for texture
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL)
	{
        printf("Error allocating space for image");
        exit(0);
    }

    if (!ImageLoad(filename, image1)) 
	{
        exit(1);
    }
    return image1;
}

/* This function just draws the scene. I used Texture mapping to draw
   a chessboard like surface. If this is too complicated for you ,
   you can just use a simple quadrilateral */

void draw()
{
	glutSolidTeapot(1);
//    glEnable(GL_TEXTURE_2D);
//    GLuint texture;
//    glGenTextures(1,&texture);
	
//	drawBox(0,0,0,1,1,1,1,2,3,4,5,6,1);
	
//    unsigned char texture_data[2][2][4] =
//                    {
//                        100,0,0,255,  220,87,21,255,
//                        220,87,21,255,    100,0,0,255
//                    };

//    glBindTexture(GL_TEXTURE_2D,texture);
//    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,2,2,0,GL_RGBA,GL_UNSIGNED_BYTE,texture_data);
//
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
//                    GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
//                    GL_NEAREST);

//    glBegin(GL_QUADS);
//
//    glTexCoord2f(0.0,0.0);  glVertex3f(-100.0,-5.0,-100.0);
//    glTexCoord2f(50.0,0.0);  glVertex3f(100.0,-5.0,-100.0);
//    glTexCoord2f(50.0,50.0);  glVertex3f(100.0,-5.0,100.0);
//    glTexCoord2f(0.0,50.0);  glVertex3f(-100.0,-5.0,100.0);
//
//    glEnd();
    
    
    
//    glBegin(GL_QUADS);
//
//    glTexCoord2f(0.0,0.0);  glVertex3f(-100.0,5.0,-50.0);
//    glTexCoord2f(25.0,0.0);  glVertex3f(50.0,5.0,-50.0);
//    glTexCoord2f(25.0,25.0);  glVertex3f(50.0,5.0,50.0);
//    glTexCoord2f(0.0,25.0);  glVertex3f(-100.0,5.0,50.0);
//
//    glEnd();
    
	
//    glDisable(GL_TEXTURE_2D);
//	glBegin(GL_TRIANGLES);           // Begin drawing the pyramid with 4 triangles
//      // Front
//      glColor3f(1.0f, 0.0f, 0.0f);     // Red
//      glVertex3f( 0.0f, 1.0f, 0.0f);
//      glColor3f(0.0f, 1.0f, 0.0f);     // Green
//      glVertex3f(-1.0f, -1.0f, 1.0f);
//      glColor3f(0.0f, 0.0f, 1.0f);     // Blue
//      glVertex3f(1.0f, -1.0f, 1.0f);
// 
//      // Right
//      glColor3f(1.0f, 0.0f, 0.0f);     // Red
//      glVertex3f(0.0f, 1.0f, 0.0f);
//      glColor3f(0.0f, 0.0f, 1.0f);     // Blue
//      glVertex3f(1.0f, -1.0f, 1.0f);
//      glColor3f(0.0f, 1.0f, 0.0f);     // Green
//      glVertex3f(1.0f, -1.0f, -1.0f);
// 
//      // Back
//      glColor3f(1.0f, 0.0f, 0.0f);     // Red
//      glVertex3f(0.0f, 1.0f, 0.0f);
//      glColor3f(0.0f, 1.0f, 0.0f);     // Green
//      glVertex3f(1.0f, -1.0f, -1.0f);
//      glColor3f(0.0f, 0.0f, 1.0f);     // Blue
//      glVertex3f(-1.0f, -1.0f, -1.0f);
// 
//      // Left
//      glColor3f(1.0f,0.0f,0.0f);       // Red
//      glVertex3f( 0.0f, 1.0f, 0.0f);
//      glColor3f(0.0f,0.0f,1.0f);       // Blue
//      glVertex3f(-1.0f,-1.0f,-1.0f);
//      glColor3f(0.0f,1.0f,0.0f);       // Green
//      glVertex3f(-1.0f,-1.0f, 1.0f);
//   glEnd();   // Done drawing the pyramid    
//	
//	glPushMatrix();
//	glColor3f(1, 0, 0);
//    glTranslatef(10, 0, 0);
//	glutSolidTetrahedron();
//	glPopMatrix();
//
//	glPushMatrix();
//	glColor3f(0, 0, 1);
////	glLoadIdentity();
//	glTranslatef(0, 10, 0);
//	glutSolidTetrahedron();
//	glPopMatrix();
//	
//	//buat tangga 1, 2, 3, 4
//	make_ledder(50,-5,60, 11,5, false);
//	make_ledder(10,-5,60, 11,5, false);
//	make_ledder(-5,-5,60, 11,5, false);
//	make_ledder(-45,-5,60, 11,5, false);
//	
//	//buat pasang deret tiang yg paling dekat dengan tangga
//	build_pillar(50,5,50, -70,5,50);
//	build_pillar(50,5,40, -70,5,40);
//	
//	//tiang ke ged listrik
//	build_pillar(50,5,30, 50,5,-10);
//	build_pillar(40,5,30, 50,5,-10);
//	
//	//tiang ke pintu utama
//	build_pillar(-10,5,30, -10,5,20);
//	build_pillar(-20,5,30, -20,5,20);
//	
//	//tiang ke pintu belakang
//	build_pillar(-60,5,30, -60,5,20);
//	build_pillar(-70,5,30, -70,5,20);
//	
//	//buat atap tiang
//	make_roof_of_pillar(50,14,50, -70,14,40); //front
//	make_roof_of_pillar(-70,14,38, -60,14,20); //ke pintu belakang 
//	make_roof_of_pillar(40,14,38, 50,14,-10); // ke ged listrik
//	make_roof_of_pillar(-20,14,38, -10,14,10); //ke pintu utama
//
//	//buat tangga ke lt 2
//	make_ledder(-7,5,25, 11,3, true);
//	make_ledder(-4,16,32, 5,3, false);
////	glColor3f(1, 1, 1); //color for tile
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    camera();
    draw();

    glutSwapBuffers();
}

void reshape(int w,int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,16.0/9.0,1,75);
    glMatrixMode(GL_MODELVIEW);

}


/*this funtion is used to keep calling the display function periodically
  at a rate of FPS times in one second. The constant FPS is defined above and
  has the value of 60
*/
void timer(int)
{
    glutPostRedisplay();
    glutWarpPointer(width/2,height/2);
    glutTimerFunc(1000/FPS,timer,0);
}

void passive_motion(int x,int y)
{
    /* two variables to store X and Y coordinates, as observed from the center
      of the window
    */
    int dev_x,dev_y;
    dev_x = (width/2)-x;
    dev_y = (height/2)-y;

    /* apply the changes to pitch and yaw*/
    yaw+=(float)dev_x/10.0;
    pitch+=(float)dev_y/10.0;
}

void camera()
{
    if(motion.Forward)
    {
        camX += cos((yaw+90)*TO_RADIANS)/5.0;
        camZ -= sin((yaw+90)*TO_RADIANS)/5.0;
    }
    if(motion.Backward)
    {
        camX += cos((yaw+90+180)*TO_RADIANS)/5.0;
        camZ -= sin((yaw+90+180)*TO_RADIANS)/5.0;
    }
    if(motion.Left)
    {
        camX += cos((yaw+90+90)*TO_RADIANS)/5.0;
        camZ -= sin((yaw+90+90)*TO_RADIANS)/5.0;
    }
    if(motion.Right)
    {
        camX += cos((yaw+90-90)*TO_RADIANS)/5.0;
        camZ -= sin((yaw+90-90)*TO_RADIANS)/5.0;
    }
    if(motion.Up)
    {
    	camY += 0.1;
	}
	if(motion.Down)
	{
		camY -= 0.1;
	}

    /*limit the values of pitch
      between -60 and 70
    */
    if(pitch>=70)
        pitch = 70;
    if(pitch<=-60)
        pitch=-60;

    glRotatef(-pitch,1.0,0.0,0.0); // Along X axis
    glRotatef(-yaw,0.0,1.0,0.0);    //Along Y axis

    glTranslatef(-camX,-camY,-camZ);
}

void keyboard(unsigned char key,int x,int y)
{
    switch(key)
    {
    case 'W':
    case 'w':
        motion.Forward = true;
        break;
    case 'A':
    case 'a':
        motion.Left = true;
        break;
    case 'S':
    case 's':
        motion.Backward = true;
        break;
    case 'D':
    case 'd':
        motion.Right = true;
        break;
    case 'f':
    case 'F':
    	motion.Up = true;
    	break;
    case 'v':
    case 'V':
    	motion.Down = true;
    	break;
	}
}
void keyboard_up(unsigned char key,int x,int y)
{
    switch(key)
    {
    case 'W':
    case 'w':
        motion.Forward = false;
        break;
    case 'A':
    case 'a':
        motion.Left = false;
        break;
    case 'S':
    case 's':
        motion.Backward = false;
        break;
    case 'D':
    case 'd':
        motion.Right = false;
        break;
    case 'F':
    case 'f':
        motion.Up = false;
        break;
    case 'V':
    case 'v':
        motion.Down = false;
        break;
	}
}

void make_ledder(int x, int y, int z, int height, int width, bool reverse){
	
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			glPushMatrix();
			glColor3f(1, 1, 1);
			glTranslatef(x-j, y+i, z-i);
			glutWireCube(1);
			glPopMatrix();
		}
		if(reverse) z+= 2;
	}	
}

void make_pillar(int x, int y, int z){
	for(int i = 0; i < 9; i++){
		glPushMatrix();
		glColor3f(1, 1, 1);
		glTranslatef(x, y+i, z);
		glutSolidCube(1);
		glPopMatrix();
	}
}

void build_pillar(int src_x, int src_y, int src_z, int to_x, int to_y, int to_z){
	
	int x1 = src_x < to_x ? src_x : to_x; //min
	int x2 = src_x > to_x ? src_x : to_x; //max
	
	for(;x1 <= x2; x1 += 10){
		make_pillar(x1, src_y, src_z);
	}
	
	int z1 = src_z < to_z ? src_z : to_z;
	int z2 = src_z > to_z ? src_z : to_z;
	
	for(;z1 <= z2; z1 += 10){
		make_pillar(src_x, src_y, z1);
	}
}

void make_roof_of_pillar(int src_x, int src_y, int src_z, int to_x, int to_y, int to_z){
	
	int x1 = src_x < to_x ? src_x : to_x; //min
	int x2 = src_x > to_x ? src_x : to_x; //max
	int z1 = src_z < to_z ? src_z : to_z;
	int z2 = src_z > to_z ? src_z : to_z;
	int temp = z1;
	
	for(;x1<=x2; x1+=2){
		for(z1=temp; z1<=z2; z1+=2){
			glPushMatrix();
			glColor3f(42, 0, 0); //brown
			glTranslatef(x1, src_y, z1);
			glutWireCube(2);
			glPopMatrix();
//			printf("x1 : %d\nz1 : %d\n\n", x1, z1);
		}
	}
}

void drawBox(int x, int y, int z, int sizeX, int sizeY, int sizeZ, int texFront, int texBack, int texTop, int texBottom, int texLeft, int texRight, int texSize) {
	glPushMatrix();
	glTranslatef(x, y, z);
	
	// Front
	glBindTexture(GL_TEXTURE_2D, texture[texFront]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(sizeX/2 , -sizeY/2 , sizeZ/2);
		glTexCoord2f(0, texSize);
		glVertex3f(-sizeX/2 , -sizeY/2 , sizeZ/2);
		glTexCoord2f(texSize, 0);
		glVertex3f(-sizeX/2 , sizeY/2 , sizeZ/2);
		glTexCoord2f(texSize, texSize);
		glVertex3f(sizeX/2 , sizeY/2 , sizeZ/2);
	glEnd();
	
	// Back
	glBindTexture(GL_TEXTURE_2D, texture[texBack]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(sizeX/2 , -sizeY/2 , -sizeZ/2);
		glTexCoord2f(0, texSize);
		glVertex3f(-sizeX/2 , -sizeY/2 , -sizeZ/2);
		glTexCoord2f(texSize, 0);
		glVertex3f(-sizeX/2 , sizeY/2 , -sizeZ/2);
		glTexCoord2f(texSize, texSize);
		glVertex3f(sizeX/2 , sizeY/2 , -sizeZ/2);
	glEnd();
	
	// Top
	glBindTexture(GL_TEXTURE_2D, texture[texTop]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(sizeX/2 , sizeY/2 , sizeZ/2);
		glTexCoord2f(0, texSize);
		glVertex3f(-sizeX/2 , sizeY/2 , sizeZ/2);
		glTexCoord2f(texSize, 0);
		glVertex3f(-sizeX/2 , sizeY/2 , -sizeZ/2);
		glTexCoord2f(texSize, texSize);
		glVertex3f(sizeX/2 , sizeY/2 , -sizeZ/2);
	glEnd();
	
	// Bottom
	glBindTexture(GL_TEXTURE_2D, texture[texBottom]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(sizeX/2 , -sizeY/2 , sizeZ/2);
		glTexCoord2f(0, texSize);
		glVertex3f(-sizeX/2 , -sizeY/2 , sizeZ/2);
		glTexCoord2f(texSize, 0);
		glVertex3f(-sizeX/2 , -sizeY/2 , -sizeZ/2);
		glTexCoord2f(texSize, texSize);
		glVertex3f(sizeX/2 , -sizeY/2 , -sizeZ/2);
	glEnd();
	
	// Left
	glBindTexture(GL_TEXTURE_2D, texture[texLeft]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(-sizeX/2 , -sizeY/2 , sizeZ/2);
		glTexCoord2f(0, texSize);
		glVertex3f(-sizeX/2 , -sizeY/2 , -sizeZ/2);
		glTexCoord2f(texSize, 0);
		glVertex3f(-sizeX/2 , sizeY/2 , -sizeZ/2);
		glTexCoord2f(texSize, texSize);
		glVertex3f(-sizeX/2 , sizeY/2 , sizeZ/2);
	glEnd();
	
	// Right
	glBindTexture(GL_TEXTURE_2D, texture[texRight]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(sizeX/2 , -sizeY/2 , sizeZ/2);
		glTexCoord2f(0, texSize);
		glVertex3f(sizeX/2 , -sizeY/2 , -sizeZ/2);
		glTexCoord2f(texSize, 0);
		glVertex3f(sizeX/2 , sizeY/2 , -sizeZ/2);
		glTexCoord2f(texSize, texSize);
		glVertex3f(sizeX/2 , sizeY/2 , sizeZ/2);
	glEnd();
	glPopMatrix();
}
