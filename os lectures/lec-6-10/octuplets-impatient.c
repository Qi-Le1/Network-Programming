/* octuplets-impatient.c -- based on a true story.... */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

/* function prototypes */
int child();
void parent( int children, pid_t childpids[] );


int main()
{
  int i, children = 8;
  pid_t childpids[8];

  for ( i = 0 ; i < children ; i++ )
  {
    pid_t pid = fork();

    if ( pid == -1 )
    {
      perror( "fork() failed" );
      return EXIT_FAILURE;
    }

    if ( pid == 0 )
    {
      int rc = child();
      exit( rc );        /* be sure to exit the child process */
    }

    childpids[i] = pid;
  }

  parent( children, childpids );

  return EXIT_SUCCESS;
}


/* each child process will sleep for t second,    */
/*  then return t as exit status when it wakes up */
int child()
{
#if 0
  srand( 1234 );
#endif

  srand( time( NULL ) * getpid() * getpid() );

  int t = 10 + ( rand() % 21 );  /* [10,30] */

  printf( "CHILD %d: I'm gonna nap for %d seconds\n", getpid(), t );
  sleep( t );
  printf( "CHILD %d: I'm awake!\n", getpid() );

  return t;
}


void parent( int children, pid_t childpids[] )
{
  int status;  /* exit status from each child process */

  pid_t child_pid;

  printf( "PARENT: I'm waiting for my children to wake up\n" );

  while ( children > 0 )
  {
    /* wait until a child process exits */
    child_pid = waitpid( 0, &status, WNOHANG );

    if ( child_pid == 0 )
    {
      if ( children > 4 )
      {
        printf( "PARENT: No child processes woke up yet...\n" );
        sleep( 3 );
      }
      else  /* IMPATIENT */
      {
        printf( "PARENT: I'm tired of waiting!\n" );
        int j;
        for ( j = 0 ; j < 8 ; j++ )
        {
          /* send a SIGINT signal to each child process */
          kill( childpids[j], SIGINT );

          /* we could try using 0 or -1 as the first argument to kill()
              and thereby avoid keeping track of the childpids */

          /* TO DO: capture the return value from kill() */

          /* TO DO: manage child processes better by keeping track
                     of which ones were terminated */

          /* TO DO: also get rid of the hard-coded 8 value everywhere,
                     ideally by using dynamically allocated memory */
        }

        sleep( 1 );  /* give child processes a chance to terminate */
      }
    }
    else
    {
      children--;

      printf( "PARENT: child %d terminated...", child_pid );

      if ( WIFSIGNALED( status ) )
      {
        printf( "abnormally\n" );  /* core dump or kill or kill -9 */
      }
      else if ( WIFEXITED( status ) )
      {
        int rc = WEXITSTATUS( status );
        printf( "successfully with exit status %d\n", rc );
      }

      printf( "PARENT: %d children to go....\n", children );
    }
  }

  printf( "PARENT: All of my children are awake!\n" );
}
