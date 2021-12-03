/* immutable-strings.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  char * x = "David";   /* The string "David" here is immutable */
  char * y = "David";
  char * z = "Edie";
  char * q = "David";

  /* these two arrays are mutable: */
  char a[] = "ABCD";  /* char a[] = { 'A', 'B', 'C', 'D', '\0' };  */
  char b[3] = "QRS";  /* char b[3]; b[0] = 'Q'; b[1] = 'R'; b[2] = 'S'; */

  printf( "address of x is %p\n", x );
  printf( "address of y is %p\n", y );
  printf( "address of z is %p\n", z );
  printf( "address of q is %p\n", q );

  return EXIT_SUCCESS;
}
