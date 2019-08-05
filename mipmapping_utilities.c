// This document contains the variables and the functions to allow mip-mapping for our 3D software renderer.

/* ============================ GLOBAL VARIABLES ============================ */
typedef struct mm_image{
    
    unsigned char data[1024][1024][4];
    int height;
    int width;
    
}MM_IMAGE;


typedef struct mipmap{
    
    int big_map;        // the largest map that's currently being stored.
    
    MM_IMAGE zero;      // max 1024x1024
    MM_IMAGE one;       // max 512x512
    MM_IMAGE two;       // max 256x256
    MM_IMAGE three;     // max 128x128
    MM_IMAGE four;      // max 64x64
    MM_IMAGE five;      // max 32x32
    MM_IMAGE six;       // max 16x16
    MM_IMAGE seven;     // max 8x8
    MM_IMAGE eight;     // max 4x4
    MM_IMAGE nine;      // max 2x2
    MM_IMAGE ten;       // max 1x1      FINAL

}MIPMAP;

MIPMAP mm;

/* ========================================================================== */

/* ============================   PROTOTYPES     ============================ */
void average_RGBA_channels( unsigned char zero[4], unsigned char one[4], unsigned char two[4], unsigned char three[4], unsigned char output[4] );
int mipmap();
void triage();
void mip_map_fill( int LOD, int cascade );
/* ========================================================================== */

void average_RGBA_channels( unsigned char zero[4], unsigned char one[4], unsigned char two[4], unsigned char three[4], unsigned char output[4] )
{
    output[R] = ( zero[R] + one[R] + two[R] + three[R] ) / 4;
    output[G] = ( zero[G] + one[G] + two[G] + three[G] ) / 4;
    output[B] = ( zero[B] + one[B] + two[B] + three[B] ) / 4;
    output[A] = ( zero[A] + one[A] + two[A] + three[A] ) / 4;
}

int mipmap()
{
    int height    = current_texture.height;
    int width     = current_texture.width;
    int threshold = height > width ? height : width;        // determines the max_mipmap
    
    if      ( threshold >= 1024 )   mm.big_map = 0;          // taking lower-bound size of mm. 506x506 /=> 512x512.

    else if ( threshold >= 512 )    mm.big_map = 1;

    else if ( threshold >= 256 )    mm.big_map = 2;

    else if ( threshold >= 128 )    mm.big_map = 3;

    else if ( threshold >= 64 )     mm.big_map = 4;

    else if ( threshold >= 32 )     mm.big_map = 5;

    else if ( threshold >= 16 )     mm.big_map = 6;

    else if ( threshold >= 8 )      mm.big_map = 7;

    else if ( threshold >= 4 )      mm.big_map = 8;

    else if ( threshold >= 2 )      mm.big_map = 9;

    else if ( threshold >= 1 )      mm.big_map = 10;

    else
    {
        printf("Texture size invalid.\n");
    }
    
    triage();
    
    return mm.big_map;  // returns the max_lod of the texture.
}

void triage()
{
    int cascade = 0;                // are we reading the current_texture data to a level for the first time or are we reading from one level higher?
    switch ( mm.big_map )
    {
        case 0:
            mip_map_fill(0, 0);
            cascade = 1;
        case 1:
            mip_map_fill(1, cascade);
            cascade = 1;
        case 2:
            mip_map_fill(2, cascade);
            cascade = 1;
        case 3:
            mip_map_fill(3, cascade);
            cascade = 1;
        case 4:
            mip_map_fill(4, cascade);
            cascade = 1;
        case 5:
            mip_map_fill(5, cascade);
            cascade = 1;
        case 6:
            mip_map_fill(6, cascade);
            cascade = 1;
        case 7:
            mip_map_fill(7, cascade);
            cascade = 1;
        case 8:
            mip_map_fill(8, cascade);
            cascade = 1;
        case 9:
            mip_map_fill(9, cascade);
            cascade = 1;
        case 10:
            mip_map_fill(10, cascade);
            break;
        default:
            printf("Error triaging the current mipmap level.\n");
    }
}

    
void mip_map_fill( int LOD, int cascade )
{
    int read_height;    // dimensions of the data array that is read from.
    int read_width;
    
    MM_IMAGE *dest;     // destination texture image
    
    IMAGE *send_i;      // the sending image if the sender is the current_texture
    MM_IMAGE *send_mm;  // the sending image if if the sender is one of the previous mipmap textures.
    
    switch ( LOD )
    {
        case 0:
            dest = &mm.zero;
            send_i = &current_texture;
            break;
        case 1:
            dest = &mm.one;
            if( cascade ) send_mm = &mm.zero;
            else          send_i = &current_texture;
            break;
        case 2:
            dest = &mm.two;
            if( cascade ) send_mm = &mm.one;
            else          send_i = &current_texture;
            break;
        case 3:
            dest = &mm.three;
            if( cascade ) send_mm = &mm.two;
            else          send_i = &current_texture;
            break;
        case 4:
            dest = &mm.four;
            if( cascade ) send_mm = &mm.three;
            else          send_i = &current_texture;
            break;
        case 5:
            dest = &mm.five;
            if( cascade ) send_mm = &mm.four;
            else          send_i = &current_texture;
            break;
        case 6:
            dest = &mm.six;
            if( cascade ) send_mm = &mm.five;
            else          send_i = &current_texture;
            break;
        case 7:
            dest = &mm.seven;
            if( cascade ) send_mm = &mm.six;
            else          send_i = &current_texture;
            break;
        case 8:
            dest = &mm.eight;
            if( cascade ) send_mm = &mm.seven;
            else          send_i = &current_texture;
            break;
        case 9:
            dest = &mm.nine;
            if( cascade ) send_mm = &mm.eight;
            else          send_i = &current_texture;
            break;
        case 10:
            dest = &mm.ten;
            if( cascade ) send_mm = &mm.nine;
            else          send_i = &current_texture;
            break;
        default:
            printf("Oops! Something went wrong with the cases!\n");
            break;
    }
    
    if( cascade )
    {
        read_height = send_mm->height;
        read_width =  send_mm->width;
        
        
        int m = 0;
        int n = 0;
        
        for( int j = 0; j < read_height - 1; j += 2 )
        {
            for( int i = 0; i < read_width - 1; i += 2) // box-filter
            {
                average_RGBA_channels( send_mm->data[j][i], send_mm->data[j + 1][i], send_mm->data[j][i + 1], send_mm->data[j + 1][i + 1], dest->data[m][n++] );
            }
            n = 0;
            m++;
        }
    }
    else
    {
        read_height = send_i->height;   // integer division to handle odd sized- areas.
        read_width =  send_i->width;
        
        
        int m = 0;
        int n = 0;
        
        for( int j = 0; j < read_height - 1; j += 2 )
        {
            for( int i = 0; i < read_width - 1; i += 2) // box-filter
            {
                average_RGBA_channels( send_i->data[j][i], send_i->data[j + 1][i], send_i->data[j][i + 1], send_i->data[j + 1][i + 1], dest->data[m][n++] );
            }
            n = 0;
            m++;
        }
    }
    dest->height = read_height / 2;
    dest->width = read_width / 2;
}

void mm_to_ct( int LOD ) // copies a mipmap image data into the current_texture image data so that an object will be textured by mipmap level.
{
    MM_IMAGE *mm_l;
    
    switch ( LOD )
    {
        case 0:
            mm_l = &mm.zero;
            break;
        case 1:
            mm_l = &mm.one;
            break;
        case 2:
            mm_l = &mm.two;
            break;
        case 3:
            mm_l = &mm.three;
            break;
        case 4:
            mm_l = &mm.four;
            break;
        case 5:
            mm_l = &mm.five;
            break;
        case 6:
            mm_l = &mm.six;
            break;
        case 7:
            mm_l = &mm.seven;
            break;
        case 8:
            mm_l = &mm.eight;
            break;
        case 9:
            mm_l = &mm.nine;
            break;
        case 10:
            mm_l = &mm.ten;
            break;
        default:
            printf("Oops!\n");
            break;
    }
    
    int height = mm_l->height;
    int width = mm_l->width;
    
    printf("new image size: %d, %d\n", height, width);
    
    for( int j = 0; j < height; j++ )
    {
        for( int i = 0; i < width; i++ ) // box-filter
        {
            copy_vect_RGBA( mm_l->data[j][i], current_texture.data[j][i] );
        }
    }
    current_texture.height = height;
    current_texture.width = width;
    
    
}
/* max_lod == max texture density, psi == proportion of max_lod in final output, color == implicit output of color value from texture.
    colors selected in ratio (psi) from n and n + 1 mipmap texture.*/
void brew( float s_correct, float t_correct, float point_depth, unsigned char color[4] )
{
    int max_lod = mm.big_map;   // maximum detail offset
    int real_map_level;         // where the object is for detail
    float psi;                  // proportion of top vs bottom texture.
    MM_IMAGE *top_most;
    MM_IMAGE *bottom_most;
    
//    if( point_depth >= -70 )
//    {
//        real_map_level = 0;
//        psi = (point_depth) / 80;
//    }
//    else if( point_depth >= -80 )
//    {
//        real_map_level = 1;
//        psi = (point_depth) / 90;
//    }
//    else if( point_depth >= -90 )
//    {
//        real_map_level = 2;
//        psi = point_depth / 100;
//    }
//    else if( point_depth >= -100 )
//    {
//        real_map_level = 3;
//        psi = point_depth / 110;
//    }
//    else if( point_depth >= -110 )
//    {
//        real_map_level = 4;
//        psi = point_depth / 120;
//    }
//    else if( point_depth >= -120 )
//    {
//        real_map_level = 5;
//        psi = point_depth / 130;
//    }
//    else if( point_depth >= -130 )
//    {
//        real_map_level = 6;
//        psi = point_depth / 140;
//    }
//    else if( point_depth >= -140 )
//    {
//        real_map_level = 7;
//        psi = point_depth / 150;
//    }
//    else if( point_depth >= -150 )
//    {
//        real_map_level = 8;
//        psi = point_depth / 160;
//    }
//    else if( point_depth >= -160 )
//    {
//        real_map_level = 9;
//        psi = point_depth / 170;
//    }
//    else if( point_depth >= -170 )
//    {
//        real_map_level = 10;
//        psi = point_depth / 180;
//    }
    if( point_depth >= -70 )
    {
        real_map_level = 0;
        psi = (point_depth) / 75;
    }
    else if( point_depth >= -75 )
    {
        real_map_level = 1;
        psi = (point_depth) / 80;
    }
    else if( point_depth >= -100 )
    {
        real_map_level = 2;
        psi = point_depth / 85;
    }
    else if( point_depth >= -100 )
    {
        real_map_level = 3;
        psi = point_depth / 90;
    }
    else if( point_depth >= -110 )
    {
        real_map_level = 4;
        psi = point_depth / 110;
    }
    else if( point_depth >= -110 )
    {
        real_map_level = 5;
        psi = point_depth / 120;
    }
    else if( point_depth >= -120 )
    {
        real_map_level = 6;
        psi = point_depth / 130;
    }
    else if( point_depth >= -130 )
    {
        real_map_level = 7;
        psi = point_depth / 150;
    }
    else if( point_depth >= -150 )
    {
        real_map_level = 8;
        psi = point_depth / 160;
    }
    else if( point_depth >= -160 )
    {
        real_map_level = 9;
        psi = point_depth / 170;
    }
    else if( point_depth >= -170 )
    {
        real_map_level = 10;
        psi = point_depth / 180;
    }
    else // jamesk: need to take input from rest of the levels.
    {
        printf("too far back");
    }
    real_map_level += max_lod;  // specifies that the texture that we read from is the topmost available.
    
    //printf("real_map_level = %d, point_depth = %f\n", real_map_level, point_depth);
    
    CLAMP(real_map_level, 0, 10);

    switch ( real_map_level )
    {
        case 0:
            top_most = &mm.zero;
            bottom_most = &mm.one;
            break;
        case 1:
            top_most = &mm.one;
            bottom_most = &mm.two;
            break;
        case 2:
            top_most = &mm.two;
            bottom_most = &mm.three;
            break;
        case 3:
            top_most = &mm.three;
            bottom_most = &mm.four;
            break;
        case 4:
            top_most = &mm.four;
            bottom_most = &mm.five;
            break;
        case 5:
            top_most = &mm.five;
            bottom_most = &mm.six;
            break;
        case 6:
            top_most = &mm.six;
            bottom_most = &mm.seven;
            break;
        case 7:
            top_most = &mm.seven;
            bottom_most = &mm.eight;
            break;
        case 8:
            top_most = &mm.eight;
            bottom_most = &mm.nine;
            break;
        case 9:
            top_most = &mm.nine;
            bottom_most = &mm.ten;
            break;
        case 10:
            top_most = &mm.ten;
            bottom_most = &mm.ten;
            break;
        default:
            printf("Oops!\n");
            break;
    }
    
    int top_most_s = (int)(top_most->width * s_correct);
    int top_most_t = (int)(top_most->height * t_correct);
    
    int bottom_most_s = (int)(bottom_most->width * s_correct);
    int bottom_most_t = (int)(bottom_most->height * t_correct); // texture coordinates for both the top and the bottom texture.
    
    unsigned char top_colors[4];
    unsigned char bottom_colors[4];
    
    copy_vect_RGBA( top_most->data[top_most_t][top_most_s], top_colors);
    copy_vect_RGBA( bottom_most->data[bottom_most_t][bottom_most_s], bottom_colors);
    psi = 1.0;
    
    color[R] = (( psi * top_colors[R]) + ( (1.0 - psi) * bottom_colors[R]));
    color[G] = (( psi * top_colors[G]) + ( (1.0 - psi) * bottom_colors[G]));
    color[B] = (( psi * top_colors[B]) + ( (1.0 - psi) * bottom_colors[B]));
    color[A] = (( psi * top_colors[A]) + ( (1.0 - psi) * bottom_colors[A])); // local value of color data stored in the pixel space.
    
    
    
}
