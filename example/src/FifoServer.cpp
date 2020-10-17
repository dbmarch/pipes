#include <thread>
#include <deque>
#include <mutex>
#include <cstring>

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
static void ReceiveThread ( const std::string &fifoName );
static void PacketGeneratorThread ( TQueueConcurrent<Message> &mDQ);

constexpr bool TRACE{false};

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
   if ((mkfifo(myFifoServer.c_str(), 0666) == -1 ) && (errno != EEXIST))
   {
      printf ("Unable to create fifo '%s'\n", strerror(errno));
   }
   printf ("Fifo opened with err %s\n", strerror(errno));  

   // Creating the named file(FIFO) 
   // mkfifo(<pathname>, <permission>) 
   if ((mkfifo(myFifoClient.c_str(), 0666) == -1 ) && (errno != EEXIST))
   {
      printf ("Unable to create fifo '%s'\n", strerror(errno));
   }
   printf ("Fifo opened with err %s\n", strerror(errno));  

   std::thread pktThread (PacketGeneratorThread, std::ref(mDQ));
   std::thread sendThread (SendThread, std::ref(myFifoClient), std::ref(mDQ));
   std::thread receiveThread (ReceiveThread, std::ref(myFifoServer));

   while (!shutdown)
   {
      if (std::getc(stdin) == 'q')
      {
         printf ("Shutdown\n");
         shutdown = true;
      }
   }

   if (remove (myFifoServer.c_str())  < 0)
   {
      printf ("Unable to remove Server fifo: '%s'\n", strerror(errno));
   }
   if (remove (myFifoClient.c_str())  < 0)
   {
      printf ("Unable to remove Client fifo: '%s'\n", strerror(errno));
   }

   pktThread.join();
   sendThread.join();
   printf("sendThread joined()\n");
   receiveThread.join();
   printf("receiveThread joined()\n");

   return 0; 
} 

//-----------------------------------------------------------------------------
// Function: PacketGeneratorThread
//-----------------------------------------------------------------------------
void PacketGeneratorThread ( TQueueConcurrent<Message> &mDQ)
{
   int pktNumber{0};
   printf ("Packet Generator started\n");
   while (!shutdown)
   {
      char buffer[Message::MAX_MSG_SIZE];
      sleep(2);
      sprintf (buffer, "[PACKET %d]", ++pktNumber);
      Message msg (strlen (buffer), buffer);
      printf ("Generating a '%s' (%lu bytes)\n", buffer, msg.size());
      mDQ.emplace_back(std::move(msg));
      if (TRACE) printf ("Packet Added\n");
   }
   printf ("Packet Generator exit\n");
}

//-----------------------------------------------------------------------------
// Function: SendThread
//-----------------------------------------------------------------------------
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
      if (TRACE) printf ("Server Waiting on MQ\n");
      Message msg = mDQ.pop_front();
      if (TRACE) printf ("Server writing to %s (%lu bytes) '%s'\n", fifoName.c_str(), msg.size(), reinterpret_cast<char*>(msg.GetBuffer()));
      int numSent = write (fd, msg.GetBuffer(), msg.size());
      if (numSent < 0)
      {
         printf ("Unable to write fifo '%s'\n", strerror(errno));
      }
      else
      {
         if (TRACE) printf ("%d bytes pushed into fifo\n", numSent);
      }
   }
   close(fd);
   printf ("SendThread exit\n");   
}


//-----------------------------------------------------------------------------
// Function: ReceiveThread
//-----------------------------------------------------------------------------
void ReceiveThread ( const std::string & fifoName )
{ 
   char buf[Message::MAX_MSG_SIZE];
   printf ("Server receive thread started.  Opening fifo '%s'\n", fifoName.c_str());
   
   int fd = open(fifoName.c_str(), O_RDWR); 
   if (fd < 0)
   {
    printf ("Unable to open fifo '%s' Error '%s'\n", fifoName.c_str(), strerror(errno));
    return;
   }
   printf ("Server Receive thread fifo opened\n");
   while (!shutdown)
   {
      fd_set rfds;
      FD_ZERO(&rfds);
      FD_SET(fd, &rfds);
      struct timeval tv{.tv_sec=1, .tv_usec=0};
      int retval = select(fd+1, &rfds, nullptr, nullptr, &tv);
      if (retval == -1)
      {
         printf ("Select Error '%s'\n", strerror(errno));
      }
      else if (retval > 0)
      {
         if (TRACE) printf ("FifoServer rxthread calling read\n");
         size_t numBytes = read(fd, buf, Message::MAX_MSG_SIZE);
         std::string rxData(buf, numBytes);
         printf ("===> Received data: %lu bytes '%s'\n",rxData.size(), rxData.c_str());
      }
      else
      {
         if (TRACE) printf ("receive thread polling\n");
      }
   }
   close(fd);
   printf ("receive thread exit\n");     
}