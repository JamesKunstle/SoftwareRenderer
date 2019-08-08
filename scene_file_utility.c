/* GLOBAL STRUCTS AND VARIABLES */
typedef struct model{
    int type;
    int material_index;
    float center[4];
    float scale[4];
    float rotation[4];
}MODEL;

#define SPHERE 100
#define QUAD   101
#define TEAPOT 102
#define MAX_OBJECTS 50

/*------------------------------*/

MODEL object_list[MAX_OBJECTS];
int num_objects = 0;

void read_scene_file(char* filename)
{
    FILE *fp;
    char object_type[16];
    if( ( fp = fopen(filename, "r" ) ) == NULL )
        return;
    
    num_objects = 0;
    
    while( 1 )
    {
        MODEL *model = &object_list[num_objects];
        
        if( fscanf( fp, "MODEL %s [%d] [%f %f %f] [%f %f %f] [%f %f %f]\n", object_type, &model->material_index, &model->center[X], &model->center[Y], &model->center[Z], &model->scale[X], &model->scale[Y], &model->scale[Z], &model->rotation[X], &model->rotation[Y], &model->rotation[Z]) != 11)
        {
            break;
        }
        if( strncmp(object_type, "QUAD", 4) == 0)
        {
            model->type = QUAD;
        }
        else if( strncmp(object_type, "SPHERE", 6) == 0)
        {
            model->type = SPHERE;
        }
        num_objects++;
    }
    fclose(fp);
}

