/* realloc.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int * x = calloc( 100, sizeof( int ) );
  printf( "address of x is %p\n", x );

  if ( x == NULL )
  {
    fprintf( stderr, "ERROR: calloc() failed\n" );
    return EXIT_FAILURE;
  }

  x[10] = 1234;
  *(x + 10) = 1234;

  printf( "x[10] is %d\n", *(x + 10) );

  x[150] = 2345;
  printf( "x[150] is %d\n", *(x + 150) );

  int * y = x + 10;
  printf( "y points to %d\n", *y );

  /* double the size of the memory allocated for ptr x */
  x = realloc( x, 200 * sizeof( int ) );
    /* always assume that return value x is different from input arg x */

  printf( "address of x is %p\n", x );

  /* y is now an invalid pointer bc x may have been moved */
  printf( "y points to %d\n", *y );

  *y = 5555;
  printf( "y points to %d\n", *y );

  printf( "x[10] is %d\n", *(x + 10) );
  printf( "x[150] is %d\n", *(x + 150) );

  free( x );

  return EXIT_SUCCESS;
}
