#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <stdio.h>
#include <math.h>
//#include "to_display.cpp"

#define FPS 160
#define TO_RADIANS 3.14/180.0

#define GENTENG 1
#define BROWN 2
#define KERAMIK 3
#define PAVING_BLOCK 4
#define RUMPUT 5
#define SEMEN 6

//width and height of the window ( Aspect ratio 16:9 )
const int width = 16*90;
const int height = 9*90;

float pitch = -30.0, yaw= 0.0;
float camX=20.0, camY=10.0, camZ=10.0;

GLuint texture[9];

struct Image 
{
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};

typedef struct Image Image;

float floorY;
float wallY;
float pillarY;

#define checkImageWidth 64
#define checkImageHeight 64

GLubyte checkImage[checkImageWidth][checkImageHeight][3];

struct Motion
{
    bool Forward,Backward,Left,Right,Up,Down;
};

Motion motion = {false,false,false,false,false,false};

// Module declaration
void display();
void reshape(int w,int h);
void timer(int);
void passive_motion(int,int);
void camera();
void keyboard(unsigned char key,int x,int y);
void keyboard_up(unsigned char key,int x,int y);

//image and texture
void makeCheckImage(void);
int ImageLoad(const char *filename, Image *image);
Image * loadTexture(const char * filename);

//to draw
void draw_box(float x, float y, float z, float x_size, float y_size, float z_size, int id_front, int id_back, int id_left, int id_right, int id_top, int id_bottom, int texSize);
void make_ledder(int x, int y, int z, int height, int width, bool reverse);
void make_pillar(float x, float y, float z);
void make_main_pillar(float x, float y, float z);
void make_roof_of_pillar(int src_x, int src_y, int src_z, int to_x, int to_y, int to_z);

void build_pillar(int src_x, int src_y, int src_z, int to_x, int to_y, int to_z);
void build_main_pillar(float src_x, float src_y, float src_z, float to_x, float to_y, float to_z, float jarak);
void build_paving_block(int src_x, int src_y, int src_z, int to_x, int to_y, int to_z);

// Afif's Module
void drawBox(float x, float y, float z, float sizeX, float sizeY, float sizeZ, int texFront, int texBack, int texTop, int texBottom, int texLeft, int texRight, int texSize);
void drawBox2(float x, float y, float z, float sizeX, float sizeY, float sizeZ, int texFront, int texBack, int texTop, int texBottom, int texLeft, int texRight, int texSize);
void drawPrism(float x, float y, float z, float bottomX, float bottomZ, float topX, float topZ, float high, int texFront, int texBack, int texTop, int texBottom, int texLeft, int texRight, int texSize);
void buildPillarX(int numOfPillar, float startX, float poinY, float poinZ, float difference, float sizeX, float sizeY, float sizeZ, int texId, int texSize);
void buildPillarZ(int numOfPillar, float poinX, float poinY, float startZ, float difference, float sizeX, float sizeY, float sizeZ, int texId, int texSize);
void buildFloor(int numOfFloor, float startY, float poinX, float poinZ, float difference, float sizeX, float sizeY, float sizeZ, int texId, int texSize);
void putWindows(int numOfWindows, float startX, float poinY, float poinZ, float sizeX, float sizeY, int texId, int texSize);

void init()
{	
    glutSetCursor(GLUT_CURSOR_NONE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
	glutWarpPointer(width/2,height/2);

	Image *image1 = loadTexture("texture-data/genteng.bmp");
	Image *image2 = loadTexture("texture-data/coklat.bmp");
	Image *image3 = loadTexture("texture-data/lantai.bmp");
	Image *image4 = loadTexture("texture-data/paving-block.bmp");
	Image *image5 = loadTexture("texture-data/Grass.bmp");
	Image *image6 = loadTexture("texture-data/semen.bmp");
	Image *image7 = loadTexture("texture-data/skybox2/posz.bmp"); // front
	Image *image8 = loadTexture("texture-data/skybox2/negz.bmp"); // back
	Image *image9 = loadTexture("texture-data/skybox2/posy.bmp"); // top
	Image *image10 = loadTexture("texture-data/skybox2/negy.bmp"); // bottom
	Image *image11 = loadTexture("texture-data/skybox2/posx.bmp"); // left
	Image *image12 = loadTexture("texture-data/skybox2/negx.bmp"); // right
	Image *image13 = loadTexture("texture-data/window.bmp");
	Image *image14 = loadTexture("texture-data/Pintu JTK.bmp");
	 
    if(image1 == NULL || image2 == NULL || image3 == NULL || image4 == NULL || image5 == NULL || image6 == NULL)
	{
        printf("Image was not returned from loadTexture\n");
        exit(0);
    }

    makeCheckImage();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Create Texture
    glGenTextures(14, texture);
    
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
    
    //Load Texture 7
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image7->sizeX, image7->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image7->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    //Load Texture 8
    glBindTexture(GL_TEXTURE_2D, texture[8]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image8->sizeX, image8->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image8->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    //Load Texture 9
    glBindTexture(GL_TEXTURE_2D, texture[9]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image9->sizeX, image9->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image9->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    //Load Texture 10
    glBindTexture(GL_TEXTURE_2D, texture[10]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image10->sizeX, image10->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image10->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    //Load Texture 11
    glBindTexture(GL_TEXTURE_2D, texture[11]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image11->sizeX, image11->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image11->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    //Load Texture 12
    glBindTexture(GL_TEXTURE_2D, texture[12]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image12->sizeX, image12->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image12->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    //Load Texture 13
    glBindTexture(GL_TEXTURE_2D, texture[13]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image13->sizeX, image13->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image13->data);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    //Load Texture 14
    glBindTexture(GL_TEXTURE_2D, texture[14]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); //scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image14->sizeX, image14->sizeY, 0,
    GL_RGB, GL_UNSIGNED_BYTE, image14->data);
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

/* Pengolahan Gambar dan Texture */
void makeCheckImage(void)
{
    int i, j, c;
    for (i = 0; i < checkImageWidth; i++) 
	{
        for (j = 0; j < checkImageHeight; j++) 
		{
            c = ((((i&0x8)==0)^((j&0x8)==0)))*255;
            checkImage[i][j][0] = (GLubyte) c;
            checkImage[i][j][1] = (GLubyte) c;
            checkImage[i][j][2] = (GLubyte) c;
        }
    }
}

int ImageLoad(const char *filename, Image *image) 
{
    FILE *file;
    unsigned long size; // size of the image in bytes.
    unsigned long i; // standard counter.
    unsigned short int planes; // number of planes in image (must be 1)
    unsigned short int bpp; // number of bits per pixel (must be 24)
    char temp; // temporary color storage for bgr-rgb conversion.

    // make sure the file is there.
    if ((file = fopen(filename, "rb"))==NULL)
	{
        printf("File Not Found : %s\n",filename);
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

/* Menggambar */
void draw()
{
//	draw_box(0,0,0, 200,200,200, 5,5,5,5,5,5);
	int i = 0, startX = -38;
	
	// Skybox
	drawBox2(0, 0, 0, 500, 500, 500, 7, 8, 9, 10, 11, 12, 1);
	drawBox2(0, -0.4, 0, 300, 0.2, 300, 10, 10, 10, 10, 10, 10, 10);
	
	glScalef(1,1,1);
	
	/* lantai JTK */
	buildFloor(1, -0.3, 0, -5.5, 2, 34, 0.3, 11, 3, 100); // panjang 34, lebar 11
	
	/* Atap JTK */
//	drawPrism(0, 5.55, -5.5, 36, 13, 28, 5, 2.5, 1, 1, 1, 1, 1, 1, 1);
//	drawPrism(0, 7.5, -5.5, 28, 5, 33, 0, 2, 1, 1, 1, 1, 1, 1, 1);

	/* Tembok lt. 1 */
	// Tembok penutup depan belakang
	drawBox(0, 1, -0.1, 33.6, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
	drawBox(0, 1, -10.9, 33.6, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
	
	// Tembok penutup ujung barat dan timur
	drawBox(-16.9, 1, -5.5, 0.2, 2, 11, 6, 6, 6, 6, 6, 6, 1);
	drawBox(16.9, 1, -5.5, 0.2, 2, 11, 6, 6, 6, 6, 6, 6, 1);
	
	// Tembok pemisah lab proyek barat dan timur
	drawBox(14.35, 1, -5.5, 4.9, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
	drawBox(-14.35, 1, -5.5, 4.9, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
	
	// Tembok penutup lab proyek
	drawBox(-11.8, 1, -5.5, 0.2, 2, 10.6, 6, 6, 6, 6, 6, 6, 1);
	drawBox(11.8, 1, -5.5, 0.2, 2, 10.6, 6, 6, 6, 6, 6, 6, 1);
	
	/* Tiang Lab Proyek */
	// Bagian gedung (barat)
	buildPillarX(6, -17, 2.15, 0.1, 0.82, 0.2, 4.3, 0.2, 2, 1);
	// kompelemen belakang
	buildPillarX(6, -17, 2.15, -11.1, 0.82, 0.2, 4.3, 0.2, 2, 1);
	
	/* Jendela Lab Proyek */
	// Barat
	putWindows(3, -16.77, 1, 0.05, 0.25, 1, 13, 1);
	putWindows(3, -15.75, 1, 0.05, 0.25, 1, 13, 1);
	putWindows(3, -14.73, 1, 0.05, 0.25, 1, 13, 1);
	putWindows(3, -13.73, 1, 0.05, 0.25, 1, 13, 1);
	putWindows(3, -12.71, 1, 0.05, 0.25, 1, 13, 1);
	//duplikat atas
	putWindows(3, -16.77, 3.3, 0.05, 0.25, 1, 13, 1);
	putWindows(3, -15.75, 3.3, 0.05, 0.25, 1, 13, 1);
	putWindows(3, -14.73, 3.3, 0.05, 0.25, 1, 13, 1);
	putWindows(3, -13.73, 3.3, 0.05, 0.25, 1, 13, 1);
	putWindows(3, -12.71, 3.3, 0.05, 0.25, 1, 13, 1);
	// Komplemen Bawah
	putWindows(3, -16.77, 1, -11.05, 0.25, 1, 13, 1);
	putWindows(3, -15.75, 1, -11.05, 0.25, 1, 13, 1);
	putWindows(3, -14.73, 1, -11.05, 0.25, 1, 13, 1);
	putWindows(3, -13.73, 1, -11.05, 0.25, 1, 13, 1);
	putWindows(3, -12.71, 1, -11.05, 0.25, 1, 13, 1);
	// Komplemen Atas
	putWindows(3, -16.77, 3.3, -11.05, 0.25, 1, 13, 1);
	putWindows(3, -15.75, 3.3, -11.05, 0.25, 1, 13, 1);
	putWindows(3, -14.73, 3.3, -11.05, 0.25, 1, 13, 1);
	putWindows(3, -13.73, 3.3, -11.05, 0.25, 1, 13, 1);
	putWindows(3, -12.71, 3.3, -11.05, 0.25, 1, 13, 1);
	
	// Timur
	putWindows(3, -(-16.77), 1, 0.05, -0.25, 1, 13, 1);
	putWindows(3, -(-15.75), 1, 0.05, -0.25, 1, 13, 1);
	putWindows(3, -(-14.73), 1, 0.05, -0.25, 1, 13, 1);
	putWindows(3, -(-13.73), 1, 0.05, -0.25, 1, 13, 1);
	putWindows(3, -(-12.71), 1, 0.05, -0.25, 1, 13, 1);
	// duplikat atas
	putWindows(3, -(-16.77), 3.3, 0.05, -0.25, 1, 13, 1);
	putWindows(3, -(-15.75), 3.3, 0.05, -0.25, 1, 13, 1);
	putWindows(3, -(-14.73), 3.3, 0.05, -0.25, 1, 13, 1);
	putWindows(3, -(-13.73), 3.3, 0.05, -0.25, 1, 13, 1);
	putWindows(3, -(-12.71), 3.3, 0.05, -0.25, 1, 13, 1);
	// Komplemen Bawah
	putWindows(3, -(-16.77), 1, -11.05, -0.25, 1, 13, 1);
	putWindows(3, -(-15.75), 1, -11.05, -0.25, 1, 13, 1);
	putWindows(3, -(-14.73), 1, -11.05, -0.25, 1, 13, 1);
	putWindows(3, -(-13.73), 1, -11.05, -0.25, 1, 13, 1);
	putWindows(3, -(-12.71), 1, -11.05, -0.25, 1, 13, 1);
	// Komplemen Atas
	putWindows(3, -(-16.77), 3.3, -11.05, -0.25, 1, 13, 1);
	putWindows(3, -(-15.75), 3.3, -11.05, -0.25, 1, 13, 1);
	putWindows(3, -(-14.73), 3.3, -11.05, -0.25, 1, 13, 1);
	putWindows(3, -(-13.73), 3.3, -11.05, -0.25, 1, 13, 1);
	putWindows(3, -(-12.71), 3.3, -11.05, -0.25, 1, 13, 1);
	
	// Bagian depan gedung (timur)
	buildPillarX(6, 17, 2.15, 0.1, -0.82, -0.2, 4.3, 0.2, 2, 1);
	// komplemen belakang
	buildPillarX(6, 17, 2.15, -11.1, -0.82, -0.2, 4.3, 0.2, 2, 1);
	
	// Ruangan Manajer
	drawBox(-10.8, 1, -10.15, 0.2, 2, 1.3, 6, 6, 6, 6, 6, 6, 1);
	drawBox(-9.95, 1, -9.4, 3.5, 2, 0.2, 6, 6, 6, 6, 6,6 , 1);
	
	/* Jendela Gudang deket Lab Proyek */
	putWindows(3, -11.6, 1, 0.05, 0.25, 1, 13, 1);
	putWindows(3, -10.75, 1, 0.05, 0.25, 1, 13, 1);
	// duplikat atas
	putWindows(3, -11.6, 3.3, 0.05, 0.25, 1, 13, 1);
	putWindows(3, -10.75, 3.3, 0.05, 0.25, 1, 13, 1);
	// Komplemen Bawah
	putWindows(3, -11.6, 1, -11.05, 0.25, 1, 13, 1);
	putWindows(3, -10.75, 1, -11.05, 0.25, 1, 13, 1);
	// Komplemen Atas
	putWindows(3, -11.6, 3.3, -11.05, 0.25, 1, 13, 1);
	putWindows(3, -10.75, 3.3, -11.05, 0.25, 1, 13, 1);
	
	// Gudang deket lab proyek barat
	drawBox(-10.7, 1, -3.5, 2, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
	drawBox(-9.8, 1, -1.8, 0.2, 2, 3.2, 6, 6, 6, 6, 6, 6, 1);
	
	/* Tiang Gudang Deket Lab Proyek */
	buildPillarX(1, -9.9, 2.15, 0.1, 0, 0.2, 4.3, 0.2, 4, 1);
	// komplemen belakang
	buildPillarX(1, -9.9, 2.15, -11.1, 0, 0.2, 4.3, 0.2, 4, 1);
	
	/* Pintu */
	drawBox2(-8.95, 0.75, 0.05, 1.5, 1.5, 0.1, 14, 14, 14, 14, 14, 14, 1); // -9,7 - -8,2
	
	// Tembok lorong lt 1
	drawBox(-0.5, 1, -4.5, 15.4, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
	drawBox(-0.5, 1, -6.5, 15.4, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
	
	// Tembok penutup deretan kelas bagian barat
	drawBox(-8.1, 1, -2.3, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
	drawBox(-8.1, 1, -8.7, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
	
	/* Tiang Kelas no 1 */
	buildPillarX(4, -8.2, 2.15, 0.1, 1.13, 0.2, 4.3, 0.2, 2, 1);
	// komplemen belakang
	buildPillarX(4, -8.2, 2.15, -11.1, 1.13, 0.2, 4.3, 0.2, 2, 1);
	
	/* Jendela Ruang Kelas No 1 */
	putWindows(3, -7.81, 1, 0.05, 0.25, 1, 13, 1);
	putWindows(3, -6.48, 1, 0.05, 0.25, 1, 13, 1); // 6,67 mulai
	putWindows(3, -5.15, 1, 0.05, 0.25, 1, 13, 1); // 5,34
	// duplikat atas
	putWindows(3, -7.81, 3.3, 0.05, 0.25, 1, 13, 1);
	putWindows(3, -6.48, 3.3, 0.05, 0.25, 1, 13, 1); // 6,67 mulai
	putWindows(3, -5.15, 3.3, 0.05, 0.25, 1, 13, 1); // 5,34
	// komplemen bawah
	putWindows(3, -7.81, 1, -11.05, 0.25, 1, 13, 1);
	putWindows(3, -6.48, 1, -11.05, 0.25, 1, 13, 1); // 6,67 mulai
	putWindows(3, -5.15, 1, -11.05, 0.25, 1, 13, 1); // 5,34
	// komplemen atas
	putWindows(3, -7.81, 3.3, -11.05, 0.25, 1, 13, 1);
	putWindows(3, -6.48, 3.3, -11.05, 0.25, 1, 13, 1); // 6,67 mulai
	putWindows(3, -5.15, 3.3, -11.05, 0.25, 1, 13, 1); // 5,34
	
	// Kelas 1 timur
	drawBox(-4.1, 1, -2.3, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
	drawBox(-4.1, 1, -8.7, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
	
	/* Jendela Ruang 2 */
	putWindows(3, -3.9, 1, 0.05, 0.25, 1, 13, 1);
	putWindows(3, -3.05, 1, 0.05, 0.25, 1, 13, 1);
	// duplikat atas
	putWindows(3, -3.9, 3.3, 0.05, 0.25, 1, 13, 1);
	putWindows(3, -3.05, 3.3, 0.05, 0.25, 1, 13, 1);
	// komplemen bawah
	putWindows(3, -3.9, 1, -11.05, 0.25, 1, 13, 1);
	putWindows(3, -3.05, 1, -11.05, 0.25, 1, 13, 1);
	// komplemen atas
	putWindows(3, -3.9, 3.3, -11.05, 0.25, 1, 13, 1);
	putWindows(3, -3.05, 3.3, -11.05, 0.25, 1, 13, 1);
	
	// Ruang 2 timur
	drawBox(-2.1, 1, -2.3, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
	drawBox(-2.1, 1, -8.7, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
	
	/* Tiang Kelas no 2 */
	buildPillarX(4, -2.2, 2.15, 0.1, 1.2, 0.2, 4.3, 0.2, 2, 1);
	// komplemen belakang
	buildPillarX(4, -2.2, 2.15, -11.1, 1.2, 0.2, 4.3, 0.2, 2, 1);
	
	/* Jendela Ruang Kelas No 2 */
	putWindows(4, -1.9, 1, 0.05, 0.25, 1, 13, 1);
	putWindows(4, -0.5, 1, 0.05, 0.25, 1, 13, 1); // 6,67 mulai
	putWindows(4, 0.9, 1, 0.05, 0.25, 1, 13, 1); // 5,34
	// duplikat atas
	putWindows(4, -1.9, 3.3, 0.05, 0.25, 1, 13, 1);
	putWindows(4, -0.5, 3.3, 0.05, 0.25, 1, 13, 1); // 6,67 mulai
	putWindows(4, 0.9, 3.3, 0.05, 0.25, 1, 13, 1); // 5,34
	// komplemen bawah
	putWindows(4, -1.9, 1, -11.05, 0.25, 1, 13, 1);
	putWindows(4, -0.5, 1, -11.05, 0.25, 1, 13, 1); // 6,67 mulai
	putWindows(4, 0.9, 1, -11.05, 0.25, 1, 13, 1); // 5,34
	// komplemen atas
	putWindows(4, -1.9, 3.3, -11.05, 0.25, 1, 13, 1);
	putWindows(4, -0.5, 3.3, -11.05, 0.25, 1, 13, 1); // 6,67 mulai
	putWindows(4, 0.9, 3.3, -11.05, 0.25, 1, 13, 1); // 5,34
	
	// Kelas 2 timur
	drawBox(2.1, 1, -2.3, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
	drawBox(2.1, 1, -8.7, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
	
	// Kelas 3 (deket wc) timur
	drawBox(6.1, 1, -8.7, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
	
	// Lorong wc
	drawBox(6.7, 1, -8.5, 1, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
	drawBox(6.7, 1, -9, 1, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
	
	// Tembok wc timur
	drawBox(7.1, 1, -7.5, 0.2, 2, 1.8, 6, 6, 6, 6, 6, 6, 1);
	drawBox(7.1, 1, -9.95, 0.2, 2, 1.7, 6, 6, 6, 6, 6, 6, 1);
	
	/* Tiang Lab Database */
	buildPillarX(4, 7.2, 2.15, 0.1, -1.05, -0.2, 4.3, 0.2, 2, 1);
	// komplemen belakang
	buildPillarX(4, 7.2, 2.15, -11.1, -1.05, -0.2, 4.3, 0.2, 2, 1);
	
	/* Jendela Ruang Kelas No 1 */
	putWindows(4, 5.975, 1, 0.05, 0.25, 1, 13, 1);
	putWindows(4, 4.725, 1, 0.05, 0.25, 1, 13, 1); // 6,67 mulai
	putWindows(4, 3.475, 1, 0.05, 0.25, 1, 13, 1); // 5,34
	putWindows(4, 2.225, 1, 0.05, 0.25, 1, 13, 1); // 5,34
	// duplikat atas
	putWindows(4, 5.975, 3.3, 0.05, 0.25, 1, 13, 1);
	putWindows(4, 4.725, 3.3, 0.05, 0.25, 1, 13, 1); // 6,67 mulai
	putWindows(4, 3.475, 3.3, 0.05, 0.25, 1, 13, 1); // 5,34
	putWindows(4, 2.225, 3.3, 0.05, 0.25, 1, 13, 1); // 5,34
	// komplemen bawah
	putWindows(4, 5.975, 1, -11.05, 0.25, 1, 13, 1);
	putWindows(4, 4.725, 1, -11.05, 0.25, 1, 13, 1); // 6,67 mulai
	putWindows(4, 3.475, 1, -11.05, 0.25, 1, 13, 1); // 5,34
	putWindows(4, 2.225, 1, -11.05, 0.25, 1, 13, 1); // 5,34
	// komplemen atas
	putWindows(4, 5.975, 3.3, -11.05, 0.25, 1, 13, 1);
	putWindows(4, 4.725, 3.3, -11.05, 0.25, 1, 13, 1); // 6,67 mulai
	putWindows(4, 3.475, 3.3, -11.05, 0.25, 1, 13, 1); // 5,34
	putWindows(4, 2.225, 3.3, -11.05, 0.25, 1, 13, 1); // 5,34
	
	// lab Database timur
	drawBox(7.1, 1, -2.3, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
	
	/* Pintu Lt 1 */
	drawBox2(8.1, 0.75, 0.05, 1.8, 1.5, 0.1, 14, 14, 14, 14, 14, 14, 1);
	
	// Penutup Mushola Barat
	drawBox(10, 1, -8, 3.4, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
	
	// Pembatas Mushola - Gudang
	drawBox(9.6, 1, -9.45, 0.2, 2, 2.7, 6, 6, 6, 6, 6, 6, 1);
	
	// Lorong gudang samping mushola
	drawBox(8.9, 1, -9.5, 1.2, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
	drawBox(8.9, 1, -9, 1.2, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
	
	// Penutup gudang barat
	drawBox(8.4, 1, -10.2, 0.2, 2, 1.2, 6, 6, 6, 6, 6, 6, 1);
	drawBox(8.4, 1, -8.5, 0.2, 2, 0.8, 6, 6, 6, 6, 6, 6, 1);
	
	/* Tiang Gudang Deket Ruang Manajer Timur */
	buildPillarX(1, 9, 2.15, 0.1, 0, 0.2, 4.3, 0.2, 4, 1);
	// komplemen belakang
	buildPillarX(1, 9, 2.15, -11.1, 0, 0.2, 4.3, 0.2, 4, 1);
	
	/* Jendela Gudang deket Ruang Manajer timur */
	putWindows(3, 9.25, 1, 0.05, 0.25, 1, 13, 1);
	// komplemen atas
	putWindows(3, 9.25, 3.3, 0.05, 0.25, 1, 13, 1);
	// komplemen bawah
	putWindows(3, 9.25, 1, -11.05, 0.25, 1, 13, 1);
	// komplemen atas
	putWindows(3, 9.25, 3.3, -11.05, 0.25, 1, 13, 1);
	
	// Ruang Gudang barat (samping manajer)
	drawBox(9.1, 1, -1.6, 0.2, 2, 2.8, 6, 6, 6, 6, 6, 6, 1);
	
	/* Tiang Ruang Manajer Timur */
	buildPillarX(1, 10, 2.15, 0.1, 0, 0.2, 4.3, 0.2, 2, 1);
	// komplemen belakang
	buildPillarX(1, 10, 2.15, -11.1, 0, 0.2, 4.3, 0.2, 2, 1);
	
	/* Jendela Gudang deket Ruang Manajer timur */
	putWindows(2, 10.4, 1, 0.05, 0.25, 1, 13, 1);
	putWindows(2, 11, 1, 0.05, 0.25, 1, 13, 1);
	// duplikat atas
	putWindows(2, 10.4, 3.3, 0.05, 0.25, 1, 13, 1);
	putWindows(2, 11, 3.3, 0.05, 0.25, 1, 13, 1);
	// komplemen bawah
	putWindows(2, 10.4, 1, -11.05, 0.25, 1, 13, 1);
	putWindows(2, 11, 1, -11.05, 0.25, 1, 13, 1);
	// komplemen atas
	putWindows(2, 10.4, 3.3, -11.05, 0.25, 1, 13, 1);
	putWindows(2, 11, 3.3, -11.05, 0.25, 1, 13, 1);
	
	// Ruang Manajer barat
	drawBox(10.1, 1, -1.6, 0.2, 2, 2.8, 6, 6, 6, 6, 6, 6, 1);
	
	// Penutup Ruang Manajer (depan mushola)
	drawBox(10.35, 1, -3.1, 2.7, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
	
//	buildFloor(2, -0.5, 0, -5.5, 4, 34, 0.5, 11, 3, 1); // panjang 34, lebar 11
	
	/*** Lorong depan ***/
	
	/** Lorong depan Z **/
	// Pintu 1 Bawah : -9,7 -> -8,2
	// Pintu 2 Bawah : 7,2 -> 9
	/* Semen bawah */
	drawBox2(-8.95, -0.15, 3, 1.2, 0.3, 4, 4, 4, 4, 4, 4, 4, 4); // -9,45 -> -8,45
	drawBox2(8.1, -0.15, 3, 1.2, 0.3, 4, 4, 4, 4, 4, 4, 4, 4); // 7,6 -> 8,6
	/* Tiang 1 */
	buildPillarZ(4, -9.65, 0.7, 1, 1.06, 0.2, 2, 0.2, 6, 1);
	buildPillarZ(4, -8.25, 0.7, 1, 1.06, 0.2, 2, 0.2, 6, 1);
	/* Tiang 2 */
	buildPillarZ(4, 7.4, 0.7, 1, 1.06, 0.2, 2, 0.2, 6, 1);
	buildPillarZ(4, 8.8, 0.7, 1, 1.06, 0.2, 2, 0.2, 6, 1);
	
	/** lorong Depan X **/
	/* Semen bawah */
	drawBox2(3.725, -0.15, 5.6, 26.55, 0.3, 1.2, 4, 4, 4, 4, 4, 4, 10); // -9,45 -> -8,45
	/* Tiang Horizontal 1 */
	// 15.45 antar pintu, 11 tiang
	buildPillarX(11, -7.2, 0.7, 4.9, 1.1, 0.2, 2, 0.2, 6, 1);
	// 8.1 antara lorong pintu dan ujung horizontal, 7 tiang
	buildPillarX(7, 16.8, 0.7, 4.9, -1.38, 0.2, 2, 0.2, 6, 1); // mulai 17 ke kiri, dg selisih 1.08, -1.28 hanya manipulasi
	/* Tiang Horizontal 2 */
	// Duplikat dari pintu 1
	buildPillarX(2, -9.75, 0.7, 6.3, 1.2, 0.2, 2, 0.2, 6, 1);
	// Duplikat dari pintu 2
	buildPillarX(2, 7.3, 0.7, 6.3, 1.2, 0.2, 2, 0.2, 6, 1);
	// 15.45 antar pintu, 11 tiang
	buildPillarX(11, -7.2, 0.7, 6.3, 1.1, 0.2, 2, 0.2, 6, 1);
	// 8.1 antara lorong pintu dan ujung horizontal, 7 tiang
	buildPillarX(7, 16.8, 0.7, 6.3, -1.38, 0.2, 2, 0.2, 6, 1); // mulai 17 ke kiri, dg selisih 1.08, -1.28 hanya manipulasi
	
	/* Atap Lorong X */
	/* Lorong X */
	drawBox2(3.475, 1.85, 5.6, 27.05, 0.3, 2.2, 4, 4, 4, 4, 4, 4, 4); // lebar 4,5 - 6,7
	/* Lorong Pintu 1 */
	drawBox2(-8.95, 1.85, 2.35, 2.2, 0.3, 4.3, 4, 4, 4, 4, 4, 4, 4); // 7,35 -> 8,85
	/* Lorong Pintu 2 */
	drawBox2(8.1, 1.85, 2.35, 2.2, 0.3, 4.3, 4, 4, 4, 4, 4, 4, 4); // 7,35 -> 8,85

	/* Tembok Lt 2 */
	// Tembok Penutup depan-belakang
	drawBox(0, 3.3, -0.1, 34, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
	drawBox(0, 3.3, -10.9, 34, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
	
	// Tembok Penutup samping
	drawBox(-16.9, 3.3, -5.5, 0.2, 2, 10.6, 6, 6, 6, 6, 6, 6, 1);
	drawBox(16.9, 3.3, -5.5, 0.2, 2, 10.6, 6, 6, 6, 6, 6, 6, 1);
//	
//	// Ruang dosen
//	drawBox(-13.1, 3.3, -5.5, 0.2, 2, 10.6, 6, 6, 6, 6, 6, 6, 1);
//	
//	// Tembok lorong lt. 2
//	drawBox(-4, 3.3, -6.5, 18, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
//	drawBox(-3, 3.3, -4.5, 16, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
//	
//	// Tembok pembatas ruang dapur sisi timur
//	drawBox(-11.5, 3.3, -8.7, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
//	drawBox(-10.9, 3.3, -2.3, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
//	
//	// Sekat pertama
//	drawBox(-7, 3.3, -8.7, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
//	drawBox(-7., 3.3, -2.3, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
//	
//	// Sekat kedua
//	drawBox(-3.5, 3.3, -8.7, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
//	drawBox(-3.5, 3.3, -2.3, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
//	
//	// Sekat ketiga
//	drawBox(0, 3.3, -8.7, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
//	drawBox(0, 3.3, -2.3, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
//	
//	// Sekat keempat
//	drawBox(3.9, 3.3, -8.7, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
//	drawBox(3.9, 3.3, -2.3, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
//	
//	// Penutup WC
//	drawBox(4.9, 3.3, -10.05, 0.2, 2, 1.5, 6, 6, 6, 6, 6, 6, 1);
//	drawBox(4.9, 3.3, -7.35, 0.2, 2, 1.5, 6, 6, 6, 6, 6, 6, 1);
//	
//	// Lorong WC
//	drawBox(4.5, 3.3, -9.2, 1, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
//	drawBox(4.5, 3.3, -8.2, 1, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
//	
//	// Elib
//	drawBox(4.9, 3.3, -2.3, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
//	
//	// Loromg lanjutan di timur
//	drawBox(12.4, 3.3, -6.5, 8.8, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
//	drawBox(12.4, 3.3, -4.5, 8.8, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
//	
//	// sekat kantin bu oneng
//	drawBox(8.1, 3.3, -8.7, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
//	
//	// Sekat kelas 1 (deket mushola)
//	drawBox(12.3, 3.3, -8.7, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
//	
//	// sekat RSG
//	drawBox(9.8, 3.3, -2.3, 0.2, 2, 4.2, 6, 6, 6, 6, 6, 6, 1);
//	
//	// sekat gudang 
//	drawBox(8.1, 3.3, -2.9, 0.2, 2, 3, 6, 6, 6, 6, 6, 6, 1);
//	
//	// sekat cermin
//	drawBox(8.85, 3.3, -1.3, 1.7, 2, 0.2, 6, 6, 6, 6, 6, 6, 1);
//	
//	// Sekat Mushola
//	drawBox(14.8, 3.3, -5.5, 0.2, 2, 1.8, 6, 6, 6, 6, 6, 6, 1);
		
//	while(i < 26) {
//		drawBox(startX, 10, -5, 1, 29, 2, 1, 2, 3, 4, 5, 6, 1);	
//		startX += 3;
//		i++;
//	}
	
//	while(i < 26) {
//		drawBox(startX, 10, -5, 1, 29, 2, 1, 2, 3, 4, 5, 6, 1);	
//		startX += 3;
//		i++;
//	}
		
//
//	//buat tangga 1, 2, 3, 4
//	make_ledder(50,-5,60, 11,5, false);
//	make_ledder(10,-5,60, 11,5, false);
//	make_ledder(-5,-5,60, 11,5, false);
//	make_ledder(-45,-5,60, 11,5, false);
//	
//	//buat pasang deret tiang yg paling dekat dengan tangga
//	build_pillar(80,5-20,50-60, -100,5-20,50-60);
//	build_pillar(80,5-20,40-60, -100,5-20,40-60);
//	//tiang ke pintu utama
//	build_pillar(0,5-20,30-60, 0,5-20,10-60);
//	build_pillar(-10,5-20,30-60, -10,5-20,10-60);
//	//tiang ke ged listrik
//	build_pillar(80,5-20,30-60, 80,5-20,-50-60);
//	build_pillar(70,5-20,30-60, 70,5-20,-50-60);
//	//tiang ke pintu belakang
//	build_pillar(-90,5-20,30-60, -90,5-20,10-60);
//	build_pillar(-100,5-20,30-60, -100,5-20,10-60);
//	
//	//buat atap tiang
//	make_roof_of_pillar(80,14-20,50-60, -100,14-20,40-60); //front
//	make_roof_of_pillar(70,14-20,38-60, 80,14-20,-50-60); // ke ged listrik
//	make_roof_of_pillar(-10,14-20,38-60, 0,14-20,10-60); //ke pintu utama
//	make_roof_of_pillar(-100,14-20,38-60, -90,14-20,10-60); //ke pintu belakang 
//	
//	//buat taman 1
//	draw_box(35,-15,-35, 60,1,20, RUMPUT,RUMPUT,RUMPUT,RUMPUT,RUMPUT,RUMPUT, 1);
//	//buat taman 2
//	draw_box(-50,-15,-35, 70,1,20, RUMPUT,RUMPUT,RUMPUT,RUMPUT,RUMPUT,RUMPUT, 1);
//	
//	//buat lantai koridor luar
//	build_paving_block(-90,-15,-15, 75,-15,-15); //dekat tangga
//	build_paving_block(75,-15,-15, 75,-15,-105); //ke ged listrik
//	build_paving_block(-95,-15,-15, -95,-15,-45); //ke pintu belakang
//	build_paving_block(-5,-15,-15, -5,-15,-45); //ke pintu utama
//	
//	//buat pillar utama depan
//	build_main_pillar(55,0,-55, 0,0,-55, 8); //dari kanan sampai pintu utama
//	build_main_pillar(-10,0,-55, -90,0,-55, 7.2); //dari pintu utama sampai pintu belakang
//	build_main_pillar(-150,0,-55, -100,0,-55, 7.8); //dari pintu belakang sampe belakang
//	
//	//buat pillar tengah depan
//	build_main_pillar(-10,0,-77, -90,0,-77, 7.2); //dari pintu utama sampai pintu belakang
//	
//	//buat pillar tengah
//	
//		
//	//buat pillar tengah belakang
//	build_main_pillar(-10,0,-88, -90,0,-88, 7.2); //dari pintu utama sampai pintu belakang
//	
//	//buat pillar utama belakang
//	build_main_pillar(55,0,-110, 0,0,-110, 8); //dari kanan sampai pintu utama
//	build_main_pillar(-10,0,-110, -90,0,-110, 7.2); //dari pintu utama sampai pintu belakang
//	build_main_pillar(-150,0,-110, -100,0,-110, 7.8); //dari pintu belakang sampe belakang
//	
//	
//	//buat lantai gedung lt1
//	draw_box(-50,-15,-82.5, 200,1,55, KERAMIK,KERAMIK,KERAMIK,KERAMIK,KERAMIK,KERAMIK, 100);
//	//buat lantai gd lt 2
//	draw_box(-50,0,-82.5, 200,1,55, KERAMIK,KERAMIK,KERAMIK,KERAMIK,KERAMIK,KERAMIK, 100);
//	
//	
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
    gluPerspective(60,16.0/9.0,1,1000);
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

/* Gerakan dan Kamera */
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

/* Input Data */
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

/* Menggambar */
void draw_box(float x, float y, float z, float x_size, float y_size, float z_size, int id_front, int id_back, int id_left, int id_right, int id_top, int id_bottom, int texSize)
{
	glPushMatrix();
	glTranslatef(x, y, z);
	
	// FRONT
	glBindTexture(GL_TEXTURE_2D, texture[id_front]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0,0);
		glVertex3f(  x_size/2, -y_size/2, z_size/2);
		glTexCoord2f(0,texSize);
		glVertex3f(  x_size/2,  y_size/2, z_size/2);
		glTexCoord2f(texSize,0);
		glVertex3f( -x_size/2,  y_size/2, z_size/2);
		glTexCoord2f(texSize,texSize);
		glVertex3f( -x_size/2, -y_size/2, z_size/2);
	glEnd();
	
	 //BACK
    glBindTexture(GL_TEXTURE_2D, texture[id_back]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0,0);
		glVertex3f( x_size/2, -y_size/2, -z_size/2);      
		glTexCoord2f(0,texSize);
		glVertex3f( x_size/2,  y_size/2, -z_size/2);     
		glTexCoord2f(texSize,0);
		glVertex3f( -x_size/2,  y_size/2, -z_size/2);   
		glTexCoord2f(texSize,texSize);
		glVertex3f( -x_size/2, -y_size/2, -z_size/2);
	glEnd();
	 
	  // RIGHT
	glBindTexture(GL_TEXTURE_2D, texture[id_right]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0,0);
		glVertex3f( x_size/2, -y_size/2, -z_size/2);
		glTexCoord2f(texSize,0);
		glVertex3f( x_size/2,  y_size/2, -z_size/2);
		glTexCoord2f(0,texSize);
		glVertex3f( x_size/2,  y_size/2,  z_size/2);
		glTexCoord2f(texSize,texSize);
		glVertex3f( x_size/2, -y_size/2,  z_size/2);
	glEnd();
	 
	  // LEFT
	glBindTexture(GL_TEXTURE_2D, texture[id_left]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0,0);
		glVertex3f( -x_size/2, -y_size/2,  z_size/2);
		glTexCoord2f(texSize,0);
		glVertex3f( -x_size/2,  y_size/2,  z_size/2);
		glTexCoord2f(0,texSize);
		glVertex3f( -x_size/2,  y_size/2, -z_size/2);
		glTexCoord2f(texSize,texSize);
		glVertex3f( -x_size/2, -y_size/2, -z_size/2);
	glEnd();
	 
	  // TOP
	glBindTexture(GL_TEXTURE_2D, texture[id_top]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0,0);
		glVertex3f(  x_size/2,  y_size/2,  z_size/2);
		glTexCoord2f(texSize,0);
		glVertex3f(  x_size/2,  y_size/2, -z_size/2);
		glTexCoord2f(0,texSize);
		glVertex3f( -x_size/2,  y_size/2, -z_size/2);
		glTexCoord2f(texSize,texSize);
		glVertex3f( -x_size/2,  y_size/2,  z_size/2);
	glEnd();
	 
	  // BOTTOM
	glBindTexture(GL_TEXTURE_2D, texture[id_bottom]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0,0);
		glVertex3f(  x_size/2, -y_size/2, -z_size/2);
		glTexCoord2f(texSize,0);
		glVertex3f(  x_size/2, -y_size/2,  z_size/2);
		glTexCoord2f(0,texSize);
		glVertex3f( -x_size/2, -y_size/2,  z_size/2);
		glTexCoord2f(texSize,texSize);
		glVertex3f( -x_size/2, -y_size/2, -z_size/2);
	glEnd();
	glPopMatrix();	
}

void make_ledder(int x, int y, int z, int height, int width, bool reverse)
{
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			glPushMatrix();
			glColor3f(1, 1, 1);
			glTranslatef(x-j, y+i, z-i);
			glutSolidCube(1);
			glPopMatrix();
		}
		if(reverse) z+= 2;
	}	
}

void make_pillar(float x, float y, float z){
	//ALAS BERWARNA COKLAT
	draw_box(x, y, z, 1, 1, 1, BROWN, BROWN, BROWN, BROWN, SEMEN, SEMEN, 1);
	//SISANYA DENGAN TEXTURE SEMEN
	draw_box(x, y+4.5, z, 1, 8, 1, SEMEN, SEMEN, SEMEN, SEMEN, SEMEN, SEMEN, 1);
}

void build_pillar(int src_x, int src_y, int src_z, int to_x, int to_y, int to_z){
	
	int x1 = src_x < to_x ? src_x : to_x; //min
	int x2 = src_x > to_x ? src_x : to_x; //max
	
	for(;x1 <= x2; x1 += 10){
		make_pillar((float)x1, (float)src_y, (float)src_z);
	 }
	
	int z1 = src_z < to_z ? src_z : to_z;
	int z2 = src_z > to_z ? src_z : to_z;
	
	for(;z1 <= z2; z1 += 10){
		make_pillar((float)src_x, (float)src_y, (float)z1);
	}
}

void make_main_pillar(float x, float y, float z){
	draw_box(x,y,z, 1,30,1.5, SEMEN,SEMEN,SEMEN,SEMEN,SEMEN,SEMEN, 1);
}

void build_main_pillar(float src_x, float src_y, float src_z, float to_x, float to_y, float to_z, float jarak){
	
	float x1 = src_x < to_x ? src_x : to_x; //min
	float x2 = src_x > to_x ? src_x : to_x; //max
	
	for(;x1 < x2; x1 += jarak){
		make_main_pillar((float)x1, (float)src_y, (float)src_z);
	}
	
	float z1 = src_z < to_z ? src_z : to_z;
	float z2 = src_z > to_z ? src_z : to_z;
	
	for(;z1 < z2; z1 += jarak){
		make_main_pillar((float)src_x, (float)src_y, (float)z1);
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
			draw_box((int)x1, (int)src_y, (int)z1, 2, 2, 2, BROWN, BROWN, BROWN, BROWN, SEMEN, SEMEN, 1);
		}
	}
}

void build_paving_block(int src_x, int src_y, int src_z, int to_x, int to_y, int to_z){
	
	int x1 = src_x < to_x ? src_x : to_x; //min
	int x2 = src_x > to_x ? src_x : to_x; //max
	
	for(;x1 <= x2; x1 += 8){
		draw_box((float)x1,(float)src_y,(float)src_z, 8,1,8, SEMEN,SEMEN,SEMEN,SEMEN,PAVING_BLOCK,PAVING_BLOCK, 5);
	}
	
	int z1 = src_z < to_z ? src_z : to_z;
	int z2 = src_z > to_z ? src_z : to_z;
	
	for(;z1 <= z2; z1 += 8){
		draw_box((float)src_x,(float)src_y,(float)z1, 8,1,8, SEMEN,SEMEN,SEMEN,SEMEN,PAVING_BLOCK,PAVING_BLOCK, 5);
	}
}

/***************** Modul Afif ***********************/

void drawBox(float x, float y, float z, float sizeX, float sizeY, float sizeZ, int texFront, int texBack, int texTop, int texBottom, int texLeft, int texRight, int texSize) {
	glPushMatrix();
	glTranslatef(x, y, z);
	
	// Front
	glBindTexture(GL_TEXTURE_2D, texture[texFront]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(sizeX/2 , -sizeY/2 , sizeZ/2); // bawah kanan
		glTexCoord2f(0, texSize);
		glVertex3f(-sizeX/2 , -sizeY/2 , sizeZ/2); // bawah kiri
		glTexCoord2f(texSize, 0);
		glVertex3f(-sizeX/2 , sizeY/2 , sizeZ/2); // atas kiri
		glTexCoord2f(texSize, texSize);
		glVertex3f(sizeX/2 , sizeY/2 , sizeZ/2); // atas kanan
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

void drawBox2(float x, float y, float z, float sizeX, float sizeY, float sizeZ, int texFront, int texBack, int texTop, int texBottom, int texLeft, int texRight, int texSize) {
	glPushMatrix();
	glTranslatef(x, y, z);
	
	// Front
	glBindTexture(GL_TEXTURE_2D, texture[texFront]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0); 
		glVertex3f(sizeX/2 , -sizeY/2 , sizeZ/2); // bawah kanan
		glTexCoord2f(texSize, 0);
		glVertex3f(-sizeX/2 , -sizeY/2 , sizeZ/2); // bawah kiri
		glTexCoord2f(texSize, texSize);
		glVertex3f(-sizeX/2 , sizeY/2 , sizeZ/2); // atas kiri
		glTexCoord2f(0, texSize);
		glVertex3f(sizeX/2 , sizeY/2 , sizeZ/2); // atas kanan
	glEnd();
	
	// Back
	glBindTexture(GL_TEXTURE_2D, texture[texBack]);
	glBegin(GL_POLYGON);
		glTexCoord2f(texSize, 0);
		glVertex3f(sizeX/2 , -sizeY/2 , -sizeZ/2);
		glTexCoord2f(0, 0);
		glVertex3f(-sizeX/2 , -sizeY/2 , -sizeZ/2);
		glTexCoord2f(0, texSize);
		glVertex3f(-sizeX/2 , sizeY/2 , -sizeZ/2);
		glTexCoord2f(texSize, texSize);
		glVertex3f(sizeX/2 , sizeY/2 , -sizeZ/2);
	glEnd();
	
	// Top
	glBindTexture(GL_TEXTURE_2D, texture[texTop]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(sizeX/2 , sizeY/2 , sizeZ/2);
		glTexCoord2f(texSize, 0);
		glVertex3f(-sizeX/2 , sizeY/2 , sizeZ/2);
		glTexCoord2f(texSize, texSize);
		glVertex3f(-sizeX/2 , sizeY/2 , -sizeZ/2);
		glTexCoord2f(0, texSize);
		glVertex3f(sizeX/2 , sizeY/2 , -sizeZ/2);
	glEnd();
	
	// Bottom
	glBindTexture(GL_TEXTURE_2D, texture[texBottom]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(sizeX/2 , -sizeY/2 , sizeZ/2);
		glTexCoord2f(texSize, 0);
		glVertex3f(-sizeX/2 , -sizeY/2 , sizeZ/2);
		glTexCoord2f(texSize, texSize);
		glVertex3f(-sizeX/2 , -sizeY/2 , -sizeZ/2);
		glTexCoord2f(0, texSize);
		glVertex3f(sizeX/2 , -sizeY/2 , -sizeZ/2);
	glEnd();
	
	// Left
	glBindTexture(GL_TEXTURE_2D, texture[texLeft]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(-sizeX/2 , -sizeY/2 , sizeZ/2);
		glTexCoord2f(texSize, 0);
		glVertex3f(-sizeX/2 , -sizeY/2 , -sizeZ/2);
		glTexCoord2f(texSize, texSize);
		glVertex3f(-sizeX/2 , sizeY/2 , -sizeZ/2);
		glTexCoord2f(0, texSize);
		glVertex3f(-sizeX/2 , sizeY/2 , sizeZ/2);
	glEnd();
	
	// Right
	glBindTexture(GL_TEXTURE_2D, texture[texRight]);
	glBegin(GL_POLYGON);
		glTexCoord2f(texSize, 0);
		glVertex3f(sizeX/2 , -sizeY/2 , sizeZ/2);
		glTexCoord2f(0, 0);
		glVertex3f(sizeX/2 , -sizeY/2 , -sizeZ/2);
		glTexCoord2f(0, texSize);
		glVertex3f(sizeX/2 , sizeY/2 , -sizeZ/2);
		glTexCoord2f(texSize, texSize);
		glVertex3f(sizeX/2 , sizeY/2 , sizeZ/2);
	glEnd();
	glPopMatrix();
}


void drawPrism(float x, float y, float z, float bottomX, float bottomZ, float topX, float topZ, float high, int texFront, int texBack, int texTop, int texBottom, int texLeft, int texRight, int texSize) {
	glPushMatrix();
	glTranslatef(x, y, z);
	
	// Front
	glBindTexture(GL_TEXTURE_2D, texture[texFront]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(bottomX/2 , -high/2 , bottomZ/2); // bawah kanan
		glTexCoord2f(texSize, 0);
		glVertex3f(-bottomX/2 , -high/2 , bottomZ/2); // bawah kiri
		glTexCoord2f(texSize, texSize);
		glVertex3f(-topX/2 , high/2 , topZ/2); // atas kiri
		glTexCoord2f(0, texSize);
		glVertex3f(topX/2 , high/2 , topZ/2); // atas kanan
	glEnd();
	
	// Back
	glBindTexture(GL_TEXTURE_2D, texture[texBack]);
	glBegin(GL_POLYGON);
		glTexCoord2f(texSize, 0);
		glVertex3f(bottomX/2 , -high/2 , -bottomZ/2);
		glTexCoord2f(0, 0);
		glVertex3f(-bottomX/2 , -high/2 , -bottomZ/2);
		glTexCoord2f(0, texSize);
		glVertex3f(-topX/2 , high/2 , -topZ/2);
		glTexCoord2f(texSize, texSize);
		glVertex3f(topX/2 , high/2 , -topZ/2);
	glEnd();
	
	// Top
	glBindTexture(GL_TEXTURE_2D, texture[texTop]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(topX/2 , high/2 , topZ/2);
		glTexCoord2f(texSize, 0);
		glVertex3f(-topX/2 , high/2 , topZ/2);
		glTexCoord2f(texSize, texSize);
		glVertex3f(-topX/2 , high/2 , -topZ/2);
		glTexCoord2f(0, texSize);
		glVertex3f(topX/2 , high/2 , -topZ/2);
	glEnd();
	
	// Bottom
	glBindTexture(GL_TEXTURE_2D, texture[texBottom]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(bottomX/2 , -high/2 , bottomZ/2);
		glTexCoord2f(texSize, 0);
		glVertex3f(-bottomX/2 , -high/2 , bottomZ/2);
		glTexCoord2f(texSize, texSize);
		glVertex3f(-bottomX/2 , -high/2 , -bottomZ/2);
		glTexCoord2f(0, texSize);
		glVertex3f(bottomX/2 , -high/2 , -bottomZ/2);
	glEnd();
	
	// Left
	glBindTexture(GL_TEXTURE_2D, texture[texLeft]);
	glBegin(GL_POLYGON);
		glTexCoord2f(0, 0);
		glVertex3f(-bottomX/2 , -high/2 , bottomZ/2);
		glTexCoord2f(texSize, 0);
		glVertex3f(-bottomX/2 , -high/2 , -bottomZ/2);
		glTexCoord2f(texSize, texSize);
		glVertex3f(-topX/2 , high/2 , -topZ/2);
		glTexCoord2f(0, texSize);
		glVertex3f(-topX/2 , high/2 , topZ/2);
	glEnd();
	
	// Right
	glBindTexture(GL_TEXTURE_2D, texture[texRight]);
	glBegin(GL_POLYGON);
		glTexCoord2f(texSize, 0);
		glVertex3f(bottomX/2 , -high/2 , bottomZ/2);
		glTexCoord2f(0, 0);
		glVertex3f(bottomX/2 , -high/2 , -bottomZ/2);
		glTexCoord2f(0, texSize);
		glVertex3f(topX/2 , high/2 , -topZ/2);
		glTexCoord2f(texSize, texSize);
		glVertex3f(topX/2 , high/2 , topZ/2);
	glEnd();
	glPopMatrix();
}

void buildPillarX(int numOfPillar, float startX, float poinY, float poinZ, float difference, float sizeX, float sizeY, float sizeZ, int texId, int texSize) {
	int i = 0;
	float next = startX;
	
	while (i < numOfPillar) {
		drawBox2(next+(sizeX/2), poinY, poinZ, sizeX, sizeY, sizeZ, texId, texId, texId, texId, texId, texId, texSize);
		i++;
		next += (difference+sizeX);
	}
}

void buildPillarZ(int numOfPillar, float poinX, float poinY, float startZ, float difference, float sizeX, float sizeY, float sizeZ, int texId, int texSize) {
	int i = 0;
	float next = startZ;
	
	while (i < numOfPillar) {
		drawBox2(poinX, poinY, next+(sizeZ/2), sizeX, sizeY, sizeZ, texId, texId, texId, texId, texId, texId, texSize);
		i++;
		next += (difference+sizeZ);
	}
}


void buildFloor(int numOfFloor, float startY, float poinX, float poinZ, float difference, float sizeX, float sizeY, float sizeZ, int texId, int texSize) {
	int i = 0;
	float next = startY;
	
	while (i < numOfFloor) {
		drawBox2(poinX, next+(sizeY/2), poinZ, sizeX, sizeY, sizeZ, texId, texId, texId, texId, texId, texId, texSize);
		i++;
		next += (difference+sizeY);
	}
}

void putWindows(int numOfWindows, float startX, float poinY, float poinZ, float sizeX, float sizeY, int texId, int texSize) {
	int i = 0;
	float next = startX;
	
	while (i < numOfWindows) {
		drawBox2(next+(sizeX/2), poinY, poinZ, sizeX, sizeY, 0.1, texId, texId, texId, texId, texId, texId, texSize);
		i++;
		next += sizeX;
	}
}
