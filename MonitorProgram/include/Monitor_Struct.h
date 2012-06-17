#ifndef MONITOR_STRUCT__H__
#define MONITOR_STRUCT__H__

struct KEYTHLEYdata{
  time_t now;
  char   data[4096];
};

struct TEMPData {
  bool   readFlag;
  time_t now; 
  double temp[60];
};


#endif
