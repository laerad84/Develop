#ifndef KEYTHLEYFUNCTION__H__
#define KEYTHLEYFUNCTION__H__
#include <cstring>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cmath>
#include <ctime>
#include <csignal>

#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include <string>

#include <unidstd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>


////////////////////////////////////////////////////////////////////////////////////////////////////
// Basic Functions 
////////////////////////////////////////////////////////////////////////////////////////////////////
class SerialIO {
 private:
  int  m_fd;
  int  m_fileflag;
  int  m_cnt;
  int  m_ndx;
  int  m_baud;
  int  m_parity;
  int  m_bit;

  char m_buf[4096];
  char m_rbuf[4096];
  
  struct termios m_tio;
  std::string m_port;  
    
 public :
  SerialIO();
  virtual ~SerialIO();
  virtual int InitTermios();
  virtual int OpenPort();
  virtual int ClosePort();
  virtual int RunCommand();
  virtyal int RunCommandWrite();
};

SerialIO::SerialIO(char* Port, int baud){
  m_port = Port;
  m_baud = baud;
  InitPort();
  InitTermios();
}

SerialIO::~SerialIO(){
  ClosePort();
}

bool SerialIO::OpenPort(){
  m_fileflag = O_RDWR | O_NOCTTY | ONONBLOCK ;
  if( ( m_fd = open( m_port.c_str(), m_fileflag ) ) < 0 ){
    perror( "InitPort" );
    return false;
  }else{
    return true; 
  }
}

bool SerialIO::InitTermios(){
  m_baud = baud;
  m_bit  = bit;

  m_tio.c_cflag = 0;
  m_tio.c_iflag = 0;
  m_tio.c_oflag = 0;
  m_tio.c_lflag = 0; 
  m_tio.c_cc[VTIME] = 0;
  m_tio.c_cc[VMIN]  = 0;
  // init baud rate;
  switch ( m_baud ){
  case 115200:
    m_tio.c_cflag |= B115200;
    break;
  case 57600:
    m_tio.c_cflag |= B57600;
    break;
  case 38400:
    m_tio.c_cflag |= B38400;
    break;
  case 19200:
    m_tio.c_cflag |= B19200; 
    break;    
  case 9600:
    m_tio.c_cflag |= B9600;
    break;
  case 4800:
    m_tio.c_cflag |= B4800;
    break;
  case 2400:
    m_tio.c_cflag |= B2400;
    break;
  default:
    perror( __FUNCTION__ );
    return false;
  }
  
  // init bit
  switch ( m_bit ){
  case 8: 
    m_tio.c_cflag |=  CS8;
    break;
  case 7: 
    m_tio.c_cflag |=  CS7;
    break;
  case 6: 
    m_tio.c_cflag |=  CS6;
    break;
  case 5:
    m_tio.c_cflag |=  CS5;
    break;
  default:
    perror( __FUNCTION__ );
    return false;
  }

  // init 
  m_tio.c_cflag |= CLOCAL;
  m_tio.c_cflag |= CREAD;
  m_tio.c_iflag |= IGNPAR;
  m_tio.c_oflag |= 0;
  m_tio.c_cc[VTIME] = 0;
  m_tio.c_cc[VMIN]  = 0; 
  
  tcflush ( m_fd, TCIFLUSH );
  tcsetattr ( m_fd, TCSANOW, &m_tio );
  fcntl( m_fd, F_SETFL, FNDELAY );
  return true;
}

bool  SerialIO::CloseSerialPort(){
  close( m_fd );
  return true;
}

int SerialIO::RunCommand(char* command, int WriteDelay, int ReadDelay){
  memset( m_buf,  0, sizeof( m_buf)  );
  memset( m_rbuf, 0, sizeof( m_rbuf) );
  sprintf( m_buf, "%s\r", command.c_str());
  cnt = write( m_fd, m_buf, command.size()+1);
  tcdrain(m_fd);
  usleep ( delayWrite );
  int cnt = read( m_fd, m_rbuf, sizeof( m_rbuf ));
  usleep(delayRead);
  tcdrain( m_fd );
  return cnt;
}

int SerialIO::GetResult(char* rtnData){
  for( int i =0; i< sizeof(m_rbuf)/sizeof(m_rbuf[0]); i++){
    rtnData[i] = m_rbuf[i];
  }
  return 0;
}

int init(int fd, int delayWrite = 400000,int delayRead=400000);
int RunCommand( int fd, std::string command, int delayWrite = 400000, int delayRead = 400000);
int RunCommandWrite( int fd, std::string command, char* nowFilename, struct KEYTHLEYdata *KEYdata,
		     int delayWrite = 400000, int delayRead = 3000000);
int Tempinit(int fd);
int DAQ( int fd);

#endif
