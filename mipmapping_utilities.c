// This document contains the variables and the functions to allow mip-mapping for our 3D software renderer.

/* ============================ GLOBAL VARIABLES ============================ */
typeder struct mm_image{
    
    float data[1024][1024][4];
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

/* ========================================================================== */


void mipmap()
{
    int height    = current_texture.height;
    int width     = current_texture.width;
    int threshold = height < width ? height : width;        // determines the max_mipmap
    
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
    
}











//void mipmap()
//{
//    int height = current_texture.height;
//    int width = current_texture.width;
//    int threshold = height < width ? height : width;        // determines the max_mipmap
//
//    if      ( threshold > 1024 )   mm.big_map = 0;   // taking lower-bound size of mm. 506x506 /=> 512x512.
//
//    else if ( threshold > 512 )    mm.big_map = 1;
//
//    else if ( threshold > 256 )    mm.big_map = 2;
//
//    else if ( threshold > 128 )    mm.big_map = 3;
//
//    else if ( threshold > 64 )     mm.big_map = 4;
//
//    else if ( threshold > 32 )     mm.big_map = 5;
//
//    else if ( threshold > 16 )     mm.big_map = 6;
//
//    else if ( threshold > 8 )      mm.big_map = 7;
//
//    else if ( threshold > 4 )      mm.big_map = 8;
//
//    else if ( threshold > 2 )      mm.big_map = 9;
//
//    else if ( threshold > 1 )      mm.big_map = 10;
//
//    else
//    {
//        printf("Texture size invalid.\n");
//    }
//
//    triage_mipmap_fill();               // pass the determined LOD and a reference to the current mipmap
//}
//
//void triage_mipmap_fill()                      // sends the correct textures to be minimized to their correct places.
//{
//    int xstep;                  // integer division to determin how many steps it takes to downsample in to out for a 2x2 box filter.
//    int ystep;
//    int height;
//    int width;
//    int stepdown = 0;           // indicates whether we're reading from the current_texture (in which case we're intializing the first of the mipmap textures) or whether we're stepping down to the next smaller texture from a larger one above that was already initialized.
//    switch ( cmm.big_map )
//    {
//        case 0:
//            height = 1024;
//            width = 1024;
//            xstep = width / 2;                  // integer division to determin how many steps it takes to downsample in to out for a 2x2 box filter.
//            ystep = height / 2;
//
//            for ( int j = 0; j < height; j += 2 )   // step of the box-filter. jamesk: needs to be able to accomodate 1x1 and 2x2. Probably with conditions in the below assignment.
//            {
//                for ( int i = 0; i < width; i += 2 )
//                {
//                    mm.zero[j][i][R] = (current_texture[j][i][R] + current_texture[j + 1][i][R] + current_texture[j][i + 1][R] + current_texture[j + 1][i + 1][R]) / 4.0;
//                    mm.zero[j][i][G] = (current_texture[j][i][G] + current_texture[j + 1][i][G] + current_texture[j][i + 1][G] + current_texture[j + 1][i + 1][G]) / 4.0;
//                    mm.zero[j][i][B] = (current_texture[j][i][B] + current_texture[j + 1][i][B] + current_texture[j][i + 1][B] + current_texture[j + 1][i + 1][B]) / 4.0;
//                    mm.zero[j][i][A] = (current_texture[j][i][A] + current_texture[j + 1][i][A] + current_texture[j][i + 1][A] + current_texture[j + 1][i + 1][A]) / 4.0;
//                }
//            }
//            continue;
//        case 1:
//            height = 512;
//            width = 512;
//            xstep = width / 2;                  // integer division to determin how many steps it takes to downsample in to out for a 2x2 box filter.
//            ystep = height / 2;
//            if( !stepdown )
//            {
//                for ( int j = 0; j < height; j += 2 )   // step of the box-filter. jamesk: needs to be able to accomodate 1x1 and 2x2. Probably with conditions in the below assignment.
//                {
//                    for ( int i = 0; i < width; i += 2 )
//                    {
//                        mm.one[j][i][R] = (current_texture[j][i][R] + current_texture[j + 1][i][R] + current_texture[j][i + 1][R] + current_texture[j + 1][i + 1][R]) / 4.0;
//                        mm.one[j][i][G] = (current_texture[j][i][G] + current_texture[j + 1][i][G] + current_texture[j][i + 1][G] + current_texture[j + 1][i + 1][G]) / 4.0;
//                        mm.one[j][i][B] = (current_texture[j][i][B] + current_texture[j + 1][i][B] + current_texture[j][i + 1][B] + current_texture[j + 1][i + 1][B]) / 4.0;
//                        mm.one[j][i][A] = (current_texture[j][i][A] + current_texture[j + 1][i][A] + current_texture[j][i + 1][A] + current_texture[j + 1][i + 1][A]) / 4.0;
//                    }
//                }
//            }
//            else
//            {
//                for ( int j = 0; j < height; j += 2 )   // step of the box-filter. jamesk: needs to be able to accomodate 1x1 and 2x2. Probably with conditions in the below assignment.
//                {
//                    for ( int i = 0; i < width; i += 2 )
//                    {
//                        mm.one[j][i][R] = (zero[j][i][R] + zero[j + 1][i][R] + zero[j][i + 1][R] + zero[j + 1][i + 1][R]) / 4.0;
//                        mm.one[j][i][G] = (zero[j][i][G] + zero[j + 1][i][G] + zero[j][i + 1][G] + zero[j + 1][i + 1][G]) / 4.0;
//                        mm.one[j][i][B] = (zero[j][i][B] + zero[j + 1][i][B] + zero[j][i + 1][B] + zero[j + 1][i + 1][B]) / 4.0;
//                        mm.one[j][i][A] = (zero[j][i][A] + zero[j + 1][i][A] + zero[j][i + 1][A] + zero[j + 1][i + 1][A]) / 4.0;
//                    }
//                }
//            }
//            continue;
//        case 2:
//            height = 256;
//            width = 256;
//            xstep = width / 2;                  // integer division to determin how many steps it takes to downsample in to out for a 2x2 box filter.
//            ystep = height / 2;
//            if( !stepdown )
//            {
//                for ( int j = 0; j < height; j += 2 )   // step of the box-filter. jamesk: needs to be able to accomodate 1x1 and 2x2. Probably with conditions in the below assignment.
//                {
//                    for ( int i = 0; i < width; i += 2 )
//                    {
//                        mm.two[j][i][R] = (current_texture[j][i][R] + current_texture[j + 1][i][R] + current_texture[j][i + 1][R] + current_texture[j + 1][i + 1][R]) / 4.0;
//                        mm.two[j][i][G] = (current_texture[j][i][G] + current_texture[j + 1][i][G] + current_texture[j][i + 1][G] + current_texture[j + 1][i + 1][G]) / 4.0;
//                        mm.two[j][i][B] = (current_texture[j][i][B] + current_texture[j + 1][i][B] + current_texture[j][i + 1][B] + current_texture[j + 1][i + 1][B]) / 4.0;
//                        mm.two[j][i][A] = (current_texture[j][i][A] + current_texture[j + 1][i][A] + current_texture[j][i + 1][A] + current_texture[j + 1][i + 1][A]) / 4.0;
//                    }
//                }
//            }
//            else
//            {
//                for ( int j = 0; j < height; j += 2 )   // step of the box-filter. jamesk: needs to be able to accomodate 1x1 and 2x2. Probably with conditions in the below assignment.
//                {
//                    for ( int i = 0; i < width; i += 2 )
//                    {
//                        mm.two[j][i][R] = (one[j][i][R] + one[j + 1][i][R] + one[j][i + 1][R] + one[j + 1][i + 1][R]) / 4.0;
//                        mm.two[j][i][G] = (one[j][i][G] + one[j + 1][i][G] + one[j][i + 1][G] + one[j + 1][i + 1][G]) / 4.0;
//                        mm.two[j][i][B] = (one[j][i][B] + one[j + 1][i][B] + one[j][i + 1][B] + one[j + 1][i + 1][B]) / 4.0;
//                        mm.one[j][i][A] = (one[j][i][A] + one[j + 1][i][A] + one[j][i + 1][A] + one[j + 1][i + 1][A]) / 4.0;
//                    }
//                }
//            }
//            continue;
//        default:
//            printf("Unable to triage.\n");
//    }
//}
//
//void mipmap_fill( float in[][][], float out[][][], int height, int width)
//{
//
//}
