/* dyn.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* TO DO:
 * (1) free() up the memory that has been dynamically allocated
 * (2) translate this to use pointer arithmetic (and no square brackets)
 */

int main()
{
  /* dynamically allocate 100 bytes (on the heap) */
  char * s = malloc( 100 );
    /* do not use malloc() bc it leaves the memory uninitialized */

  char * t = calloc( 100, sizeof( char ) );

  printf( "[%s] [%s]\n", s, t );



#if 0
  int * int_array = malloc( 47 * sizeof( int ) );
#endif

  int * int_array = calloc( 47, sizeof( int ) );

  int_array[18] = 1234;

  printf( "%d\n", int_array[18] );
  printf( "%d\n", *(int_array + 18) );  /* int_array + (18*sizeof(int)) */


  /* array of strings */

  /* char * cache[]; */
  char ** cache;         /* array of char* ... */

  cache = calloc( 47, sizeof( char * ) );    /* 47*8 bytes allocated */

  if ( cache == NULL )
  {
    fprintf( stderr, "ERROR: calloc() failed\n" );
    return EXIT_FAILURE;
  }

  cache[2] = calloc( 6, sizeof( char ) );
  strcpy( cache[2], "Duvid" );
  cache[2][1] = 'a';
  printf( "Hi, %s\n", cache[2] );

#if 0
                 char*
               +------+
  cache---> [0]| NULL |
               +------+
            [1]| NULL |
               +------+    +---------+
            [2]|   =======>|"David\0"|
               +------+    +---------+
            [3]| NULL |
               +------+
                 ....  
               +------+
           [46]| NULL |
               +------+
#endif

  free( cache[2] );
  free( cache );

  return EXIT_SUCCESS;
}
