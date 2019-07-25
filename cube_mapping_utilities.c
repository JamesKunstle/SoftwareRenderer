void cube_map_vect( float v[4], float tex[4], int *index )
{
    float maxAxis, s, t;
    float absX = fabsf( v[X] );
    float absY = fabsf( v[Y] );
    float absZ = fabsf( v[Z] );
    
    if( absX > absY && absX > absZ )
    {
        if( v[X] < 0 )
        {
            s =  v[Z];
            t =  -v[Y];
            *index = 0; // right face
            
        }
        else
        {
            s =  -v[Z];
            t =  v[Y];
            *index = 1; // left face
            
        }
    }
    else if( absY > absX && absY > absZ )
    {
        if( v[Y] < 0 )
        {
            s = -v[X];
            t =  v[Y];
            *index = 2; // top face
            
        }
        else
        {
            s =  v[X];
            t =  v[Y];
            *index = 3; // bottom face
            
        }
    }
    else if( absZ > absY && absZ > absX )
    {
        if( v[Z] > 0 )
        {
            s =   v[X];
            t =   v[Y];
            *index = 4; // back face
            
        }
        else
        {
            s =  v[X];
            t =  -v[Y];
            *index = 5; // front face
            
        }
    }
    
    maxAxis = 1.0 / sqrt( 2.0 );
    tex[S] = (( s / maxAxis ) + 1.0 ) / 2.0;
    tex[T] = (( t / maxAxis ) + 1.0 ) / 2.0;
    tex[2] = 0.0;
    tex[3] = 1.0;
    
}

/* Reads 6 textures into the cube_map IMAGE array to be used by drawpoint if cube_mapping is enabled. Texture hardcoded because 6 required.*/
void read_cube_texture_test()
{
    r_binary_text_file_two_row( &cubemap[0], "test_right.ppm");

    r_binary_text_file_two_row( &cubemap[1], "test_left.ppm");

    r_binary_text_file_two_row( &cubemap[2], "test_top.ppm");

    r_binary_text_file_two_row( &cubemap[3], "test_bottom.ppm");
    r_binary_text_file_two_row( &cubemap[4], "test_back.ppm");
    r_binary_text_file_two_row( &cubemap[5], "test_front.ppm"); // the cube_map IMAGE array should contain all of the requisite textures.
}
void read_cube_texture_sky()
{
    r_binary_text_file_two_row( &cubemap[0], "sky_right.ppm");
    r_binary_text_file_two_row( &cubemap[1], "sky_left.ppm");
    r_binary_text_file_two_row( &cubemap[2], "sky_top.ppm");
    r_binary_text_file_two_row( &cubemap[3], "sky_bottom.ppm");
    r_binary_text_file_two_row( &cubemap[4], "sky_back.ppm");
    r_binary_text_file_two_row( &cubemap[5], "sky_front.ppm"); // the cube_map IMAGE array should contain all of the requisite textures.
}

void naive_map_vect(float normal[4], float tex[4])
{
    tex[S] = (normal[X] + 1.0) / 2.0;
    tex[T] = (normal[Y] + 1.0) / 2.0;
    tex[2] = 0.0;
    tex[3] = 1.0;
    
}
/* Uses normals in vertices */
void cylindrical_map_vect( float normal[4], float tex[4] )
{
    float xsq = normal[X] * normal[X];
    float zsq = normal[Z] * normal[Z];
    float PI = 3.1415926;
    
    float radius = sqrt( xsq + zsq );
    float theta = asin( normal[Z] / radius );
    
    if(isnan(theta))
    {
        theta = 0;
    }
    
    tex[S] = (theta + PI / 2.0)/(2.0 * PI) + ((normal[Z] > 0) ? 0 : 0.5);
    tex[T] = ( normal[Y] + 1.0 ) / 2.0;
    tex[2] = 0.0;
    tex[3] = 1.0;
    
}

void spherical_map_vect( float normal[4], float tex[4] )
{
    float xsq = normal[X] * normal[X];
    float zsq = normal[Z] * normal[Z];
    float PI = 3.1415926;
    
    float radius = sqrt( xsq + zsq );
    float theta = asin( normal[Z] / radius );
    float phi = asin( normal[Y] );
    
    if(isnan(theta))
    {
        theta = 0;
    }
    if(isnan(phi))
    {
        phi = 0;
    }
    
    tex[S] = ( theta + PI / 2.0)/( PI * 2.0) + ((normal[Z] > 0) ? 0 : 0.5);
    tex[T] = (phi + PI / 2.0) / PI;
    tex[2] = 0.0;
    tex[3] = 1.0;
}

/* works through the vertices in the vertex buffer and uses the appropriate mapping algorithm to fill the [4]STRQ vector with the correct coords.*/
void map_texture_coordinates()
{
    if( texturing == ON )
    {
        if( naive_mapping == ON )
        {
            printf("Naive-mapping the current object.\n");
            for( int i = 0; i < numvertices; i++ )
            {
                naive_map_vect( vertex_list[i].normal, vertex_list[i].STRQ );
            }
        }
        else if( cylindrical_mapping == ON )
        {
            printf("Cylindrical-mapping the current object.\n");
            for( int i = 0; i < numvertices; i++ )
            {
                cylindrical_map_vect( vertex_list[i].normal, vertex_list[i].STRQ );
            }
        }
        else if( spherical_mapping == ON )
        {
            printf("Spherical-mapping the current object.\n");
            for( int i = 0; i < numvertices; i++ )
            {
                spherical_map_vect( vertex_list[i].normal, vertex_list[i].STRQ );
            }
        }
        else
        {
            printf( "Texturing mapping algorithm has not been selected.\n" );
        }
    }
    else
    {
        return;
    }
}

void reflection_map_vect( float normal[4], float tex[4] )
{
    float reflect[4];
    
    reflect_vect( eye, normal, reflect);
    spherical_map_vect(reflect, tex);
}

