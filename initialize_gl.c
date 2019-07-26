/*
 * draw_triangle_gl()
 */
void draw_triangle_gl( POINT *v0, POINT *v1, POINT *v2 )
{
    glBegin( GL_TRIANGLES );
    if( modulate ) glColor4fv( v0->RGBA );
    v0->STRQ[3] = 1;
    
    if( texturing && !tex_gen ) glTexCoord4fv( v0->STRQ );
    if( face_lighting || phong_lighting || tex_gen ) glNormal3fv( v0->normal );
    if( sw_vertex_processing )
        glVertex3f( v0->position[X] + 0.5, v0->position[Y] + 0.5, v0->position[Z] );
    else
        glVertex3f( v0->world[X], v0->world[Y], -v0->world[Z] );
    
    if( modulate ) glColor4fv( v1->RGBA );
    v1->STRQ[3] = 1;
    if( texturing && !tex_gen ) glTexCoord4fv( v1->STRQ );
    if( face_lighting || phong_lighting || tex_gen ) glNormal3fv( v1->normal );
    if( sw_vertex_processing )
        glVertex3f( v1->position[X] + 0.5, v1->position[Y] + 0.5, v1->position[Z] );
    else
        glVertex3f( v1->world[X], v1->world[Y], -v1->world[Z] );
    
    if( modulate ) glColor4fv( v2->RGBA );
    v2->STRQ[3] = 1;
    if( texturing && !tex_gen ) glTexCoord4fv( v2->STRQ );
    if( face_lighting || phong_lighting || tex_gen ) glNormal3fv( v2->normal );
    if( sw_vertex_processing )
        glVertex3f( v2->position[X] + 0.5, v2->position[Y] + 0.5, v2->position[Z] );
    else
        glVertex3f( v2->world[X], v2->world[Y], -v2->world[Z] );
    glEnd();
}


/*
 * draw_line_gl()
 */
/*
 * draw_line_gl()
 */
void draw_line_gl( POINT *start, POINT *end )
{
    glBegin( GL_LINES );
    /*
     * start vertex
     */
    if( modulate ) glColor4fv( start->RGBA );
    if( texturing && !tex_gen ) glTexCoord4fv( start->STRQ );
    if( face_lighting || phong_lighting || tex_gen )  glNormal3fv( start->normal );
    if( sw_vertex_processing )
        glVertex3f( start->position[X] + 0.5, start->position[Y] + 0.5, start->position[Z] );
    else
        glVertex3f( start->world[X], start->world[Y], -start->world[Z] );
    
    /*
     * end vertex
     */
    if( modulate ) glColor4fv( end->RGBA );
    if( texturing && !tex_gen ) glTexCoord4fv( end->STRQ );
    if( face_lighting || phong_lighting || tex_gen )  glNormal3fv( end->normal );
    if( sw_vertex_processing )
        glVertex3f( end->position[X] + 0.5, end->position[Y] + 0.5, end->position[Z] );
    else
        glVertex3f( end->world[X], end->world[Y], -end->world[Z] );
    glEnd();
}
/*
 * convert_image_to_gl
 */
void convert_image_to_gl( IMAGE *input, GL_IMAGE *output )
{
    int w = input->width;
    int h = input->height;
    
    output->width   = w;
    output->height  = h;
    
    output->data = (unsigned char *)malloc( w * h * 4 );
    
    for( int j = 0; j < h; j++ )
    {
        for( int i = 0; i < w; i++ )
        {
            output->data[((j*w)+i)*4+0] = input->data[j][i][R];
            output->data[((j*w)+i)*4+1] = input->data[j][i][G];
            output->data[((j*w)+i)*4+2] = input->data[j][i][B];
            output->data[((j*w)+i)*4+3] = input->data[j][i][A];
        }
    }
}

/*
 * init_gl_state
 */
void init_gl_state( void )
{
    float clear_color[4] = { 0, 0, 0, 1 };
    
    /*
     * GL draw state
     */
    glClearColor(clear_color[R], clear_color[G], clear_color[B], clear_color[A] );
    glPointSize(1.0);
    glColor4f(0.0,1.0,1.0,1.0);
    
    /*
     * GL depth state
     */
    glClearDepth( 1.0 );
    glDisable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    
    /*
     * GL view state
     */
    glViewport( 0, 0, WIN_WIDTH, WIN_HEIGHT );
    
    if( sw_vertex_processing )
    {
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        glOrtho( -window_size, window_size, -window_size, window_size, -40, 40 );
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
    }
    else
    {
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        
        if( perspective_draw )
        {
            glFrustum( -1, 1, -1, 1, near, far );
        }
        else
        {
            glOrtho( -near, near, -near, near, near, far );
        }
        
        glMatrixMode( GL_MODELVIEW );
    }
    
    /*
     * GL texture state
     */
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &textureID );
    glBindTexture( GL_TEXTURE_2D, textureID );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL,   0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,    0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    
    convert_image_to_gl( &starter_texture, &gl_texture );
    
    glTexImage2D( GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 gl_texture.width,
                 gl_texture.height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 gl_texture.data );
    
    glBindTexture( GL_TEXTURE_2D, 0 );
    glDisable( GL_TEXTURE_2D );
    
    free( gl_texture.data );
    
    /*
     * cube map state
     */
    glEnable( GL_TEXTURE_CUBE_MAP );
    glGenTextures( 1, &cubemapID );
    glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapID );
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    for( int i = 0; i < 6; i++ )
    {
        convert_image_to_gl( &cubemap[i], &gl_texture );
        
        glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     GL_RGBA8,
                     gl_texture.width,
                     gl_texture.height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     gl_texture.data );
        
        free( gl_texture.data );
    }
    
    glDisable( GL_TEXTURE_CUBE_MAP );
    glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
    glDisable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
    
    /*
     * texgen state
     */
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); //  GL_OBJECT_LINEAR, GL_EYE_LINEAR, GL_SPHERE_MAP, GL_NORMAL_MAP, or GL_REFLECTION_MAP
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); //  GL_OBJECT_LINEAR, GL_EYE_LINEAR, GL_SPHERE_MAP, GL_NORMAL_MAP, or GL_REFLECTION_MAP
    glDisable( GL_TEXTURE_GEN_S );
    glDisable( GL_TEXTURE_GEN_T );
    
    /*
     * culling state
     */
    glDisable( GL_CULL_FACE );
    
    /*
     * GL fog state
     */
    glFogfv( GL_FOG_COLOR, clear_color );
    glDisable( GL_FOG );
    
    /*
     * GL blend state
     */
    glBlendEquation( GL_FUNC_ADD );
    glBlendColor( 0.5, 0.5, 0.5, 1.0 );
    glBlendFunc( GL_CONSTANT_COLOR, GL_CONSTANT_COLOR );
}

/*
 * default_gl_state
 */
void default_gl_state( void )
{
    if( sw_vertex_processing )
    {
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        glOrtho( -window_size, window_size, -window_size, window_size, -40, 40 );
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
    }
    else
    {
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        
        if( perspective_draw )
        {
            glFrustum( -1, 1, -1, 1, near, far );
        }
        else
        {
            glOrtho( -near, near, -near, near, near, far );
        }
        
        glMatrixMode( GL_MODELVIEW );
    }
    
    /*
     * reset GL state to "pass-through" defaults
     */
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glDisable( GL_DEPTH_TEST );
    glShadeModel( GL_FLAT );
    glDisable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, 0 );
    glDisable( GL_TEXTURE_CUBE_MAP );
    glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
    glDisable( GL_TEXTURE_GEN_S );
    glDisable( GL_TEXTURE_GEN_T );
    glDisable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
    glDisable( GL_CULL_FACE );
    glDisable( GL_FOG );
    glDisable( GL_BLEND );
    glDisable( GL_LIGHTING );
    glDisable( GL_NORMALIZE );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,   zero_vect           );
    glMaterialf(  GL_FRONT_AND_BACK, GL_SHININESS,  0.0                 );
}

/*
 * change_gl_state
 */
void change_gl_state( void )
{
    /*
     + sw_vertex_processing
     + sw_pixel_processing
     + modulate
     + perspective
     + correction
     + alpha_blending
     + depth_testing
     + texturing
     + tex_gen
     + env_mapping
     + wireframe
     bump_mapping
     + draw_specular
     + per_vertex_lighting
     + per_pixel_lighting
     local_light
     */
    
    /*
     * GL projection state
     */
    if( sw_vertex_processing )
    {
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        
        glOrtho( -window_size, window_size, -window_size, window_size, -40, 40 );
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
    }
    else
    {
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        
        if( perspective_draw )
        {
            glFrustum( -1, 1, -1, 1, near, far );
        }
        else
        {
            glOrtho( -near, near, -near, near, near, far );
        }
        
        glMatrixMode( GL_MODELVIEW );
    }
    
    /*
     * GL polygon state
     */
    if( !rasterize )
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    }
    else
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }
    
    /*
     * GL depth state
     */
    if( d_buff_active )
    {
        glEnable( GL_DEPTH_TEST );
    }
    else
    {
        glDisable( GL_DEPTH_TEST );
    }
    
    /*
     * GL shading state
     */
    if( face_lighting )
    {
        glShadeModel( GL_FLAT );
    }
    
    if( phong_lighting )
    {
        glShadeModel( GL_SMOOTH );
    }
    
    /*
     * GL texturing state
     */
    if( texturing )
    {
        /*
         * cube map state
         */
        if( cube_mapping )
        {
            glDisable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, 0 );
            
            glEnable( GL_TEXTURE_CUBE_MAP );
            glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
            glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapID );
        }
        else
        {
            glEnable( GL_TEXTURE_2D );
            glBindTexture( GL_TEXTURE_2D, textureID );
            
            glDisable( GL_TEXTURE_CUBE_MAP );
            glDisable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
            glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
        }
        
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, (modulate) ? GL_MODULATE : GL_DECAL );
        glHint( GL_PERSPECTIVE_CORRECTION_HINT, (perspective_correct) ? GL_NICEST : GL_FASTEST );
        
        if( tex_gen )
        {
            glEnable( GL_TEXTURE_GEN_S );
            glEnable( GL_TEXTURE_GEN_T );
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP); //  GL_OBJECT_LINEAR, GL_EYE_LINEAR, GL_SPHERE_MAP, GL_NORMAL_MAP, or GL_REFLECTION_MAP
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP); //  GL_OBJECT_LINEAR, GL_EYE_LINEAR, GL_SPHERE_MAP, GL_NORMAL_MAP, or GL_REFLECTION_MAP
        }
    }
    else
    {
        glDisable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, 0 );
        
        glDisable( GL_TEXTURE_CUBE_MAP );
        glDisable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
        glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
    }
    
    /*
     * GL culling state
     */
    if( back_face_culling )
    {
        glEnable( GL_CULL_FACE );
    }
    else
    {
        glDisable( GL_CULL_FACE );
    }
    
    /*
     * GL fog state
     */
    if( fog )
    {
        glEnable( GL_FOG );
    }
    else
    {
        glDisable( GL_FOG );
    }
    
    /*
     * GL blend state
     */
    if( alpha_blending )
    {
        glEnable( GL_BLEND );
    }
    else
    {
        glDisable( GL_BLEND );
    }
    
    /*
     * GL lighting state
     */
    if( phong_lighting || local_lighting ) // jamesk
    {
        float gl_light[4];
        
        if( local_lighting )
        {
            copy_vect( light_position, gl_light );
        }
        else
        {
            copy_vect( light, gl_light );
            gl_light[W] = 0;
        }
        
        gl_light[X] *= -1;
        
        glLightfv( GL_LIGHT0, GL_POSITION, gl_light );
        glEnable( GL_LIGHTING );
        glEnable( GL_LIGHT0 );
        glEnable( GL_NORMALIZE );
        
        glLightfv( GL_LIGHT0, GL_AMBIENT,   light_ambient   );
        glLightfv( GL_LIGHT0, GL_DIFFUSE,   light_diffuse   );
        glLightfv( GL_LIGHT0, GL_SPECULAR,  light_specular  );
        
        glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
        
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT,    material_ambient    );
        glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE,    material_diffuse    );
        
        if( specular_lighting )
        {
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,   material_specular   );
            glMaterialf(  GL_FRONT_AND_BACK, GL_SHININESS,  shinyness           );
        }
        else
        {
            glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR,   zero_vect           );
            glMaterialf(  GL_FRONT_AND_BACK, GL_SHININESS,  0.0                 );
        }
    }
    else
    {
        glDisable( GL_LIGHTING );
    }
}
