#ifndef __LOG_
#define __LOG_

typedef enum
{
    DEBUG,
    ERROR,
    INFO,
}LOG_LEVEL;

LOG_LEVEL log_level;
// static int(*export_log)(char* content, size_t len);
typedef int(*export_log)(char*, size_t);

void log_set_export_func(export_log write_func);
void log_set_level(LOG_LEVEL level);
void writelog(char* file, int line, int level, const char* logtext, ...);

#define LOG_INFO(content, ...) \
        do{         \
            if(log_level <= INFO) \
                writelog(__FILE__, __LINE__, INFO, #content, ##__VA_ARGS__);   \
        }while(0);


#define LOG_DEBUG(content, ...) \
        do{         \
            if(log_level == DEBUG) \
                writelog(__FILE__, __LINE__, DEBUG, #content, ##__VA_ARGS__);   \
        }while(0);
        

#define LOG_ERROR(content, ...) \
        do{         \
            if(log_level == ERROR) \
                writelog(__FILE__, __LINE__, ERROR, #content, ##__VA_ARGS__);   \
        }while(0);

#define ASSERT_RET(c, exec) {if(c){LOG_ERROR("assert failed");exec;}}

#endif
