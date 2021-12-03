/* fork-interleaving.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  pid_t pid;   /* process id (pid) */

  /* create a new (child) process */
  pid = fork();

  /* fork() will attempt to create a new process by
     duplicating/copying the existing running process */

  if ( pid == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }

  /* delay BOTH parent and child processes -- could also use sleep() */
  int i;
  for ( i = 0 ; i < 100000000 ; i++ )
    ;

  if ( pid == 0 )
  {
    printf( "CHILD: happy birthday!\n" );
/*    sleep( 10 ); */
  }
  else /* pid > 0 */
  {
    printf( "PARENT: my child process is %d\n", pid );
/*    sleep( 10 ); */
  }

#if 1
  printf( "?????????????????\n" );
#endif

  return EXIT_SUCCESS;
}
