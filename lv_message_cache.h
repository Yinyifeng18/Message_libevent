#ifndef __LV_MESSAGE_CACHE_H__
#define __LV_MESSAGE_CACHE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <pthread.h>
#include <sys/queue.h>
#include "evt_sys.h"
#include "evt.h"
#include "event.h"

#define DEFAULT_SIZE  25
typedef struct queue
{
    int header;
    int tail;
    int size;
    int capcity;
    void **_buf;
} lv_queue_t;

typedef struct async_queue
{
    pthread_mutex_t mutex;
    lv_queue_t *_queue;
} async_queue_t;
typedef void (*do_worked_cb)(int, int,  void*);

typedef struct queue_message_info_s
{
    evt_module_e module_id; //模块ID
    uint32_t event_id;      //事件ID
    char *data;             //用户数据
    do_worked_cb worked_cb; //用户事件通知完成后的回调
}queue_message_info_t;
int Ui_Message_Queue_Init(int cache_size);
int Ui_Message_Queue_Sync_Push(evt_module_e module_id,uint32_t event_id,void*arg,do_worked_cb worked_cb);
int Ui_Message_Queue_Push(evt_module_e module_id,uint32_t event_id,void*arg,do_worked_cb worked_cb);
int Ui_Message_Queue_Pop();

#ifdef __cplusplus
}
#endif

#endif

