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
