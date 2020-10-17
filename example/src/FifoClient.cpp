#include <thread>
#include <deque>
#include <mutex>

#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <errno.h>
#include <signal.h>

#include "TQueueConcurrent.h"
#include "Message.h"

static void SendThread (const std::string &fifoName, TQueueConcurrent<Message> &mDQ);
static void ReceiveThread ( const std::string &fifoName, TQueueConcurrent<Message> &mDQ );

bool shutdown {false};

int main() 
{ 
   std::mutex mMutex ;
   TQueueConcurrent<Message>mDQ;

   signal(SIGPIPE, SIG_IGN);  // Ignore the SIGPIPE signal

   // FIFO file path 
   std::string myFifoServer {"/tmp/myfifo-server"}; 
   std::string myFifoClient {"/tmp/myfifo-client"}; 

   // Creating the named file(FIFO) 
   // mkfifo(<pathname>, <permission>) 
   if ((mkfifo(myFifoServer.c_str(), 0664) == -1 ) && (errno != EEXIST))
   {
      printf ("Unable to create fifo '%s'\n", strerror(errno));
   }
   printf ("Fifo opened with err %s\n", strerror(errno));  

   // Creating the named file(FIFO) 
   // mkfifo(<pathname>, <permission>) 
   if ((mkfifo(myFifoClient.c_str(), 0664) == -1 ) && (errno != EEXIST))
   {
      printf ("Unable to create fifo '%s'\n", strerror(errno));
   }
   printf ("Fifo opened with err %s\n", strerror(errno));  

   
   std::thread sendThread (SendThread, std::ref(myFifoServer), std::ref(mDQ));
   std::thread receiveThread (ReceiveThread, std::ref(myFifoClient), std::ref(mDQ));

   while (!shutdown)
   {
      if (std::getc(stdin) == 'q')
      {
         printf ("Shutdown\n");
         shutdown = true;
      }
   }

   // std::thread SendThread ( SendThread);
   
   sendThread.join();
   printf ("sendThread Joined\n");
   receiveThread.join();
   printf ("receive Joined\n");

   if (remove (myFifoServer.c_str())  < 0)
   {
      printf ("Unable to remove Server fifo: '%s'\n", strerror(errno));
   }
   if (remove (myFifoClient.c_str())  < 0)
   {
      printf ("Unable to remove Server fifo: '%s'\n", strerror(errno));
   }

   return 0; 
} 

void SendThread ( const std::string &fifoName, TQueueConcurrent<Message> &mDQ)
{ 
   printf ("SendThread started\n");   
   int fd = open(fifoName.c_str(), O_WRONLY); 
   if (fd < 0)
   {
    printf ("Unable to open fifo '%s' Error '%s'\n", fifoName.c_str(), strerror(errno));
    return;
   }
   
   while (!shutdown)
   {
      Message msg = mDQ.pop_front();
      printf ("Client sending message back to server!\n");
      int numSent = write (fd, msg.GetBuffer(), msg.size());
      if (numSent < 0)
      {
         printf ("Unable to write fifo '%s'\n", strerror(errno));
      }
      else
      {
         printf ("%d bytes pushed into fifo\n", numSent);
         
      }
   }
   close(fd);
   printf ("SendThread exit\n");   
}

void ReceiveThread ( const std::string & fifoName, TQueueConcurrent<Message> &mDQ )
{ 
   char buf[100];
   printf ("receive thread started\n");     
   int fd = open(fifoName.c_str(), O_RDONLY); 
   if (fd < 0)
   {
    printf ("Unable to open fifo '%s' Error '%s'\n", fifoName.c_str(), strerror(errno));
    return;
   }

   fd_set rfds;
   
   while (!shutdown)
   {
      FD_ZERO(&rfds);
      FD_SET(0, &rfds);
      struct timeval tv{.tv_sec=1, .tv_usec=0};
      int retval = select(1, &rfds, NULL, NULL, &tv);
      if (retval == -1)
      {
         printf ("Select Error '%s'\n", strerror(errno));
      }
      else if (retval > 0)
      {
         size_t numBytes = read(fd, buf, 80);
         printf ("Received data: %lu bytes '%s'\n",numBytes, buf);
         Message msg(numBytes, buf);
         mDQ.emplace_back(std::move(msg));
      }
      else
      {
         printf ("receive thread polling\n");
      }
   }
   close(fd);
   printf ("receive thread exit\n");     
}