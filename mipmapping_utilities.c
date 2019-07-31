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

/* ============================   PROTOTYPES     ============================ */
void triage_mipmap_fill( MIPMAP *cmm);
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
    
    triage_mipmap_fill();               // pass the determined LOD and a reference to the current mipmap
}

void triage_mipmap_fill()                      // sends the correct textures to be minimized to their correct places.
{
    int xstep;                  // integer division to determin how many steps it takes to downsample in to out for a 2x2 box filter.
    int ystep;
    int height;
    int width;
    int stepdown = 0;           // indicates whether we're reading from the current_texture (in which case we're intializing the first of the mipmap textures) or whether we're stepping down to the next smaller texture from a larger one above that was already initialized.
    switch ( cmm.max_mipmap_ind )
    {
        case 0:
            height = 1024;
            width = 1024;
            xstep = width / 2;                  // integer division to determin how many steps it takes to downsample in to out for a 2x2 box filter.
            ystep = height / 2;
            
            for ( int j = 0; j < height; j += 2 )   // step of the box-filter. jamesk: needs to be able to accomodate 1x1 and 2x2. Probably with conditions in the below assignment.
            {
                for ( int i = 0; i < width; i += 2 )
                {
                    mm.zero[j][i][R] = (current_texture[j][i][R] + current_texture[j + 1][i][R] + current_texture[j][i + 1][R] + current_texture[j + 1][i + 1][R]) / 4.0;
                    mm.zero[j][i][G] = (current_texture[j][i][G] + current_texture[j + 1][i][G] + current_texture[j][i + 1][G] + current_texture[j + 1][i + 1][G]) / 4.0;
                    mm.zero[j][i][B] = (current_texture[j][i][B] + current_texture[j + 1][i][B] + current_texture[j][i + 1][B] + current_texture[j + 1][i + 1][B]) / 4.0;
                    mm.zero[j][i][A] = (current_texture[j][i][A] + current_texture[j + 1][i][A] + current_texture[j][i + 1][A] + current_texture[j + 1][i + 1][A]) / 4.0;
                }
            }
            continue;
        case 1:
            height = 512;
            width = 512;
            xstep = width / 2;                  // integer division to determin how many steps it takes to downsample in to out for a 2x2 box filter.
            ystep = height / 2;
            if( !stepdown )
            {
                for ( int j = 0; j < height; j += 2 )   // step of the box-filter. jamesk: needs to be able to accomodate 1x1 and 2x2. Probably with conditions in the below assignment.
                {
                    for ( int i = 0; i < width; i += 2 )
                    {
                        mm.one[j][i][R] = (current_texture[j][i][R] + current_texture[j + 1][i][R] + current_texture[j][i + 1][R] + current_texture[j + 1][i + 1][R]) / 4.0;
                        mm.one[j][i][G] = (current_texture[j][i][G] + current_texture[j + 1][i][G] + current_texture[j][i + 1][G] + current_texture[j + 1][i + 1][G]) / 4.0;
                        mm.one[j][i][B] = (current_texture[j][i][B] + current_texture[j + 1][i][B] + current_texture[j][i + 1][B] + current_texture[j + 1][i + 1][B]) / 4.0;
                        mm.one[j][i][A] = (current_texture[j][i][A] + current_texture[j + 1][i][A] + current_texture[j][i + 1][A] + current_texture[j + 1][i + 1][A]) / 4.0;
                    }
                }
            }
            else
            {
                for ( int j = 0; j < height; j += 2 )   // step of the box-filter. jamesk: needs to be able to accomodate 1x1 and 2x2. Probably with conditions in the below assignment.
                {
                    for ( int i = 0; i < width; i += 2 )
                    {
                        mm.one[j][i][R] = (zero[j][i][R] + zero[j + 1][i][R] + zero[j][i + 1][R] + zero[j + 1][i + 1][R]) / 4.0;
                        mm.one[j][i][G] = (zero[j][i][G] + zero[j + 1][i][G] + zero[j][i + 1][G] + zero[j + 1][i + 1][G]) / 4.0;
                        mm.one[j][i][B] = (zero[j][i][B] + zero[j + 1][i][B] + zero[j][i + 1][B] + zero[j + 1][i + 1][B]) / 4.0;
                        mm.one[j][i][A] = (zero[j][i][A] + zero[j + 1][i][A] + zero[j][i + 1][A] + zero[j + 1][i + 1][A]) / 4.0;
                    }
                }
            }
            continue;
        case 2:
            height = 256;
            width = 256;
            xstep = width / 2;                  // integer division to determin how many steps it takes to downsample in to out for a 2x2 box filter.
            ystep = height / 2;
            if( !stepdown )
            {
                for ( int j = 0; j < height; j += 2 )   // step of the box-filter. jamesk: needs to be able to accomodate 1x1 and 2x2. Probably with conditions in the below assignment.
                {
                    for ( int i = 0; i < width; i += 2 )
                    {
                        mm.two[j][i][R] = (current_texture[j][i][R] + current_texture[j + 1][i][R] + current_texture[j][i + 1][R] + current_texture[j + 1][i + 1][R]) / 4.0;
                        mm.two[j][i][G] = (current_texture[j][i][G] + current_texture[j + 1][i][G] + current_texture[j][i + 1][G] + current_texture[j + 1][i + 1][G]) / 4.0;
                        mm.two[j][i][B] = (current_texture[j][i][B] + current_texture[j + 1][i][B] + current_texture[j][i + 1][B] + current_texture[j + 1][i + 1][B]) / 4.0;
                        mm.two[j][i][A] = (current_texture[j][i][A] + current_texture[j + 1][i][A] + current_texture[j][i + 1][A] + current_texture[j + 1][i + 1][A]) / 4.0;
                    }
                }
            }
            else
            {
                for ( int j = 0; j < height; j += 2 )   // step of the box-filter. jamesk: needs to be able to accomodate 1x1 and 2x2. Probably with conditions in the below assignment.
                {
                    for ( int i = 0; i < width; i += 2 )
                    {
                        mm.two[j][i][R] = (one[j][i][R] + one[j + 1][i][R] + one[j][i + 1][R] + one[j + 1][i + 1][R]) / 4.0;
                        mm.two[j][i][G] = (one[j][i][G] + one[j + 1][i][G] + one[j][i + 1][G] + one[j + 1][i + 1][G]) / 4.0;
                        mm.two[j][i][B] = (one[j][i][B] + one[j + 1][i][B] + one[j][i + 1][B] + one[j + 1][i + 1][B]) / 4.0;
                        mm.one[j][i][A] = (one[j][i][A] + one[j + 1][i][A] + one[j][i + 1][A] + one[j + 1][i + 1][A]) / 4.0;
                    }
                }
            }
            continue;
        default:
            printf("Unable to triage.\n");
    }
}

void mipmap_fill( float in[][][], float out[][][], int height, int width)
{

}
