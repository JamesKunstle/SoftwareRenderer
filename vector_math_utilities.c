
void add_vectors(float vector1[4], float vector2[4], float output[4])
{
    for(int i = 0; i < 4; i++)
    {
        output[i] = vector1[i] + vector2[i];
    }
}

void subtract_vectors(float vector1[4], float vector2[4], float output[4])
{ // second vector is subtracted from the first one.
    for(int i = 0; i < 4; i++)
    {
        output[i] = vector1[i] - vector2[i];
    }
}

void mult_scalar_vector(float vector[4], float scalar, float output[4])
{
    for(int i = 0; i < 4; i++)
    {
        output[i] = vector[i] * scalar;
    }
}

void div_scalar_vector(float vector[4], float scalar, float output[4])
{
    for(int i = 0; i < 4; i++)
    {
        output[i] = vector[i] / scalar;
    }
}

/* returns vector length */
float length_vector(float v[4])
{
    
    return sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
    
}

void copy_vect(float i[4], float o[4])
{
    o[0] = i[0];
    o[1] = i[1];
    o[2] = i[2];
    o[3] = i[3];
}

void copy_vect_RGBA( unsigned char i[4], unsigned char o[4])
{
    o[0] = i[0];
    o[1] = i[1];
    o[2] = i[2];
    o[3] = i[3];
}

void set_vect(float out[4], float x, float y, float z)
{
    out[X] = x;
    out[Y] = y;
    out[Z] = z;
}

void set_vect_4(float out[4], float x, float y, float z, float w)
{
    out[X] = x;
    out[Y] = y;
    out[Z] = z;
    out[W] = w;
}

void interpolate_two_vect( float factor, float a[4], float b[4], float r[4] ) // from Chris
{
    r[X] = (a[X] * factor) + (b[X] * (1-factor));
    r[Y] = (a[Y] * factor) + (b[Y] * (1-factor));
    r[Z] = (a[Z] * factor) + (b[Z] * (1-factor));
    r[W] = (a[W] * factor) + (b[W] * (1-factor));
}

void average_two_vect( float a[4], float b[4], float r[4] ) // from Chris
{
    float factor = 1.0/2.0;
    
    r[X] = (a[X] + b[X]) * factor;
    r[Y] = (a[Y] + b[Y]) * factor;
    r[Z] = (a[Z] + b[Z]) * factor;
    r[W] = (a[W] + b[W]) * factor;
}


void average_three_vect( float a[4], float b[4], float c[4], float r[4] ) // from Chris
{
    float factor = 1.0/3.0;
    
    r[X] = (a[X] + b[X] + c[X]) * factor;
    r[Y] = (a[Y] + b[Y] + c[Y]) * factor;
    r[Z] = (a[Z] + b[Z] + c[Z]) * factor;
    r[W] = (a[W] + b[W] + c[W]) * factor;
}

void mult_vect(float a[4], float b[4], float n[4]) // multiply two vectors and put the product into one final one.
{
    n[X] = a[X] * b[X];
    n[Y] = a[Y] * b[Y];
    n[Z] = a[Z] * b[Z];
    n[W] = 1.0;
}

void normalize_vector(float in[4], float out[4])
{
    float length = length_vector(in);
    div_scalar_vector(in, length, out);
}


/*cross product of a and b goes into the vector n*/
void cross_vect(float a[4], float b[4], float n[4])
{
    n[X] = (a[Y] * b[Z]) - (a[Z] * b[Y]);
    n[Y] = (a[Z] * b[X]) - (a[X] * b[Z]);
    n[Z] = (a[X] * b[Y]) - (a[Y] * b[X]);
    n[W] = 1.0;
}

/* dot product of two input vectors */
float dot(float vec1[4], float vec2[4])
{
    return (vec1[X] * vec2[X]) + (vec1[Y] * vec2[Y]) + (vec1[Z] * vec2[Z]);
}

void reflect_vect( float n[4], float l[4], float r[4]) // passes the calculated value to the passed in reflection array.
{
    float temp[4] = { 0, 0, 0, 0 };
    float a = dot( n, l );
    mult_scalar_vector( n, a, temp );
    mult_scalar_vector( temp, 2, temp );
    subtract_vectors( l, temp, r);
}

