/* directories.c */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 *  A "file" is essentially a stream of bytes
 *
 *  A "file" is used to represent a text file, binary/image file,
 *   a device, etc. -- but a directory is also a file
 *
 *    .   is the current directory
 *    ..  is the parent directory
 */

int main()
{
  DIR * dir = opendir( "." );  /* open the current working directory */

  if ( dir == NULL )
  {
    perror( "opendir() failed" );
    return EXIT_FAILURE;
  }

  struct dirent * file;

  while ( ( file = readdir( dir ) ) != NULL )
  {
    printf( "found %s", file->d_name );

    struct stat buffer;
    int rc = lstat( file->d_name, &buffer );

    if ( rc == -1 )
    {
      perror( "lstat() failed" );
      return EXIT_FAILURE;
    }

    printf( " (%ld bytes)", buffer.st_size );

    if ( S_ISREG( buffer.st_mode ) )
    {
      printf( " -- regular file\n" );
    }
    else if ( S_ISDIR( buffer.st_mode ) )
    {
      printf( " -- directory\n" );
    }
    else if ( S_ISLNK( buffer.st_mode ) )
    {
      printf( " -- symlink\n" );
    }
  }

  closedir( dir );

  return EXIT_SUCCESS;
}
