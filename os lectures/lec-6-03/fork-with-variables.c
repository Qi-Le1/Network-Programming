/* fork-with-variables.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int x = 5;
  int * y = malloc( sizeof( int ) );
  *y = 555;

  /* variables x and y (on the stack) and the 4 bytes allocated on
      the heap get copied to the child process when we call fork() */

  pid_t pid;   /* process id (pid) */

  printf( "PID %d: before fork()\n", getpid() );

    /* the above line of output will be displayed on the terminal
        exactly once, but if output is redirected to a file, then
         this buffered line of output is copied to the child process */

  /* create a new (child) process */
  pid = fork();

  printf( "PID %d: after fork(), x is %d\n", getpid(), x );

  /* fork() will attempt to create a new process by
     duplicating/copying the existing running process */

  if ( pid == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }

  if ( pid == 0 )
  {
    printf( "CHILD: happy birthday!\n" );
    printf( "CHILD: x is %d\n", x );
      /* the above output line will always be "x is 5" because
          the parent's x variable is in a separate process space */

    printf( "CHILD: *y is %d\n", *y );
    printf( "CHILD: y has address %p\n", y );
  }
  else /* pid > 0 */
  {
    printf( "PARENT: my child process is %d\n", pid );
    x = 100;
    printf( "PARENT: x is %d\n", x );

    *y = 777;
    printf( "PARENT: *y is %d\n", *y );
    printf( "PARENT: y has address %p\n", y );
  }

  /* since both parent and child end up here in the code,
      we can call free() here */
  printf( "PID %d: about to call free() on y\n", getpid() );
  free( y );

    /* keep using valgrind to detect memory leaks, since
        valgrind will follow both parent and child processes */

  return EXIT_SUCCESS;
}
