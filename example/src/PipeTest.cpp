#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

int main(void)
{
   int     fd[2];
   pid_t   childpid;
   char    string[] = "Hello, world!\n";
   char    readbuffer[80];
   

   if (pipe(fd) < 0)
   {
      printf ("pipe failed %s\n", strerror(errno));
      exit(1) ;
   }
   printf("PARENT %d Opening pipe fd[0]=%d fd[1]=%d\n", getpid(), fd[0], fd[1]);
   if((childpid = fork()) == -1)
   {
          perror("fork");
          exit(1);
   }

   if(childpid == 0)
   {
          /* Child process closes up input side of pipe */
          printf ("CHILD %d closing fd[0]=%d\n", getpid(), fd[0]);
          close(fd[0]);

          printf("%d Writing pipe fd[1]=%d\n", getpid(), fd[1]);
          /* Send "string" through the output side of pipe */
          int num = write(fd[1], string, (std::strlen(string)+1));
          printf ("# bytes written %d\n", num);
          exit(0);
   }
   else
   {
      
       printf("%d closing fd[1]=%d\n", getpid(), fd[1]);
       /* Parent process closes up output side of pipe */
       close(fd[1]);
       /* Read in a string from the pipe */
       int nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
       printf("%d Received fd[0]=%d string: [%d] '%s'", getpid(), fd[0], nbytes, readbuffer);
   }

   return(0);
}