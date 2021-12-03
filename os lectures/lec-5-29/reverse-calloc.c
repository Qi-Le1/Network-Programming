/* reverse-calloc.c */

#include <stdio.h>  /* printf */
#include <stdlib.h> /* EXIT_SUCCESS, EXIT_FAILURE, malloc, free */
#include <string.h> /* strlen, strcpy, etc. */

/* reverses a string in place
   returns a pointer to the string
*/                           /*       +------------------------+ */
char * reverse( char * s )   /* s --> | D | a | v | i | d | \0 | */
{                            /*       +------------------------+ */
  char * buf;
  int i, len;
  len = strlen( s );	/* find the length */

#if 0
  buf = malloc( len + 1 );   /* for "David" we would allocate 6 bytes */
#endif

  buf = calloc( len + 1, sizeof( char ) );

  if ( buf == NULL )
  {
    fprintf( stderr, "ERROR: calloc() failed\n" );
    exit( EXIT_FAILURE );
  }

  for ( i = 0 ; i < len ; i++ )
  {
    buf[i] = s[len-i-1];
  }
  buf[i] = '\0';	/* null terminate the string! */
  strcpy( s, buf );	/* copy the reversed string from buf back into s */

  free( buf );

  return s;   /* return buf;  <== this would return a pointer to buf */
}

int main( int argc, char* argv[] )
{
  if ( argc < 2 )
  {
    fprintf( stderr, "Invalid usage - must supply a string as argv[1]\n" );
    return EXIT_FAILURE;
  }

  printf( "%s\n", reverse( argv[1] ) );

  return EXIT_SUCCESS;
}
