/* printf-examples.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{
  int i;
  for ( i = 1 ; i < 5 ; i++ )
  {
    printf( "%2d %f %20.15f\n", i, sqrt(i), sqrt(i) );
    printf( "%-2d %f %-20.15f\n", i, sqrt(i), sqrt(i) );
  }

  char c = 'X';
  char name[16] = "ABCD";

  printf( "%c\n", c );
  printf( "%s\n", name );

  return EXIT_SUCCESS;
}
