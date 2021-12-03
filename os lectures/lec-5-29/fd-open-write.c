/* fd-open-write.c */

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
  char * name = "testfile.txt";

                   /* attempt to open this file:
                       O_WRONLY for writing
                       O_CREAT create the file if necessary
                       O_TRUNC truncate the file if it already exists
                              (i.e., erase the contents, set size to 0)
                       (also O_APPEND) */
  int fd = open( name, O_WRONLY | O_CREAT | O_TRUNC, 0660 );
             /*                                      ^^^^
                                                      |
                                      leading 0 means this is in octal
                                                               (base 8)
                0660 ==> 110 110 000
                         rwx rwx rwx
                         ^^^ ^^^ ^^^
                          |   |   |
                          |   |  no permissions for other/world
                          |   |
                          | rw for group permissions
                          |
                     rw for file owner */

  printf( "fd is %d\n", fd );

  if ( fd == -1 )
  {
    perror( "open() failed" );
    return EXIT_FAILURE;
  }

  /* fd table:
       0 stdin
       1 stdout
       2 stderr
       3 testfile.txt (O_WRONLY)
   */

  char buffer[20];
  sprintf( buffer, "ABCD%dEFGH", fd );
  int rc = write( fd, buffer, 5 );

  printf( "wrote %d bytes to fd %d\n", rc, fd );


  /* redirect stdout (fd 1) to the output file (fd 3) */
  /* -- note that we could close(1) before the open() call... */
  rc = dup2( fd, 1 );

  /* fd table:
       0 stdin
       1 testfile.txt (O_WRONLY)
       2 stderr
       3 testfile.txt (O_WRONLY)
   */

  printf( "did it work?\n" );

  close( fd );

  /* fd table:
       0 stdin
       1 testfile.txt (O_WRONLY)
       2 stderr
   */

  printf( "does this go anywhere?\n" );

  return EXIT_SUCCESS;
}
