/* pipe-ipc.c */

/* TO DO: identify and draw a diagram that shows all possible
           outputs for this code.... */

/* A "pipe" is a unidirectional communication channel */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int p[2];   /* array to hold the two pipe (file) descriptors:
                 p[0] is the "read" end of the pipe
                 p[1] is the "write" end of the pipe */

  int rc = pipe( p );  /* the input argument p will be filled in
                           with the assigned descriptors */

  if ( rc == -1 )
  {
    perror( "pipe() failed" );
    return EXIT_FAILURE;
  }

  /* fd table:

     0: stdin
     1: stdout
     2: stderr                +--------+
     3: p[0] <========READ====| buffer | think of this buffer as a
     4: p[1] =========WRITE==>| buffer |  temporary hidden transient file
                              +--------+
   */

  printf( "Created pipe; p[0] is %d and p[1] is %d\n", p[0], p[1] );

  pid_t pid = fork();  /* fork() will copy the fd table to the child */

  /* fd table:

     [PARENT]                                         [CHILD]
     0: stdin                                         0: stdin
     1: stdout                                        1: stdout
     2: stderr                +--------+              2: stderr
     3: p[0] <========READ====| buffer |====READ====> 3: p[0]
     4: p[1] =========WRITE==>| buffer |<===WRITE==== 4: p[1]
                              +--------+

     after the two calls to close() in parent and child below,
      the fd table looks like this:

     [PARENT]                                         [CHILD]
     0: stdin                                         0: stdin
     1: stdout                                        1: stdout
     2: stderr                +--------+              2: stderr
     3: p[0] <========READ====| buffer |              3:
     4:                       | buffer |<===WRITE==== 4: p[1]
                              +--------+

     after the child process is done writing and terminates:

     [PARENT]
     0: stdin
     1: stdout
     2: stderr                +--------+
     3: p[0] <========READ====| buffer |  (no write descriptors
     4:                       | buffer |    associated with this pipe)
                              +--------+
   */


  if ( pid == 0 )
  {
#if 1
    close( p[0] );  /* close the read end in the child process */
#endif

    /* write data to the pipe */                               /**vvvv**/
    int bytes_written = write( p[1], "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 18 );
    printf( "CHILD: Wrote %d bytes to the pipe\n", bytes_written );

    close( p[1] );  /* child process is done writing to the pipe */
  }
  else /* pid > 0 */
  {
#if 1
    close( p[1] );  /* close the write end in the parent process */
#endif

    /* read data from the pipe */
    char buffer[80];
    int bytes_read = read( p[0], buffer, 10 );
    buffer[bytes_read] = '\0';   /* assume the data is string data */
    printf( "PARENT: Read %d bytes: \"%s\"\n", bytes_read, buffer );

    bytes_read = read( p[0], buffer, 10 );
    buffer[bytes_read] = '\0';   /* assume the data is string data */
    printf( "PARENT: Read %d bytes: \"%s\"\n", bytes_read, buffer );

    bytes_read = read( p[0], buffer, 10 );
    buffer[bytes_read] = '\0';   /* assume the data is string data */
    printf( "PARENT: Read %d bytes: \"%s\"\n", bytes_read, buffer );

    close( p[0] );  /* parent process is done reading from the pipe */
  }

  return EXIT_SUCCESS;
}
