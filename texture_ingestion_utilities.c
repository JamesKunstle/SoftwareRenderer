void r_text_file(IMAGE *image, char *name) // read an ASCII ppm texture
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

void r_binary_text_file(IMAGE *image, char *name) // read a binary ppm texture
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
                image->data[y][x][A] = 1; 
            }
        }
        fclose(fp);
    }
    else
    {
        printf("Not a valid binary P6 file~\n");
    }
}

void r_binary_text_file_two_row(IMAGE *image, char *name) // for specific file type
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

