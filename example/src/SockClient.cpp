#include <thread>

#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <errno.h>
#include "TQueueConcurrent.h"
#include "Message.h"

static void ReceiveThread ( int fd );
//char *socket_path = "./socket";

const char *socket_path = "\0hidden";

bool running{true};

int main(int argc, char *argv[]) {
  struct sockaddr_un addr;
  char buf[100];
  int fd,rc;
  if (argc > 1) socket_path=argv[1];
  if ( (fd = socket(AF_UNIX, SOCK_SEQPACKET, 0)) == -1) {
    perror("socket error");
    exit(-1);
  }

  printf ("socket opened\n");
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  if (*socket_path == '\0') {
    *addr.sun_path = '\0';
    strncpy(addr.sun_path+1, socket_path+1, sizeof(addr.sun_path)-2);
  } else {
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
  }

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("connect error");
    exit(-1);
  }

  printf ("socket connected\n");
  std::thread receive (ReceiveThread, fd);

  while( (rc=read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
    if ( rc > 1) rc--;
    if (write(fd, buf, rc) != rc) {
      if (rc > 0) fprintf(stderr,"partial write");
      else {
        perror("write error");
        exit(-1);
      }
    }
  }

  receive.join();
  close (fd);

  return 0;
}

//-----------------------------------------------------------------------------
// Function: ReceiveThread
//-----------------------------------------------------------------------------
void ReceiveThread ( int fd )
{ 
   char buf[Message::MAX_MSG_SIZE];
   int rc;
   printf ("Server receive thread started.\n");
   while ( running ) {
     while ( (rc=read(fd,buf,sizeof(buf))) > 0) {
        buf[rc] = 0;
        printf("Client Received %u bytes: %.*s\n", rc, rc, buf);
      }
      if (rc == -1) {
        printf ("Receive thread error %s\n", strerror(errno));
        running = false;
      }
   }
   printf ("receive thread exit\n");     
}