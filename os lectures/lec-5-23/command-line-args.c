/* command-line-args.c */

#include <stdio.h>
#include <stdlib.h>

                 /* char * argv[] */
int main( int argc, char ** argv )
{
  printf( "argc is %d\n", argc );    /* argument count */
  printf( "argv[0] is %s\n", argv[0] );

  if ( argc != 4 )
  {
    fprintf( stderr, "ERROR: Invalid arguments\n" );
    fprintf( stderr, "USAGE: %s <filename> <x> <y>\n", argv[0] );
    return EXIT_FAILURE;
  }

  printf( "argv[1] is %s\n", *(argv+1) );
  printf( "argv[2] is %s\n", argv[2] );
  printf( "argv[3] is %s\n", argv[3] );

  return EXIT_SUCCESS;
}
