// This document contains the variables and the functions to allow mip-mapping for our 3D software renderer.

/* ============================ GLOBAL VARIABLES ============================ */
typedef struct mipmap{
    
    int max_mipmap_ind;
    
    float zero[1024][1024][4];  // the maximum supported size of a mipmap is 1024x1024.
    float one[512][512][4];
    float two[256][256][4];
    float three[128][128][4];
    float four[64][64][4];
    float five[32][32][4];
    float six[16][16][4];
    float seven[8][8][4];
    float eight[4][4][4];
    float nine[2][2][4];
    float ten[1][1][4];

}MIPMAP;

MIPMAP mm;

/* ========================================================================== */

void mipmap()                                      // reads in the data from the current_texture array.
{
    int height = current_texture.height;
    int width = current_texture.width;
    int threshold = height < width ? height : width;        // determines the max_mipmap
    
    if      ( threshold > 1024 )   mm.max_mipmap_ind = 0;   // taking lower-bound size of mm. 506x506 /=> 512x512.

    else if ( threshold > 512 )    mm.max_mipmap_ind = 1;

    else if ( threshold > 256 )    mm.max_mipmap_ind = 2;
 
    else if ( threshold > 128 )    mm.max_mipmap_ind = 3;

    else if ( threshold > 64 )     mm.max_mipmap_ind = 4;

    else if ( threshold > 32 )     mm.max_mipmap_ind = 5;
    
    else if ( threshold > 16 )     mm.max_mipmap_ind = 6;

    else if ( threshold > 8 )      mm.max_mipmap_ind = 7;

    else if ( threshold > 4 )      mm.max_mipmap_ind = 8;

    else if ( threshold > 2 )      mm.max_mipmap_ind = 9;

    else if ( threshold > 1 )      mm.max_mipmap_ind = 10;
    
    else
    {
        printf("Texture size invalid.\n");
    }
    
    triage_mipmap_fill( &mm) ;               // pass the determined LOD and a reference to the current mipmap
}

void triage_mipmap_fill( int *cmm)                      // sends the correct textures to be minimized to their correct places.
{
    switch ( cmm->max_mipmap_ind )
    {
        case 0:
            mipmap_fill() /* jamesk unfinished.*/
            continue;
        case 2: // code to be executed if n = 2;
            break;
        default: // code to be executed if n doesn't match any cases
    }
}

void mipmap_fill( float *in, float *out, int height, int width
{
    int xstep = width / 2;                  // integer division to determin how many steps it takes to downsample in to out for a 2x2 box filter.
    int ystep = height / 2;
    
    for ( int j = 0; j < height; j += 2 )   // step of the box-filter. jamesk: needs to be able to accomodate 1x1 and 2x2. Probably with conditions in the below assignment.
    {
        for ( int i = 0; i < width; i += 2 )
        {
            out[j][i][R] = (in[j][i][R] + in[j + 1][i][R] + in[j][i + 1][R] + in[j + 1 ][i + 1][R]) / 4.0;  // the average color channels are read to the output
            out[j][i][G] = (in[j][i][G] + in[j + 1][i][G] + in[j][i + 1][G] + in[j + 1 ][i + 1][G]) / 4.0;
            out[j][i][B] = (in[j][i][B] + in[j + 1][i][B] + in[j][i + 1][B] + in[j + 1 ][i + 1][B]) / 4.0;
            out[j][i][A] = (in[j][i][A] + in[j + 1][i][A] + in[j][i + 1][A] + in[j + 1 ][i + 1][A]) / 4.0;
        }
    }
}
