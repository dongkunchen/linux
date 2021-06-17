#include "log.h"

static int  msgopt, wanopt;
static char msgdatefmt[100], wandatefmt[100], ident_name[100];
static struct timeval be_stime;
static FILE *msgfile = NULL, *wanfile = NULL;

int msgInit(char *pName)
{
    if (msgLogOpen(pName, LOG_MESSAGE_FILE, LOG_POSTFIX_MESS,LOG_WARNING_FILE, LOG_POSTFIX_WARN) == 0)
    {
        msgLogFormat(LOG_PROCNAME|LOG_PID, LOG_MESSAGE_DFMT, LOG_PROCNAME|LOG_PID, LOG_WARNING_DFMT);
    }
    else                                                                                                
    {
        printf("can not create log!\n");
        return -1;
    }
    return 0;
}
int msgLogOpen(char *ident, char *mpre, char *mdate, char *wpre, char *wdate) 
{
    time_t now_time;
    char openfilename[200], timestring[100];

    now_time = time(NULL);
    if ((!msgfile) && (*mpre))
    {
        strcpy(openfilename, mpre);
        if (*mdate)
        {
            strftime(timestring, sizeof(timestring), mdate, localtime(&now_time));
            strcat(openfilename, ".");
            strcat(openfilename, timestring);
        }
        if ((msgfile = fopen(openfilename, "a+b")) == NULL)
        { 
            printf("openfilename=%s\n", openfilename);
            return -1;
        }
        setlinebuf(msgfile);
    }
    if ((!wanfile) && (*wpre))
    {
        strcpy(openfilename, wpre);
        if (*wdate)
        {
            strftime(timestring, sizeof(timestring), wdate, localtime(&now_time));
            strcat(openfilename, ".");
            strcat(openfilename, timestring);
        }
        if ((wanfile = fopen(openfilename, "a+b")) == NULL)
        {
            return -1;
        }
        setlinebuf(wanfile);
    }
    if ((msgfile) && (wanfile))
    {
        if (*ident)
        {
            strcpy(ident_name, ident);
        } else {
            ident_name[0] = '\0';
        }
        msgopt = LOG_PROCNAME|LOG_PID;          
        wanopt = LOG_PROCNAME|LOG_PID;          
        strcpy(msgdatefmt, "%m-%d %H:%M:%S");   
        strcpy(wandatefmt, "%m-%d %H:%M:%S");   

        msglog(MSG_INFO,"File is msgfile=[%d],wanfile=[%d].",fileno(msgfile),fileno(wanfile));
        return 0;
    } else {
        return -1;
    }
}
int msglog(int mtype, char *outfmt, ...)
{
    time_t now_time;
    va_list ap;
    char logprefix[1024], tmpstring[1024];

    time(&now_time);
    if (mtype & MSG_INFO)
        strftime(logprefix, sizeof(logprefix), msgdatefmt, localtime(&now_time));
        strcat(logprefix, " ");
        if (msgopt&LOG_PROCNAME)
        {
            strcat(logprefix, ident_name);
            strcat(logprefix, " ");
        }
        if (msgopt&LOG_PID)
        {
            sprintf(tmpstring, "[%6d]", getpid());
            strcat(logprefix, tmpstring);
        }
        fprintf(msgfile, "%s: ", logprefix);
        va_start(ap, outfmt);
        vfprintf(msgfile, outfmt, ap);
        va_end(ap);
        fprintf(msgfile, "\n");
    }
    if (mtype & MSG_WARN)
    {
        strftime(logprefix, sizeof(logprefix), wandatefmt, localtime(&now_time));
        strcat(logprefix, " ");
        if (wanopt & LOG_PROCNAME)
        {
            strcat(logprefix, ident_name);
            strcat(logprefix, " ");
        }
        if (wanopt & LOG_PID)
        {
            sprintf(tmpstring, "[%6d]", getpid());
            strcat(logprefix, tmpstring);
        }
        fprintf(wanfile, "%s: ", logprefix);
        va_start(ap, outfmt);
        vfprintf(wanfile, outfmt, ap);
        va_end(ap);
        fprintf(wanfile, "\n");
        if (wanopt & LOG_PERROR)
        {
            fprintf(stderr, "%s: ", logprefix);
            va_start(ap, outfmt);
            vfprintf(stderr, outfmt, ap);
            va_end(ap);
            fprintf(stderr, "\n");
        }
    }

    return 0;
}
int msgLogFormat(int mopt, char *mdfmt, int wopt, char *wdfmt) 
{
    if (mopt >= 0)
    {
        msgopt = mopt;
    } else {
        msgopt = msgopt & mopt;
    }
    if (wopt >= 0)
    {
        wanopt = wopt;
    } else {
        wanopt = wanopt & wopt;
    }

    if (*mdfmt) strcpy(msgdatefmt, mdfmt);
    if (*wdfmt) strcpy(wandatefmt, wdfmt);

    return 0;
}
int msgLogClose(void)
{
    if (msgfile) fclose(msgfile);
    if (wanfile) fclose(wanfile);

    return 0;
}
long begusec_process(void)
{
    gettimeofday(&be_stime,NULL);

    return 0;
}
long getusec_process(void)                      
{
    struct timeval ed_stime;

    gettimeofday(&ed_stime,NULL);

    return ((ed_stime.tv_sec-be_stime.tv_sec)*1000000+ed_stime.tv_usec-be_stime.tv_usec);
}
