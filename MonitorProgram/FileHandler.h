#ifndef FILE_HANDLER__H__
#define FILE_HANDLER__H__
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <cstring>

void setFilename( const char*, char*, time_t );
void manageFilename( FILE*, const char*, char* , time_t );

#endif 
