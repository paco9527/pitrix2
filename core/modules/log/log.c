#include "log.h"

static time_t t;
static struct tm *btime;
static struct tm storetime;
static FILE *lf;

const struct tm * getbtime(void)
{
    time(&t);
    btime = localtime_r(&t,&storetime);
    return btime;
}

time_t getatime(void)
{
    time(&t);
    return t;
}

void writelog(char* file, int line, int level, const char* logtext, ...)
{
    char head[128] = {0};
    char logcontent[1024];
    va_list arg;
    va_start(arg,logtext);
    getbtime();
    int head_len = 0;
    switch(level)
    {
        case 0:
        {
            head_len = sprintf(head,"[%s][%d][info] :", file, line);
            break;
        }
        case 1:
        {
            head_len = sprintf(head,"[%s][%d][debug]:", file, line);
            break;
        }
        case 2: 
        {
            head_len = sprintf(head,"[%s][%d][error]:", file, line);
            break;
        }
    }
    sprintf(head + head_len, " %02d-%02d %02d:%02d:%02d\t",(btime->tm_mon)+1,   \
                                    btime->tm_mday,     \
                                    btime->tm_hour,     \
                                    btime->tm_min,      \
                                    btime->tm_sec);
    strcpy(logcontent,head);
    vsnprintf(logcontent + head_len, sizeof(logcontent) - HEAD_TIMESTAMP_LEN, logtext, arg);
    strcat(logcontent,"\n");
    va_end(arg);
    //fputs(logcontent,lf);
    printf(logcontent);
}

static void init(void)
{
    lf = fopen("logfile", "a");
}

void log_init(void)
{
    printf("log construct complete\n");
    lf = NULL;
    init();
}

void log_deinit(void)
{
    if(lf != NULL)
    {
        fflush(lf);
        fclose(lf);
    }
}


