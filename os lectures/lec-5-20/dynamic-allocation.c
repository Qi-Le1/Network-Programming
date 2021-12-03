/* dynamic-allocation.c */

/* compile this code from bash shell (or terminal):

   bash$ gcc -Wall -Werror main.c

   (you'll need to fix the warning before a.out is created....!)

   once compiled, execute this code as follows:

   bash$ ./a.out
*/

/* Be sure to allocate space for the '\0' character when
    treating data as a printable character string! */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
/*  int y = 12; */
  char * x = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

#if 0
  x[2] = 'Q';
  /* we cannot do this assignment statement because
      x points to read-only memory (i.e., the code/data segment) */
#endif

  printf( "... %s ...\n", x );


  char name[5] = "David";
               /* David\0 */
  printf( "Hi, %s\n", name );

  name[0] = 'd';
  printf( "Hi, %s\n", name );

  char xyz[5] = "QRSTU";
  printf( "Hi again, %s\n", name );

/* correct the above by allocating [6] instead of [5] for name/xyz */

  char * path = malloc( 20 );   /* dynamically allocate 20 bytes */
  strcpy( path, "/cs/goldsd/u19/os" );
  printf( "path is %s\n", path );

  char * path2 = malloc( 20 );   /* dynamically allocate 20 bytes */
  strcpy( path2, "/cs/goldsd/u19/os" );
  printf( "path2 is %s\n", path2 );

  /* the next string is more than the allocated 20 bytes... */
  strcpy( path, "/cs/goldsd/u19/os/blah/blah/blah/meme" );
  printf( "path is %s\n", path );

  printf( "path2 is %s\n", path2 );

  free( path );
  free( path2 );

  return EXIT_SUCCESS;
} 
