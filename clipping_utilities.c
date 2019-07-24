/* Adds a new triangle into the triangle_list array.*/
// compiles vertices from vertex list into a new triangle in triangle_list
void add_triangle(int v0, int v1, int v2)
{
    triangle_list[numtriangles].vertex[0] = v0;
    triangle_list[numtriangles].vertex[1] = v1;
    triangle_list[numtriangles].vertex[2] = v2;
    numtriangles++;
}

void setup_clip_frustum( void )
{
    float inv_sqrt_2 = 1.0/sqrt(2.0);
    
    // left
    set_vect( frustum[0].normal,  inv_sqrt_2, 0, -inv_sqrt_2 );
    frustum[0].dist = 0;
    
    // right
    set_vect( frustum[1].normal, -inv_sqrt_2, 0, -inv_sqrt_2 );
    frustum[1].dist = 0;
    
    // top
    set_vect( frustum[2].normal, 0, -inv_sqrt_2, -inv_sqrt_2 );
    frustum[2].dist = 0;
    
    // bottom
    set_vect( frustum[3].normal, 0,  inv_sqrt_2, -inv_sqrt_2 );
    frustum[3].dist = 0;
    
    // near
    set_vect( frustum[4].normal, 0, 0, -1 );
    frustum[4].dist = near;
    
    //far
    set_vect( frustum[5].normal, 0, 0, 1 );
    frustum[5].dist = far;
}

/*
 * vect_distance_to_plane()
 */
float vect_distance_to_plane( float v[4], PLANE *p )
{
    return( dot(v, p->normal) - p->dist);
}


/*
 
 * line_clip_plane()
 
 */

int line_clip_plane( POINT *a, POINT *b, POINT *new, int k )
{
    float   da = a->dist[k]; // temp chrisb vect_distance_to_plane( a->world, p );   // distance from plane to point a
    float   db = b->dist[k]; // temp chrisb vect_distance_to_plane( b->world, p );   // distance from plane to point b
    float   s;
    
    if( da < 0 && db < 0 )          // both points outside plane
    {
        return( -1 );
    }
    else if( da > 0 && db > 0 )     // both points inside plane
    {
        return( 1 );
    }
    else
    {
        s = da/(da-db);             // intersection factor (between 0 and 1)
        
        interpolate_two_vect( s, b->world,  a->world,   new->world    );
        
        interpolate_two_vect( s, b->RGBA,  a->RGBA,   new->RGBA    );
        
        interpolate_two_vect( s, b->STRQ,    a->STRQ,     new->STRQ      );
        
        interpolate_two_vect( s, b->normal, a->normal,  new->normal   );
        
        interpolate_two_vect( s, b->light,  a->light,   new->light    );
        
        for( int k = 0; k < 6; k++ )
        {
            new->dist[k] = a->dist[k] + s * (b->dist[k] - a->dist[k]);
        }
        return( 0 );
    }
}


/*
 * tri_entirely_inside_frustum()
 */
int tri_entirely_inside_frustum( POINT v[3] )

{
    /*
     * for each vertex, check if it's on the inside of all frustum planes
     */
    for( int k = 0; k < 5; k++ )
    {
        if( (v[0].dist[k] < 0) || (v[1].dist[k] < 0) || (v[2].dist[k] < 0) )
            return( 0 );
    }

    return( 1 );
}

/*
 * tri_entirely_outside_frustum()
 */
int tri_entirely_outside_frustum( POINT v[3] )
{
    /*
     * check if all three vertices are on the outside of any frustum plane
     */
    for( int k = 0; k < 5; k++ )
    {
        if( (v[0].dist[k] < 0) && (v[1].dist[k] < 0) && (v[2].dist[k] < 0) )
            return( 1 );
    }
    return( 0 );
}

/*
 * tri_clip_frustum()
 */
int tri_clip_frustum( POINT *verts )
{
    POINT   new;
    POINT   clipped[8];
    POINT   *in;
    POINT   *out;
    POINT   *tmp;
    int     num_in = 3;
    int     num_out;
    
    in  = verts;
    out = clipped;
    
    
    for( int k = 0; k < 5; k++ )
    {
        num_out = 0;
        
        /*
         * walk around edges of polygon - each edge goes from i to next
         */
        for( int i = 0; i < num_in; i++ )
        {
            int next = (i+1) % num_in;
        
//            in[i].dist[k] = vect_distance_to_plane( in[i].world, &frustum[k] );
//            in[next].dist[k] = vect_distance_to_plane( in[next].world, &frustum[k] );
//
            if( in[i].dist[k] < 0 && in[next].dist[k] < 0 )
            {
                ; // both points are outside, do nothing
            }
            else if( in[i].dist[k] >= 0 && in[next].dist[k] >= 0 )
            {
                // both points are inside, add second to output list
                out[num_out++] = in[next];
            }
            else if( in[i].dist[k] >= 0 && in[next].dist[k] < 0 )
            {
                // first point is inside, second one is out, add intersection to output list
                line_clip_plane( &in[i], &in[next], &new, k );
                out[num_out++] = new;
            }
            else if( in[i].dist[k] < 0 && in[next].dist[k] >= 0 )
            {
                // first point is outside, second one is in, add intersection and second point to output list
                line_clip_plane( &in[i], &in[next], &new, k );
                out[num_out++] = new;
                out[num_out++] = in[next];
            }
            
        }
        
        tmp = in;
        
        in  = out;
        
        out = tmp;
        
        num_in  = num_out;
    }
    for( int i = 0; i < num_out; i++ )
        verts[i] = in[i];
    
    return( num_out );
}


/*
 
 * calculate_clip_distances()
 
 */

void calculate_clip_distances( void )
{
    POINT   *p;
    POINT   verts[6];
    POINT   *v0, *v1, *v2;
    int     limit = numtriangles;
    
    
    /*
     * for each vertex, check if it's on the inside of all frustum planes
     */
    for( int i = 0; i < numvertices; i++ )
    {
        p = &vertex_list[i];
        
        for( int k = 0; k < 6; k++ )
        {
            p->dist[k] = vect_distance_to_plane( p->world, &frustum[k]);
            //printf("Distance %f\n", p->dist[k]);
        }
    }
    
    /*
     * for each triangle, check if it's inside, outside, or needs to be clipped
     */
    
    printf("Number of vertices before loop: %d\n", numvertices);
    for( int i = 0; i < limit; i++ )
    {
        verts[0] = vertex_list[triangle_list[i].vertex[0]];
        verts[1] = vertex_list[triangle_list[i].vertex[1]];
        verts[2] = vertex_list[triangle_list[i].vertex[2]];
        
        triangle_list[i].clip_flag = CLIPPED;
        
        if( tri_entirely_inside_frustum(verts) )
        {
            triangle_list[i].clip_flag = IN;
        }
        else if( tri_entirely_outside_frustum(verts) )
        {
            triangle_list[i].clip_flag = CLIPPED;
        }
        else
        {
            triangle_list[i].clip_flag = CLIPPED;
            int num_clipped = tri_clip_frustum( verts );
            printf("num_clipped: %d\n", num_clipped);
            if( num_clipped == 0 )
                continue;
            
            for( int j = 1; j < num_clipped-1; j++ )
            {
                int new_verts = numvertices;
                
                p               = &vertex_list[numvertices++];
                *p              = verts[0];
                p->num_tri      = 0;
                
                p               = &vertex_list[numvertices++];
                *p              = verts[j];
                p->num_tri      = 0;
    
                p               = &vertex_list[numvertices++];
                *p              = verts[j + 1];
                p->num_tri      = 0;
                
                int new_tri = numtriangles;
                
                add_triangle( new_verts, new_verts+1, new_verts+2 );
                
                printf("Adding triangle = %d %d %d %d\n", new_verts, new_verts+1, new_verts+2, new_tri );
                copy_vect( triangle_list[i].normal, triangle_list[new_tri].normal    );
                //copy_vect( triangle_list[i].light,  triangle_list[new_tri].light     );
                triangle_list[new_tri].clip_flag = IN;
            }
        }
    }
    printf("Number of vertices after loop: %d\n", numvertices);
}
