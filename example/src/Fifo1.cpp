// C program to implement one side of FIFO 
// This side writes first, then reads 
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <errno.h>

int main() 
{ 
   int fd; 

   // FIFO file path 
   const char * myfifo = "/tmp/myfifo"; 

   // Creating the named file(FIFO) 
   // mkfifo(<pathname>, <permission>) 
   mkfifo(myfifo, 0666); 

   char arr1[80], arr2[80]; 
   while (1) 
   { 
      printf ("We will send a mesage, then receive one\n");
      // Open FIFO for write only 
      fd = open(myfifo, O_WRONLY); 

      // Take an input arr2ing from user. 
      // 80 is maximum length 
      if (fgets(arr2, 80, stdin))
      {
         // Write the input arr2ing on FIFO 
         // and close it 
         if (write(fd, arr2, strlen(arr2)+1)< 0)
         {
            printf ("Write Error '%s'\n", strerror(errno));
         }
      }
      else
      {
         printf ("unable to read\n");
      }
      close(fd); 

      // Open FIFO for Read only 
      fd = open(myfifo, O_RDONLY); 

      // Read from FIFO 
      if (read(fd, arr1, sizeof(arr1))<0)
      {
         printf ("Read Error '%s'\n", strerror(errno));
      }

      // Print the read message 
      printf("User2: %s\n", arr1); 
      close(fd); 
   } 
   return 0; 
} 
