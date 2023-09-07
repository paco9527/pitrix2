#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>
#include "log.h"
#include "pitrix_config.h"

#define HEAD_FLAG_LEN   8
#define HEAD_TIMESTAMP_LEN 16
#define HEAD_LEN HEAD_FLAG_LEN+HEAD_TIMESTAMP_LEN

LOG_LEVEL log_level = ERROR;

static struct tm storetime;
static export_log g_write_func = NULL;
static char logcontent[MAX_EXPORT_CONTENT_LEN];

struct tm * get_tm_time(void)
{
    struct tm *btime;
    time_t t;
    time(&t);
    btime = localtime_r(&t,&storetime);
    return btime;
}

void writelog(char* file, int line, int level, const char* logtext, ...)
{
    char head[128] = {0};
    va_list arg;
    struct tm *btime = NULL;
    va_start(arg,logtext);
    btime = get_tm_time();
    int head_len = 0;
    int total_len = 0;
    switch(level)
    {
        case DEBUG:
        {
            head_len = sprintf(head,"[%s][%d][debug]:", file, line);
            break;
        }
        case ERROR: 
        {
            head_len = sprintf(head,"[%s][%d][error]:", file, line);
            break;
        }
        case INFO:
        {
            head_len = sprintf(head,"[%s][%d][info] :", file, line);
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
    strcat(logcontent,"\r\n");
    va_end(arg);
    fprintf(stdout, logcontent);

    if(g_write_func)
    {
        total_len = strlen(logcontent);
        if(total_len > MAX_EXPORT_CONTENT_LEN)
        {
            total_len = MAX_EXPORT_CONTENT_LEN;
        }
        g_write_func(head, total_len);
    }
    va_end(arg);
}

void log_set_export_func(export_log write_func)
{
    printf("log construct complete\n");
    g_write_func = write_func;
}

void log_set_level(LOG_LEVEL level)
{
    log_level = level;
}


