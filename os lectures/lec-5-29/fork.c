/* fork.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if 0
FROM THE MORNING LECTURE:
  Two possible outputs to this code:
goldsd@linux04:~/u19/os$ gcc -Wall fork.c
goldsd@linux04:~/u19/os$ a.out
PARENT: my child process is 31820
CHILD: happy birthday!
goldsd@linux04:~/u19/os$ a.out
PARENT: my child process is 31822
CHILD: happy birthday!
goldsd@linux04:~/u19/os$ a.out
PARENT: my child process is 31824
CHILD: happy birthday!
goldsd@linux04:~/u19/os$ a.out
PARENT: my child process is 31832
CHILD: happy birthday!
goldsd@linux04:~/u19/os$ a.out
CHILD: happy birthday!                     <==============
PARENT: my child process is 31841          <==============
goldsd@linux04:~/u19/os$ a.out
PARENT: my child process is 31851
CHILD: happy birthday!

FROM THE AFTERNOON LECTURE:
goldsd@linux04:~/u19/os$ a.out
PARENT: my child process is 5379
CHILD: happy birthday!
goldsd@linux04:~/u19/os$ a.out
CHILD: happy birthday!                 <==== unlikely "other" order
PARENT: my child process is 5381       <====
goldsd@linux04:~/u19/os$ a.out
PARENT: my child process is 5383
CHILD: happy birthday!

FROM THE AFTERNOON LECTURE:
goldsd@linux04:~/u19/os$ a.out
CHILD: happy birthday!              <==
PARENT: my child process is 5680    <== unusual interleaving of output
?????????????????                   <==
?????????????????                   <==
goldsd@linux04:~/u19/os$ a.out
PARENT: my child process is 5690
?????????????????
CHILD: happy birthday!
?????????????????
goldsd@linux04:~/u19/os$ a.out
PARENT: my child process is 5692
?????????????????
CHILD: happy birthday!
?????????????????
goldsd@linux04:~/u19/os$ a.out
PARENT: my child process is 5694
?????????????????
CHILD: happy birthday!
?????????????????
#endif

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

  if ( pid == 0 )
  {
    printf( "CHILD: happy birthday!\n" );
  }
  else /* pid > 0 */
  {
    printf( "PARENT: my child process is %d\n", pid );
  }

#if 0
  printf( "?????????????????\n" );
#endif

  return EXIT_SUCCESS;
}
