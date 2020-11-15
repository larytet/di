#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
 
#define BUFFER_LENGTH 256
static char receive[BUFFER_LENGTH];
 
int main(){
   int ret, fd;
   char stringToSend[BUFFER_LENGTH];
   printf("Starting sort device...\n");
   fd = open("/dev/sort", O_RDWR);
   if (fd < 0){
      perror("Failed to open the device...");
      return errno;
   }
   unsigned char *s = [0x0a, ‘D’, ‘e’, ‘e’, ‘p’, ‘ ‘, ‘I’, ‘n’, ‘s’, ‘t’, ‘i’, ‘n’, ‘c’, ‘t’, 0x0a];
   ret = write(fd, s, strlen(s));
   if (ret < 0){
      perror("Failed to write the message to the device.");
      return errno;
   }
 
   ret = read(fd, receive, BUFFER_LENGTH);
   if (ret < 0){
      perror("Failed to read the message from the device.");
      return errno;
   }
   printf("The received message is: [%s]\n", receive);
   return 0;
}