// This document contains the variables and the functions to allow mip-mapping for our 3D software renderer.

/* GLOBAL VARIABLES */

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

int max_mipmap[10];         // array of the references to the mip-map arrays.
max_mipmap[0] = &zero;
max_mipmap[1] = &one;
max_mipmap[2] = &two;
max_mipmap[3] = &three;
max_mipmap[4] = &four;
max_mipmap[5] = &five;
max_mipmap[6] = &six;
max_mipmap[7] = &seven;
max_mipmap[8] = &eight;
max_mipmap[9] = &nine;
max_mipmap[10] = &ten;

int max_mipmap_ind;             // index into the max_mipmap array of references for the current mipmap of max dimensions.

void generate_mipmap()      // reads in the data from the current_texture array. This must be initialized first.
{
    int height = current_texture.height;
    int width = current_texture.width;
    int threshold = (height) < (width)) ? (height) : (width)); // determines the max_mipmap
    
    if( threshold > 1024 )
    {
        max_mipmap_ind = 0;
    }
    else if ( threshold > 512 )
    {
        max_mipmap_ind = 1;
    }
    else if ( threshold > 256 )
    {
        max_mipmap_ind = 2;
    }
    else if ( threshold > 128 )
    {
        max_mipmap_ind = 3;
    }
    else if ( threshold > 64 )
    {
        max_mipmap_ind = 4;
    }
    else if ( threshold > 32 )
    {
        max_mipmap_ind = 5;
    }
    else if ( threshold > 16 )
    {
        max_mipmap_ind = 6;
    }
    else if ( threshold > 8 )
    {
        max_mipmap_ind = 7;
    }
    else if ( threshold > 4 )
    {
        max_mipmap_ind = 8;
    }
    else if ( threshold > 2 )
    {
        max_mipmap_ind = 9;
    }
    else if ( threshold > 1 )
    {
        max_mipmap_ind = 10;
    }
    else
    {
        printf("Texture size invalid.\n");
    }
    
    
}
