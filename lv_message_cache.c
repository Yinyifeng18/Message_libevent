/*****************************************************


*****************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "lv_create_pipe.h"
#include "lv_message_cache.h"


#define LV_CACHE_RET_SUCCESS                0x0000
#define LV_CACHE_RET_INIT_ERROR 			0X0001
#define LV_CACHE_RET_POP_ERROR 		    	0X0002
#define LV_CACHE_RET_MALLOC_ERROR 		    0X0003
#define LV_CACHE_RET_QUEUE_FULL_ERROR 	    0X0003


static async_queue_t * sg_ui_msg_queue;
lv_queue_t *queue_create(int size)
{
    lv_queue_t *q = malloc(sizeof (lv_queue_t));
    if (q != NULL)
    {
        if (size > 0)
        {
            q->_buf = malloc(size * sizeof (void *));
            q->capcity = size;
        }
        else
        {
            q->_buf = malloc(DEFAULT_SIZE * sizeof (void *));
            q->capcity = DEFAULT_SIZE;
        }
        q->header = q->tail = q->size = 0;
    }
    return q;
}

int queue_is_full(lv_queue_t *q)
{
    return q->size == q->capcity;
}

int queue_is_empty(lv_queue_t *q)
{
    return q->size == 0;
}

void queue_push_tail(lv_queue_t *q, void *data)
{
    if (!queue_is_full(q))
    {
        q->_buf[q->tail] = data;
        q->tail = (q->tail + 1) % q->capcity;
        q->size++;
    }
}

void *queue_pop_head(lv_queue_t *q)
{
    void *data = NULL;
    if (!queue_is_empty(q))
    {
        data = q->_buf[(q->header)];
        q->header = (q->header + 1) % q->capcity;
        q->size--;
    }
    return data;
}

int *queue_free(lv_queue_t *q)
{
    free(q->_buf);
    free(q);
}

async_queue_t *async_queue_create(int cache_size)
{
	async_queue_t *queue = NULL;
    queue = malloc(sizeof (async_queue_t));
	if(queue == NULL)
	{
		return NULL;
	}
    queue->_queue = queue_create(cache_size);
    pthread_mutex_init(&(queue->mutex), NULL);
	return queue;
}

int async_queue_push_tail(async_queue_t *q, void *data)
{
	if(q == NULL)
	{
		return -1;
	}
    if (!queue_is_full(q->_queue))
    {
        pthread_mutex_lock(&(q->mutex));
        queue_push_tail(q->_queue, data);
        pthread_mutex_unlock(&(q->mutex));
		return 0;
    }
	return -1;
}

void *async_queue_pop_head(async_queue_t *q, struct timeval *tv)
{
	if(q == NULL)
	{
		return NULL;
	}
    void *retval = NULL;
    pthread_mutex_lock(&(q->mutex));
    if (queue_is_empty(q->_queue))
    {	
  
    }
    retval = queue_pop_head(q->_queue);
    pthread_mutex_unlock(&(q->mutex));
    return retval;
}


void *queue_get_head(lv_queue_t *q)
{
    void *data = NULL;
    if (!queue_is_empty(q))
    {
        data = q->_buf[(q->header)];
    }
    return data;
}

void async_queue_free(async_queue_t *q)
{
	if(q == NULL)
	{
		return;
	}

    queue_free(q->_queue);
    pthread_mutex_destroy(&(q->mutex));
    free(q);
	q = NULL;

    return;
}


/*******************************************************************
** 函数名:     Ui_Message_Queue_Free
** 函数描述:    释放接口
** 参数:       [×] msg : 
** 返回:       void
** 注意:
********************************************************************/ 
int Ui_Message_Queue_Free(queue_message_info_t *msg)
{
	if(msg != NULL)
	{
		free(msg);
		msg = NULL;
	}

}
/*******************************************************************
** 函数名:     Ui_Message_Queue_Push
** 函数描述:    消息推送接口 非阻塞
** 参数:       [evt_module_e] module_id,[uint32_t] event_id,[void*]arg
** 返回:       void
** 注意:
********************************************************************/   
int Ui_Message_Queue_Push(evt_module_e module_id, uint32_t event_id, void *arg, do_worked_cb worked_cb)
{
	if(sg_ui_msg_queue == NULL)
	{
		printf( "sg_ui_msg_queue %p,error\n",sg_ui_msg_queue);
		return LV_CACHE_RET_INIT_ERROR;
	}
	queue_message_info_t *session = NULL;
	session = (queue_message_info_t *)malloc(sizeof(queue_message_info_t));
	if(session == NULL)
	{
		return LV_CACHE_RET_MALLOC_ERROR;
	}
	memset(session,0,sizeof(queue_message_info_t));
	session->module_id = module_id;
	session->event_id  = event_id;
	session->data      = arg;
	session->worked_cb      = worked_cb;
	// printf( "Ui_Message_Queue_Push %d,module_id=%d,event_id=%d\n",sg_ui_msg_queue->_queue->size,module_id,event_id);
	int ret = async_queue_push_tail(sg_ui_msg_queue, (void *)session);
	if(ret != 0)
	{
		// printf( "Ui_Message_Queue_Push failure was full,drop fisrt file upload request\n");
		usleep(300000);
		ret = async_queue_push_tail(sg_ui_msg_queue, (void *)session);
		if(ret != 0)
		{
			printf( "Ui_Message_Queue_Push failure,Message_Queue is full.\n");
			free(session);
			session = NULL;
			return LV_CACHE_RET_QUEUE_FULL_ERROR;
		}
		else
		{
			lv_write_data_to_pipe(1);
			// printf( "1Ui_Message_Queue_Push %d\n",sg_ui_msg_queue->_queue->size);
		}
	}
	else
	{
		/* code */
		lv_write_data_to_pipe(1);
		// printf( "2Ui_Message_Queue_Push %d\n",sg_ui_msg_queue->_queue->size);
	}
	return LV_CACHE_RET_SUCCESS;
}
/*******************************************************************
** 函数名:     Ui_Message_Queue_Sync_Push
** 函数描述:    消息推送接口 阻塞
** 参数:       [evt_module_e] module_id,[uint32_t] event_id,[void*]arg,do_worked_cb worked_cb
** 返回:       
** 注意:
********************************************************************/  
int Ui_Message_Queue_Sync_Push(evt_module_e module_id,uint32_t event_id,void*arg,do_worked_cb worked_cb)
{
	if(sg_ui_msg_queue == NULL)
	{
		printf( "sg_ui_msg_queue %p,error\n",sg_ui_msg_queue);
		return LV_CACHE_RET_INIT_ERROR;
	}
	queue_message_info_t *session = NULL;
	session = (queue_message_info_t *)malloc(sizeof(queue_message_info_t));
	if(session == NULL)
	{
		return LV_CACHE_RET_MALLOC_ERROR;
	}
	memset(session,0,sizeof(queue_message_info_t));
	session->module_id = module_id;
	session->event_id  = event_id;
	session->data      = arg;
	session->worked_cb      = worked_cb;
	// printf( "Ui_Message_Queue_Push %d,module_id=%d,event_id=%d\n",sg_ui_msg_queue->_queue->size,module_id,event_id);
	while(1)
	{
		int ret = async_queue_push_tail(sg_ui_msg_queue, (void *)session);
		if(ret != 0)
		{
			printf( "Ui_Message_Queue_Push failure was full,drop fisrt file upload request\n");
			usleep(300000);
		}
		else
		{
			/* code */
			lv_write_data_to_pipe(1);
			// printf( "2Ui_Message_Queue_Push %d\n",sg_ui_msg_queue->_queue->size);
			break;
		}
	}
	return LV_CACHE_RET_SUCCESS;

}
/*******************************************************************
** 函数名:     Ui_Message_Queue_Pop
** 函数描述:    消息pop接口
** 参数:       
** 返回:       void
** 注意:
********************************************************************/ 
int Ui_Message_Queue_Pop()
{
	// printf( "pop start!\n");
	int pipe_read_fd = lv_get_read_pipe_fd();
	char pipe_buf[LV_PIPE_DATA_MAX_LEN] = {0};
	int read_len = read(pipe_read_fd, pipe_buf, LV_PIPE_DATA_MAX_LEN);//读取管道中数据

    if (read_len < 0)
    {
        printf( "read_len:%d", read_len);
    }

	queue_message_info_t *msg = NULL;
	msg = (queue_message_info_t *)async_queue_pop_head(sg_ui_msg_queue, NULL);
	// printf( "Pop队列剩余个数:%d,fd=%d,可读,len=%d,buf:%d,\n",sg_ui_msg_queue->_queue->size,pipe_read_fd,read_len,pipe_buf[0]);
    if(msg == NULL)
	{
		printf( "msg =%p error!\n",msg);
		return LV_CACHE_RET_POP_ERROR;
	}
	// printf( "pop end 模块ID=%d,事件ID=%d,data=%p\n",msg->module_id,msg->event_id,msg->data);

	int ret = event_trigger(msg->module_id,msg->event_id,(void *)msg->data);
    if (ret == -1)
    {
        printf( "event_trigger fail %d\n", ret);
    }

	if(msg->worked_cb != NULL)
	{
		msg->worked_cb(msg->module_id,msg->event_id,(void *)msg->data);
	}
	Ui_Message_Queue_Free(msg);
       
	return LV_CACHE_RET_SUCCESS;
}

/*******************************************************************
** 函数名:     Ui_Message_Queue_Init
** 函数描述:    int cache_size  消息缓存个数
** 参数:       
** 返回:       void
** 注意:
********************************************************************/ 
int Ui_Message_Queue_Init(int cache_size)
{
	printf( "Ui_Message_Queue_Init\n");
	sg_ui_msg_queue = async_queue_create(cache_size);
	if(sg_ui_msg_queue == NULL)
	{
		printf( "Ui_Message_Queue_Init queue_create failure \n");
		return LV_CACHE_RET_INIT_ERROR;
	}
	return LV_CACHE_RET_SUCCESS;
}

