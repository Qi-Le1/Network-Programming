/* static-allocation.c */

/* compile this code from bash shell (or terminal):

   bash$ gcc -Wall -Werror main.c

   once compiled, execute this code as follows:

   bash$ ./a.out
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int x = 5;   /* x is statically allocated memory
                  (4 bytes are allocated on the stack) */

  printf( "sizeof int is %ld\n", sizeof( int ) );
  printf( "x is %d\n", x );

  int * y = NULL;   /* y is statically allocated memory
                       (8 bytes are allocated on the stack);
                        this will (hopefully) point to a 
                         valid memory address.... */

  /* use this in the shell to see debugging statements:

      bash$ gcc -Wall -D DEBUG_MODE static-allocation.c
   */

#ifdef DEBUG_MODE
  printf( "sizeof int * is %ld\n", sizeof( int * ) );
  printf( "sizeof char * is %ld\n", sizeof( char * ) );
  printf( "sizeof void * is %ld\n", sizeof( void * ) );
#endif


  y = &x;
  printf( "y points to %d\n", *y );

  return EXIT_SUCCESS;   /* 0 */

  /* bash$  echo $?  */
}
