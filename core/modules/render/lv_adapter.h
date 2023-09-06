#ifndef _LV_ADAPTER_H
#define _LV_ADAPTER_H

#include <pthread.h>

pthread_mutex_t lv_mutex;

#define lv_exec(exec) \
do \
{ \
    pthread_mutex_lock(&lv_mutex); \
    { \
        exec; \
    } \
    pthread_mutex_unlock(&lv_mutex); \
} while (0);

#define lv_start() pthread_mutex_lock(&lv_mutex);

#define lv_end() pthread_mutex_unlock(&lv_mutex);

#endif
