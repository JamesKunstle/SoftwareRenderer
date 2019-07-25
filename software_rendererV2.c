/* This is a recompilation of our graphics lessons, with some components kept constant from functions such as draw_point, draw_triangle baricentric, etc*/

/*
 *
 * point.c - simple GLUT app that draws one frame with a single point at origin
 *
 * To build:  gcc -framework OpenGL -framework GLUT software_rendererV2.c -o swV2
 *
 */
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif

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

#define CLAMP(a, low, high)     ((a) < (low)) ? (low) : (((a) > (high)) ? (high) : (a))

#define MILLION             1000000

#define IN 56

#define CLIPPED 57

#define WIN_HEIGHT 800

#define WIN_WIDTH 800

/*************************************************************************/
/* structs                                                               */
/*************************************************************************/

typedef struct POINT{
    
    float           position[4];  // X / Y / Z / W | SCREEN COORDINATES
    
    float           RGBA[4];      // R / G / B / A
    
    float           STRQ[4];      // S / T / R / Q
    
    float           world[4];     // X / Y / Z / W | WORLD COORDINATES
    
    int             tri_list[20]; // indices to triangles that share this vertex.
    
    int             num_tri;      // the number of triangles that share this vertex.
    
    float           light[4];     // the direction of the point to the finitely distant light source.
    
    float           normal[4];
    
    float           dist[6];      // the distance of a point-vertex to each of the 6 points of the frustum.
    int             rendered;
    
}POINT;


typedef struct IMAGE{
    
    int             width;
    
    int             height;
    
    unsigned char   data[3000][3000][4]; // T / S / RGBA
}IMAGE;


typedef struct tri{
    
    int             vertex[3];    // indices into vertex-list of composite triangles.
    
    float           normal[4];    // the face normal of the triangle.
    
    int             clip_flag;    // specifier for clip-state used in frustum culling.
    
}TRI;

typedef struct material {
    
    float           ambient[4];
    
    float           diffuse[4];
    
    float           specular[4];
    
} MATERIAL;


typedef struct timer{
    
    struct timeval  start;
    
    struct timeval   end;
    
}TIMER;


typedef struct timersense{           // struct that will store consecutive times recorded using the timers.
    
    float           sw_time[1000];   // stored rendering times of the software renderer.
    
    float           gl_time[1000];   // stored rendering times of GL
    
    int             current_sw;      // location of next save location for swr time.
    
    int             current_gl;      // location of next save location for glr time.
    
}TIMERSENSE;

typedef struct plane{               // used in the creation of the frustum for frustum culling
    
    float           normal[4];      // the direction that the plane is facing.
    
    float           dist;           // how far the plane is from the origin.
    
}PLANE;

typedef struct camera{
    
    float           location[4];    //world coordinate
    float           u[4];           // perpendicular vector
    float           v[4];           // forware vector
    float           up[4];          // vertical vector
    
}CAMERA;

typedef struct matrix_4{
    
    float table[4][4];
    
}MATRIX_4;

/*************************************************************************/
/* global variables                                                      */
/*************************************************************************/
int window_size = 400;

int Mojave_WorkAround = 1;

int draw_one_frame = 1;

CAMERA global_camera;


float lookat[4] = {0, 0, 0, 0};

POINT spans[800][2];    //2 x 800 array containing the coordinates of all of the spans for a triangle

int count[800];         // holds the current current number of values stored per row in spans

float c_buff[800][800][4];

float d_buff[800][800];


IMAGE current_texture; // final location of used texture.

IMAGE starter_texture; // location that texture is read into from file.

IMAGE swap_texture;    // place-holder texture struct used for image processing.


// 3D modeling
int d_buff_active =             OFF;

int c_buff_blending =           OFF;

int texturing =                 OFF;

int modulate =                  OFF;

int polar =                     OFF;

float polar_rotate_degrees =    0.0;

PLANE frustum[6];

//global variables that impact rendering parameters
int rasterize =                 OFF;

int r_cube =                    OFF;

int r_mesh =                    OFF;

int r_t_cube =                  OFF;

int r_plane =                   OFF;

int r_sphere =                  OFF;

int r_cylinder =                OFF;

int r_cone =                    OFF;

int r_torus =                   OFF;

int perspective_correct =       ON;

int perspective_draw =          OFF;
//

TRI triangle_list[40000];

int numtriangles;                           // number of triangles currently initialized.

POINT vertex_list[100000];

int numvertices;                            // number of initialized vertices.

float xangle =  0.0;

float yangle =  0.0;

float zangle =  0.0;                         //^ angles 3D world model rotated by


float near =    10.0;                         // definition of furthest 'z' axis boundaries of our 3D models

float far =     -40.0;


float wave =    0.0;


float light_x = 1.0;                        // location of a naive light source.

float light_y = 0;

float light_z = 0;

int phong_lighting =                OFF;

int face_lighting = OFF;                    // types of lighting used on 3D models

float ambient[4] = { 0.9, 0.9, 0.9, 0.9 };  // background light added to scenes.

float first_render =                OFF;

float mult_coords[1000][3];                 // random coords used when drawing multiple objects

int current_coord = 0;                      // next available random coord

int multiple = OFF;                         // global condition of rendering multiple object or not

float eye[4]                = { 0, 0, 0, 1 };   // location of eye array

float light[4]              = { 1, -1, -1, 1 };

float light_ambient[4]      = { 1.0, 1.0, 1.0, 1.0 };

float light_diffuse[4]      = { 1, 1, 1, 1 };

float light_specular[4]     = { 1, 1, 1, 1 };

float material_ambient[4]   = {0.7,     0.7,     0.7,     1};

float material_diffuse[4]   = {0.75,    0.75,    0.75,    1};

float material_specular[4]  = {0.75,    0.75,    0.75,   1}; // CURRENTLY USING GENERAL MATERIAL PROPERTIES FOR LIGHTING

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


float max_depth = -10000000; // max amount that a 3D object can be moved into the plane.

float translation_value = 5.0; // increment of translation

float fog_color[4] = { 0, 0, 0, 1};

int fog =                       OFF;

IMAGE bumpmap;              // location that bump-map texture is read into

int bumpmapping =               OFF;

//kinds of texture mapping done to 3D objects with synthetic STRQ values generated in-renderer.
int naive_mapping =             OFF;

int cylindrical_mapping =       OFF;

int spherical_mapping =         OFF;

int cube_mapping =              OFF;
//

int depth_of_field =            OFF;

int reflection =                OFF;

IMAGE cubemap[6]; // Contains the six textures of the cube.

//timing utility
TIMER sw_renderer_timer;

TIMER gl_timer;

TIMERSENSE current_savefile;
//

//local lighting global values
float light_position[4] = {1, 1, 0, 1.0};

int local_lighting =            OFF;
//


POINT g_buffer[WIN_HEIGHT][WIN_WIDTH];
int deferred_rendering = OFF;

/*************************************************************************/
/* utility functions                                                     */
/*************************************************************************/
/*
 * random_float()
 */
float random_float( float low, float high )
{
    return( (float)(low + (rand()/(float)RAND_MAX)*(high - low)) );
}

void r_binary_text_file_two_row(IMAGE *image, char *name);

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

void copy_vect(float i[4], float o[4]);


// These are collections of libraries that the renderer uses. Each is within the same folder as SWRV2.c
#include "vector_math_utilities.c"
#include "cube_mapping_utilities.c"
#include "timing_utilities.c"
#include "clipping_utilities.c"
#include "texture_ingestion_utilities.c"
#include "matrix_utilities.c"

void draw_point(POINT *p, float blend_weight)
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
    
    if( deferred_rendering )
    {
        p->rendered = 1;
        g_buffer[y][x] = *p;
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

void store_point(POINT *p)
{
    int row = (int) (p->position[Y] + 400);
    int col = count[row];
    if(col <= 1)
    {
        spans[row][col] = *p;
        count[row]++;
    }
}

float edgeFunction( float a[4], float b[4], float c[4] ) // used in draw_triangle barycentric

{
    
    return( c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0] );
    
}

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
    
    draw_line_from_Chris(p0, p1, WALK, 1.0);
    
    draw_line_from_Chris(p1, p2, WALK, 1.0);
    
    draw_line_from_Chris(p2, p0, WALK, 1.0);
    
    draw_spans();
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
    for( int i = 0; i < numvertices; i++ )
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
    }
}

void t_model(float d) // translates a generated model down the Z-axis by depth d
{
    //move all points in our model into the plane by d.
    for( int i = 0; i < numvertices; i++ )
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
            //printf("ignored.\n");
            continue;
        }
        
        POINT v1 = vertex_list[triangle_list[i].vertex[0]];
        POINT v2 = vertex_list[triangle_list[i].vertex[1]];
        POINT v3 = vertex_list[triangle_list[i].vertex[2]]; // copy the points over to be passed to the drawing.
        
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
        draw_triangle_barycentric(&vertex_list[triangle_list[i].vertex[0]],
                                  &vertex_list[triangle_list[i].vertex[1]],
                                  &vertex_list[triangle_list[i].vertex[2]]);
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
        xprime = vertex_list[i].world[X] * cos(z_angle / 360.0 * 2.0 * 3.1415926)
                    - vertex_list[i].world[Y] * sin(z_angle / 360.0 * 2.0 * 3.1415926);
        
        yprime = vertex_list[i].world[X] * sin(z_angle / 360.0 * 2.0 * 3.1415926)
                    + vertex_list[i].world[Y] * cos(z_angle / 360.0 * 2.0 * 3.1415926);
        
        
        vertex_list[i].world[X] = xprime;
        vertex_list[i].world[Y] = yprime;
    }
    for(int i = 0; i < numvertices; i++)
    {
        // for every point in the vertices, change the spatial x, y, z dimensions using the input angles and the point world properties
        xprime = vertex_list[i].world[X] * cos(y_angle / 360.0 * 2.0 * 3.1415926)
                    - vertex_list[i].world[Z] * sin(y_angle / 360.0 * 2.0 * 3.1415926);
        
        zprime = vertex_list[i].world[X] * sin(y_angle / 360.0 * 2.0 * 3.1415926)
                    + vertex_list[i].world[Z] * cos(y_angle / 360.0 * 2.0 * 3.1415926);
        
        
        vertex_list[i].world[X] = xprime;
        vertex_list[i].world[Z] = zprime;
    }
    for(int i = 0; i < numvertices; i++)
    {
        // for every point in the vertices, change the spatial x, y, z dimensions using the input angles and the point world properties
        yprime = vertex_list[i].world[Y] * cos(x_angle / 360.0 * 2.0 * 3.1415926)
                    - vertex_list[i].world[Z] * sin(x_angle / 360.0 * 2.0 * 3.1415926);
        
        zprime = vertex_list[i].world[Y] * sin(x_angle / 360.0 * 2.0 * 3.1415926)
                    + vertex_list[i].world[Z] * cos(x_angle / 360.0 * 2.0 * 3.1415926);
        
        
        vertex_list[i].world[Y] = yprime;
        vertex_list[i].world[Z] = zprime;
    }
}

void calculate_vertex_normals(void)
{
    for( int i = 0; i < numvertices; i++ )
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

void show_color_buffer( void ) // passing our color buffer to GL all at once
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

void rand_mult_coord_fill() //V2 ADDING THIS JUST IN CASE
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

/*
 
 * add_vertex()
 
 */

void add_vertex( float world[4], float pos[4], float color[4], float tex[4], float normal[4], int num_tri, int tri_list[8], int spike_index )

{
    
    POINT *p;
    
    
    
    p = &vertex_list[numvertices];
    
    
    
    copy_vect( world,    p->world    );  p->world[X] *= -1; // temp chrisb
    
    copy_vect( pos,      p->position      );
    
    copy_vect( color,    p->RGBA    );
    
    copy_vect( tex,      p->STRQ      );
    
    copy_vect( normal,   p->normal   );
    
    
    
    for( int i = 0; i < num_tri; i++ )
        
        p->tri_list[i] = tri_list[i];
    
    
    
    p->num_tri      = num_tri;
    
    
    
    numvertices++;
    
}

void init_model()
{
    numvertices = 0;
    numtriangles = 0;
}

/*
 
 * init_cube()
 
 */

void init_cube( float cx, float cy, float cz, float s )
{
    float world[4];
    float pos[4];
    float color[4];
    float tex[4];
    float normal[4];
    
    init_model();
    
    set_vect_4( world,  s+cx,  s+cy, -s+cz,  1 );
    set_vect_4( color,  1,  0,  0,  0 );
    set_vect_4( tex,    0,  0,  0,  1 );
    add_vertex( world, pos, color, tex, normal, 0, NULL, 0 );
    
    set_vect_4( world,  s+cx,  s+cy,  s+cz,  1 );
    set_vect_4( color,  0,  1,  0,  0 );
    set_vect_4( tex,    1,  0,  0,  1 );
    add_vertex( world, pos, color, tex, normal, 0, NULL, 0 );
    
    set_vect_4( world,  s+cx, -s+cy,  s+cz,  1 );
    set_vect_4( color,  0,  0,  1,  0 );
    set_vect_4( tex,    1,  1,  0,  1 );
    add_vertex( world, pos, color, tex, normal, 0, NULL, 0 );
    
    set_vect_4( world,  s+cx, -s+cy, -s+cz,  1 );
    set_vect_4( color,  1,  0,  1,  0 );
    set_vect_4( tex,    0,  1,  0,  1 );
    add_vertex( world, pos, color, tex, normal, 0, NULL, 0 );
    
    set_vect_4( world, -s+cx,  s+cy, -s+cz,  1 );
    set_vect_4( color,  1,  0,  0,  0 );
    set_vect_4( tex,    0,  0,  0,  1 );
    add_vertex( world, pos, color, tex, normal, 0, NULL, 0 );

    set_vect_4( world, -s+cx,  s+cy,  s+cz,  1 );
    set_vect_4( color,  0,  1,  0,  0 );
    set_vect_4( tex,    1,  0,  0,  1 );
    add_vertex( world, pos, color, tex, normal, 0, NULL, 0 );
    
    set_vect_4( world, -s+cx, -s+cy,  s+cz,  1 );
    set_vect_4( color,  0,  0,  1,  0 );
    set_vect_4( tex,    1,  1,  0,  1 );
    add_vertex( world, pos, color, tex, normal, 0, NULL, 0 );
    
    set_vect_4( world, -s+cx, -s+cy, -s+cz,  1 );
    set_vect_4( color,  1,  0,  1,  0 );
    set_vect_4( tex,    0,  1,  0,  1 );
    add_vertex( world, pos, color, tex, normal, 0, NULL, 0 );
    
    // right face
    add_triangle( 0, 1, 2 );
    add_triangle( 2, 3, 0 );
    
    // front face
    add_triangle( 0, 3, 7 );
    add_triangle( 7, 4, 0 );
    
    // left face
    add_triangle( 4, 7, 6 );
    add_triangle( 6, 5, 4 );
    
    // back face
    add_triangle( 5, 6, 2 );
    add_triangle( 2, 1, 5 );
    
    // top face
    add_triangle( 1, 0, 4 );
    add_triangle( 4, 5, 1 );
    
    // bottom face
    add_triangle( 3, 2, 6 );
    add_triangle( 6, 7, 3 );
}

void set_camera_matrix(MATRIX_4 m, float eye[4], float lookat[4], float up[4])
{
    float u[4];
    float v[4];
    MATRIX_4 t;
    MATRIX_4 r;
    
    
    
    subtract_vectors( lookat, eye, v );
    normalize_vector( v, v );
    
    cross_vect( up, v, u ); // getting the perpendicular vector
    normalize_vector( u, u );
    
    cross_vect( v, u, up ); // getting the perpendicular vector
    normalize_vector( up, up );
    
    set_identity_matrix_4( r );
    set_identity_matrix_4( t );
    
    for( int j = 0; j < 3; j++ )
    {
        r.table[j][0] = u[j];
        r.table[j][1] = up[j];
        r.table[j][2] = v[j];
    }
    
    set_translate_matrix_camera( m.table, -eye[X], -eye[X], -eye[X] );
    mult_matrix_matrix( t.table, r.table, m.table );
    // m matrix implicitly passed out of the function.
    // using this after rotation and translation will allow all of our other math to work.
}

void clear_g_buffer( float r, float g, float b, float a )
{
    for( int j = 0; j < WIN_HEIGHT; j++ )
    {
        for( int i = 0; i < WIN_WIDTH; i++ )
        {
            set_vect_4( g_buffer[j][j].RGBA, r, g, b, a );
            g_buffer[j][i].rendered = 0;
        }
    }
}

void draw_g_buffer()
{
    int save_deferred = deferred_rendering;
    deferred_rendering = 0;
    
    float temp_back_color[4] = {1.0, 0, 0, 1};
    
    for( int j = 0; j < WIN_HEIGHT; j++ )
    {
        for( int i = 0; i < WIN_WIDTH; i++ )
        {
            if( g_buffer[j][i].rendered == 0 )
            {
                copy_vect( g_buffer[j][i].RGBA, c_buff[j][i] );
            }
            else
            {
                draw_point( &g_buffer[j][i], 1.0 );
            }
        }
    }
    deferred_rendering = save_deferred;
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
    
    if( draw_one_frame == 0 )
        return;

    r_binary_text_file( &current_texture, "rocks_color.ppm" );          // READ IN TEXTURE AND BUMP MAP
    r_binary_text_file( &bumpmap, "rocks_bump.ppm");
    read_cube_texture_test();                                          // READ IN THE CUBE MAP
    
    glClear(GL_COLOR_BUFFER_BIT );                                     // CLEAR BUFFERS AND INITIALIZE FILE
    clear_c_buff(0, 0, 0, 1);
    clear_d_buff(1000000);
    clear_g_buffer( 0.0, 0.0, 0.0, 1.0 );
    
    init_sphere(1.0, 1.0, 0, 0, -10.0);                                // 3D OBJECT LOADED INTO THE VERTEX/TRIANGLE LIST
    //init_cube( 0.0, 0.0, 0.0, 1);
    
//    cal_face_normal();
//    calculate_vertex_normals();                                        // 3D MODEL HAS ALL NORMALS CALUCULATED
//
    rotate_model_xy(xangle, yangle, zangle);                           // IT IS ROTATED
    t_model(translation_value - 30);                                        // IT IS TRANSLATED
//    rotate_model_matrix(xangle, yangle, zangle);
//    t_model_matrix(translation_value - 30);                              // matrix versions of^
    
    //rotate_translate_matrix( xangle, yangle, zangle, translation_value - 30 );
    
//    MATRIX_4 camera_matrix;
//    set_camera_matrix( camera_matrix, eye, lookat, global_camera.up );
    
    cal_face_normal();
    calculate_vertex_normals();                                         // NORMALS RECALCULATED
    
    // FROM HERE ON, BEGINNING TO WORK WITH SCREEN COORDINATES
    
    setup_clip_frustum();
    calculate_clip_distances();                                         // CALCULATES WHICH TRIANGLES ARE WITHIN BOUNDS
    
    
    if(perspective_draw == ON)
    {
        p_form_model(near, far);
    }
    else
    {
        form_model(1.0);
    }
    
    scale_p_model(100.0);

    draw_model();                                                       // LOADS COLOR BUFFER, USES DRAW TRIANGLE ETC
    
    if( deferred_rendering )
    {
        clear_g_buffer( 0.0, 0.0, 0.0, 1.0 );
        draw_g_buffer();
    }
    
    show_color_buffer();                                                // GIVES THE COLOR BUFFER TO GL TO DRAW

    numvertices = 0;                                                    // READY FOR NEXT RENDER
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
 */
static void Key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'a':       draw_one_frame = 1;     glutPostRedisplay();    break;
        case 'q':       exit(0);                                        break;
        case '\033':    exit(0);                                        break;
            
        case 't':       xangle += 5;                                    break;
        case 'g':       xangle -= 5;                                    break;
        case 'y':       yangle += 5;                                    break;
        case 'h':       yangle -= 5;                                    break;
        case 'u':       zangle += 5;                                    break;
        case 'j':       zangle -= 5;                                    break; // these orient the object that we've drawn.
            
        case 'd':       translation_value += 0.5;                          break;
        case 'c':       translation_value -= 0.5;                          break; // these move the object closer or further from the depth camera
            
        case 'b':        texturing = (1 - texturing);                    break; // turns on texturing
        case 'n':        c_buff_blending = (1 - c_buff_blending);        break; // turns on color buffer blending
        case 'm':        d_buff_active = (1 - d_buff_active);            break; // turns on the depth buffer
            
        case 'p':        perspective_draw = (1 - perspective_draw);      break;
        case 'l':       perspective_correct = (1-perspective_correct);   break;
            
        case 'r':       rasterize = (1 - rasterize);                     break;
        
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
        case 'Q':       deferred_rendering = ( 1 - deferred_rendering );
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
    glClearColor(0.0,0.0,0.0,1.0);
    gluOrtho2D(-window_size,window_size,-window_size,window_size);
    glPointSize(2.0);
    glColor4f(1.0,0.0,0.0,1.0);
    
    /*
     * start loop that calls display() and Key() routines
     */
    glutMainLoop();
    
    return 0;
}
