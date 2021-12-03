/* buffering.c */

/* When printing to the terminal via stdout, a '\n' character will "flush"
 * the buffer, i.e., output everything that has been buffered thus far
 * (this is line-based buffering)
 *
 * When we output to a file, a '\n' character no longer flushes the buffer
 * (this is block-based buffering)
 */

#include <stdio.h>
#include <stdlib.h>

int main()
{
  /* change stdout buffering behavior to be unbuffered: */
  setvbuf( stdout, NULL, _IONBF, 0 );
    /* by using setvbuf() here, we can remove the fflush() calls below */

  fprintf( stderr, "A" );

  printf( "HERE0" );
  fflush( stdout );
  int * xyz = NULL;

  printf( "HERE1" );
  fflush( stdout );
  *xyz = 1234;

  printf( "HERE2" );
  fflush( stdout );
  printf( "%d\n", *xyz );

  printf( "HERE3" );
  fflush( stdout );

  return EXIT_SUCCESS;
}
