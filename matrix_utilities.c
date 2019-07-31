/* raw matrix copy. no structs. */
void copy_matrix( float in[4][4], float out[4][4] )
{
    for( int j = 0; j < 4; j++ )
    {
        for( int i = 0; i < 4; i++ )
        {
            out[j][i] = in[j][i];
        }
    }
}

void set_identity_matrix( float m[4][4] )
{
    for( int i = 0; i < 4; i++ )
    {
        for( int j = 0; j < 4; j++ )
        {
            m[j][i] = 0;
        }
    }
    m[0][0] = 1;
    m[1][1] = 1;
    m[2][2] = 1;
    m[3][3] = 1;
}

void set_identity_matrix_4( MATRIX_4 m )
{
    for( int i = 0; i < 4; i++ )
    {
        for( int j = 0; j < 4; j++ )
        {
            m.table[j][i] = 0;
        }
    }
    m.table[0][0] = 1;
    m.table[1][1] = 1;
    m.table[2][2] = 1;
    m.table[3][3] = 1;
}

void mult_vect_matrix( float v[4], float m[4][4], float r[4] )
{
    float temp[4] = { 0, 0, 0, 0 };
    
    for( int j = 0; j < 4; j++ )
    {
        for( int i = 0; i < 4; i++ )
        {
            temp[j] += v[i] * m[i][j];
        }
        
    }
    copy_vect( temp, r );   // the contents of the temporary vector are copied into the r vector which is passed to the function by memory.
}

void set_translate_matrix( float m[4][4], float zedepth )
{
    m[0][0] = 1;
    m[0][1] = 0;
    m[0][2] = 0;
    m[0][3] = 0;
    
    m[1][0] = 0;
    m[1][1] = 1;
    m[1][2] = 0;
    m[1][3] = 0;
    
    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 1;
    m[2][3] = 0;
    
    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = zedepth;
    m[3][3] = 1;
    
}

void set_translate_matrix_camera( float m[4][4], float xdepth, float ydepth, float zdepth )
{
    m[0][0] = 1;
    m[0][1] = 0;
    m[0][2] = 0;
    m[0][3] = xdepth;
    
    m[1][0] = 0;
    m[1][1] = 1;
    m[1][2] = 0;
    m[1][3] = ydepth;
    
    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 1;
    m[2][3] = zdepth;
    
    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;
    
}


void mult_matrix_matrix( float m1[4][4], float m2[4][4], float mout[4][4] )
{
    float temp[4][4];
    for( int j = 0; j < 4; j++ )
    {
        for( int i = 0; i < 4; i++ )
        {
            temp[j][i] = 0;
            
            for( int k = 0; k < 4; k++ )
            {
                temp[j][i] += m1[j][k] * m2[k][i];
            }
        }
    }
    
    copy_matrix( temp, mout );
}


// generates the perspective transform matrix
void gen_p_matrix( float mask[4][4], float near, float far )
{
    set_identity_matrix( mask ); // interior set to an identity matrix of diagonal 1's.
    
    mask[0][0] = near;
    mask[1][1] = near;
    mask[2][2] = 0;
    mask[2][3] = 1;
    mask[3][2] = ( far - near );
    mask[3][3] = 0;
}

// forms model using the perspective transform matrix
void p_form_model_matrix( float near, float far )
{
    float mask[4][4];
    gen_p_matrix( mask, near, far);
    for( int i = 0; i < numvertices; i++)
    {
        mult_vect_matrix(vertex_list[i].world, mask, vertex_list[i].position);
        
        //vertex_list[i].position[3] = 1.0 / vertex_list[i].position[3]; // z becomes 1/z
        
        div_scalar_vector(vertex_list[i].position, vertex_list[i].position[3], vertex_list[i].position); // homogenous divide by Z in W position.
        
        if( perspective_correct )
        {
            //vertex_list[i].position[Z] = 1.0 / vertex_list[i].position[Z];
            vertex_list[i].STRQ[S] *= vertex_list[i].position[Z];
            vertex_list[i].STRQ[T] *= vertex_list[i].position[Z];
        }
    }
}

void t_model_matrix(float depth) // translates a generated model down the Z-axis by depth.
{
    //move all points in our model into the plane by d.
    float translate_matrix[4][4];
    set_translate_matrix( translate_matrix, depth ); // fill the matrix with the correct values
    for(int i = 0; i < numvertices; i++)
    {
        vertex_list[i].world[W] = 1.0;
        mult_vect_matrix( vertex_list[i].world, translate_matrix, vertex_list[i].world );
    }
}

void set_scale_matrix( float m[4][4], float sx, float sy, float sz )
{
    set_identity_matrix(m);
    m[0][0] = sx;
    m[1][1] = sy;
    m[2][2] = sz;
}
/* create matrix that will rotate a point around the x-axis. */
void set_rotate_x_matrix( float m[4][4], float xangle )
{
    float theta = xangle / 360.0 * 2.0 * 3.141592654;
    
    m[0][0] = 1;
    m[0][1] = 0;
    m[0][2] = 0;
    m[0][3] = 0;
    
    m[1][0] = 0;
    m[1][1] = cos(theta);
    m[1][2] = sin(theta);
    m[1][3] = 0;
    
    m[2][0] = 0;
    m[2][1] = -sin(theta);
    m[2][2] = cos(theta);
    m[2][3] = 0;
    
    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;
    
}

void set_rotate_y_matrix( float m[4][4], float yangle )
{
    float theta = yangle / 360.0 * 2.0 * 3.141592654;
    
    m[0][0] = cos(theta);
    m[0][1] = 0;
    m[0][2] = -sin(theta);
    m[0][3] = 0;
    
    m[1][0] = 0;
    m[1][1] = 1;
    m[1][2] = 0;
    m[1][3] = 0;
    
    m[2][0] = sin(theta);
    m[2][1] = 0;
    m[2][2] = cos(theta);
    m[2][3] = 0;
    
    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;
    
}

void set_rotate_z_matrix( float m[4][4], float zangle )
{
    float theta = zangle / 360.0 * 2.0 * 3.141592654;
    
    m[0][0] = cos(theta);
    m[0][1] = sin(theta);
    m[0][2] = 0;
    m[0][3] = 0;
    
    m[1][0] = -sin(theta);
    m[1][1] = cos(theta);
    m[1][2] = 0;
    m[1][3] = 0;
    
    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 1;
    m[2][3] = 0;
    
    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;
    
}

// This is used for the translate function. Sets the matrix such that a vector . matrix will translate the z value.

void scale_model_matrix( float sx, float sy, float sz )
{
    float m[4][4];
    
    for( int i = 0; i < numvertices; i++ )
    {
        mult_vect_matrix( vertex_list[i].world, m, vertex_list[i].world);
    }
}
//void rotate_model_xy(float x_angle, float y_angle, float z_angle)
// for every point in the vertices, change the spatial x, y, z dimensions using the input angles and the point world properties
void rotate_model_matrix(float ex_angle, float ey_angle, float ez_angle)
{
    float xrotate[4][4]; //jamesk come back to concatenate
    float yrotate[4][4];
    float zrotate[4][4];
    float m[4][4];
    
    set_rotate_x_matrix( xrotate, ex_angle );
    set_rotate_y_matrix( yrotate, ey_angle );
    set_rotate_z_matrix( zrotate, ez_angle ); // local matrices are filled with values.
    
    set_identity_matrix( m );
    
    mult_matrix_matrix(xrotate, yrotate, m);
    mult_matrix_matrix(zrotate, m, m);
    
    
    for( int i = 0; i < numvertices; i++) // every vertex is transformed based on the matrices provided.
    {
//        mult_vect_matrix( vertex_list[i].world, xrotate, vertex_list[i].world );
//        mult_vect_matrix( vertex_list[i].world, yrotate, vertex_list[i].world );
//        mult_vect_matrix( vertex_list[i].world, zrotate, vertex_list[i].world );
          mult_vect_matrix( vertex_list[i].world, m, vertex_list[i].world );
        
    }
}

void set_camera_matrix(MATRIX_4 m, float eye[4], float lookat[4], float up[4])
{
    float u[4];
    float v[4];
    MATRIX_4 t;
    MATRIX_4 r;
    
    
    
    subtract_vectors( lookat, eye, v );
    normalize_vector( v, v );
    
    cross_vect( up, v, u ); // getting the perpendicular vector
    normalize_vector( u, u );
    
    cross_vect( v, u, up ); // getting the perpendicular vector
    normalize_vector( up, up );
    
    set_identity_matrix_4( r );
    set_identity_matrix_4( t );
    
    for( int j = 0; j < 3; j++ )
    {
        r.table[j][0] = u[j];
        r.table[j][1] = up[j];
        r.table[j][2] = v[j];
    }
    
    set_translate_matrix_camera( m.table, -eye[X], -eye[X], -eye[X] );
    mult_matrix_matrix( t.table, r.table, m.table );
    // m matrix implicitly passed out of the function.
    // using this after rotation and translation will allow all of our other math to work.
}

void rotate_translate_matrix( float ex_angle, float ey_angle, float ez_angle, float depth )
{
    float m[4][4];
    
    if( !camera )
    {
        float xrotate[4][4];
        float yrotate[4][4];
        float zrotate[4][4];
        
        set_rotate_x_matrix( xrotate, ex_angle );
        set_rotate_y_matrix( yrotate, ey_angle );
        set_rotate_z_matrix( zrotate, ez_angle );
        
        float translate_matrix[4][4];
        set_translate_matrix( translate_matrix, depth ); // fill the matrix with the correct values
        
        mult_matrix_matrix(xrotate, yrotate, m);
        mult_matrix_matrix(zrotate, m, m);
        mult_matrix_matrix(m, translate_matrix, m); // rotate, then translate
        //mult_matrix_matrix(master_matrix, mask, master_matrix);
    }
    else
    {   MATRIX_4 c;
        set_camera_matrix( c, eye, lookat, global_camera.up );
        copy_matrix( c.table, m );
    }

    
    for( int i = 0; i < numvertices; i++)
    {
        vertex_list[i].world[W] = 1.0;
        mult_vect_matrix( vertex_list[i].world, m, vertex_list[i].world );
    }
}
