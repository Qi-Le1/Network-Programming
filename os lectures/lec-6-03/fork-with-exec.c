/* fork-with-exec.c */

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
    sleep( 2 );

#if 0
    execl( "./a.out", "xyz", "-whatev", NULL );
#endif

    /* the child process will execute the "ls -l dyn.c" command */
    /*                argv[0], argv[1], argv[2], ... */
    execl( "/bin/ls", "ls",    "-l",    "dyn.c", NULL );

    fprintf( stderr, "we should never get here!\n" );
    perror( "execl() failed" );
    return EXIT_FAILURE;        /* exit the failed child process */
  }
  else /* pid > 0 */
  {
    printf( "PARENT: my child process is %d\n", pid );

    printf( "PARENT: blocking on wait()...\n" );

    int status;
    pid_t child_pid = wait( &status );  /* blocking call */

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
