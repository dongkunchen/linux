#ifndef LOG_H
#define LOG_H

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define LOG_PROCNAME      0x00000001              
#define LOG_PID           0x00000010              
#define LOG_PERROR        0x00000100              
#define NLO_PROCNAME      0x11111110              
#define NLO_PID           0x11111101              
#define NLO_PERROR        0x11111011             

#define MSG_INFO          0x00000001              
#define MSG_WARN          0x00000010              
#define MSG_BOTH          MSG_INFO|MSG_WARN       

#define LOG_MESSAGE_FILE  "/home/itheima/log/tcpsvr"           
#define LOG_MESSAGE_DFMT  "%m-%d %H:%M:%S"        
#define LOG_POSTFIX_MESS  "%y%m"                  
#define LOG_WARNING_FILE  "/home/itheima/log/log.sys_warn"   
#define LOG_WARNING_DFMT  "%m-%d %H:%M:%S"       
#define LOG_POSTFIX_WARN  ""    

int msglog(int mtype, char *outfmt, ...);
int msgLogFormat(int mopt, char *mdfmt, int wopt, char *wdfmt); 
int msgLogOpen(char *ident, char *mpre, char *mdate, char *wpre, char *wdate);
int msgLogClose(void);

long begusec_process(void);                      
long getusec_process(void);                      

int msgInit(char *pName);
#endif
