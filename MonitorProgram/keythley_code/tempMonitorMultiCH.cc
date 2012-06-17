/*
  This program was written by Lee with supported by Nakaya 
  Usage::
   ./pm100_ver1 /dev/ttyS0 COMMAND_OF_PM100
   ex>./pm_100ver1 /dev/ttyS0 :POWER? ->power return
*/

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <ctime>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <fcntl.h>

void 
Usage()
{
  printf("Usage::./laser [Option] [Name]\n");
  printf("Option\n -d [Device name]\n -c [Command name]\n -sc [SubCommand name]\n");
  printf("Default:: -d:/dev/ttyS1 -c:SE -sc:[nothing]\n");
}

void
showTime()
{
  time_t n_time;
  time(&n_time);
  printf("%d\t", n_time); 
  //printf("%s" ,ctime(&n_time));
}

int main( int argc, char** argv)
{
   int     fd;
   int     cnt;
   int     ndx;
   char    buf[256];
   char    rbuf[256];
   char    res[256];
   char    preres[256];
   char    command[256];

   char    dev[256]="/dev/ttyS2";

   struct termios newtio;

   /********* Command Processing *********/
   if((fd = open( dev, O_RDWR | O_NOCTTY | O_NONBLOCK ))<0){
     printf("Open Error :%s\n",dev);
     Usage();
     return -1;
   }
   
   memset(&newtio, 0, sizeof(newtio) );

   newtio.c_cflag = B9600;
   newtio.c_cflag |= CS8;   
   newtio.c_cflag |= CLOCAL;
   newtio.c_cflag |= CREAD; 
   newtio.c_iflag = IGNPAR; 
   newtio.c_oflag = 0;
   newtio.c_lflag = 0;
   newtio.c_cc[VTIME] = 0;
   newtio.c_cc[VMIN] = 0;
   
   tcflush (fd, TCIFLUSH ); 
   tcsetattr(fd, TCSANOW, &newtio );
   fcntl(fd,F_SETFL,FNDELAY);  
   
   /********************** command IO *****************************/
   memset(buf,     0, sizeof(buf));   
   memset(res,     0, sizeof(res));   
   memset(preres,  0, sizeof(preres));
   memset(command, 0, sizeof(command));  

   std::string commandStr;
   commandStr = "*OPT?";
   //std::cout << commandStr << std::endl;

   
   memset(buf, 0 , sizeof(buf));
   memset(rbuf, 0, sizeof(rbuf));
   sprintf(buf,"ROUT:SCAN (@1%02d)\r",atoi(argv[1]));
   cnt = write(fd, buf, sizeof(buf));
   tcdrain(fd);
   usleep(1000000);
   cnt = read(fd, rbuf, sizeof(rbuf));
   //tcdrain(fd);
   std::cout << buf << std::endl;
   std::cout << rbuf<< std::endl;
  
   memset(buf, 0 , sizeof(buf));
   memset(rbuf, 0, sizeof(rbuf));
   sprintf(buf,"%s\r","STAT:QUE?");
   cnt = write(fd, buf, sizeof(buf));
   tcdrain(fd);
   usleep(1000000);
   cnt = read(fd, rbuf, sizeof(rbuf));
   //tcdrain(fd);
   std::cout << buf << std::endl;
   std::cout << rbuf<< std::endl;

   
   memset(buf, 0 , sizeof(buf));
   memset(rbuf, 0, sizeof(rbuf));
   sprintf(buf,"%s\r","ROUT:SCAN:LSEL INT");
   cnt = write(fd, buf, sizeof(buf));
   tcdrain(fd);
   usleep(1000000);
   cnt = read(fd, rbuf, sizeof(rbuf));
   //tcdrain(fd);
   std::cout << buf << std::endl;
   std::cout << rbuf<< std::endl;

   
   memset(buf, 0 , sizeof(buf));
   memset(rbuf, 0, sizeof(rbuf));
   sprintf(buf,"%s\r","INIT");
   cnt = write(fd, buf, sizeof(buf));
   tcdrain(fd);
   usleep(1000000);
   cnt = read(fd, rbuf, sizeof(rbuf));
   tcdrain(fd);
   std::cout << buf << std::endl;
   std::cout << rbuf<< std::endl;

   
   memset(buf, 0 , sizeof(buf));
   memset(rbuf, 0, sizeof(rbuf));
   sprintf(buf,"%s\r","ROUT:SCAN:LSEL NONE");
   cnt = write(fd, buf, sizeof(buf));
   tcdrain(fd);
   usleep(1000);
   cnt = read(fd, rbuf, sizeof(rbuf));
   tcdrain(fd);
   std::cout << buf << std::endl;
   std::cout << rbuf<< std::endl;

   
  
   memset(buf, 0 , sizeof(buf));
   memset(rbuf, 0, sizeof(rbuf));
   sprintf(buf,"%s\r","STAT:QUE?");
   cnt = write(fd, buf, sizeof(buf));
   tcdrain(fd);
   usleep(1000);
   cnt = read(fd, rbuf, sizeof(rbuf));
   tcdrain(fd);
   std::cout << buf << std::endl;
   std::cout << rbuf<< std::endl;

   memset(buf, 0 , sizeof(buf));
   memset(rbuf, 0, sizeof(rbuf));
   sprintf(buf,"%s\r","DATA?");
   cnt = write(fd, buf, sizeof(buf));
   tcdrain(fd);
   usleep(1000);
   cnt = read(fd, rbuf, sizeof(rbuf));
   tcdrain(fd);
   std::cout << buf << std::endl;
   std::cout << rbuf<< std::endl;

   
  
   memset(buf, 0 , sizeof(buf));
   memset(rbuf, 0, sizeof(rbuf));
   sprintf(buf,"%s\r","STAT:QUE?");
   cnt = write(fd, buf, sizeof(buf));
   tcdrain(fd);
   usleep(1000);
   cnt = read(fd, rbuf, sizeof(rbuf));
   tcdrain(fd);
   std::cout << buf << std::endl;
   std::cout << rbuf<< std::endl;

   memset(buf, 0 , sizeof(buf));
   memset(rbuf, 0, sizeof(rbuf));
   sprintf(buf,"%s\r","*OPT?");
   cnt = write(fd, buf, sizeof(buf));
   tcdrain(fd);
   usleep(1000);
   cnt = read(fd, rbuf, sizeof(rbuf));
   tcdrain(fd);
   std::cout << buf << std::endl;
   std::cout << rbuf<< std::endl;

   close(fd);
   
   return 0;
}


