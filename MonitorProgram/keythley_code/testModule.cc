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
#include <signal.h>


const int nBit = 4096;
FILE* fp;
time_t getTime(){
  time_t seconds;
  seconds = time( NULL );
  return seconds;
}

void setFilename(char *filename, time_t nowtime ){
  struct tm* timeInfo = localtime( &nowtime );
  sprintf(filename, "/share/nfs/KEYTHLEY_%04d_%02d_%02d.dat",(timeInfo->tm_year+1900),(timeInfo->tm_mon+1),timeInfo->tm_mday);
  //  std::cout << filename << std::endl;
}

void manageFilename(char* currentFilename,time_t timer){
  char newFilename[128];
  setFilename(newFilename, timer);
  //  std::cout << newFilename << std::endl;

  int result = strcmp(newFilename, currentFilename);
  //  std::cout << "result: " << result << std::endl;
  if( fp == NULL ){
    fp = fopen(newFilename, "aw");
  }else if( result != 0){
    if(fp != NULL){
      fclose(fp);
    }
    fp = NULL;
    fp = fopen(newFilename, "aw");
    currentFilename = newFilename;
  }else{
    ;
  }
  
}

int SigAlrm = 0;
void sigalrm_handler(int){
  SigAlrm++;
}





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


int init(int fd, int delayWrite = 400000,int delayRead=400000){
  int cnt;
  char buf[nBit];
  char rbuf[nBit];
  const int nCommand = 13;
  std::string initStr[nCommand]={"*RST","TRAC:CLE","STAT:QUE?",
				 "FORMAT:ELEM CHAN, UNIT, TST, READ",
				 "STAT:QUE?","TRAC:POIN 60",
				 "INIT:CONT OFF","TRIG:SOUR IMM",
				 "TRIG:COUN 1","SAMP:COUN 60",
				 "ROUT:SCAN (@101:140,201:220)",
				 "STAT:QUE?","STAT:QUE?"};

  for( int commandIndex = 0; commandIndex < 13; commandIndex++){
    //std::cerr << initStr[commandIndex] << std::endl;

    memset(buf, 0, sizeof( buf )); 
    memset(rbuf, 0, sizeof( rbuf ));

    sprintf(buf,"%s\r",initStr[commandIndex].c_str());
    cnt = write(fd,buf,initStr[commandIndex].size()+1);

    //std::cerr << initStr[commandIndex] << " : " << cnt << std::endl;

    tcdrain(fd);
    usleep(delayWrite);
    cnt = read(fd, rbuf, sizeof(rbuf));
    usleep(delayRead);
    tcdrain(fd);
    //std::cerr << rbuf << " : "  << cnt << std::endl; 
    //std::cerr << "FUNCTION END" << std::endl;
  }

  return 0;
}




int RunCommand( int fd, std::string command, int delayWrite = 400000, int delayRead = 400000){
  int cnt;
  char buf[nBit];
  char rBuf[nBit];
  memset( buf, 0, sizeof( buf ));
  memset( rBuf, 0, sizeof( rBuf ));
  sprintf( buf, "%s\r",command.c_str());
  cnt = write(fd, buf, command.size()+1);
  //std::cerr << command << " : " << cnt  << std::endl;
  tcdrain(fd);
  usleep(delayWrite);
  cnt = read(fd, rBuf,sizeof(rBuf));
  usleep(delayRead);
  tcdrain(fd);
  // std::cerr << rBuf << " : " << cnt << std::endl;
  //std::cerr << "FUNCTION END" << std::endl;
  return 0;
}

int RunCommandWrite( int fd, std::string command, char* nowFilename, int delayWrite = 400000, int delayRead = 3000000){
  int cnt;
  char buf[nBit];
  char rBuf[nBit];
  memset( buf, 0, sizeof( buf ));
  memset( rBuf, 0, sizeof( rBuf ));
  sprintf( buf, "%s\r",command.c_str());
  cnt = write(fd, buf, command.size()+1);
  //std::cerr << command << " : " << cnt  << std::endl;
  tcdrain(fd);
  usleep(delayWrite);
  cnt = read(fd, rBuf,sizeof(rBuf));
  usleep(delayRead);
  //tcdrain(fd);

  struct tm* now_time;  
  time_t n_time;
  time(&n_time);
  now_time = localtime(&n_time);
  manageFilename(nowFilename, n_time);
  std::cout<< nowFilename << std::endl;
  fprintf(fp,"%d\t%s\t%d\n",n_time,rBuf,cnt);
  fflush(fp)
  std::cout << rBuf << " : " << cnt << std::endl;
  return 0;

}

int TEMPinit( int fd){
  const int NCom = 7;
  std::string commandStrList[NCom] = {"STAT:QUE?",
				      "SENS:FUNC 'TEMP', (@101:140,201:220)",
				      "STAT:QUE?","ROUT:SCAN:TSO IMM",
				      "STAT:QUE?","ROUT:SCAN:LSEL INT",
				      "STAT:QUE?"};
  for( int commandIndex = 0; commandIndex < NCom; commandIndex++){
    RunCommand(fd, commandStrList[commandIndex],400000,900000);
  }
  return 0;
}



int DAQ(int fd){
  std::string commandStrList[7] = {"STAT:QUE?",
				   "ROUT:SCAN:TSO IMM","STAT:QUE?",
				   "ROUT:SCAN:LSEL INT","TRAC:CLE",
				   "READ?","STAT:QUE?"};
  int delaywriteArr[7] = {400000,400000,400000,400000,400000,400000,2000000};
  int delayreadArr[7] = {400000,400000,400000,400000,400000,400000,400000};
  for( int commandIndex = 0; commandIndex < 5; commandIndex++){
    RunCommand(fd, commandStrList[commandIndex],delaywriteArr[commandIndex],delayreadArr[commandIndex]);
    
  }
  return 0;
}


int main( int argc, char** argv)
{
  int     fd;
  int     cnt;
  int     ndx;
  char    buf[nBit];
  char    rbuf[nBit];
  char    res[nBit];
  char    preres[nBit];
  char    command[nBit];

   char    dev[256]="/dev/ttyS3";

   struct termios newtio;

   /********* Command Processing *********/

   std::string commandStr;
   
   if((fd = open( dev, O_RDWR | O_NOCTTY | O_NONBLOCK ))<0){
     printf("Open Error :%s\n",dev);
     Usage();
     return -1;
   }
   /*
   int nloop = 0;
   if( argc < 2 ){
     nloop = 43200;
   }else if(argc == 2){
     nloop = atoi(argv[1]);
   }
   */




   char filename[128];
   time_t nowTime = time( NULL );
   setFilename(filename,  nowTime);
   fp = fopen(filename, "aw");
   std::cout << filename << std::endl;

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
   /*
   memset(buf,     0, sizeof(buf));   
   memset(res,     0, sizeof(res));   
   memset(preres,  0, sizeof(preres));
   memset(command, 0, sizeof(command));  
   */
   



   std::string com="TRAC:DATA:SEL? 0, 60";
   std::string comRead="READ?";
   std::string comQUE="STAT:QUE?";
   std::string comDATA="DATA?";
   init(fd);
   TEMPinit(fd);
   while(1){//for( int loopIndex = 0; loopIndex < nloop; loopIndex++){
     DAQ(fd);
     sleep(1);
     RunCommand(fd, comRead,3000000,4000000);
     sleep(22);
     RunCommand(fd, comDATA,3000000,4000000);
     sleep(1);
     showTime();
     RunCommandWrite(fd, com, filename, 3000000, 1000000);
   }

   close(fd);
   return 0;
}
