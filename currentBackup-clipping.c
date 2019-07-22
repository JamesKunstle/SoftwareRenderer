/*
 *
 * point.c - simple GLUT app that draws one frame with a single point at origin
 *
 * To build:  gcc -framework OpenGL -framework GLUT currentBackup-clipping.c -o currentBackup-clipping
 *
 */
#ifndef GL_SILENCE_DEPRECATION
#endif
#define GL_SILENCE_DEPRECATION




/*************************************************************************/
/* header files                                                          */
/*************************************************************************/
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <stdio.h>
#include <sys/time.h>

/*************************************************************************/
/* defines                                                               */
/*************************************************************************/
#define SWAP(a, b)  \
{                   \
float temp = a; \
a = b;          \
b = temp;       \
}                   \

#define MIN3(a,b,c)         (((a) < (b)) ? (((a) < (c)) ? (a) : (c)) : (((b) < (c)) ? (b) : (c)))

#define MAX3(a,b,c)         (((a) > (b)) ? (((a) > (c)) ? (a) : (c)) : (((b) > (c)) ? (b) : (c)))


#define X 0
#define Y 1
#define Z 2
#define W 3

#define R 0
#define G 1
#define B 2
#define A 3

#define WALK 4
#define DRAW 5

#define S 0
#define T 1


#define ON 1
#define OFF 0

#define EMERALD             0

#define JADE                1

#define OBSIDIAN            2

#define PEARL               3

#define RUBY                4

#define TURQUOISE           5

#define BRASS               6

#define BRONZE              7

#define CHROME              8

#define COPPER              9

#define GOLD                10

#define SILVER              11

#define BLACK_PLASTIC       12

#define CYAN_PLASTIC        13

#define GREEN_PLASTIC       14

#define RED_PLASTIC         15

#define WHITE_PLASTIC       16

#define YELLOW_PLASTIC      17

#define BLACK_RUBBER        18

#define CYAN_RUBBER         19

#define GREEN_RUBBER        20

#define RED_RUBBER          21

#define WHITE_RUBBER        22

#define YELLOW_RUBBER       23

#define CLAMP(a, low, high)     ((a) < (low)) ? (low) : (((a) > (high)) ? (high) : (a))

#define MILLION             1000000

#define IN 56

#define CLIPPED 57

/*************************************************************************/
/* structs                                                               */
/*************************************************************************/

typedef struct POINT{
    float position[4]; // X / Y / Z / W
    float RGBA[4]; // R / G / B / A
    float STRQ[4]; // S / T / R / Q values for our texture mapping
    float world[4]; //literal coordinates for the world-space
    int tri_list[20]; // the indices of of the triangles that utilize this vertex.
    int num_tri; // the number of triangles that utilize this vertex.
    float light[4]; // the direction of the point to the finitely distant light source.
    float normal[4];
    float dist[6]; // distance between point and the six planes
}POINT;

typedef struct IMAGE{ // width and height abstracted: maintained by function that reads image data into this struct.
    int width;
    int height;
    unsigned char data[3000][3000][4]; // T / S / RGBA
}IMAGE;

typedef struct tri{
    int vertex[3];
    float normal[4];
    float dist[3];
    int clip_flag;
}TRI;

typedef struct material {
    
    float   ambient[4];
    
    float   diffuse[4];
    
    float   specular[4];
    
} MATERIAL;

typedef struct timer{
    
    struct timeval  start;
    struct timeval  end;
}TIMER;

typedef struct timersense{ // struct that will store consecutive times recorded using the timers.
    
    float sw_time[1000];
    float gl_time[1000];
    int current_sw; // current index for sw save
    int current_gl; // current index for gl save.
    
}TIMERSENSE;

typedef struct plane{
    float normal[4];
    float dist;
}PLANE;

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 500;
int Mojave_WorkAround = 1;
int draw_one_frame = 1;

POINT spans[800][2]; //2 x 800 array containing the coordinates of all of the spans for a triangle
int count[800]; // holds the current current number of values stored per row in spans
float c_buff[800][800][4];
float d_buff[800][800];


int d_buff_active = OFF;
int c_buff_blending = OFF;
int texturing = OFF; // pixel color value comes from the interpolated color value or the supplied texture?
int modulate = OFF;
int polar = OFF;
float polar_rotate_degrees = 0.0;

PLANE frustum[6];

int perspective_draw = OFF;

int rasterize = OFF;

int r_cube = OFF;

int r_mesh = OFF;

int r_t_cube = OFF;

int r_plane = OFF;

int r_sphere = OFF;

int r_cylinder = OFF;

int r_cone = OFF;

int r_torus = OFF;

int perspective_correct = ON;

IMAGE current_texture; // global location of the texture that we're working with.
IMAGE starter_texture;
IMAGE swap_texture;

// 3D modeling
TRI triangle_list[40000]; // 4000 triangles with 3 points each.
int numtriangles;


POINT vertex_list[100000]; //each slot will actually hold a reference to a point
int numvertices;

float xangle = 0.0;
float yangle = 0.0;
float zangle = 0.0;

float distance = 2.0;
float far = 40.0;

float wave = 0.0;

float light_x = 1.0;
float light_y = 0;
float light_z = 0;

float light[4];

int phong_lighting = ON;

int face_lighting = OFF;

float ambient[4] = { 0.9, 0.9, 0.9, 0.9 };

float first_render = ON;
float mult_coords[1000][3]; // Holds the randomized coordinates that shapes may be placed at when they're drawn.
int current_coord = 0; // holds index of the next available coords that can be used to draw a shape.

int multiple = OFF;

float eye[4]                = { 0, 0, 1, 1 };

float light[4]              = { 1, -1, -1, 1 };

float light_ambient[4]      = { 1.0, 1.0, 1.0, 1.0 };

float light_diffuse[4]      = { 1, 1, 1, 1 };

float light_specular[4]     = { 1, 1, 1, 1 };


MATERIAL material_list[] = {
    
    {{0.0215,       0.1745,         0.0215, 1},     {0.07568,       0.61424,        0.07568,1},     {0.633,         0.727811,       0.633,          1}},
    
    {{0.135,        0.2225,         0.1575, 1},     {0.54,          0.89,           0.63,   1},     {0.316228,      0.316228,       0.316228,       1}},
    
    {{0.05375,      0.05,           0.06625,1},     {0.18275,       0.17,           0.22525,1},     {0.332741,      0.328634,       0.346435,       1}},
    
    {{0.25,         0.20725,        0.20725,1},     {1,             0.829,          0.829,  1},     {0.296648,      0.296648,       0.296648,       1}},
    
    {{0.1745,       0.01175,        0.01175,1},     {0.61424,       0.04136,        0.04136,1},     {0.727811,      0.626959,       0.626959,       1}},
    
    {{0.1,          0.18725,        0.1745, 1},     {0.396,         0.74151,        0.69102,1},     {0.297254,      0.30829,        0.306678,       1}},
    
    {{0.329412,     0.223529,       0.02745,1},     {0.780392,      0.568627,       0.11372,1},     {0.992157,      0.941176,       0.807843,       1}},
    
    {{0.2125,       0.1275,         0.054,  1},     {0.714,         0.4284,         0.18144,1},     {0.393548,      0.271906,       0.166721,       1}},
    
    {{0.25,         0.25,           0.25,   1},     {0.4,           0.4,            0.4,    1},     {0.774597,      0.774597,       0.774597,       1}},
    
    {{0.19125,      0.0735,         0.0225, 1},     {0.7038,        0.27048,        0.0828, 1},     {0.256777,      0.137622,       0.086014,       1}},
    
    {{0.24725,      0.1995,         0.0745, 1},     {0.75164,       0.60648,        0.22648,1},     {0.628281,      0.555802,       0.366065,       1}},
    
    {{0.19225,      0.19225,        0.19225,1},     {0.50754,       0.50754,        0.50754,1},     {0.508273,      0.508273,       0.508273,       1}},
    
    {{0.0,          0.0,            0.0,    1},     {0.01,          0.01,           0.01,   1},     {0.50,          0.50,           0.50,           1}},
    
    {{0.0,          0.1,            0.06,   1},     {0.0,           0.50980392,     0.50980,1},     {0.50196078,    0.50196078,     0.50196078,     1}},
    
    {{0.0,          0.0,            0.0,    1},     {0.1,           0.35,           0.1,    1},     {0.45,          0.55,           0.45,           1}},
    
    {{0.0,          0.0,            0.0,    1},     {0.5,           0.0,            0.0,    1},     {0.7,           0.6,            0.6,            1}},
    
    {{0.0,          0.0,            0.0,    1},     {0.55,          0.55,           0.55,   1},     {0.70,          0.70,           0.70,           1}},
    
    {{0.0,          0.0,            0.0,    1},     {0.5,           0.5,            0.0,    1},     {0.60,          0.60,           0.50,           1}},
    
    {{0.02,         0.02,           0.02,   1},     {0.01,          0.01,           0.01,   1},     {0.4,           0.4,            0.4,            1}},
    
    {{0.0,          0.05,           0.05,   1},     {0.4,           0.5,            0.5,    1},     {0.04,          0.7,            0.7,            1}},
    
    {{0.0,          0.05,           0.0,    1},     {0.4,           0.5,            0.4,    1},     {0.04,          0.7,            0.04,           1}},
    
    {{0.05,         0.0,            0.0,    1},     {0.5,           0.4,            0.4,    1},     {0.7,           0.04,           0.04,           1}},
    
    {{0.05,         0.05,           0.05,   1},     {0.5,           0.5,            0.5,    1},     {0.7,           0.7,            0.7,            1}},
    
    {{0.05,         0.05,           0.0,    1},     {0.5,           0.5,            0.4,    1},     {0.7,           0.7,            0.04,           1}},
    
};


float shinyness_list[] =  {
    
    0.6,
    
    0.1,
    
    0.3,
    
    0.088,
    
    0.6,
    
    0.1,
    
    0.21794872,
    
    0.2,
    
    0.6,
    
    0.1,
    
    0.4,
    
    0.4,
    
    .25,
    
    .25,
    
    .25,
    
    .25,
    
    .25,
    
    .25,
    
    .078125,
    
    .078125,
    
    .078125,
    
    .078125,
    
    .078125,
    
    .078125
    
};


int num_material = GOLD;


// gold

//float material_ambient[4]   = {0.24725,     0.1995,     0.0745,     1};
//
//float material_diffuse[4]   = {0.75164,     0.60648,    0.22648,    1};
//
//float material_specular[4]  = {0.628281,    0.555802,   0.366065,   1};

float material_ambient[4]   = {0.7,     0.7,     0.7,     1};

float material_diffuse[4]   = {0.75,    0.75,    0.75,    1};

float material_specular[4]  = {0.75,    0.75,    0.75,   1}; // current material properties are essentially nothing. It's an inocuous material0l

float shinyness = (0.4 * 128);

int display_render = ON;

IMAGE postprocessingimage; // written to from c_buff, read from for image processing.

IMAGE postprocessingimagefinal; // written to for image processing, read from to c_buff


/*these are all of the global objects that are required for the depth of field affect*/
IMAGE mask;
IMAGE sharp;
IMAGE depth_blur;
IMAGE depth_blur_pair;
IMAGE output;
//
float max_depth = -10000000;

float translation_value = 5.0;

float fog_color[4] = { 0, 0, 0, 1};

int fog = OFF;

IMAGE bumpmap;

int bumpmapping = OFF;

int naive_mapping = ON;

int cylindrical_mapping = OFF;

int spherical_mapping = OFF;

int cube_mapping = OFF;

int depth_of_field = OFF;

int reflection = ON;

IMAGE cubemap[6]; // Contains the six textures of the cube.

TIMER sw_renderer_timer;

TIMER gl_timer;

TIMERSENSE current_savefile;

float light_position[4] = {1, 1, 0, 1.0};

int local_lighting = ON;

/*************************************************************************/
/* utility functions                                                     */
/*************************************************************************/
/*
 * random_float()
 */

float dot(float vec1[4], float vec2[4]);

void print_current_spans_table(void);

void normalize_vector(float in[4], float out[4]);

void reset_all_vertex_color();

void mult_scalar_vector(float vector[4], float scalar, float output[4]);

void render_object();

void reflect_vect( float n[4], float l[4], float r[4]);

void add_vectors(float vector1[4], float vector2[4], float output[4]);

void reflection_map_vect( float normal[4], float tex[4] );

void cube_map_vect( float v[4], float tex[4], int *index );




void interpolate_two_vect( float factor, float a[4], float b[4], float r[4] )

{
    
    r[X] = (a[X] * factor) + (b[X] * (1-factor));
    
    r[Y] = (a[Y] * factor) + (b[Y] * (1-factor));
    
    r[Z] = (a[Z] * factor) + (b[Z] * (1-factor));
    
    r[W] = (a[W] * factor) + (b[W] * (1-factor));
    
}


void average_two_vect( float a[4], float b[4], float r[4] )

{
    
    float factor = 1.0/2.0;
    
    r[X] = (a[X] + b[X]) * factor;
    
    r[Y] = (a[Y] + b[Y]) * factor;
    
    r[Z] = (a[Z] + b[Z]) * factor;
    
    r[W] = (a[W] + b[W]) * factor;
    
}


void average_three_vect( float a[4], float b[4], float c[4], float r[4] )

{
    
    float factor = 1.0/3.0;
    
    r[X] = (a[X] + b[X] + c[X]) * factor;
    
    r[Y] = (a[Y] + b[Y] + c[Y]) * factor;
    
    r[Z] = (a[Z] + b[Z] + c[Z]) * factor;
    
    r[W] = (a[W] + b[W] + c[W]) * factor;
    
}


void start_timer( TIMER *timer )
{
    gettimeofday( &timer->start, NULL );
}

void stop_timer( TIMER *timer )
{
    gettimeofday( &timer->end, NULL);
}

double elapsed_time( TIMER *timer)
{
    double usec = ( (timer->end.tv_sec - timer->start.tv_sec) * MILLION);
    usec += ( timer->end.tv_usec - timer->start.tv_usec );
    return (usec / MILLION);
}

/*returns the length of an input vector of four components without considering the w component*/
float length_vector(float v[4])
{
    return sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
}

void copy_vect(float i[4], float o[4])
{
    o[0] = i[0];
    o[1] = i[1];
    o[2] = i[2];
    o[3] = i[3];
}

/* Adds a new triangle into the triangle_list array.*/
// !! not passed vertices themselves, passed the indices at which the constituent vertices are located in the vertec_list array.
void add_triangle(int v0, int v1, int v2)
{
    triangle_list[numtriangles].vertex[0] = v0;
    triangle_list[numtriangles].vertex[1] = v1;
    triangle_list[numtriangles].vertex[2] = v2;
    numtriangles++;
}


/*cross product of a and b goes into the vector n*/
void cross_vect(float a[4], float b[4], float n[4])
{
    n[X] = (a[Y] * b[Z]) - (a[Z] * b[Y]);
    n[Y] = (a[Z] * b[X]) - (a[X] * b[Z]);
    n[Z] = (a[X] * b[Y]) - (a[Y] * b[X]);
    n[W] = 1.0;
}

void mult_vect(float a[4], float b[4], float n[4]) // multiply two vectors and put the product into one final one.
{
    n[X] = a[X] * b[X];
    n[Y] = a[Y] * b[Y];
    n[Z] = a[Z] * b[Z];
    n[W] = 1.0;
}


void clear_c_buff(float r, float g, float b, float a) // fills the color buffer with the values given.
{
    for(int i = 0; i < 800; i++)
    {
        for(int j = 0; j < 800; j++)
        {
            c_buff[i][j][R] = r;
            c_buff[i][j][G] = g;
            c_buff[i][j][B] = b;
            c_buff[i][j][A] = a;
        }
    }
}

void clear_d_buff(float v) // fills the depth buffer with the value given.
{
    for(int i = 0; i < 800; i++)
    {
        for(int j = 0; j < 800; j++)
        {
            d_buff[i][j] = v;
        }
    }
}

void zero_out_count() // zeroes out the count array for each new triangle
{
    for(int i = 0; i < 800; i++)
    {
        count[i] = 0;
    }
}
float random_float( float low, float high )
{
    return( (float)(low + (rand()/(float)RAND_MAX)*(high - low)) );
}

#if 0
void draw_point(POINT *p, float blend_weight) // Updating to take in color-buffer information
{
    
    int x = (int) p->position[X] + 400;
    int y = (int) p->position[Y] + 400;
    
    if(x < 0 || x  > 800 || y < 0 || y > 800)
    {
        return;
    }
    
    if((d_buff_active == ON && d_buff[(int)(p->position[Y] + 400)][(int)(p->position[X] + 400)] > p->position[Z]) || d_buff_active == OFF)
    {
        
        //glBegin(GL_POINTS);
        if(d_buff_active == ON && d_buff[(int)(p->position[Y] + 400)][(int)(p->position[X] + 400)] > p->position[Z])
        {
            d_buff[(int)(p->position[Y] + 400)][(int)(p->position[X] + 400)] = p->position[Z]; // updates the depth buffer for the input point.
        }
        
        if(texturing == ON)
        {
            float z = 1.0/p->position[Z];
            
            if(perspective_correct)
            {
                p->STRQ[S] *= z;
                p->STRQ[T] *= z;
            }
            
            
            float local_s = current_texture.width * p->STRQ[S];
            float local_t = current_texture.height * p->STRQ[T];
            
            
            p->RGBA[R] = current_texture.data[(int)local_t][(int)local_s][R] / 255.0;
            //printf("Text r value = %f\n", text_r_val);
            p->RGBA[G] = current_texture.data[(int)local_t][(int)local_s][G] / 255.0;
            //printf("Text g value = %f\n", text_g_val);
            p->RGBA[B] = current_texture.data[(int)local_t][(int)local_s][B] / 255.0; // local value of color data stored in the pixel space.
            //printf("Text b value = %f\n", text_b_val);
            
            //put the color in the texture for the point into the point
        }
        
        
        else if((c_buff_blending == ON && d_buff_active == ON && d_buff[(int)(p->position[Y] + 400)][(int)(p->position[X] + 400)] > p->position[Z] ))
        {
            c_buff[(int)(p->position[Y] + 400)][(int)(p->position[X] + 400)][R] =
            (blend_weight * c_buff[(int)(p->position[Y] + 400)][(int)(p->position[X] + 400)][R]) + (1.0-blend_weight * p->RGBA[R]); // c_buff red value updated and blended.
            
            c_buff[(int)(p->position[Y] + 400)][(int)(p->position[X] + 400)][G] =
            (blend_weight * c_buff[(int)(p->position[Y] + 400)][(int)(p->position[X] + 400)][G]) + (1.0-blend_weight * p->RGBA[G]); // c_buff green value updated and blended.
            
            c_buff[(int)(p->position[Y] + 400)][(int)(p->position[X] + 400)][B] =
            (blend_weight * c_buff[(int)(p->position[Y] + 400)][(int)(p->position[X] + 400)][B]) + (1.0-blend_weight * p->RGBA[B]); // c_buff blue value updated and blended.
            
            c_buff[(int)(p->position[Y] + 400)][(int)(p->position[X] + 400)][A] =
            (blend_weight * c_buff[(int)(p->position[Y] + 400)][(int)(p->position[X] + 400)][A]) + (1.0-blend_weight * p->RGBA[A]); // c_buff alpha value updated and blended.
            //printf("second channel\n");
            
            p->RGBA[R] = c_buff[(int)(p->position[Y] + 400)][(int)(p->position[X] + 400)][R];
            p->RGBA[G] = c_buff[(int)(p->position[Y] + 400)][(int)(p->position[X] + 400)][G];
            p->RGBA[B] = c_buff[(int)(p->position[Y] + 400)][(int)(p->position[X] + 400)][B];
            p->RGBA[A] = c_buff[(int)(p->position[Y] + 400)][(int)(p->position[X] + 400)][A];
            
        }
        else
        {
            
        }
        
        if(phong_lighting == ON && face_lighting == OFF)
        {
            printf("n: %f, %f, %f, %f\n", p->normal[0], p->normal[1], p->normal[2], p->normal[3]);
            float brightness = dot(p->normal, light);
            printf("b: %f\n", brightness);
            p->RGBA[R] = p->RGBA[R] * brightness;
            p->RGBA[G] = p->RGBA[G] * brightness;
            p->RGBA[B] = p->RGBA[B] * brightness;
            p->RGBA[A] = p->RGBA[A] * brightness;
        }
        add_vectors(p->RGBA, )
        
        GLCell[c_points] = *p;
        c_points+=1;
        //        glVertex2f(p->position[X], p->position[Y]);
        //        glEnd();
    }
    else
    {
        //nothing is drawn if the depth buffer is on and the point to be drawn is below an already drawn point.
    }
}
#else
void draw_point(POINT *p, float blend_weight) // Updating to take in color-buffer information
{
    
    int x = (int) p->position[X] + 400;
    int y = (int) p->position[Y] + 400;
    float r, g, b, a;
    float bump[4];
    float local_normal[4];
    float l_color[4];
    float tmp[4];
    
    local_normal[0] = p->normal[0];
    local_normal[1] = p->normal[1];
    local_normal[2] = p->normal[2];
    local_normal[3] = p->normal[3];
    
    if(x < 0 || x  > 800 || y < 0 || y > 800)
        return;
    
    if( d_buff_active == ON && d_buff[y][x] < p->position[Z] )
    {
        return;
    }
    
    l_color[0] = p->RGBA[R]; // local color that we can use for Phong lighting
    l_color[1] = p->RGBA[G];
    l_color[2] = p->RGBA[B];
    l_color[3] = p->RGBA[A];
    
    if( bumpmapping ) // we need to alter the normal before it's used to calculate the lighting of the vertex.
    {
        int s = p->STRQ[S] * bumpmap.width;
        int t = p->STRQ[T] * bumpmap.height;
        
        if( reflection == ON )
        {
            reflection_map_vect(p->normal, p->STRQ);
            s = p->STRQ[S] * bumpmap.width;
            t = p->STRQ[T] * bumpmap.height;
        }
        
        bump[R] = (float)bumpmap.data[t][s][R] / 255.0;
        bump[G] = (float)bumpmap.data[t][s][G] / 255.0;
        bump[B] = (float)bumpmap.data[t][s][B] / 255.0;
        
        //float normalizer[4] = { -0.5, -0.5, -0.5, 0.0};
        //add_vectors(bump, normalizer, bump);
        normalize_vector(bump, bump);
        mult_vect(bump, local_normal, local_normal);
    }
    
    if( phong_lighting )
    {
        float *current_light;
        if(local_lighting)
        {
            current_light = p->light;
        }
        else
        {
            current_light = light;
        }
        
        /*      diffuse         */
        float diffuse = dot(current_light, local_normal);
        mult_scalar_vector(material_diffuse, diffuse, tmp);
        mult_vect(tmp, light_diffuse, tmp);
        mult_vect(tmp, l_color, l_color);
        
        /*      specular         */
        float r[4]; // reflection vector that will be filled by the function call below.
        reflect_vect( local_normal, light, r );
        float specular = dot( eye, r );
        specular = pow( specular, 1 );
        
        float specular_array[4];
        mult_scalar_vector(material_specular, specular, specular_array);
        mult_vect(specular_array, light_specular, specular_array);
        add_vectors(l_color, specular_array, l_color);
        
        /*      ambient         */
        float temp_ambient[4];
        mult_vect(material_ambient, ambient, temp_ambient);
        mult_vect(temp_ambient, light_ambient, temp_ambient);
        add_vectors(l_color, temp_ambient, l_color);
        
    }
    
    d_buff[y][x] = p->position[Z]; // updates the depth buffer for the input point.
    if(p->position[Z] > max_depth)
    {
        max_depth = p->position[Z];
    }
    
    if(texturing == ON) // perhaps should come first
    {
        int index = -1;
        IMAGE *text = &current_texture;
        if( cube_mapping )
        {
            cube_map_vect( p->normal, p->STRQ, &index );
            text = &cubemap[index];
            
        }
        
        float z = 1.0/p->position[Z];
        float s_pcorrect = p->STRQ[S];
        float t_pcorrect = p->STRQ[T];
        float position_vector[4];
        copy_vect(p->STRQ, position_vector);
        if( reflection == ON )
        {
            reflection_map_vect(p->normal, position_vector);
            s_pcorrect = position_vector[S];
            t_pcorrect = position_vector[T];
        }
        
        if(perspective_correct)
        {
            s_pcorrect *= z;
            t_pcorrect *= z;
        }
        
        int local_s = (int)(text->width * s_pcorrect);
        int local_t = (int)(text->height * t_pcorrect);
        
        r = text->data[local_t][local_s][R] / 255.0;
        g = text->data[local_t][local_s][G] / 255.0;
        b = text->data[local_t][local_s][B] / 255.0;
        a = text->data[local_t][local_s][A] / 255.0; // local value of color data stored in the pixel space.
        
        
        //put the color in the texture for the point into the point
        if( modulate )
        {
            r *= l_color[R];
            g *= l_color[G];
            b *= l_color[B];
            a *= l_color[A];
        }
        
    }
    else
    {
        r = l_color[R];
        g = l_color[G];
        b = l_color[B];
        a = l_color[A];
    }
    
    if( c_buff_blending == ON )
    {
        r = blend_weight * c_buff[y][x][R] + (1 - blend_weight) * r;
        g = blend_weight * c_buff[y][x][G] + (1 - blend_weight) * g;
        b = blend_weight * c_buff[y][x][B] + (1 - blend_weight) * b;
        a = blend_weight * c_buff[y][x][A] + (1 - blend_weight) * a;
    }
    
    if( face_lighting )
    {
        r += ambient[R];
        g += ambient[G];
        b += ambient[B];
        a += ambient[A];
    }
    
    if( fog == ON )
    {
        float z = p->position[Z];
        r = z * fog_color[R] + (1 - z) * l_color[R];
        g = z * fog_color[G] + (1 - z) * l_color[G];
        b = z * fog_color[B] + (1 - z) * l_color[B];
        a = z * fog_color[A] + (1 - z) * l_color[A];
    }
    
    c_buff[y][x][R] = r;
    c_buff[y][x][G] = g;
    c_buff[y][x][B] = b;
    c_buff[y][x][A] = a;
}
#endif

void add_vectors(float vector1[4], float vector2[4], float output[4])
{
    for(int i = 0; i < 4; i++)
    {
        output[i] = vector1[i] + vector2[i];
    }
}

void subtract_vectors(float vector1[4], float vector2[4], float output[4])
{ // second vector is subtracted from the first one.
    for(int i = 0; i < 4; i++)
    {
        output[i] = vector1[i] - vector2[i];
    }
}

void mult_scalar_vector(float vector[4], float scalar, float output[4])
{ // second vector is subtracted from the first one.
    for(int i = 0; i < 4; i++)
    {
        output[i] = vector[i] * scalar;
    }
}

void div_scalar_vector(float vector[4], float scalar, float output[4])
{ // second vector is subtracted from the first one.
    for(int i = 0; i < 4; i++)
    {
        output[i] = vector[i] / scalar;
    }
}

void reflect_vect( float n[4], float l[4], float r[4]) // passes the calculated value to the passed in reflection array.
{
    float temp[4] = { 0, 0, 0, 0 };
    float a = dot( n, l );
    mult_scalar_vector( n, a, temp );
    mult_scalar_vector( temp, 2, temp );
    subtract_vectors( l, temp, r);
}

void store_point(POINT *p)
{
    int row = (int) (p->position[Y] + 400);
    int col = count[row];
    if(col <= 1)
    {
        spans[row][col] = *p;
        count[row]++;
    }
    /*
     else if(row == 400)
     {
     spans[row][1] = *p;
     }
     
     else if(col > 1) // if column has been visited two or more times, simply replace the contents over by one.
     {
     spans[row][0] = spans[row][1];
     spans[row][1] = *p;
     count[row]++;
     }
     
     else
     {
     spans[row][1] = *p;
     }
     */
}

/*
 * draw_line()
 */

/*
 
 * edgeFunction()
 
 */

float edgeFunction( float a[4], float b[4], float c[4] )

{
    
    return( c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0] );
    
}


/*
 
 * draw_triangle_barycentric()
 
 */

void draw_triangle_barycentric( POINT *v0, POINT *v1, POINT *v2 )

{
    
    int     minx = MIN3(v0->position[X],v1->position[X],v2->position[X]);
    
    int     miny = MIN3(v0->position[Y],v1->position[Y],v2->position[Y]);
    
    int     maxx = MAX3(v0->position[X],v1->position[X],v2->position[X]);
    
    int     maxy = MAX3(v0->position[Y],v1->position[Y],v2->position[Y]);
    
    float   area = edgeFunction(v0->position, v1->position, v2->position);
    
    int     x, y;
    
    float   w[4];
    
    POINT   p;
    
    
    /*
     
     * for all the points in triangle bounding box
     
     */
    
    for( y = miny-1; y < maxy+2; y++ )
        
    {
        
        for( x = minx-1; x < maxx+2; x++ )
            
        {
            p.position[X] = x + 0.5;
            p.position[Y] = y + 0.5;
            p.position[Z] = 0;
            p.position[W] = 1;
            
            
            
            /*
             
             * compute barycentric weights
             
             */
            
            w[0] = edgeFunction(v1->position, v2->position, p.position);
            
            w[1] = edgeFunction(v2->position, v0->position, p.position);
            
            w[2] = edgeFunction(v0->position, v1->position, p.position);
            
            w[3] = 0.0;
            
            
            
            if( w[0] >= 0 && w[1] >= 0 && w[2] >= 0 )
                
            {
                
                /*
                 
                 * if point is inside triangle, compute barycentric weighting of vertex attributes (e.g. z, color, s, t)
                 
                 */
                
                div_scalar_vector(w, area, w);
                
                
                p.position[Z]    = w[0] * v0->position[Z] + w[1] * v1->position[Z] + w[2] * v2->position[Z];
                
                
                p.RGBA[R]  = w[0] * v0->RGBA[R] + w[1] * v1->RGBA[R] + w[2] * v2->RGBA[R];
                
                p.RGBA[G]  = w[0] * v0->RGBA[G] + w[1] * v1->RGBA[G] + w[2] * v2->RGBA[G];
                
                p.RGBA[B]  = w[0] * v0->RGBA[B] + w[1] * v1->RGBA[B] + w[2] * v2->RGBA[B];
                
                
                p.STRQ[S]    = w[0] * v0->STRQ[S] + w[1] * v1->STRQ[S] + w[2] * v2->STRQ[S];
                
                p.STRQ[T]    = w[0] * v0->STRQ[T] + w[1] * v1->STRQ[T] + w[2] * v2->STRQ[T];
                
                
                p.normal[0] = w[0] * v0->normal[0] + w[1] * v1->normal[0] + w[2] * v2->normal[0];
                p.normal[1] = w[0] * v0->normal[1] + w[1] * v1->normal[1] + w[2] * v2->normal[1];
                p.normal[2] = w[0] * v0->normal[2] + w[1] * v1->normal[2] + w[2] * v2->normal[2];
                p.normal[3] = w[0] * v0->normal[3] + w[1] * v1->normal[3] + w[2] * v2->normal[3]; // interpolation of normal across all points
                
                p.light[0] = w[0] * v0->light[0] + w[1] * v1->light[0] + w[2] * v2->light[0];
                p.light[1] = w[0] * v0->light[1] + w[1] * v1->light[1] + w[2] * v2->light[1];
                p.light[2] = w[0] * v0->light[2] + w[1] * v1->light[2] + w[2] * v2->light[2];
                p.light[3] = w[0] * v0->light[3] + w[1] * v1->light[3] + w[2] * v2->light[3];
                
                draw_point( &p, 1.0 );
                
            }
            
        }
        
    }
    
}


void draw_line_from_Chris( POINT *start, POINT *end, int mode, float blend_weight )
{
    POINT   delta;
    POINT   step;
    POINT   p;
    int     i;
    
    /*
     * calculate deltas in position, color
     */
    subtract_vectors( end->position,     start->position,     delta.position   );
    subtract_vectors( end->RGBA,   start->RGBA,   delta.RGBA );
    subtract_vectors( end->STRQ,    start->STRQ,    delta.STRQ);
    subtract_vectors( end->light,    start->light,    delta.light);
    
    
    /*
     * determine whether line is x-major or y-major
     */
    i = (fabsf(delta.position[X]) >= fabsf(delta.position[Y]) && mode == DRAW ) ? X : Y;
    
    /*
     * calculate slope (i.e. increment per iteration) for position, color
     *
     * for x-major divide by deltax, for y-major divide by deltay
     */
    div_scalar_vector( delta.position, fabsf(delta.position[i]),   step.position   );
    div_scalar_vector( delta.RGBA, fabsf(delta.position[i]), step.RGBA );
    div_scalar_vector( delta.STRQ, fabsf(delta.position[i]), step.STRQ );
    div_scalar_vector( delta.light, fabsf(delta.position[i]), step.light );
    
    if( step.position[i] > 0 )
    {
        for( p = *start; (int)p.position[i] < (int)end->position[i]; )
        {
            if( mode == DRAW )
                draw_point( &p, blend_weight );
            else
                store_point( &p );
            add_vectors( p.position,    step.position,   p.position   );
            add_vectors( p.RGBA,  step.RGBA, p.RGBA );
            add_vectors( p.STRQ,  step.STRQ, p.STRQ );
            add_vectors( p.light,  step.light, p.light );
        }
    }
    else
    {
        for( p = *start; (int)p.position[i] > (int)end->position[i]; )
        {
            if( mode == DRAW )
                draw_point( &p, blend_weight);
            else
                store_point( &p );
            
            add_vectors( p.position,    step.position,   p.position   );
            add_vectors( p.RGBA,  step.RGBA, p.RGBA );
            add_vectors( p.STRQ,  step.STRQ, p.STRQ );
            add_vectors( p.light,  step.light, p.light );
        }
    }
}

void draw_spans()
{
    // for every 2 in count, draw the [y][0] through [y][1] lines.
    // for all 1's, draw the [y][0] point and
    // skip all 0.
    for(int i = 0; i < 800; i++)
    {
        if(count[i] != 0 && count[i] >= 2)
        {
            draw_line_from_Chris(&spans[i][0], &spans[i][1], DRAW, 1.0);
        }
    }
    
}

void draw_triangle(POINT *p0, POINT *p1, POINT *p2)
{
    zero_out_count();
    //printf("all of the counts have been zeroed");
    draw_line_from_Chris(p0, p1, WALK, 1.0);
    draw_line_from_Chris(p1, p2, WALK, 1.0);
    draw_line_from_Chris(p2, p0, WALK, 1.0);
    //print_current_spans_table();
    draw_spans();
}


void random_text(IMAGE *image)  // WHY ARE YOU BROKEN?!??!?!??!?!
// this SHOULD put a random texture into the current_texture data space so that it can be read onto a triangle.
{
    image->height = 255;
    image->width = 255;
    for(int y = 0; y < image->height; y++)
    {
        for(int x = 0; x <= image->width; x++)
        {
            image->data[y][x][R] = 0; //(int)(random_float( 0, 1) * 255);
            image->data[y][x][G] = 255; //(int)(random_float( 0, 1) * 255);
            image->data[y][x][B] = 255; //(int)(random_float( 0, 1) * 255);
            image->data[y][x][A] = 255;
        }
    }
}

void checker_board_text(IMAGE *image)
// this SHOULD put a checkerboard texture into the current_texture data space so that it can be read onto a triangle.
{
    image->height = 255;
    image->width = 255;
    for(int y = 0; y < image->height; y++)
    {
        for(int x = 0; x < image->width; x++)
        {
            if((y / 10) & 1)
            {
                image->data[y][x][R] = (x / 10 & 1) ? 0:255;
                image->data[y][x][G] = (x / 10 & 1) ? 0:255;
                image->data[y][x][B] = (x / 10 & 1) ? 0:255;
                image->data[y][x][A] = 255;
            }
            else
            {
                image->data[y][x][R] = (x / 10 & 1) ? 255:0;
                image->data[y][x][G] = (x / 10 & 1) ? 255:0;
                image->data[y][x][B] = (x / 10 & 1) ? 255:0;
                image->data[y][x][A] = 255;
            }
        }
    }
}

void r_text_file(IMAGE *image, char *name)
{
    FILE *fp;
    char buffer[10];
    int r, g, b, max;
    
    fp = fopen(name, "r");
    
    fscanf(fp, "%s", buffer);
    fscanf(fp, "%d %d", &image->width, &image->height);
    fscanf(fp, "%d", &max);
    
    for (int y = 0; y < image->height; y++)
    {
        for(int x = 0; x < image->width; x++)
        {
            fscanf(fp, "%d %d %d", &r, &g, &b);
            image->data[y][x][R] = ((float)r / max) * 255.0;
            image->data[y][x][G] = ((float)g / max) * 255.0;
            image->data[y][x][B] = ((float)b / max) * 255.0;
        }
    }
    fclose(fp);
}

void r_binary_text_file(IMAGE *image, char *name)
{
    FILE *fp;
    char buffer[10];
    int r, g, b, max;
    
    fp = fopen(name, "r");
    
    fscanf(fp, "%s", buffer);
    if( buffer[0] == 'P' && buffer[1] == '6' )
    {
        fscanf(fp, "%d %d %d", &image->width, &image->height, &max);
        unsigned char *data = malloc( image->width * image->height * 3);
        
        fread( (void*)data, 3, image->width * image->height, fp );
        
        int n = 1; // tracks chars of data array.
        
        for (int y = 0; y < image->height; y++)
        {
            for(int x = 0; x < image->width; x++)
            {
                image->data[y][x][R] = data[n++];
                image->data[y][x][G] = data[n++];
                image->data[y][x][B] = data[n++];
            }
        }
        fclose(fp);
    }
    else
    {
        printf("Not a valid binary P6 file~\n");
    }
}

void r_binary_text_file_two_row(IMAGE *image, char *name)
{
    FILE *fp;
    char buffer[10];
    int max;
    
    fp = fopen(name, "r");
    fscanf(fp, "%s", buffer);
    if( buffer[0] == 'P' && buffer[1] == '6' )
    {
        fscanf(fp, "%d %d", &image->width, &image->height);
        fscanf(fp, "%d", &max);
        unsigned char *data = malloc( image->width * image->height * 3);
        
        fread( (void*)data, 3, image->width * image->height, fp );
        
        int n = 1; // tracks chars of data array.
        printf("Made it to before the loop\n");
        for (int y = 0; y < image->height; y++)
        {
            for(int x = 0; x < image->width; x++)
            {
                image->data[y][x][R] = data[n++];
                image->data[y][x][G] = data[n++];
                image->data[y][x][B] = data[n++];
            }
        }
        fclose(fp);
    }
    else
    {
        printf("Not a valid binary P6 file~\n");
    }
}

void one_color_text(float r_channel, float g_channel, float b_channel, IMAGE *image)
{
    // overwrites one pixel with new color and changes width and height to one.
    image->height = 255;
    image->width = 255;
    for(int j = 0; j < image->height; j++)
    {
        for(int i = 0; i <= image->width; i++)
        {
            image->data[j][i][R] = r_channel * 255.0;
            image->data[j][i][G] = g_channel * 255.0;
            image->data[j][i][B] = b_channel * 255.0;
            
        }
    }
}

void copy_texture(IMAGE *sender)
{
    current_texture.height = sender->height;
    current_texture.width = sender->width;
    
    //    printf("The current texture dimensions are: (%d, %d)\n", current_texture.width, current_texture.height);
    //    printf("The starter texture dimensions are: (%d, %d)\n", sender->width, sender->height);
    for(int j = 0; j < sender->height; j++)
    {
        for(int i = 0; i <= sender->width; i++)
        {
            current_texture.data[j][i][R] = sender->data[j][i][R];
            current_texture.data[j][i][G] = sender->data[j][i][G];
            current_texture.data[j][i][B] = sender->data[j][i][B];
            //printf("current RBA = %f, %f, %f\n", (float)current_texture.data[j][i][R], (float)current_texture.data[j][i][R], (float)current_texture.data[j][i][R]);
            
        }
    }
}
void negative_image(IMAGE *image)
{
    // overwrites one pixel with new color and changes width and height to one.
    for(int j = 0; j < image->height; j++)
    {
        for(int i = 0; i <= image->width; i++)
        {
            image->data[j][i][R] = 255.0 - image->data[j][i][R];
            image->data[j][i][G] = 255.0 - image->data[j][i][G];
            image->data[j][i][B] = 255.0 - image->data[j][i][B];
            
        }
    }
}

void text_vertical_flip(IMAGE *sender, IMAGE *recipient)
{
    recipient->height = sender->height;
    recipient->width = sender->width;
    
    printf("Recipient width and height are (%d, %d)\n", recipient->width, recipient->height);
    printf("Sender width and height are (%d, %d)\n", sender->width, sender->height);
    
    for(int j = 0; j < recipient->height; j++)
    {
        for(int i = 0; i < recipient->width; i++)
        {
            recipient->data[j][recipient->width - i - 1][R] = sender->data[j][i][R];
            recipient->data[j][recipient->width - i - 1][G] = sender->data[j][i][G];
            recipient->data[j][recipient->width - i - 1][B] = sender->data[j][i][B];
            recipient->data[j][recipient->width - i - 1][A] = sender->data[j][i][A];
        }
    }
}

void luminosity_text(IMAGE *sender, IMAGE *recipient)
{
    recipient->height = sender->height;
    recipient->width = sender->width;
    
    for(int j = 0; j < recipient->height; j++)
    {
        for(int i = 0; i < recipient->width; i++)
        {
            float lume = (sender->data[j][i][R] + sender->data[j][i][G] + sender->data[j][i][B]) / 3.0;
            
            recipient->data[j][i][R] = (int)lume;
            recipient->data[j][i][G] = (int)lume;
            recipient->data[j][i][B] = (int)lume;
            recipient->data[j][i][A] = sender->data[j][i][A];
        }
    }
}

void sepia_text(IMAGE *sender, IMAGE *recipient)
{
    recipient->height = sender->height;
    recipient->width = sender->width;
    
    float sepia_r;
    float sepia_g;
    float sepia_b;
    
    for(int j = 0; j < recipient->height; j++)
    {
        for(int i = 0; i < recipient->width; i++)
        {
            sepia_r = (0.393 * sender->data[j][i][R]) + (0.769 * sender->data[j][i][G]) + (0.189 * sender->data[j][i][B]);
            sepia_g = (0.349 * sender->data[j][i][R]) + (0.686 * sender->data[j][i][G]) + (0.168 * sender->data[j][i][B]);
            sepia_b = (0.272 * sender->data[j][i][R]) + (0.534 * sender->data[j][i][G]) + (0.131 * sender->data[j][i][B]);
            
            
            if(sepia_r >= 255.0)
            {
                sepia_r = 255.0;
                printf("Red too large\n");
            }
            if(sepia_g >= 255.0)
            {
                sepia_g = 255.0;
                printf("Green too large\n");
            }
            if(sepia_b >= 255.0)
            {
                sepia_b = 255.0;
                printf("Blue too large\n");
            }
            
            
            recipient->data[j][i][R] = sepia_r;
            recipient->data[j][i][G] = sepia_g;
            recipient->data[j][i][B] = sepia_b;
            recipient->data[j][i][A] = sender->data[j][i][A];
        }
    }
}

void avg_color(float *r_channel, float *g_channel, float *b_channel, int x_coord, int y_coord, IMAGE *image)
{
    // takes pointers for color channels as input so that they can be assigned and passed out of the function. function works through the 3x3 neighborhood around an input pixel and calculates the average color for all channels. Assumes by input that it's being passed a valid pixel (one with 3x3 neighborhood.)
    
    float sum_r = 0.0;
    float sum_g = 0.0;
    float sum_b = 0.0;
    
    for(int j = (y_coord - 1); j < (y_coord + 2); j++)
    {
        for(int i = (x_coord - 1); i < (x_coord + 2); i++)
        {
            sum_r += (float)image->data[j][i][R];
            sum_g += (float)image->data[j][i][G];
            sum_b += (float)image->data[j][i][B];
        }
    }
    sum_r /= 9.0;
    sum_g /= 9.0;
    sum_b /= 9.0;
    
    *r_channel = sum_r;
    printf("Passing out average r value %f\n", sum_r);
    *g_channel = sum_g;
    *b_channel = sum_b;
}

void min_color(float *r_channel, float *g_channel, float *b_channel, int x_coord, int y_coord, IMAGE *image)
{
    // takes pointers for color channels as input so that they can be assigned and passed out of the function. function works through the 3x3 neighborhood around an input pixel and calculates the min color for all channels. Assumes by input that it's being passed a valid pixel (one with 3x3 neighborhood.)
    
    float min_r = 1000000.0;
    float min_g = 1000000.0;
    float min_b = 1000000.0;
    
    for(int j = y_coord - 1; j < y_coord + 2; j++)
    {
        for(int i = x_coord - 1; i < x_coord + 2; i++)
        {
            if(image->data[j][i][R] < min_r)
            {
                min_r = (float)image->data[j][i][R];
            }
            if(image->data[j][i][G] < min_g)
            {
                min_g = (float)image->data[j][i][G];
            }
            if(image->data[j][i][B] < min_b)
            {
                min_b = (float)image->data[j][i][B];
            }
            
        }
    }
    
    *r_channel = min_r;
    *g_channel = min_g;
    *b_channel = min_b;
}

void max_color(float *r_channel, float *g_channel, float *b_channel, int x_coord, int y_coord, IMAGE *image)
{
    // takes pointers for color channels as input so that they can be assigned and passed out of the function. function works through the 3x3 neighborhood around an input pixel and calculates the min color for all channels. Assumes by input that it's being passed a valid pixel (one with 3x3 neighborhood.)
    
    float max_r = 0.0;
    float max_g = 0.0;
    float max_b = 0.0;
    
    for(int j = y_coord - 1; j < (y_coord + 2); j++)
    {
        for(int i = x_coord - 1; i < (x_coord + 2); i++)
        {
            if((float)image->data[j][i][R] > max_r)
            {
                max_r = (float)image->data[j][i][R];
            }
            if((float)image->data[j][i][G] > max_g)
            {
                max_g = (float)image->data[j][i][G];
            }
            if((float)image->data[j][i][B] > max_b)
            {
                max_b = (float)image->data[j][i][B];
            }
            
        }
    }
    
    *r_channel = max_r;
    *g_channel = max_g;
    *b_channel = max_b;
}

void avg_down_sample(IMAGE *sender, IMAGE *recipient)
{
    float r_channel = 0.0;
    float g_channel = 0.0;
    float b_channel = 0.0;
    
    printf("the color channels begin as %f, %f, %f\n", r_channel, g_channel, b_channel);
    
    //^these are the holding variables that we will be using per-area.
    // need to pass the coords from the sending item to the pixel averaging function.
    for(int j  = 1; j < (sender->height - 2); j+= 3)
    {
        for(int i = 1; i < (sender->width - 2); i += 3)
        {
            printf("The current value for red is %f before avg function called.\n", r_channel);
            avg_color(&r_channel, &g_channel, &b_channel, i, j, sender);
            //^gets the average color of the 3x3 group of pixels that it cares about. (i, j) will always be at the center of a 3x3.
            // Then, we need to read the channels into the pixels that correspond to the (i, j) that was passed to the averager.
            
            printf("The current value for red is %f after avg function called.\n", r_channel);
            
            for(int g = j - 1; g < (j + 2); g++ ){
                for(int h = i - 1; h < (i + 2); h++)
                {
                    //Work through the 3x3 and change its color values to those that were calculated in the above function.
                    recipient->data[g][h][R] = r_channel;
                    recipient->data[g][h][G] = g_channel;
                    recipient->data[g][h][B] = b_channel;
                    
                }
            }
        }
    }
}

void max_down_sample(IMAGE *sender, IMAGE *recipient)
{
    float r_channel;
    float g_channel;
    float b_channel;
    //^these are the holding variables that we will be using per-area.
    // need to pass the coords from the sending item to the pixel averaging function.
    for(int j  = 1; j <= (sender->height - 1); j+= 3)
    {
        for(int i = 1; i <= (sender->width - 1); i += 3)
        {
            max_color(&r_channel, &g_channel, &b_channel, i, j, sender);
            //^gets the average color of the 3x3 group of pixels that it cares about. (i, j) will always be at the center of a 3x3.
            // Then, we need to read the channels into the pixels that correspond to the (i, j) that was passed to the averager.
            
            for(int g = (j - 1); g <= (j + 1); g++ ){
                for(int h = (i - 1); h <= (i + 1); h++)
                {
                    //Work through the 3x3 and change its color values to those that were calculated in the above function.
                    recipient->data[g][h][R] = r_channel;
                    recipient->data[g][h][G] = g_channel;
                    recipient->data[g][h][B] = b_channel;
                    
                }
            }
        }
    }
}

void min_down_sample(IMAGE *sender, IMAGE *recipient)
{
    float r_channel;
    float g_channel;
    float b_channel;
    //^these are the holding variables that we will be using per-area.
    // need to pass the coords from the sending item to the pixel averaging function.
    for(int j  = 1; j <= (sender->height - 1); j+= 3)
    {
        for(int i = 1; i <= (sender->width - 1); i += 3)
        {
            printf("The current value for red is %f before min function called.\n", r_channel);
            min_color(&r_channel, &g_channel, &b_channel, i, j, sender);
            printf("The current value for red is %f after min function called.\n", r_channel);
            //^gets the average color of the 3x3 group of pixels that it cares about. (i, j) will always be at the center of a 3x3.
            // Then, we need to read the channels into the pixels that correspond to the (i, j) that was passed to the averager.
            
            for(int g = (j - 1); g <= (j + 1); g++ ){
                for(int h = (i - 1); h <= (i + 1); h++)
                {
                    //Work through the 3x3 and change its color values to those that were calculated in the above function.
                    recipient->data[g][h][R] = r_channel;
                    recipient->data[g][h][G] = g_channel;
                    recipient->data[g][h][B] = b_channel;
                    
                }
            }
        }
    }
}

void cart_to_polar(float xcoord, float ycoord, float *angle, float *radius)
{
    
    *radius = sqrt(pow(xcoord, 2) + pow(ycoord , 2));
    *angle = atan2(ycoord, xcoord);
}
void polar_to_cart(float angle, float radius, int *xcoord, int *ycoord)
{
    
    *xcoord = (radius * cos(angle));
    *ycoord = (radius * sin(angle));
}


void texture_rotation_v2(IMAGE *sender, IMAGE *recipient, float degrees)
{
    //For each point in the sender image, calculate the coordinates in polar, add degrees, calculate back to cartesian, if in bounds, map the color of pixel.
    recipient->width = sender->width;
    recipient->height = sender->height;
    float angle;
    float radius;
    int new_x;
    int new_y;
    float normal_width = sender->width / 2;
    float normal_height = sender->height / 2; // used to pass the x/y coords such that they are centered around the origin within a texture.
    //printf("The texture is %d, %d\n", sender->width, sender->height);
    
    for(int j = 0; j < recipient->height; j++)
    {
        for(int i = 0; i < recipient->width; i++)
        {
            //For every point in the sending texture
            //printf("moving pixel from %d, %d\n", i, j);
            cart_to_polar(i - normal_width, j - normal_height, &angle, &radius);
            angle += degrees;
            polar_to_cart(angle, radius, &new_x, &new_y);
            new_x += normal_width;
            new_y += normal_height; // reset the new_x and new_y so that they (should) be within bounds of the texture.
            //printf("to %d, %d\n", new_x, new_y);
            if(new_x < sender->width && new_x > -1 && new_y < sender->height && new_y > -1)
            {
                //if the new point is inside the boundary of the sending image
                recipient->data[j][i][R] = sender->data[new_y][new_x][R];
                recipient->data[j][i][G] = sender->data[new_y][new_x][G];
                recipient->data[j][i][B] = sender->data[new_y][new_x][B];
                recipient->data[j][i][A] = sender->data[new_y][new_x][A];
                
                //printf("recipient pixel has RGBA of %f, %f, %f\n", (float)recipient->data[new_y][new_x][R], (float)recipient->data[new_y][new_x][G], (float)recipient->data[new_y][new_x][B]);
            }
        }
    }
    
}

void lincoln_transform(IMAGE *sender, IMAGE *recipient)
{
    recipient->width = sender->width;
    recipient->height = sender->height;
    
    
    int new_x;
    int new_y;
    
    for(int j = 0; j < recipient->height; j++)
    {
        for(int i = 0; i < recipient->width; i++)
        {
            new_x = (i / 16) * 16;
            new_y = (j / 16) * 16;
            if(new_x < sender->width && new_x > -1 && new_y < sender->height && new_y > -1)
            {
                //if the new point is inside the boundary of the sending image
                recipient->data[j][i][R] = sender->data[new_y][new_x][R];
                recipient->data[j][i][G] = sender->data[new_y][new_x][G];
                recipient->data[j][i][B] = sender->data[new_y][new_x][B];
                recipient->data[j][i][A] = sender->data[new_y][new_x][A];
                
                //printf("recipient pixel has RGBA of %f, %f, %f\n", (float)recipient->data[new_y][new_x][R], (float)recipient->data[new_y][new_x][G], (float)recipient->data[new_y][new_x][B])
            }
        }
    }
}

void lincoln_transform_polar(IMAGE *sender, IMAGE *recipient)
{
    recipient->width = sender->width;
    recipient->height = sender->height;
    
    float angle;
    float radius;
    
    int new_x;
    int new_y;
    
    float normal_width = sender->width / 2;
    float normal_height = sender->height / 2;
    
    for(int j = 0; j < recipient->height; j++)
    {
        for(int i = 0; i < recipient->width; i++)
        {
            cart_to_polar(i - normal_width, j - normal_height, &angle, &radius);
            
            radius = (radius / 64) * 64;
            angle = (angle / 64) * 64;
            
            polar_to_cart(angle, radius, &new_x, &new_y);
            
            new_x += normal_width;
            new_y += normal_height;
            
            if(new_x < sender->width && new_x > -1 && new_y < sender->height && new_y > -1)
            {
                //if the new point is inside the boundary of the sending image
                recipient->data[j][i][R] = sender->data[new_y][new_x][R];
                recipient->data[j][i][G] = sender->data[new_y][new_x][G];
                recipient->data[j][i][B] = sender->data[new_y][new_x][B];
                recipient->data[j][i][A] = sender->data[new_y][new_x][A];
                
                //printf("recipient pixel has RGBA of %f, %f, %f\n", (float)recipient->data[new_y][new_x][R], (float)recipient->data[new_y][new_x][G], (float)recipient->data[new_y][new_x][B])
            }
        }
    }
}

void fisheye(IMAGE *sender, IMAGE *recipient)
{
    recipient->width = sender->width;
    recipient->height = sender->height;
    float angle;
    float radius;
    int new_x;
    int new_y;
    float normal_width = sender->width / 2;
    float normal_height = sender->height / 2;
    float max_radius = sqrt(pow(normal_height, 2) + pow(normal_width , 2));
    for(int j = 0; j < recipient->height; j++)
    {
        for(int i = 0; i < recipient->width; i++)
        {
            // for all coorinates in the recieving texture
            cart_to_polar(i - normal_width, j - normal_height, &angle, &radius);
            radius = pow(radius, 2) / max_radius;
            polar_to_cart(angle, radius, &new_x, &new_y);
            
            new_x += normal_width;
            new_y += normal_height;
            if(new_x < sender->width && new_x > -1 && new_y < sender->height && new_y > -1)
            {
                //if the new point is inside the boundary of the sending image
                recipient->data[j][i][R] = sender->data[new_y][new_x][R];
                recipient->data[j][i][G] = sender->data[new_y][new_x][G];
                recipient->data[j][i][B] = sender->data[new_y][new_x][B];
                recipient->data[j][i][A] = sender->data[new_y][new_x][A];
                
                //printf("recipient pixel has RGBA of %f, %f, %f\n", (float)recipient->data[new_y][new_x][R], (float)recipient->data[new_y][new_x][G], (float)recipient->data[new_y][new_x][B])
            }
        }
    }
}
void reverse_fisheye(IMAGE *sender, IMAGE *recipient)
{
    recipient->width = sender->width;
    recipient->height = sender->height;
    float angle;
    float radius;
    int new_x;
    int new_y;
    float normal_width = sender->width / 2;
    float normal_height = sender->height / 2;
    float max_radius = sqrt(pow(normal_height, 2) + pow(normal_width , 2));
    for(int j = 0; j < recipient->height; j++)
    {
        for(int i = 0; i < recipient->width; i++)
        {
            // for all coorinates in the recieving texture
            cart_to_polar(i - normal_width, j - normal_height, &angle, &radius);
            radius = sqrt(radius * max_radius);
            polar_to_cart(angle, radius, &new_x, &new_y);
            
            new_x += normal_width;
            new_y += normal_height;
            if(new_x < sender->width && new_x > -1 && new_y < sender->height && new_y > -1)
            {
                //if the new point is inside the boundary of the sending image
                recipient->data[j][i][R] = sender->data[new_y][new_x][R];
                recipient->data[j][i][G] = sender->data[new_y][new_x][G];
                recipient->data[j][i][B] = sender->data[new_y][new_x][B];
                recipient->data[j][i][A] = sender->data[new_y][new_x][A];
                
                //printf("recipient pixel has RGBA of %f, %f, %f\n", (float)recipient->data[new_y][new_x][R], (float)recipient->data[new_y][new_x][G], (float)recipient->data[new_y][new_x][B])
            }
        }
    }
}


void funhouse(IMAGE *sender, IMAGE *recipient)
{
    recipient->width = sender->width;
    recipient->height = sender->height;
    
    float C1 = 100.;
    float C2 = 5;
    float C3 = 5;
    
    int new_x;
    int new_y;
    
    for(int j = 0; j < recipient->height; j++)
    {
        for(int i = 0; i < recipient->width; i++)
        {
            new_x = i + sin(C1 * i) * C2;
            new_y = j + sin(j) * C3;
            if(new_x < sender->width && new_x > -1 && new_y < sender->height && new_y > -1)
            {
                //if the new point is inside the boundary of the sending image
                recipient->data[j][i][R] = sender->data[new_y][new_x][R];
                recipient->data[j][i][G] = sender->data[new_y][new_x][G];
                recipient->data[j][i][B] = sender->data[new_y][new_x][B];
                recipient->data[j][i][A] = sender->data[new_y][new_x][A];
                
                //printf("recipient pixel has RGBA of %f, %f, %f\n", (float)recipient->data[new_y][new_x][R], (float)recipient->data[new_y][new_x][G], (float)recipient->data[new_y][new_x][B])
            }
        }
    }
}
void humed_mirror(IMAGE *sender, IMAGE *recipient)
{
    recipient->width = sender->width;
    recipient->height = sender->height;
    
    float C1 = 100.;
    float C2 = 5;
    float C3 = 5;
    
    int new_x;
    int new_y;
    
    for(int j = 0; j < recipient->height; j++)
    {
        for(int i = 0; i < recipient->width; i++)
        {
            new_x = i + (i % 32);
            new_y = j;
            if(new_x < sender->width && new_x > -1 && new_y < sender->height && new_y > -1)
            {
                //if the new point is inside the boundary of the sending image
                recipient->data[j][i][R] = sender->data[new_y][new_x][R];
                recipient->data[j][i][G] = sender->data[new_y][new_x][G];
                recipient->data[j][i][B] = sender->data[new_y][new_x][B];
                recipient->data[j][i][A] = sender->data[new_y][new_x][A];
                
                //printf("recipient pixel has RGBA of %f, %f, %f\n", (float)recipient->data[new_y][new_x][R], (float)recipient->data[new_y][new_x][G], (float)recipient->data[new_y][new_x][B])
            }
        }
    }
}

void humed_mirror_polar(IMAGE *sender, IMAGE *recipient)
{
    recipient->width = sender->width;
    recipient->height = sender->height;
    float angle;
    float radius;
    int new_x;
    int new_y;
    float normal_width = sender->width / 2;
    float normal_height = sender->height / 2;
    
    for(int j = 0; j < recipient->height; j++)
    {
        for(int i = 0; i < recipient->width; i++)
        {
            cart_to_polar(i - normal_width, j - normal_height, &angle, &radius);
            
            new_x = i + (((int)(angle + radius)) % 32) - 16;
            new_y = j;
            if(new_x < sender->width && new_x > -1 && new_y < sender->height && new_y > -1)
            {
                //if the new point is inside the boundary of the sending image
                recipient->data[j][i][R] = sender->data[new_y][new_x][R];
                recipient->data[j][i][G] = sender->data[new_y][new_x][G];
                recipient->data[j][i][B] = sender->data[new_y][new_x][B];
                recipient->data[j][i][A] = sender->data[new_y][new_x][A];
                
                //printf("recipient pixel has RGBA of %f, %f, %f\n", (float)recipient->data[new_y][new_x][R], (float)recipient->data[new_y][new_x][G], (float)recipient->data[new_y][new_x][B])
            }
        }
    }
}
void thompson_rotation(IMAGE *sender, IMAGE *recipient)
{
    recipient->width = sender->width;
    recipient->height = sender->height;
    
    float angle;
    float radius;
    
    int new_x;
    int new_y;
    
    float normal_width = sender->width / 2;
    float normal_height = sender->height / 2;
    
    for(int j = 0; j < recipient->height; j++)
    {
        for(int i = 0; i < recipient->width; i++)
        {
            cart_to_polar(i - normal_width, j - normal_height, &angle, &radius);
            
            angle = angle + (radius / 300);
            
            polar_to_cart(angle, radius, &new_x, &new_y);
            
            new_x += normal_width;
            new_y += normal_height;
            
            if(new_x < sender->width && new_x > -1 && new_y < sender->height && new_y > -1)
            {
                //if the new point is inside the boundary of the sending image
                recipient->data[j][i][R] = sender->data[new_y][new_x][R];
                recipient->data[j][i][G] = sender->data[new_y][new_x][G];
                recipient->data[j][i][B] = sender->data[new_y][new_x][B];
                recipient->data[j][i][A] = sender->data[new_y][new_x][A];
                
                //printf("recipient pixel has RGBA of %f, %f, %f\n", (float)recipient->data[new_y][new_x][R], (float)recipient->data[new_y][new_x][G], (float)recipient->data[new_y][new_x][B])
            }
        }
    }
}

/*Goes through the triangle list and calculates the face normals for each, storing the normal within the normal[4] array within the triangle struct.*/
void cal_face_normal(void)
{
    float i_normal[4] = {0, 0, 0, 0};
    float sub_vect_1[4] = {0, 0, 0, 0};
    float sub_vect_2[4] = {0, 0, 0, 0};
    POINT *v0, *v1, *v2;
    
    for(int i = 0; i < numtriangles; i++)
    {
        v0 = &vertex_list[triangle_list[i].vertex[0]];
        v1 = &vertex_list[triangle_list[i].vertex[1]];
        v2 = &vertex_list[triangle_list[i].vertex[2]];
        
        subtract_vectors(v1->world, v0->world, sub_vect_1); // vertex1 - vertex0
        subtract_vectors(v2->world, v0->world, sub_vect_2); // vertex2 - vertex0
        
        // sub_vect 1 and 2 will have the vectors that we're interested in taking the cross product of.
        cross_vect(sub_vect_1, sub_vect_2, i_normal);
        normalize_vector(i_normal, i_normal);
        //i_normal will have the value that we're interested in for each triangle's normal.
        for (int j = 0; j < 4; j++)
        {
            //move over the normal data that we've calculated into the data within triangle list.
            triangle_list[i].normal[j] = i_normal[j];
        }
    }
}



void form_model(float scale)
{
    for(int i = 0; i < numvertices; i++)
    {
        vertex_list[i].position[X] = scale * vertex_list[i].world[X];
        vertex_list[i].position[Y] = scale * vertex_list[i].world[Y];
        vertex_list[i].position[Z] = vertex_list[i].world[Z];
        vertex_list[i].position[W] = vertex_list[i].world[W];
    }
}


void scale_p_model(float scale) // increases the window viewing of the scaled 3D model in position coordinates.
{
    for(int i = 0; i < numvertices; i++)
    {
        vertex_list[i].position[X] = scale * vertex_list[i].position[X];
        vertex_list[i].position[Y] = scale * vertex_list[i].position[Y];
    }
}

void p_form_model(float d, float far) // models the object stored in the vertex list using perspective techniques.
{
    float x;
    float y;
    float z;
    for(int i = 0; i < numvertices; i++)
    {
        x = vertex_list[i].world[X];
        y = vertex_list[i].world[Y];
        z = vertex_list[i].world[Z];
        
        vertex_list[i].position[X] = d * (x / z);
        vertex_list[i].position[Y] = d * (y / z);
        if(perspective_correct == OFF)
        {
            vertex_list[i].position[Z] = z / (far - d);
        }
        else
        {
            vertex_list[i].position[Z] = 1.0 / (z / (far - d));
            
            vertex_list[i].STRQ[S] *= vertex_list[i].position[Z];
            vertex_list[i].STRQ[T] *= vertex_list[i].position[Z];
        }
        //printf("our triangle has dimensions %f, %f, %f\n",vertex_list[i].position[X], vertex_list[i].position[Y], vertex_list[i].position[Z]);
    }
    
}

void t_model(float d) // translates a generated model down the Z-axis by depth d
{
    //move all points in our model into the plane by d.
    for(int i = 0; i < numvertices; i++)
    {
        vertex_list[i].world[Z] += d;
    }
}

void draw_model(void) // using the triangles stored in the triangle_list array, generates a model of the 3D solid that we've created.
{
    int j = 0;
    for(int i = 0; i < numtriangles; i++)
    {
        // for each triangle in the triangle buffer, read in the points that are contained in the vertices buffer and feed them to the drawline command.
        
        if( triangle_list[i].clip_flag == CLIPPED )
        {
            printf("ignored.\n");
            continue;
        }
        
        POINT v1 = vertex_list[triangle_list[i].vertex[0]];
        POINT v2 = vertex_list[triangle_list[i].vertex[1]];
        POINT v3 = vertex_list[triangle_list[i].vertex[2]]; // copy the points over to be passed to the drawing.
        
        //printf("Coord %f %f %f\n", v1.world[Z], v2.world[Z], v3.world[Z]);
        if(face_lighting == ON)
        {
            if(local_lighting)
            {
                float brightness = dot(triangle_list[i].normal, v1.light);
                mult_scalar_vector( v1.RGBA, brightness, v1.RGBA);
                mult_scalar_vector( v2.RGBA, brightness, v2.RGBA);
                mult_scalar_vector( v3.RGBA, brightness, v3.RGBA);
            }
            else
            {
                float brightness = dot(triangle_list[i].normal, light);
                mult_scalar_vector( v1.RGBA, brightness, v1.RGBA);
                mult_scalar_vector( v2.RGBA, brightness, v2.RGBA);
                mult_scalar_vector( v3.RGBA, brightness, v3.RGBA);
            }
        }
        
        if( rasterize == OFF )
        {
            draw_line_from_Chris(&v1, &v2, DRAW, 1.0);
            draw_line_from_Chris(&v2, &v3, DRAW, 1.0);
            draw_line_from_Chris(&v3, &v1, DRAW, 1.0);
        }
        else
        {
            draw_triangle_barycentric( &v1, &v2, &v3 );
        }
        j = i;
    }
    
}

void draw_model_rast(void)
{
    for(int i = 0; i < numtriangles; i++)
    {
        // for each triangle in the triangle buffer, read in the points that are contained in the vertices buffer and feed them to the drawline command.
        draw_triangle_barycentric(&vertex_list[triangle_list[i].vertex[0]], &vertex_list[triangle_list[i].vertex[1]], &vertex_list[triangle_list[i].vertex[2]]);
    }
}
void rotate_model_xy(float x_angle, float y_angle, float z_angle) //rotates the 3D model that we've generated in the world coordinate space before it's moved to the position coordinate space.
{
    float xprime;
    float yprime;
    float zprime;
    
    for(int i = 0; i < numvertices; i++)
    {
        // for every point in the vertices, change the spatial x, y, z dimensions using the input angles and the point world properties
        xprime = vertex_list[i].world[X] * cos(z_angle / 360.0 * 2.0 * 3.1415926) - vertex_list[i].world[Y] * sin(z_angle / 360.0 * 2.0 * 3.1415926);
        yprime = vertex_list[i].world[X] * sin(z_angle / 360.0 * 2.0 * 3.1415926) + vertex_list[i].world[Y] * cos(z_angle / 360.0 * 2.0 * 3.1415926);
        
        
        vertex_list[i].world[X] = xprime;
        vertex_list[i].world[Y] = yprime;
    }
    for(int i = 0; i < numvertices; i++)
    {
        // for every point in the vertices, change the spatial x, y, z dimensions using the input angles and the point world properties
        xprime = vertex_list[i].world[X] * cos(y_angle / 360.0 * 2.0 * 3.1415926) - vertex_list[i].world[Z] * sin(y_angle / 360.0 * 2.0 * 3.1415926);
        zprime = vertex_list[i].world[X] * sin(y_angle / 360.0 * 2.0 * 3.1415926) + vertex_list[i].world[Z] * cos(y_angle / 360.0 * 2.0 * 3.1415926);
        
        
        vertex_list[i].world[X] = xprime;
        vertex_list[i].world[Z] = zprime;
    }
    for(int i = 0; i < numvertices; i++)
    {
        // for every point in the vertices, change the spatial x, y, z dimensions using the input angles and the point world properties
        yprime = vertex_list[i].world[Y] * cos(x_angle / 360.0 * 2.0 * 3.1415926) - vertex_list[i].world[Z] * sin(x_angle / 360.0 * 2.0 * 3.1415926);
        zprime = vertex_list[i].world[Y] * sin(x_angle / 360.0 * 2.0 * 3.1415926) + vertex_list[i].world[Z] * cos(x_angle / 360.0 * 2.0 * 3.1415926);
        
        
        vertex_list[i].world[Y] = yprime;
        vertex_list[i].world[Z] = zprime;
    }
}

void init_mesh(float scale, int centerx, int centery, int centerz)
{
    // fill the vertex list with points for each row and column such that the points are 1/31-31/31 in the x and 1/31-31/31 for the y.
    int c_v = 0;
    
    float u;
    float v;
    
    for(float y = 0; y < 32; y += 1)
    {
        for(float x = 0; x < 32; x += 1)
        {
            u = (float)(x / 31.0);
            v = (float)(y / 31.0);
            vertex_list[c_v].world[X] = scale * u + centerx;
            vertex_list[c_v].world[Y] = scale * v + centery;
            vertex_list[c_v].STRQ[S] = scale * u + centerx;
            vertex_list[c_v].STRQ[T] = scale * v + centery;
            vertex_list[c_v].world[Z] = cos(((float) x / 31.0 * 2.0 * 3.1415926) + wave) * sin(((float)y/31.0 * 2.0 * 3.1415926) + wave) + centerz;
            vertex_list[c_v].RGBA[R] = 1.0;
            vertex_list[c_v].RGBA[G] = 1.0;
            vertex_list[c_v].RGBA[B] = 1.0;
            
            vertex_list[c_v].num_tri = 0;
            c_v++;
        }
    }
    numvertices = c_v;
    
    //once the vertext list is full, read the list into the triangle_list.
    int n = 0;
    for (int row = 0; row < 31; row++)
    {
        for(int col = 0; col < 31; col++)
        {
            triangle_list[n].vertex[0] = (row * 32) + col; // location of the first vertex in the triangle
            vertex_list[(row * 32) + col].tri_list[vertex_list[(row * 32) + col].num_tri++] = n;
            
            triangle_list[n].vertex[1] = ((row + 1) * 32) + col;
            vertex_list[((row + 1) * 32) + col].tri_list[vertex_list[((row + 1) * 32) + col].num_tri++] = n;
            
            triangle_list[n].vertex[2] = ((row + 1) * 32) + (col + 1);
            vertex_list[((row + 1) * 32) + (col + 1)].tri_list[vertex_list[((row + 1) * 32) + (col + 1)].num_tri++] = n;
            
            n++;
            
            triangle_list[n].vertex[0] = (row * 32) + col;
            vertex_list[(row * 32) + col].tri_list[vertex_list[(row * 32) + col].num_tri++] = n;
            
            triangle_list[n].vertex[1] = ((row + 1) * 32) + (col + 1);
            vertex_list[((row + 1) * 32) + (col + 1)].tri_list[vertex_list[((row + 1) * 32) + (col + 1)].num_tri++] = n;
            
            triangle_list[n].vertex[2] = (row * 32) + (col + 1);
            vertex_list[(row * 32) + (col + 1)].tri_list[vertex_list[(row * 32) + (col + 1)].num_tri++] = n;
            
            n++;
            
            //creates two trianges that are within the same bin in the row slots.
        }
    }
    numtriangles = n;
}

void init_plane()
{
    // two triangles that cover the entire texture coordinate space
    
    vertex_list[0].world[X] = -0.5;
    vertex_list[0].world[Y] = 0.5;
    vertex_list[0].world[Z] = 0.0;
    vertex_list[0].STRQ[S] = 0.0;
    vertex_list[0].STRQ[T] = 0.0;//top left
    vertex_list[0].RGBA[R] = 1.0;
    vertex_list[0].RGBA[G] = 1.0;
    vertex_list[0].RGBA[B] = 1.0;
    
    vertex_list[1].world[X] = -0.5;
    vertex_list[1].world[Y] = -0.5;
    vertex_list[1].world[Z] = 0.0;
    vertex_list[1].STRQ[S] = 0.0;
    vertex_list[1].STRQ[T] = 1.0;//b left
    
    vertex_list[2].world[X] = 0.5;
    vertex_list[2].world[Y] = -0.5;
    vertex_list[2].world[Z] = 0.0;
    vertex_list[2].STRQ[S] = 1.0;
    vertex_list[2].STRQ[T] = 1.0;//b right
    
    vertex_list[3].world[X] = -0.5;
    vertex_list[3].world[Y] = 0.5;
    vertex_list[3].world[Z] = 0.0;
    vertex_list[3].STRQ[S] = 0.0;
    vertex_list[3].STRQ[T] = 0.0;//t l
    
    vertex_list[4].world[X] = 0.5;
    vertex_list[4].world[Y] = -0.5;
    vertex_list[4].world[Z] = 0.0;
    vertex_list[4].STRQ[S] = 1.0;
    vertex_list[4].STRQ[T] = 1.0;
    
    vertex_list[5].world[X] = 0.5;
    vertex_list[5].world[Y] = 0.5;
    vertex_list[5].world[Z] = 0.0;
    vertex_list[5].STRQ[S] = 1.0;
    vertex_list[5].STRQ[T] = 0.0;
    
    triangle_list[0].vertex[0] = 0;
    triangle_list[0].vertex[1] = 1;
    triangle_list[0].vertex[2] = 2;
    
    triangle_list[1].vertex[0] = 3;
    triangle_list[1].vertex[1] = 4;
    triangle_list[1].vertex[2] = 5;
    
    numvertices = 6;
    numtriangles = 2;
    
    
    
}

void set_vect(float out[4], float x, float y, float z)
{
    out[X] = x;
    out[Y] = y;
    out[Z] = z;
}

void calculate_vertex_normals(void)
{
    //process all vertices in the vertex list. From vertices, get normals from all of the triangles that it is a constituent of and average them.
    // Store the new normal average in the normal array for the vertex itself.
    
    for(int i = 0; i < numvertices; i++)
    {
        POINT *p = &vertex_list[i];
        
        if(p->num_tri > 0)
        {
            set_vect(p->normal, 0, 0, 0); // set the normal vector in the vertex to be 0, 0, 0.
            // If the vertex is a part of any triangles:
            for(int j = 0; j < p->num_tri; j++)
            {
                add_vectors(p->normal,triangle_list[p->tri_list[j]].normal ,p->normal); // sum all of the normals of faces that the vertex is a part of.
            }
            div_scalar_vector(p->normal, (float)p->num_tri, p->normal);
        }
    }
}

float dot(float vec1[4], float vec2[4])
{
    return (vec1[X] * vec2[X]) + (vec1[Y] * vec2[Y]) + (vec1[Z] * vec2[Z]);
}


void normalize_vector(float in[4], float out[4])
{
    float length = length_vector(in);
    div_scalar_vector(in, length, out);
}

void show_color_buffer( void )
{
    glBegin(GL_POINTS);
    
    for( int y = 0; y < 800; y++)
    {
        for( int x = 0; x < 800; x++)
        {
            glColor4f( c_buff[y][x][R], c_buff[y][x][G], c_buff[y][x][B], c_buff[y][x][A] );
            glVertex2f(x-400, y-400);
        }
    }
    
    glEnd();
}

void init_sphere(float scale, float radius, float centerx, float centery, float centerz)
{
    // fill the vertex list with points for each row and column such that the points are 1/31-31/31 in the x and 1/31-31/31 for the y.
    
    int c_v = 0;
    
    if(multiple == ON)
        c_v = numvertices;
    
    int made_vertices = 0;
    /*We want to use the above to keep track of how many vertices are a part of the current triangle and how many
     are a part of previous triangles. If we're trying to create a new 3D solid, we want to scale it up to the vertices that are a part of the new triangle, but we also want to generate the new vertices in the correct place.*/
    float u;
    float v;
    float PI = 3.1415926;
    
    for(float y = 0; y < 32; y += 1)
    {
        for(float x = 0; x < 32; x += 1)
        {
            u = (float)(x / 31.0);
            v = (float)(y / 31.0);
            vertex_list[c_v].world[X] = radius * cos( u * 1 * PI) + centerx;
            
            vertex_list[c_v].world[Y] = radius * cos( v * 2 * PI) * sin( u * 1 * PI) + centery;
            
            vertex_list[c_v].world[Z] = radius * sin( v * 2 * PI) * sin( u * 1 * PI) + centerz;
            
            vertex_list[c_v].STRQ[S] = u;
            vertex_list[c_v].STRQ[T] = v;
            
            
            vertex_list[c_v].RGBA[R] = 1.0;
            vertex_list[c_v].RGBA[G] = 1.0;
            vertex_list[c_v].RGBA[B] = 1.0;
            
            vertex_list[c_v].num_tri = 0;
            c_v++;
            made_vertices++;
        }
    }
    
    numvertices = c_v;
    
    
    //once the vertext list is full, read the list into the triangle_list.
    int n = 0;
    if(multiple == ON)
    {
        n = numtriangles;
    }
    for (int row = 0; row < 31; row++)
    {
        for(int col = 0; col < 31; col++)
        {
            triangle_list[n].vertex[0] = (row * 32) + col + (c_v - made_vertices); // location of the first vertex in the triangle
            vertex_list[(row * 32) + col + (c_v - made_vertices)].tri_list[vertex_list[(row * 32) + col + (c_v - made_vertices)].num_tri++] = n;
            
            triangle_list[n].vertex[1] = ((row + 1) * 32) + col + (c_v - made_vertices);
            vertex_list[((row + 1) * 32) + col].tri_list[vertex_list[((row + 1) * 32) + col + (c_v - made_vertices)].num_tri++] = n;
            
            triangle_list[n].vertex[2] = ((row + 1) * 32) + (col + 1) + (c_v - made_vertices);
            vertex_list[((row + 1) * 32) + (col + 1) + (c_v - made_vertices)].tri_list[vertex_list[((row + 1) * 32) + (col + 1) + (c_v - made_vertices)].num_tri++] = n;
            
            n++;
            
            triangle_list[n].vertex[0] = (row * 32) + col + (c_v - made_vertices);
            vertex_list[(row * 32) + col + (c_v - made_vertices)].tri_list[vertex_list[(row * 32) + col + (c_v - made_vertices)].num_tri++] = n;
            
            triangle_list[n].vertex[1] = ((row + 1) * 32) + (col + 1) + (c_v - made_vertices);
            vertex_list[((row + 1) * 32) + (col + 1) + (c_v - made_vertices)].tri_list[vertex_list[((row + 1) * 32) + (col + 1) + (c_v - made_vertices)].num_tri++] = n;
            
            triangle_list[n].vertex[2] = (row * 32) + (col + 1) + (c_v - made_vertices);
            vertex_list[(row * 32) + (col + 1) + (c_v - made_vertices)].tri_list[vertex_list[(row * 32) + (col + 1) + (c_v - made_vertices)].num_tri++] = n;
            
            n++;
            
            //creates two trianges that are within the same bin in the row slots.
        }
    }
    numtriangles = n;
}

void init_cylinder(float scale, float radius, int center_x, int center_y, int center_z)
{
    int c_v = 0;
    
    float u;
    float v;
    float PI = 3.1415926;
    
    for(float y = 0; y < 32; y += 1)
    {
        for(float x = 0; x < 32; x += 1)
        {
            u = (float)(x / 31.0);
            v = (float)(y / 31.0);
            vertex_list[c_v].world[X] = radius * cos( u * 2 * PI) + center_x;
            
            vertex_list[c_v].world[Y] = radius * sin( u * 2 * PI) + center_y;
            
            vertex_list[c_v].world[Z] = scale * v + center_z;
            
            vertex_list[c_v].STRQ[S] = scale * u + center_x;
            
            vertex_list[c_v].STRQ[T] = scale * v + center_y;
            
            
            vertex_list[c_v].RGBA[R] = 1.0;
            vertex_list[c_v].RGBA[G] = 1.0;
            vertex_list[c_v].RGBA[B] = 1.0;
            
            vertex_list[c_v].num_tri = 0;
            c_v++;
        }
    }
    numvertices = c_v;
    
    //once the vertext list is full, read the list into the triangle_list.
    int n = 0;
    for (int row = 0; row < 31; row++)
    {
        for(int col = 0; col < 31; col++)
        {
            triangle_list[n].vertex[0] = (row * 32) + col; // location of the first vertex in the triangle
            vertex_list[(row * 32) + col].tri_list[vertex_list[(row * 32) + col].num_tri++] = n;
            
            triangle_list[n].vertex[1] = ((row + 1) * 32) + col;
            vertex_list[((row + 1) * 32) + col].tri_list[vertex_list[((row + 1) * 32) + col].num_tri++] = n;
            
            triangle_list[n].vertex[2] = ((row + 1) * 32) + (col + 1);
            vertex_list[((row + 1) * 32) + (col + 1)].tri_list[vertex_list[((row + 1) * 32) + (col + 1)].num_tri++] = n;
            
            n++;
            
            triangle_list[n].vertex[0] = (row * 32) + col;
            vertex_list[(row * 32) + col].tri_list[vertex_list[(row * 32) + col].num_tri++] = n;
            
            triangle_list[n].vertex[1] = ((row + 1) * 32) + (col + 1);
            vertex_list[((row + 1) * 32) + (col + 1)].tri_list[vertex_list[((row + 1) * 32) + (col + 1)].num_tri++] = n;
            
            triangle_list[n].vertex[2] = (row * 32) + (col + 1);
            vertex_list[(row * 32) + (col + 1)].tri_list[vertex_list[(row * 32) + (col + 1)].num_tri++] = n;
            
            n++;
            
            //creates two trianges that are within the same bin in the row slots.
        }
    }
    numtriangles = n;
}

void init_cone(float scale, float radius, int center_x, int center_y, int center_z)
{
    int c_v = 0;
    
    float u;
    float v;
    float PI = 3.1415926;
    float r;
    
    for(float y = 0; y < 32; y += 1)
    {
        for(float x = 0; x < 32; x += 1)
        {
            u = (float)(x / 31.0);
            v = (float)(y / 31.0);
            r = radius * ((float)1.0) - y / 1.0;
            
            printf("r=%f\n", r);
            
            vertex_list[c_v].world[X] = r * cos( u * 2 * PI) + center_x;
            
            vertex_list[c_v].world[Y] = radius * sin( u * 2 * PI) + center_y;
            
            vertex_list[c_v].world[Z] = scale * v + center_z;
            
            vertex_list[c_v].STRQ[S] = scale * u + center_x;
            
            vertex_list[c_v].STRQ[T] = scale * v + center_y;
            
            
            vertex_list[c_v].RGBA[R] = 1.0;
            vertex_list[c_v].RGBA[G] = 1.0;
            vertex_list[c_v].RGBA[B] = 1.0;
            
            vertex_list[c_v].num_tri = 0;
            c_v++;
        }
    }
    numvertices = c_v;
    
    //once the vertext list is full, read the list into the triangle_list.
    int n = 0;
    for (int row = 0; row < 31; row++)
    {
        for(int col = 0; col < 31; col++)
        {
            triangle_list[n].vertex[0] = (row * 32) + col; // location of the first vertex in the triangle
            vertex_list[(row * 32) + col].tri_list[vertex_list[(row * 32) + col].num_tri++] = n;
            
            triangle_list[n].vertex[1] = ((row + 1) * 32) + col;
            vertex_list[((row + 1) * 32) + col].tri_list[vertex_list[((row + 1) * 32) + col].num_tri++] = n;
            
            triangle_list[n].vertex[2] = ((row + 1) * 32) + (col + 1);
            vertex_list[((row + 1) * 32) + (col + 1)].tri_list[vertex_list[((row + 1) * 32) + (col + 1)].num_tri++] = n;
            
            n++;
            
            triangle_list[n].vertex[0] = (row * 32) + col;
            vertex_list[(row * 32) + col].tri_list[vertex_list[(row * 32) + col].num_tri++] = n;
            
            triangle_list[n].vertex[1] = ((row + 1) * 32) + (col + 1);
            vertex_list[((row + 1) * 32) + (col + 1)].tri_list[vertex_list[((row + 1) * 32) + (col + 1)].num_tri++] = n;
            
            triangle_list[n].vertex[2] = (row * 32) + (col + 1);
            vertex_list[(row * 32) + (col + 1)].tri_list[vertex_list[(row * 32) + (col + 1)].num_tri++] = n;
            
            n++;
            
            //creates two trianges that are within the same bin in the row slots.
        }
    }
    numtriangles = n;
}

void init_torus(float tube_radius, float hole_radius, float scale, float center_x, float center_y, float center_z)
{
    int c_v = 0;
    
    float u;
    float v;
    float PI = 3.1415926;
    
    for(float y = 0; y < 32; y += 1)
    {
        for(float x = 0; x < 32; x += 1)
        {
            u = (float)(x / 31.0);
            v = (float)(y / 31.0);
            vertex_list[c_v].world[X] = ( tube_radius * cos( u * 2 * PI ) + hole_radius) * cos( v * 2 * PI ) + center_x;
            
            vertex_list[c_v].world[Y] = ( tube_radius * sin( u * 2 * PI ) ) + center_y;
            
            vertex_list[c_v].world[Z] = (tube_radius * cos( u * 2 * PI) + hole_radius) * sin( v * 2 * PI) + center_z;
            
            vertex_list[c_v].STRQ[S] = scale * u + center_x;
            
            vertex_list[c_v].STRQ[T] = scale * v + center_y;
            
            
            vertex_list[c_v].RGBA[R] = 1.0;
            vertex_list[c_v].RGBA[G] = 1.0;
            vertex_list[c_v].RGBA[B] = 1.0;
            
            vertex_list[c_v].num_tri = 0;
            c_v++;
        }
    }
    numvertices = c_v;
    
    //once the vertext list is full, read the list into the triangle_list.
    int n = 0;
    for (int row = 0; row < 31; row++)
    {
        for(int col = 0; col < 31; col++)
        {
            triangle_list[n].vertex[0] = (row * 32) + col; // location of the first vertex in the triangle
            vertex_list[(row * 32) + col].tri_list[vertex_list[(row * 32) + col].num_tri++] = n;
            
            triangle_list[n].vertex[1] = ((row + 1) * 32) + col;
            vertex_list[((row + 1) * 32) + col].tri_list[vertex_list[((row + 1) * 32) + col].num_tri++] = n;
            
            triangle_list[n].vertex[2] = ((row + 1) * 32) + (col + 1);
            vertex_list[((row + 1) * 32) + (col + 1)].tri_list[vertex_list[((row + 1) * 32) + (col + 1)].num_tri++] = n;
            
            n++;
            
            triangle_list[n].vertex[0] = (row * 32) + col;
            vertex_list[(row * 32) + col].tri_list[vertex_list[(row * 32) + col].num_tri++] = n;
            
            triangle_list[n].vertex[1] = ((row + 1) * 32) + (col + 1);
            vertex_list[((row + 1) * 32) + (col + 1)].tri_list[vertex_list[((row + 1) * 32) + (col + 1)].num_tri++] = n;
            
            triangle_list[n].vertex[2] = (row * 32) + (col + 1);
            vertex_list[(row * 32) + (col + 1)].tri_list[vertex_list[(row * 32) + (col + 1)].num_tri++] = n;
            
            n++;
            
            //creates two trianges that are within the same bin in the row slots.
        }
    }
    numtriangles = n;
}

void rand_mult_coord_fill()
{
    //printf("called!\n");
    current_coord = 0; // this is to reset the external last-accessed coordinate index.
    for(int i = 0; i < 1000; i++) // place three random values between -400 and +400 into the x/y/z space of coords.
    {
        mult_coords[i][0] = random_float(-5, 5);
        mult_coords[i][1] = random_float(-5, 5);
        mult_coords[i][2] = random_float(-5, 5);
        
    }
}

void draw_n_spheres(int n) // place n spheres into the vertex_list buffer but should really just export the spheres into the color buffer instead.
{
    float x, y, z;
    
    for(int j = 0; j < n; j++)
    {
        clear_d_buff(1000000);
        printf("coord is %d\n", current_coord);
        if(numvertices < 100000) // as long as there's still space to put another vertex
        {
            printf("Drawing sphere %d\n", j);
            x = mult_coords[current_coord][X]; // rand x coord
            y = mult_coords[current_coord][Y]; // rand y coord
            z = mult_coords[current_coord++][Z]; // rand z coord
            printf("Sphere should have coords %f, %f, %f\n", x, y, z);
            
            init_sphere(0.5, 0.5, x, y, z);
        }
    }
    current_coord = 0;
}

void r_obj_file_scenter(char *name, float cx, float cy, float cz)
{
    numvertices = 0; // reading in a new object
    numtriangles = 0;
    FILE *fp;
    fp = fopen(name, "r");
    float x, y, z;
    int i, j, k;
    
    while(fscanf(fp, "v %f %f %f\n", &x, &y, &z) == 3)
    {//read in the vertices from the file. Put them in the vertex list.
        vertex_list[numvertices].normal[0] = 0;
        vertex_list[numvertices].normal[1] = 0;
        vertex_list[numvertices].normal[2] = 0;
        vertex_list[numvertices].normal[3] = 0;
        vertex_list[numvertices].RGBA[R] = 1.0;
        vertex_list[numvertices].RGBA[G] = 1.0;
        vertex_list[numvertices].RGBA[B] = 1.0;
        
        vertex_list[numvertices].world[X] = x + cx;
        vertex_list[numvertices].world[Y] = y + cy;
        vertex_list[numvertices].num_tri = 0;
        vertex_list[numvertices++].world[Z] = z + cy;
        
        
    }
    int count = 0;
    while(fscanf(fp, "f %d %d %d\n", &i, &j, &k) == 3)
    {//read the indices of vertices into the triangle list. vertices are 1-ordered so must subtract 1.
        triangle_list[numtriangles].normal[0] = 0;
        triangle_list[numtriangles].normal[1] = 0;
        triangle_list[numtriangles].normal[2] = 0;
        triangle_list[numtriangles].normal[3] = 0;
        triangle_list[numtriangles].vertex[0] = i - 1;
        triangle_list[numtriangles].vertex[1] = j - 1;
        triangle_list[numtriangles++].vertex[2] = k - 1;
        vertex_list[i - 1].tri_list[vertex_list[i - 1].num_tri] = count; // the vertex adding for i knows which triangle it's a part of.
        vertex_list[j - 1].tri_list[vertex_list[j - 1].num_tri] = count;
        vertex_list[k - 1].tri_list[vertex_list[k - 1].num_tri++] = count;
        count++;
    }
    fclose(fp);
}

void draw_n_teapots(int n)
{
    float x, y, z;
    
    for(int i = 0; i < n; i++)
    {
        x = mult_coords[i][X];
        y = mult_coords[i][Y];
        z = mult_coords[i][Z];
        r_obj_file_scenter( "teapot.obj", x, y, z );
        render_object();
        
    }
}


void write_object_file()
{ // need to be able to pass in an array of chars that contain the name.
    
    FILE *fp;
    
    fp = fopen("manysphere.txt", "w+");
    
    int n = 0;
    float x, y, z;
    int i, j, k;
    while( n < numvertices )
    {
        x = vertex_list[n].world[X];
        y = vertex_list[n].world[Y];
        z = vertex_list[n].world[Z];
        
        fprintf(fp, "v %f %f %f\n", x, y, z);
        n++;
    }
    
    int m = 0;
    while( m < numtriangles )
    {
        i = triangle_list[m].vertex[0];
        j = triangle_list[m].vertex[1];
        k = triangle_list[m].vertex[2];
        
        fprintf(fp, "f %d %d %d\n", i + 1, j + 1, k + 1);
        m++;
    }
    
    fclose(fp);
    
}



/*
 reads in a file of type txt that will contain a file-format that supports the creation of multiple kinds of objects at specific locations.
 Reads the name of the object, which specifies the path that it will take. The fscanf read format is conserved across different kinds of objects, so there will be a constant amount of space required to create an object of any type.
 EOF will be a first character on a line of '0'. If it is reached, then all objects will have been drawn.
 
 */
void read_newobj_file(char *name)
{
    char a, b, c, d, e, f, g, h; // the characters that make up the object name.
    float scale, radius, cx, cy, cz, tube_radius, hole_radius; // the float values that will make up the parameters of the object.
    FILE *fp;
    fp = fopen(name, "r");
    
    while(1)//will have a breakout case.
    {
        fscanf(fp, "%c%c%c%c%c%c%c%c, %f, %f, %f, %f, %f, %f, %f", &a, &b, &c, &d, &e, &f, &g, &h, &scale, &radius, &cx, &cy, &cz, &tube_radius, &hole_radius);
        
        if(a == '0')
        {
            //printf("End of object list!\n");
            break;
        }
        else if(a == 'T') // if the object is a torus
        {
            printf("torus being drawn\n");
            printf("%f, %f, %f, %f, %f, %f\n", tube_radius, hole_radius, scale, cx, cy, cz);
            init_torus(tube_radius, hole_radius, scale, cx, cy, cz);
            render_object();
        }
        else if(a == 'S')
        {
            init_sphere(scale, radius, cx, cy, cz);
            render_object();
        }
        else if(a == 'C') // should also be able to take cone using b. Since cone is broken we won't worry about it.
        {
            init_cylinder(scale, radius, cx, cy, cz);
            render_object();
        }
        else
        {
            printf("a = %c\n", a);
            printf("That is not a valid object.\n");
        }
    }
}

/*
 All of the below rendering relies on global-scope variables and arrays. This means that it can be called after any initialization, independent of when the buffers are cleared.
 */
void render_object()
{
    //write_object_file();
    
    //    if(r_mesh == ON)
    //    {
    //        init_mesh(1.0, 0, 0, 0);
    //    }
    //
    //    if(r_plane == ON)
    //    {
    //        init_plane();
    //    }
    //
    //    if(r_cylinder == ON)
    //    {
    //        init_cylinder(1.0, 0.5, 0, 0, 0);
    //    }
    //
    //    if(r_sphere == ON)
    //    {
    //        init_sphere(1, 0.5, 0, 0, 0);
    //    }
    //
    //    if(r_cone == ON)
    //    {
    //        r_obj_file("teapot.obj");
    //    }
    //
    //    if(r_torus == ON)
    //    {
    //        init_torus(0.25, 0.75, 1.0, 0, 0, 0);
    //    }
    //    if(multiple == ON)
    //    {
    //        draw_n_spheres(5);
    //    }
    //
    //
    //    if(write == ON)
    //    {
    //      write_object_file();
    //    }
    
    
    
    //hecker_board_text(&starter_texture);
    
    copy_texture(&starter_texture); // copy the data from the written image data to our current texture, which the below functions will display.
    
    
    light[0] = 1.0;
    light[1] = 1.0;
    light[2] = 1.0;
    light[3] = 1.0;
    
    
    rotate_model_xy(xangle, yangle, zangle); // rotate our model by the coordinates we put in.
    
    
    cal_face_normal();
    calculate_vertex_normals();
    
    // From here on, working with screen coord
    t_model(5.0); // translate model
    
    if(perspective_draw == ON)
    {
        p_form_model(distance, far);
    }
    else
    {
        form_model(1.0);
    }
    
    scale_p_model(50.0);
    draw_model();
}

void colorbuffer_to_image(IMAGE *image) // converts c_buff to image. c_buff is global in this doc.
{
    image->width = 800;
    image->height = 800;
    
    for( int j = 0; j < image->height; j++ )
    {
        for( int i = 0; i < image->width; i++ )
        {
            int r = (int)(c_buff[j][i][R] * 255);
            int g = (int)(c_buff[j][i][G] * 255);
            int b = (int)(c_buff[j][i][B] * 255);
            
            image->data[j][i][R] = CLAMP( r, 0, 255 );
            image->data[j][i][G] = CLAMP( g, 0, 255 );
            image->data[j][i][B] = CLAMP( b, 0, 255 );
        }
    }
}

void image_to_colorbuffer(IMAGE *image) // converts image struct to c_buff data. c_buff is global.
{
    for( int j = 0; j < image->height; j++ )
    {
        for( int i = 0; i < image->width; i++ )
        {
            c_buff[j][i][R] = image->data[j][i][R] / 255.0;
            c_buff[j][i][G] = image->data[j][i][G] / 255.0;
            c_buff[j][i][B] = image->data[j][i][B] / 255.0;
        }
    }
}

void apply_post_processing_thompson(void)
{
    printf("being called\n");
    colorbuffer_to_image(&postprocessingimage);
    thompson_rotation(&postprocessingimage, &postprocessingimagefinal);
    image_to_colorbuffer(&postprocessingimagefinal);
}

void apply_post_processing_humed(void)
{
    printf("being called\n");
    colorbuffer_to_image(&postprocessingimage);
    humed_mirror(&postprocessingimage, &postprocessingimagefinal);
    image_to_colorbuffer(&postprocessingimagefinal);
}

void depth_buffer_to_image(IMAGE *image) // converts the depth buffer to a monochrome image that can be used as a mask.
{
    image->height = 800;
    image->width = 800;
    for( int j = 0; j < image->height; j++ )
    {
        for( int i = 0; i < image->width; i++ )
        {
            int r = (int)((1 - (d_buff[j][i] / max_depth)) * 255); // all color channels become the depth-buffer value in 255.0 space.
            
            image->data[j][i][R] = CLAMP( r, 0, 255 );
            image->data[j][i][G] = CLAMP( r, 0, 255 );
            image->data[j][i][B] = CLAMP( r, 0, 255 ); // all the same value- should be monochrome
        }
    }
}

/* reads across input image with a 5x5 box filter that averages the surrounding 3x3 valid pixelspace and writes it to the output image. */
void blur(IMAGE *input, IMAGE *output)
{
    float current_sum[4] = { 0, 0, 0, 0};
    int good_pixels = 0;
    output->height = input->height;
    output->width = input->width; // makes the dimensions of the input/outputs the same.
    
    for( int j = 0; j < input->height; j++ )
    {
        for( int i = 0; i < input->width; i++ )
        {
            for( int k = j - 2; k <= j + 2; k++ ) // beginning of gaussian mask.
            {
                for( int l = i - 2; l <= i + 2; l++ )
                {
                    if ( k > 0 && k < input->height ) // two conditions check if the pixel being checked is valid.
                    {
                        if( l > 0 && l < input->width )
                        {
                            good_pixels++;
                            current_sum[R] += (float)input->data[k][l][R];
                            current_sum[G] += (float)input->data[k][l][G];
                            current_sum[B] += (float)input->data[k][l][B];
                            current_sum[A] += (float)input->data[k][l][A];
                            
                        }
                    }
                }
            }
            current_sum[R] /= good_pixels; // average the current_sum values be the number of pixels averaged.
            current_sum[G] /= good_pixels;
            current_sum[B] /= good_pixels;
            current_sum[A] /= good_pixels;
            
            output->data[j][i][R] = (unsigned char)current_sum[R]; // write the averaged pixel data to the output->data field.
            output->data[j][i][G] = (unsigned char)current_sum[G];
            output->data[j][i][B] = (unsigned char)current_sum[B];
            output->data[j][i][A] = (unsigned char)current_sum[A];
            
            current_sum[0] = 0; // reset current_sum to zeros
            current_sum[1] = 0;
            current_sum[2] = 0;
            current_sum[3] = 0;
            
            good_pixels = 0;
        }
    }
}

void pingpong_blur(IMAGE *input, IMAGE *output, int blurs) // blurs == number of times that we'll pingpong blue back and forth.
{
    
    for(int n = 0; n < blurs; n++)
    {
        blur(input, output); // read input, write output.
        blur(output, input); // read output, write input.
    }
}

/*
 Takes in input, and copies it to a placeholder struct that will be blurred. Blur n times. Final output per-pixel will be product of sharp image, blurred image, and the mask-value for each color channel.
 */

void blend_with_mask(IMAGE *blur, IMAGE *sharp, IMAGE *mask, IMAGE *output)
{
    for( int j = 0; j < sharp->height; j++ )
    {
        for( int i = 0; i < sharp->width; i++ )
        {
            float redchannel =   ((sharp->data[j][i][R] * (mask->data[j][i][R] / 255.0)) + ((1 - (mask->data[j][i][R] / 255.0)) * blur->data[j][i][R]));
            float greenchannel = ((sharp->data[j][i][G] * (mask->data[j][i][G] / 255.0)) + ((1 - (mask->data[j][i][G] / 255.0)) * blur->data[j][i][G]));
            float bluechannel =  ((sharp->data[j][i][B] * (mask->data[j][i][B] / 255.0)) + ((1 - (mask->data[j][i][B] / 255.0)) * blur->data[j][i][B]));
            float alphachannel = ((sharp->data[j][i][A] * (mask->data[j][i][A] / 255.0)) + ((1 - (mask->data[j][i][A] / 255.0)) * blur->data[j][i][A]));
            
            CLAMP( redchannel,   0, 255 );
            CLAMP( greenchannel, 0, 255 );
            CLAMP( bluechannel,  0, 255 );
            CLAMP( alphachannel, 0, 255 );
            
            output->data[j][i][R] = redchannel;
            output->data[j][i][G] = greenchannel;
            output->data[j][i][B] = bluechannel;
            output->data[j][i][A] = alphachannel;
        }
    }
}

void apply_depth_of_field() // will move stuff from and back into c_buff.
{
    depth_buffer_to_image(&mask); // mask contains the image of the depth buffer.
    
    colorbuffer_to_image(&sharp);
    
    for( int j = 0; j < sharp.height; j++ ) // move all the image data from sharp to blur
    {
        for( int i = 0; i < sharp.width; i++ )
        {
            depth_blur.data[j][i][R] = sharp.data[j][i][R];
            depth_blur.data[j][i][G] = sharp.data[j][i][G];
            depth_blur.data[j][i][B] = sharp.data[j][i][B];
            depth_blur.data[j][i][A] = sharp.data[j][i][A];
        }
    }
    
    depth_blur.width = sharp.width;
    depth_blur.height = sharp.height;
    depth_blur_pair.width = depth_blur.width;
    depth_blur_pair.height = depth_blur.height;
    output.height = 800;
    output.width = 800;
    
    pingpong_blur( &depth_blur, &depth_blur_pair, 2 ); // fully blurred will still be in blur.
    
    blend_with_mask( &depth_blur, &sharp, &mask, &output);
    
    image_to_colorbuffer( &output ); // put the DOF image back into the color buffer.
}

/* Uses a vertex normal to calculate an naive approximation of the correct ST texture coordinates. Only uses basic mapping.*/
void naive_map_vect(float normal[4], float tex[4])
{
    tex[S] = (normal[X] + 1.0) / 2.0;
    tex[T] = (normal[Y] + 1.0) / 2.0;
    tex[2] = 0.0;
    tex[3] = 1.0;
    
}
/* Uses normals in vertices */
void cylindrical_map_vect( float normal[4], float tex[4] )
{
    float xsq = normal[X] * normal[X];
    float zsq = normal[Z] * normal[Z];
    float PI = 3.1415926;
    
    float radius = sqrt( xsq + zsq );
    float theta = asin( normal[Z] / radius );
    
    if(isnan(theta))
    {
        theta = 0;
    }
    
    tex[S] = (theta + PI / 2.0)/(2.0 * PI) + ((normal[Z] > 0) ? 0 : 0.5);
    tex[T] = ( normal[Y] + 1.0 ) / 2.0;
    tex[2] = 0.0;
    tex[3] = 1.0;
    
}

void spherical_map_vect( float normal[4], float tex[4] )
{
    float xsq = normal[X] * normal[X];
    float zsq = normal[Z] * normal[Z];
    float PI = 3.1415926;
    
    float radius = sqrt( xsq + zsq );
    float theta = asin( normal[Z] / radius );
    float phi = asin( normal[Y] );
    
    if(isnan(theta))
    {
        theta = 0;
    }
    if(isnan(phi))
    {
        phi = 0;
    }
    
    tex[S] = ( theta + PI / 2.0)/( PI * 2.0) + ((normal[Z] > 0) ? 0 : 0.5);
    tex[T] = (phi + PI / 2.0) / PI;
    tex[2] = 0.0;
    tex[3] = 1.0;
}

/* works through the vertices in the vertex buffer and uses the appropriate mapping algorithm to fill the [4]STRQ vector with the correct coords.*/
void map_texture_coordinates()
{
    if( texturing == ON )
    {
        if( naive_mapping == ON )
        {
            printf("Naive-mapping the current object.\n");
            for( int i = 0; i < numvertices; i++ )
            {
                naive_map_vect( vertex_list[i].normal, vertex_list[i].STRQ );
            }
        }
        else if( cylindrical_mapping == ON )
        {
            printf("Cylindrical-mapping the current object.\n");
            for( int i = 0; i < numvertices; i++ )
            {
                cylindrical_map_vect( vertex_list[i].normal, vertex_list[i].STRQ );
            }
        }
        else if( spherical_mapping == ON )
        {
            printf("Spherical-mapping the current object.\n");
            for( int i = 0; i < numvertices; i++ )
            {
                spherical_map_vect( vertex_list[i].normal, vertex_list[i].STRQ );
            }
        }
        else
        {
            printf( "Texturing mapping algorithm has not been selected.\n" );
        }
    }
    else
    {
        return;
    }
}

void reflection_map_vect( float normal[4], float tex[4] )
{
    float reflect[4];
    
    reflect_vect( eye, normal, reflect);
    spherical_map_vect(reflect, tex);
}

void cube_map_vect( float v[4], float tex[4], int *index )
{
    float maxAxis, s, t;
    float absX = fabsf( v[X] );
    float absY = fabsf( v[Y] );
    float absZ = fabsf( v[Z] );
    
    if( absX > absY && absX > absZ )
    {
        if( v[X] < 0 )
        {
            s =  v[Z];
            t =  -v[Y];
            *index = 0; // right face
            
        }
        else
        {
            s =  -v[Z];
            t =  v[Y];
            *index = 1; // left face
            
        }
    }
    else if( absY > absX && absY > absZ )
    {
        if( v[Y] < 0 )
        {
            s = -v[X];
            t =  v[Y];
            *index = 2; // top face
            
        }
        else
        {
            s =  v[X];
            t =  v[Y];
            *index = 3; // bottom face
            
        }
    }
    else if( absZ > absY && absZ > absX )
    {
        if( v[Z] > 0 )
        {
            s =   v[X];
            t =   v[Y];
            *index = 4; // back face
            
        }
        else
        {
            s =  v[X];
            t =  -v[Y];
            *index = 5; // front face
            
        }
    }
    
    maxAxis = 1.0 / sqrt( 2.0 );
    tex[S] = (( s / maxAxis ) + 1.0 ) / 2.0;
    tex[T] = (( t / maxAxis ) + 1.0 ) / 2.0;
    tex[2] = 0.0;
    tex[3] = 1.0;
    
}

/* Reads 6 textures into the cube_map IMAGE array to be used by drawpoint if cube_mapping is enabled. Texture hardcoded because 6 required.*/
void read_cube_texture_test()
{
    r_binary_text_file_two_row( &cubemap[0], "test_right.ppm");
    printf("Texture 1 in.\n");
    r_binary_text_file_two_row( &cubemap[1], "test_left.ppm");
    printf("Texture 2 in.\n");
    r_binary_text_file_two_row( &cubemap[2], "test_top.ppm");
    printf("Texture 3 in.\n");
    r_binary_text_file_two_row( &cubemap[3], "test_bottom.ppm");
    printf("Texture 4 in.\n");
    r_binary_text_file_two_row( &cubemap[4], "test_back.ppm");
    printf("Texture 5 in.\n");
    r_binary_text_file_two_row( &cubemap[5], "test_front.ppm"); // the cube_map IMAGE array should contain all of the requisite textures.
    printf("Texture 6 in.\n");
}
void read_cube_texture_sky()
{
    printf("Starting.\n");
    r_binary_text_file_two_row( &cubemap[0], "sky_right.ppm");
    printf("Texture 1 in.\n");
    r_binary_text_file_two_row( &cubemap[1], "sky_left.ppm");
    printf("Texture 2 in.\n");
    r_binary_text_file_two_row( &cubemap[2], "sky_top.ppm");
    printf("Texture 3 in.\n");
    r_binary_text_file_two_row( &cubemap[3], "sky_bottom.ppm");
    printf("Texture 4 in.\n");
    r_binary_text_file_two_row( &cubemap[4], "sky_back.ppm");
    printf("Texture 5 in.\n");
    r_binary_text_file_two_row( &cubemap[5], "sky_front.ppm"); // the cube_map IMAGE array should contain all of the requisite textures.
    printf("Texture 6 in.\n");
}

void write_timer_savefile()
{ // need to be able to pass in an array of chars that contain the name.
    
    FILE *fp;
    
    fp = fopen("teapot.txt", "w+");
    
    int n = 0;
    float x, y;
    int swp = current_savefile.current_sw;
    int glp = current_savefile.current_gl;
    fprintf(fp, "F1\n");
    fprintf(fp, "%d\n", swp);
    while( n <= current_savefile.current_sw )
    {
        x = current_savefile.sw_time[n++];
        fprintf(fp, "%f\n", x);
    }
    
    int m = 0;
    while( m <= current_savefile.current_gl )
    {
        x = current_savefile.gl_time[m++];
        fprintf(fp, "%f\n", x);
    }
    
    fclose(fp);
    printf("Timer savefile written!\n");
}
void caluculate_light_vectors() // will give us the normalized distance between the light point and the object point.
{
    POINT *p;
    
    for( int i = 0; i < numvertices; i++)
    {
        p = &vertex_list[i];
        subtract_vectors(light_position, p->world, p->light);
        normalize_vector(p->light, p->light);
    }
}

void setup_clip_frustum( void )

{

    float inv_sqrt_2 = 1.0/sqrt(2.0);



    // left

    set_vect( frustum[0].normal,  inv_sqrt_2, 0, -inv_sqrt_2 );

    frustum[0].dist = 0;



    // right

    set_vect( frustum[1].normal, -inv_sqrt_2, 0, -inv_sqrt_2 );

    frustum[1].dist = 0;



    // top

    set_vect( frustum[2].normal, 0, -inv_sqrt_2, -inv_sqrt_2 );

    frustum[2].dist = 0;



    // bottom

    set_vect( frustum[3].normal, 0,  inv_sqrt_2, -inv_sqrt_2 );

    frustum[3].dist = 0;



    // near

    set_vect( frustum[4].normal, 0, 0, -1 );

    frustum[4].dist = 2.0;



    //far

    set_vect( frustum[5].normal, 0, 0, 1 );

    frustum[5].dist = -30.0;

}


/*

* vect_distance_to_plane()

*/

float vect_distance_to_plane( float v[4], PLANE *p )
{

    return( dot(v, p->normal) - p->dist);

}


/*

* line_clip_plane()

*/

int line_clip_plane( POINT *a, POINT *b, POINT *new, int k )

{

    float   da = a->dist[k]; // temp chrisb vect_distance_to_plane( a->world, p );   // distance from plane to point a

    float   db = b->dist[k]; // temp chrisb vect_distance_to_plane( b->world, p );   // distance from plane to point b

    float   s;



    if( da < 0 && db < 0 )          // both points outside plane

    {

        return( -1 );

    }

    else if( da > 0 && db > 0 )     // both points inside plane

    {

        return( 1 );

    }

    else

    {

        s = da/(da-db);             // intersection factor (between 0 and 1)



        interpolate_two_vect( s, b->world,  a->world,   new->world    );

        interpolate_two_vect( s, b->RGBA,  a->RGBA,   new->RGBA    );

        interpolate_two_vect( s, b->STRQ,    a->STRQ,     new->STRQ      );

        interpolate_two_vect( s, b->normal, a->normal,  new->normal   );

        interpolate_two_vect( s, b->light,  a->light,   new->light    );



    for( int k = 0; k < 6; k++ )

    {

        new->dist[k] = a->dist[k] + s * (b->dist[k] - a->dist[k]);

    }



        return( 0 );

    }

}


/*

* tri_entirely_inside_frustum()

*/

int tri_entirely_inside_frustum( POINT v[3] )

{

/*

* for each vertex, check if it's on the inside of all frustum planes

*/

    for( int k = 0; k < 5; k++ )

    {

        if( (v[0].dist[k] < 0) || (v[1].dist[k] < 0) || (v[2].dist[k] < 0) )

        return( 0 );

    }



    return( 1 );

}


/*

* tri_entirely_outside_frustum()

*/

int tri_entirely_outside_frustum( POINT v[3] )

{

    /*

    * check if all three vertices are on the outside of any frustum plane

    */

    for( int k = 0; k < 5; k++ )

    {

        if( (v[0].dist[k] < 0) && (v[1].dist[k] < 0) && (v[2].dist[k] < 0) )

        return( 1 );

    }



    return( 0 );

}


/*

* tri_clip_frustum()

*/

int tri_clip_frustum( POINT *verts )

{

    POINT   new;

    POINT   clipped[8];

    POINT   *in;

    POINT   *out;

    POINT   *tmp;

    int     num_in = 3;

    int     num_out;


    in  = verts;

    out = clipped;


    for( int k = 0; k < 5; k++ )

    {

        num_out = 0;


        /*

        * walk around edges of polygon - each edge goes from i to next

        */

        for( int i = 0; i < num_in; i++ )

        {

            int next = (i+1) % num_in;


            if( in[i].dist[k] < 0 && in[next].dist[k] < 0 )

            {

                ; // both points are outside, do nothing

            }

            else if( in[i].dist[k] >= 0 && in[next].dist[k] >= 0 )

            {

                // both points are inside, add second to output list

                out[num_out++] = in[next];

            }

            else if( in[i].dist[k] >= 0 && in[next].dist[k] < 0 )

            {

                // first point is inside, second one is out, add intersection to output list

                line_clip_plane( &in[i], &in[next], &new, k );

                out[num_out++] = new;

            }

            else if( in[i].dist[k] < 0 && in[next].dist[k] >= 0 )

            {

                // first point is outside, second one is in, add intersection and second point to output list

                line_clip_plane( &in[i], &in[next], &new, k );

                out[num_out++] = new;

                out[num_out++] = in[next];

                }

            }



            tmp = in;

            in  = out;

            out = tmp;



            num_in  = num_out;

    }


    for( int i = 0; i < num_out; i++ )

        verts[i] = in[i];


    return( num_out );

}


/*

* calculate_clip_distances()

*/

void calculate_clip_distances( void )

{

    POINT   *p;

    POINT   verts[6];

    POINT   *v0, *v1, *v2;

    int     limit = numtriangles;


    /*

    * for each vertex, check if it's on the inside of all frustum planes

    */

    for( int i = 0; i < numvertices; i++ )

    {

        p = &vertex_list[i];



        for( int k = 0; k < 6; k++ )

        {

            p->dist[k] = vect_distance_to_plane( p->world, &frustum[k]);
            printf("Distance %f\n", p->dist[k]);

        }

    }



/*

* for each triangle, check if it's inside, outside, or needs to be clipped

*/

    for( int i = 0; i < limit; i++ )

    {

        verts[0] = vertex_list[triangle_list[i].vertex[0]];

        verts[1] = vertex_list[triangle_list[i].vertex[1]];

        verts[2] = vertex_list[triangle_list[i].vertex[2]];



        if( tri_entirely_inside_frustum(verts) )

        {

            triangle_list[i].clip_flag = IN;

        }

        else if( tri_entirely_outside_frustum(verts) )

        {

            triangle_list[i].clip_flag = CLIPPED;

        }

        else

        {

            triangle_list[i].clip_flag = CLIPPED;



            int num_clipped = tri_clip_frustum( verts );



            if( num_clipped == 0 )

            continue;



            for( int j = 1; j < num_clipped-1; j++ )

            {

                int new_verts = numvertices;



                p               = &vertex_list[numvertices++];

                *p              = verts[0];

                p->num_tri      = 0;

                //p->spike_index  = 0;



                p               = &vertex_list[numvertices++];

                *p              = verts[j];

                p->num_tri      = 0;

                //p->spike_index  = 0;



                p               = &vertex_list[numvertices++];

                *p              = verts[j+1];

                p->num_tri      = 0;

                //p->spike_index  = 0;



                int new_tri = numtriangles;



                add_triangle( new_verts, new_verts+1, new_verts+2 );



                copy_vect( triangle_list[i].normal, triangle_list[new_tri].normal    );

                //copy_vect( triangle_list[i].light,  triangle_list[new_tri].light     );



                triangle_list[new_tri].clip_flag = IN;

            }

        }

    }

}
              
/*************************************************************************/
/* GLUT functions                                                        */
/*************************************************************************/
/*
 * display routine
 */
void display(void)
{
    if( Mojave_WorkAround )
    {
        glutReshapeWindow(2 * window_size,2 * window_size);//Necessary for Mojave. Has to be different dimensions than in glutInitWindowSize();
        Mojave_WorkAround = 0;
    }
    
    start_timer( &sw_renderer_timer ); // Beginning of software renderer timer.
    
    if( draw_one_frame == 0 )
        return;
    
    fog = OFF;
    starter_texture.height = starter_texture.width; // makes the texturing that we're reading a ppm into a square.
    
    r_binary_text_file( &starter_texture, "rocks_color.ppm" ); // write a .ppm file into our image starter_texture.
    //checker_board_text(&starter_texture);
    r_binary_text_file( &bumpmap, "rocks_bump.ppm");
    
    if( cube_mapping )
    {
        read_cube_texture_test(); // will load the test texture into the cube_map array.
        //read_cube_texture_stream();
        //        read_cube_texture_sky();
    }
    
    copy_texture(&starter_texture);
    
    glClear(GL_COLOR_BUFFER_BIT );
    clear_c_buff(0, 0, 0, 1);
    clear_d_buff(1000000);
    first_render = ON;
    
    
    multiple = OFF;
    if(multiple == ON)
    {
        if(first_render == ON)
        {
            rand_mult_coord_fill();
            first_render = OFF;
        }
    }
    if(first_render == ON)
    {
        rand_mult_coord_fill();
        first_render = OFF;
    }
    
    //read_newobj_file("newOBJ.txt");
    
    //r_obj_file_scenter("teapot.obj", 0.0, 0.0, -2.0);
    init_sphere(1.0, 1.0, 0, 0, -20.0);
    
    
    //r_obj_file("teapot.obj");
    //init_plane();
    
    
    //init_torus(0.25, 0.75, 5.0, 0, 0, 0);
    //draw_n_teapots(5);
    
    //render_object(); // Operations done on the world->screen rendering process are stored here.
    
    
    if(display_render == ON)
    {
        //write_object_file();
        
        //    if(r_mesh == ON)
        //    {
        //        init_mesh(1.0, 0, 0, 0);
        //    }
        //
        //    if(r_plane == ON)
        //    {
        //        init_plane();
        //    }
        //
        //    if(r_cylinder == ON)
        //    {
        //        init_cylinder(1.0, 0.5, 0, 0, 0);
        //    }
        //
        //    if(r_sphere == ON)
        //    {
        //        init_sphere(1, 0.5, 0, 0, 0);
        //    }
        //
        //    if(r_cone == ON)
        //    {
        //        r_obj_file("teapot.obj");
        //    }
        //
        //    if(r_torus == ON)
        //    {
        //        init_torus(0.25, 0.75, 1.0, 0, 0, 0);
        //    }
        //    if(multiple == ON)
        //    {
        //        draw_n_spheres(5);
        //    }
        //
        //
        //    if(write == ON)
        //    {
        //      write_object_file();
        //    }
        
        if( 1 )// first_render == ON )
        {
            cal_face_normal();
            calculate_vertex_normals();
            first_render = OFF;
        }
        //map_texture_coordinates(); // goes through the object that is loaded into the vertex buffer and bakes the S/T values in.
        rotate_model_xy(xangle, yangle, zangle); // rotate our model by the coordinates we put in.
        
        cal_face_normal();
        calculate_vertex_normals();
        
        
        // From here on, working with screen coord
        t_model(translation_value); // translate model
        
        if(perspective_draw == ON)
        {
            p_form_model(distance, far);
        }
        else
        {
            form_model(1.0);
        }
       
        scale_p_model(100.0);
        setup_clip_frustum();
        calculate_clip_distances();
        draw_model();
    }
    
    //apply_post_processing_humed();
    if( depth_of_field == ON)
    {
        apply_depth_of_field();
    }
    
    stop_timer( &sw_renderer_timer ); // Ending of software renderer timer.
    double timer_result = elapsed_time( &sw_renderer_timer );
    
    current_savefile.sw_time[current_savefile.current_sw++] = timer_result;
    
    //printf( "The software rendered timer returned: %f\n", timer_result );
    
    
    start_timer( &gl_timer );
    show_color_buffer(); // must only be called once. Takes the current color buffer and gives all of the information to GL.
    stop_timer( &gl_timer );
    timer_result = elapsed_time( &gl_timer );
    
    current_savefile.gl_time[current_savefile.current_gl++] = timer_result;
    //printf( "The gl timer returned: %f\n", timer_result );
    
    numvertices = 0;
    numtriangles = 0;
    
    
    /*
     * show results
     */
    glutSwapBuffers();
    glutPostRedisplay();//Necessary for Mojave.
    draw_one_frame = 0;
}
/*
 * Key routine
 
 phong_lighting = ON;
 face_lighting = OFF;
 
 */
static void Key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 't':       xangle += 10;                                    break;
        case 'g':       xangle -= 10;                                    break;
        case 'y':       yangle += 10;                                    break;
        case 'h':       yangle -= 10;                                    break;
        case 'u':       zangle += 10;                                    break;
        case 'j':       zangle -= 10;                                    break; // these orient the object that we've drawn.
            
        case 'd':       translation_value += 0.2;                          break;
        case 'c':       translation_value -= 0.2;                          break; // these move the object closer or further from the depth camera
            
        case 'b':        texturing = (1 - texturing);                    break; // turns on texturing
        case 'n':        c_buff_blending = (1 - c_buff_blending);        break; // turns on color buffer blending
        case 'm':        d_buff_active = (1 - d_buff_active);            break; // turns on the depth buffer
            
        case 'p':        perspective_draw = (1 - perspective_draw);      break;
        case 'l':       perspective_correct = (1-perspective_correct);   break;
            
        case 'r':       rasterize = (1 - rasterize);                     break;
            
        case 'M':       r_mesh = (1 - r_mesh);                           break;
            //case 'P':       r_plane = (1 - r_plane);                         break;
            //case 'C':       r_cylinder = (1 - r_cylinder);                   break;
        case 'S':       r_sphere = (1 - r_sphere);                       break;
        case 'V':       r_cone = (1 - r_cone);                           break;
        case 'T':       r_torus = (1 - r_torus);                         break;
        case 'J':       multiple = (1 - multiple);                       break;
        case 'B':       bumpmapping = (1 - bumpmapping);                 break;
            
        case '!':       naive_mapping = ON; cylindrical_mapping = OFF; spherical_mapping = OFF; cube_mapping = OFF; break;
        case '@':       naive_mapping = OFF; cylindrical_mapping = ON; spherical_mapping = OFF; cube_mapping = OFF; break;
        case '#':       naive_mapping = OFF; cylindrical_mapping = OFF; spherical_mapping = ON; cube_mapping = OFF;break;
            
        case 'P':       phong_lighting = ON; face_lighting = OFF;        break;
        case 'F':       face_lighting = ON; phong_lighting = OFF;        break;
        case 'D':       depth_of_field = ( 1 - depth_of_field);          break;
        case 'R':       reflection = 1 - reflection;                     break;
        case 'C':       cube_mapping = ON; naive_mapping = OFF; cylindrical_mapping = OFF; spherical_mapping = OFF; break;
            
        case '0':       wave+=0.1;                                       break;
        case '9':       wave-=0.1;                                       break;
        case '6':       modulate = (1 - modulate);                       break;
        case '~':       write_timer_savefile();                        break;
        case 'L':       local_lighting = 1 - local_lighting;               break;
            
            
            
        case 'a':       draw_one_frame = 1;         break;
        case 'q':       exit(0);                    break;
        case '\033':    exit(0);                    break;
    }
    
    draw_one_frame = 1;
    glutPostRedisplay();
}

/*
 * main function
 */
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    srand(time(NULL));
    
    /*
     * create window
     */
    glutInitWindowSize(window_size,window_size);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("My OpenGL Window");
    
    /*
     * setup display(), Key() funtions
     */
    glutDisplayFunc(display);
    glutKeyboardFunc(Key);
    
    /*
     * setup OpenGL state
     */
    //glClearColor(0.0,0.0,0.0,1.0);
    gluOrtho2D(-window_size,window_size,-window_size,window_size);
    glPointSize(2.0);
    glColor4f(1.0,0.0,0.0,1.0);
    
    /*
     * start loop that calls display() and Key() routines
     */
    glutMainLoop();
    
    return 0;
}






