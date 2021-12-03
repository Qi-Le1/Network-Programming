/* reverse.c */

/* TO DO:
 * (1) understand this example, in particular what's happening with memory
 * (2) modify to not use the bigstring buf -- instead, use calloc()/free()
 */

#include <stdio.h>  /* printf */
#include <stdlib.h> /* EXIT_SUCCESS, EXIT_FAILURE, malloc, free */
#include <string.h> /* strlen, strcpy, etc. */

/* MAX is the size of the largest string we can handle */
#define MAX 100

/* bigstring is a new data type */
typedef char bigstring[MAX];

/* reverses a string in place
   returns a pointer to the string
*/
char *reverse( char * s )
{
  bigstring buf;
  int i, len;
  len = strlen( s );	/* find the length */

  for ( i = 0 ; i < len ; i++ )
  {
     buf[i] = s[len-i-1];
  }
  buf[i] = '\0';	/* null terminate the string! */
  strcpy( s, buf );	/* copy the reversed string from buf back into s */

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
