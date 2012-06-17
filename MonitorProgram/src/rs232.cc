#include "rs232.hh"

#ifdef __linux__

int SerialPort[20][13]={
  "/dev/ttyS0","/dev/ttyS1","/dev/ttyS2","/dev/ttyS3","/dev/ttyS4",
  "/dev/ttyS5","/dev/ttyS6","/dev/ttyS7","/dev/ttyS8","/dev/ttyS9",
  "/dev/ttyS10","/dev/ttyS11","/dev/ttyS12","/dev/ttyS13","/dev/ttyS14",
  "/dev/ttyUSB0","/dev/ttyUSB1","/dev/ttyUSB2","/dev/ttyUSB3","/dev/ttyUSB4",
};

class rs232 {
private:
  int m_comport_num;
  int m_baud_rate;
  char dev[32];
  
  FILE* m_fd;
  
  struct termios newtio;
  //terminal io   
public:
  
  rs323();
  ~rs232

