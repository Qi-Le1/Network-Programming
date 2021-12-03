/* signals.c */

/* check out: "man 2 signal"
              "man 7 signal"
              "man sigaction" */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void signal_handler( int sig )
{
  printf( "Rcvd signal %d\n", sig );

  if ( sig == SIGINT )
  {
    printf( "Stop hitting CTRL-C and answer the question!\n" );
  }
  else if ( sig == SIGUSR1 )
  {
    printf( "I'm a server and I'm going to reload my config file\n" );
  }
}

int main()
{
  signal( SIGINT, SIG_IGN );   /* ignore SIGINT (CTRL-C) */

  /* redefine SIGINT to call signal_handler() */
  signal( SIGINT, signal_handler );

  /* redefine SIGUSR1 to call signal_handler() */
  signal( SIGUSR1, signal_handler );

  char name[100];   /* why is this bad btw? */
  printf( "Enter your name: " );
  scanf( "%s", name );
  printf( "Hi, %s\n", name );

  signal( SIGINT, SIG_DFL );   /* restore default behavior for SIGINT */

  /* ... */

  return EXIT_SUCCESS;
}
