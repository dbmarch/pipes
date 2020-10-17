# pipes
Example code for linux pipes

Fifo1 & Fifo2:  Run these in separate windows.   stdin written into fifo1 goes to fifo 2.   And then it reverses.

FifoClient & FifoServer.   Run separate windows.  The server generates packets.  Client receives from one fifo and sends to server fifo.

PipeTest -> Simple fork example demonstrating a 1 way pipe

BiPipeTest -> More convoluted exampled showing bidirectional comms on forked process between parent/child.
