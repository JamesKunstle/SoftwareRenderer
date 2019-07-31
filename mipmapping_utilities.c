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
void mipmap();
void triage();
void mip_map_fill( int LOD, int cascade );
/* ========================================================================== */

void average_RGBA_channels( unsigned char zero[4], unsigned char one[4], unsigned char two[4], unsigned char three[4], unsigned char output[4] )
{
    output[R] = ( zero[R] + one[R] + two[R] + three[R] ) / 4.0;
    output[G] = ( zero[G] + one[G] + two[G] + three[G] ) / 4.0;
    output[B] = ( zero[B] + one[B] + two[B] + three[B] ) / 4.0;
    output[A] = ( zero[A] + one[A] + two[A] + three[A] ) / 4.0;
}

void mipmap()
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
    
}

void triage()
{
    int cascade = 0;                // are we reading the current_texture data to an level for the first time or are we reading from one level higher?
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
        default:
            printf("Error triaging the current mipmap level.\n");
    }
}

    
void mip_map_fill( int LOD, int cascade )
{
    int fill_height;
    int fill_width;
    MM_IMAGE *dest;
    IMAGE *send_i;
    MM_IMAGE *send_mm;
    
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
        fill_height = send_mm->height / 2;   // integer division to handle odd sized- areas.
        fill_width =  send_mm->width / 2;
        
        for( int j = 0; j < fill_height - 2; j += 2 )
        {
            for( int i = 0; i < fill_width - 2; i += 2) // box-filter
            {
                average_RGBA_channels( send_mm->data[j][i], send_mm->data[j + 1][i], send_mm->data[j][i + 1], send_mm->data[j + 1][i + 1], dest->data[j][i] );
            }
        }
    }
    else
    {
        fill_height = send_i->height / 2;   // integer division to handle odd sized- areas.
        fill_width =  send_i->width / 2;
        
        for( int j = 0; j < fill_height - 2; j += 2 )
        {
            for( int i = 0; i < fill_width - 2; i += 2) // box-filter
            {
                average_RGBA_channels( send_i->data[j][i], send_i->data[j + 1][i], send_i->data[j][i + 1], send_i->data[j + 1][i + 1], dest->data[j][i] );
            }
        }
    }
    dest->height = fill_height;
    dest->width = fill_width;
}
