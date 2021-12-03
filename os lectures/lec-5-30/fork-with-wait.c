/* fork-with-wait.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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
    sleep( 10 );
  }
  else /* pid > 0 */
  {
    printf( "PARENT: my child process is %d\n", pid );

    int status;
#if 0
    pid_t child_pid = wait( &status );  /* blocking call */

    wait( NULL );    /* blocking call */
          ^^^^
       use NULL here if we do not care how the
        child process terminated or what its
         exit status was...
          ...this will at least ensure that the zombie/defunct
              process is removed from the process table

    pid_t child_pid = waitpid( pid, &status, 0 );
#endif

#if 1
    pid_t child_pid;

    while ( 1 )
    {
      child_pid = waitpid( -1, &status, WNOHANG );
      if ( child_pid != 0 ) break;
      printf( "PARENT: child process has not terminated yet...\n" );
      sleep( 1 );
    }
#endif

    printf( "PARENT: child process %d terminated...\n", child_pid );

    if ( WIFSIGNALED( status ) )   /* child process was terminated   */
    {                              /*  by a signal (e.g., seg fault) */
      printf( "PARENT: ...abnormally\n" );
    }
    else
    {
      int exit_status = WEXITSTATUS( status );
      printf( "PARENT: ...successfully with exit status %d\n",
              exit_status );
    }
  }

#if 1
  printf( "?????????????????\n" );
#endif

  return EXIT_SUCCESS;
}
