/* fd-open-stdin.c */

/**
 *
 *  file descriptor (fd)
 *
 *  -- each process has a file descriptor table
 *      associated with it that keeps track of its
 *       open files (i.e., byte streams)
 *
 *  fd        C++   Java        C
 *  0 stdin   cin   System.in   scanf(), read(), getchar(), ...
 *  1 stdout  cout  System.out  printf(), write(), putchar(), ...
 *  2 stderr  cerr  System.err  fprintf( stderr, "ERROR: ....\n" );
 *                              perror( "open() failed" );
 *
 *     stdout and stderr (by default) are both displayed on the terminal
 *
 *     stdout is line-buffered when output to the terminal
 *
 *     stdout is block-buffered when output is redirected to a file
 *
 *     stderr (fd 2) is unbuffered
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
  close( 0 );  /* close stdin */

  /* fd table:
       0 
       1 stdout
       2 stderr
   */

  char * name = "testfile.txt";

  int fd = open( name, O_RDONLY );

  printf( "fd is %d\n", fd );

  if ( fd == -1 )
  {
    perror( "open() failed" );
    return EXIT_FAILURE;
  }

  /* fd table:
       0 testfile.txt
       1 stdout
       2 stderr
   */

  char buffer[20];
  int rc = read( fd, buffer, 19 );
  buffer[rc] = '\0';   /* assume the data is character data */
  printf( "read [%s]\n", buffer );

  rc = read( fd, buffer, 19 );
  buffer[rc] = '\0';   /* assume the data is character data */
  printf( "read [%s]\n", buffer );

  rc = read( fd, buffer, 19 );
  buffer[rc] = '\0';   /* assume the data is character data */
  printf( "read [%s]\n", buffer );

  close( fd );

  /* fd table:
       0 stdin
       1 stdout
       2 stderr
       3 testfile1.txt (O_RDONLY)
       4 testfile2.txt (O_RDONLY)
       5 testfile3.txt (O_RDONLY)


     close( 4 );  <=== close testfile2.txt

       0 stdin
       1 stdout
       2 stderr
       3 testfile1.txt (O_RDONLY)
       4 
       5 testfile3.txt (O_RDONLY)

      open( "testfile4.txt", O_WRONLY );

       0 stdin
       1 stdout
       2 stderr
       3 testfile1.txt (O_RDONLY)
       4 testfile4.txt (O_WRONLY)
       5 testfile3.txt (O_RDONLY)
   */

  return EXIT_SUCCESS;
}
