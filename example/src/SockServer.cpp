#include <thread>
#include <mutex>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

#include "TQueueConcurrent.h"
#include "Message.h"


// static void SendThread ( TQueueConcurrent<Message> &mDQ);
static void ReceiveThread ( int fd );
// static void PacketGeneratorThread ( TQueueConcurrent<Message> &mDQ);

constexpr bool TRACE{false};

//char *socket_path = "./socket";

const char *socket_path = "\0hidden";

int main(int argc, char *argv[]) {


  std::mutex mMutex ;
  TQueueConcurrent<Message>mDQ;

  struct sockaddr_un addr;

  int fd,cl;

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
    unlink(socket_path);
  }

  if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind error");
    exit(-1);
  }

  if (listen(fd, 5) == -1) {
    perror("listen error");
    exit(-1);
  }

  printf ("listening\n");

  while (1) {
    if ( (cl = accept(fd, NULL, NULL)) == -1) {
      perror("accept error");
      continue;
    }

    printf ("client connected\n");
    std::thread receive (ReceiveThread, cl);
    receive.join();
    close (cl);
  }

  return 0;

}

 


// //-----------------------------------------------------------------------------
// // Function: PacketGeneratorThread
// //-----------------------------------------------------------------------------
// void PacketGeneratorThread ( TQueueConcurrent<Message> &mDQ)
// {
//    int pktNumber{0};
//    printf ("Packet Generator started\n");
//    while (!shutdown)
//    {
//       char buffer[Message::MAX_MSG_SIZE];
//       sleep(2);
//       sprintf (buffer, "[PACKET %d]", ++pktNumber);
//       Message msg (strlen (buffer), buffer);
//       printf ("Generating a '%s' (%lu bytes)\n", buffer, msg.size());
//       mDQ.emplace_back(std::move(msg));
//       if (TRACE) printf ("Packet Added\n");
//    }
//    printf ("Packet Generator exit\n");
// }



// //-----------------------------------------------------------------------------
// // Function: SendThread
// //-----------------------------------------------------------------------------
// void SendThread ( const std::string &fifoName, TQueueConcurrent<Message> &mDQ)
// { 
//    printf ("SendThread started\n");   
//    int fd = open(fifoName.c_str(), O_WRONLY); 
//    if (fd < 0)
//    {
//     printf ("Unable to open fifo '%s' Error '%s'\n", fifoName.c_str(), strerror(errno));
//     return;
//    }
//    while (!shutdown)
//    {
//       if (TRACE) printf ("Server Waiting on MQ\n");
//       Message msg = mDQ.pop_front();
//       if (TRACE) printf ("Server writing to %s (%lu bytes) '%s'\n", fifoName.c_str(), msg.size(), reinterpret_cast<char*>(msg.GetBuffer()));
//       int numSent = write (fd, msg.GetBuffer(), msg.size());
//       if (numSent < 0)
//       {
//          printf ("Unable to write fifo '%s'\n", strerror(errno));
//       }
//       else
//       {
//          if (TRACE) printf ("%d bytes pushed into fifo\n", numSent);
//       }
//    }
//    close(fd);
//    printf ("SendThread exit\n");   
// }


//-----------------------------------------------------------------------------
// Function: ReceiveThread
//-----------------------------------------------------------------------------
void ReceiveThread ( int fd )
{ 
   char buf[Message::MAX_MSG_SIZE];
   int rc;
   printf ("Server receive thread started.\n");

   while ( (rc=read(fd,buf,sizeof(buf))) > 0) {
      buf[rc] = 0;
      printf("read %u bytes: %.*s\n", rc, rc, buf);
      char response[100];

      sprintf (response, "RESPONSE: '%s'\n", buf);

      if ( write (fd, response, strlen(response)) == -1) {
        printf ("Write failed %s\n", strerror(errno));
        return;
      }
    }
   printf ("receive thread exit\n");     
}