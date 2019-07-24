//starts timer
void start_timer( TIMER *timer )
{
    
    gettimeofday( &timer->start, NULL );
    
}

//stops timer
void stop_timer( TIMER *timer )
{
    
    gettimeofday( &timer->end, NULL);
    
}

//calculates elapsed time from timer
double elapsed_time( TIMER *timer)
{
    double usec = ( (timer->end.tv_sec - timer->start.tv_sec) * MILLION);
    
    usec += ( timer->end.tv_usec - timer->start.tv_usec );
    
    return (usec / MILLION);
}

void write_timer_savefile()
{ // need to be able to pass in an array of chars that contain the name.
    
    FILE *fp;
    
    fp = fopen("teapot.txt", "w+");
    
    int n = 0;
    float x, y;
    int swp = current_savefile.current_sw;
    int glp = current_savefile.current_gl;
    fprintf(fp, "F1\n");
    fprintf(fp, "%d\n", swp);
    while( n <= current_savefile.current_sw )
    {
        x = current_savefile.sw_time[n++];
        fprintf(fp, "%f\n", x);
    }
    
    int m = 0;
    while( m <= current_savefile.current_gl )
    {
        x = current_savefile.gl_time[m++];
        fprintf(fp, "%f\n", x);
    }
    
    fclose(fp);
    printf("Timer savefile written!\n");
}
