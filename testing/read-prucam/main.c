#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#define ROWS 1024
#define COLS 1280 
#define BYTES ROWS * COLS * 2 // 2 bytes per pixel

int main(){
  int ret, fd;

  struct timeval before, after;

  printf("Starting device test code example...\n");
  fd = open("/dev/prucam", O_RDONLY|O_LARGEFILE|O_CLOEXEC);             // Open the device with read/write access
  if (fd < 0){
    perror("Failed to open the device...");
    return errno;
  }

  char buf[BYTES];

  gettimeofday(&before , NULL);

  printf("Reading from the device...\n");
  ret = read(fd, buf, BYTES);        // Read the response from the LKM
  if (ret < 0){
    perror("Failed to read the message from the device.");
    return errno;
  }

  gettimeofday(&after , NULL);

  ret = close(fd);
  if(ret != 0) {
    perror("error closing device");
    return ret;
  }

  long uSecs = after.tv_usec - before.tv_usec;

  if(uSecs < 0) //occaisionally this number is 1000000 off??
    uSecs += 1000000;

  printf("Elapsed time: %ld uSec\n", uSecs);

  //create the file
  FILE *outf = NULL;
  outf = fopen("img.buf", "w");
  if(outf == NULL)
  {
    printf("Error in creating the file\n");
    exit(1);
  }

  //Write the buffer in file
  fwrite(buf, sizeof(buf[0]), BYTES, outf);

  fclose(outf);

  return 0;
}

