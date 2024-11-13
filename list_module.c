 
/*------------------------------- Includes ----------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <event2/event.h>
#include "list_module.h"


/*------------------- Global Definitions and Declarations -------------------*/

 
/*---------------------- Constant / Macro Definitions -----------------------*/
#define DEBUG_SW 0
#if DEBUG_SW
#include "debug.h"
#define DEBUG(...)  debug(__VA_ARGS__)//LOG_INFO(LOG_PRINT,__VA_ARGS__)
#else
#define DEBUG(...)
#endif
/*----------------------- Type Declarations ---------------------------------*/
typedef struct 
{
    list_head_node_t *task_head_node;
    list_node_t *task_node;
}list_timeout_t;

/*----------------------- Variable Declarations -----------------------------*/
 
 
/*----------------------- Function Prototype --------------------------------*/
 static void list_module_timeout(int fd, short event, void* arg);
 
/*----------------------- Function Implement --------------------------------*/

/*******************************************************************
 ** 函数名:       node_add
 ** 函数描述:    添加节点
 ** 参数:      
 ** 返回:
 ** 注意:     
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
static node_msg_t *node_add(list_head_node_t *task_head_node,uint64_t timer,list_node_t *task_node_pre,list_node_t *task_node_next)
{
    list_node_t *task_node;
    list_timeout_t *list_timeout;
    
    if(task_head_node == NULL) return NULL;
    
#if DEBUG_SW
    debug("--------------node_add------------------\n");
#endif /* DEBUG_SW */
    
    task_node = (list_node_t *)malloc(sizeof(list_node_t));
    task_node->times = 0;
    task_node->list_timeout = NULL;
    
    task_node->node_msg = (node_msg_t *)malloc(sizeof(node_msg_t));
    task_node->node_msg->interval_timer = timer;
    task_node->node_msg->retry_times = 0;
    task_node->node_msg->payload = NULL;
    task_node->node_msg->payload_len = 0;
    task_node->node_msg->task_signal = NULL;
    if(timer == 0 || task_head_node->base == NULL)
    {
        task_node->node_msg->evt = NULL;
    }
    else
    {
        struct timeval tm;
        tm.tv_sec = timer/1000;
        tm.tv_usec = (timer%1000)*1000;
        list_timeout = (list_timeout_t *)malloc(sizeof(list_timeout_t));
        list_timeout->task_head_node = task_head_node;
        list_timeout->task_node = task_node;
        task_node->node_msg->evt = event_new(task_head_node->base, -1, EV_PERSIST, list_module_timeout, (void*)list_timeout);
        evtimer_add(task_node->node_msg->evt, &tm); 
        task_node->list_timeout = (void*)list_timeout;
        DEBUG("list_timeout:%p\n",list_timeout);
    }
    task_node->pre = task_node_pre;
    if(task_node_pre == NULL)
    {
        task_head_node->task_head = task_node;
        
    }
    else
    {
        task_node_pre->next = task_node;
        
    }

    task_node->next = task_node_next;
    if(task_node_next != NULL)
    {
        task_node_next->pre = task_node;
    }
    task_head_node->node_amount++;
    return task_node->node_msg;
}

/*******************************************************************
 ** 函数名:       list_node_add
 ** 函数描述:    从头部添加节点
 ** 参数:      
 ** 返回:
 ** 注意:     
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
node_msg_t *list_node_add(list_head_node_t *task_head_node,uint64_t timer)
{
    if(task_head_node == NULL) return NULL;

    return node_add(task_head_node,timer,NULL,task_head_node->task_head);
    
}

/*******************************************************************
 ** 函数名:       list_node_add_tail
 ** 函数描述:    从尾部添加节点
 ** 参数:      
 ** 返回:
 ** 注意:     
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
node_msg_t *list_node_add_tail(list_head_node_t *task_head_node,uint64_t timer)
{
    list_node_t *task_node;
    
    if(task_head_node == NULL) return NULL;

    if(task_head_node->task_head == NULL)
    {
        return node_add(task_head_node,timer,NULL,NULL);
    }
    else
    {
        task_node = task_head_node->task_head;

        while(task_node->next != NULL)
        {
            task_node = task_node->next;
        }
        return node_add(task_head_node,timer,task_node,NULL);
    }
}

/*******************************************************************
 ** 函数名:       list_node_mid_add
 ** 函数描述:    将节点添加到链表的某个位置
 ** 参数:        task_head_node :链表头
                 task_node      :链表中已存在的节点
                 type           :0:在节点前添加，1:在节点后添加
                 timer          :节点超时时间 0:不定时
 ** 返回:
 ** 注意:     
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
node_msg_t *list_node_mid_add(list_head_node_t *task_head_node,list_node_t *task_node,uint8_t type,uint64_t timer)
{
    if(task_head_node == NULL) return NULL;

    if(task_node == NULL)
    {
        if(type == 0)//加到链表头
        {
            return list_node_add(task_head_node,timer);
        }
        else//加到链表尾
        {
            return list_node_add_tail(task_head_node,timer);
        }
    }
    else if(task_node->pre == NULL && type == 0)//加到链表头
    {
        return list_node_add(task_head_node,timer);
    }
    else if(task_node->next == NULL && type == 1)//加到链表尾
    {
        return list_node_add_tail(task_head_node,timer);
    }
    else
    {
        if(type == 0)
        {
            return node_add(task_head_node,timer,task_node->pre,task_node);
        }
        else if(type == 1)
        {
            return node_add(task_head_node,timer,task_node,task_node->next);
        }
        else
        {
            return NULL;
        }
    }
}

/*******************************************************************
 ** 函数名:       list_node_retime
 ** 函数描述:    节点计时重置
 ** 参数:      
 ** 返回:       0:成功，-1:失败
 ** 注意:      
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
size_t list_node_retime(node_msg_t *node_msg)
{
    struct timeval tm;
    if(node_msg == NULL)return -1;
    if(node_msg->evt == NULL)return -1;

    tm.tv_sec = node_msg->interval_timer/1000;
    tm.tv_usec = (node_msg->interval_timer%1000)*1000;
    return evtimer_add(node_msg->evt, &tm); //重新计数
}

/*******************************************************************
 ** 函数名:       list_node_reset_time
 ** 函数描述:    节点超时时间重新设置
 ** 参数:      
 ** 返回:       0:成功，-1:失败
 ** 注意:      
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
size_t list_node_reset_time(list_node_t *list_node,uint64_t timer)
{
    struct timeval tm;
    
    if(list_node == NULL)return -1;
    if(list_node->node_msg == NULL)return -1;
    if(list_node->node_msg->evt == NULL)return -1;

    tm.tv_sec = timer/1000;
    tm.tv_usec = (timer%1000)*1000;
    list_node->times = 0;
    list_node->node_msg->interval_timer = timer;
    return evtimer_add(list_node->node_msg->evt, &tm); 
}

/*******************************************************************
 ** 函数名:       list_node_query
 ** 函数描述:    查询节点
 ** 参数:      
 ** 返回:
 ** 注意:      允许查询中断后，下次继续查询 
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
list_node_t *list_node_query(list_head_node_t *task_head_node)
{
    if(task_head_node == NULL)  return NULL;
    if(task_head_node->task_head == NULL) return NULL;
    if(task_head_node->query_cur_task == NULL)
    {
        task_head_node->query_cur_task = task_head_node->task_head;
    }
    else
    {
        task_head_node->query_cur_task = task_head_node->query_cur_task->next;
    }
    return task_head_node->query_cur_task; 
}

/*******************************************************************
 ** 函数名:       list_msg_query
 ** 函数描述:    查询节点
 ** 参数:      
 ** 返回:
 ** 注意:      允许查询中断后，下次继续查询 
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
node_msg_t *list_msg_query(list_head_node_t *task_head_node)
{
    list_node_t *list_node;
    list_node = list_node_query(task_head_node);
    if(list_node == NULL)return NULL;
    return list_node->node_msg;
}

/*******************************************************************
 ** 函数名:       list_query_status_clean
 ** 函数描述:    清除查询状态
 ** 参数:      
 ** 返回:
 ** 注意:       该函数配合list_msg_query()或list_node_query()使用
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
void list_query_status_clean(list_head_node_t *task_head_node)
{
    task_head_node->query_cur_task = NULL;
}

/*******************************************************************
 ** 函数名:       list_msg_to_node_query
 ** 函数描述:    用户信息转换成节点信息
 ** 参数:      
 ** 返回:      
 ** 注意:      允许查询中断后，下次继续查询 
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
list_node_t *list_msg_to_node_query(list_head_node_t *task_head_node,node_msg_t *node_msg)
{
    //node_msg_t *msg;
    list_node_t *list_node;
    
    if(task_head_node == NULL)  return NULL;
    if(node_msg == NULL)  return NULL;
    if(IS_LIST_EMPTY(task_head_node))return NULL;
    list_query_status_clean(task_head_node);
    while(1)
    {
        if((list_node = list_node_query(task_head_node)) == NULL)break;
        if(list_node->node_msg == node_msg)
        {
            list_query_status_clean(task_head_node);
            return list_node;
        }
    }
    return NULL;
}

/*******************************************************************
 ** 函数名:       list_cur_node_del
 ** 函数描述:    删除当前节点
 ** 参数:      
 ** 返回:
 ** 注意:       该函数配合查询函数使用
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
void list_cur_node_del(list_head_node_t *task_head_node)
{
    list_node_t * task_node;
    
    if(task_head_node == NULL)  return;
    if(task_head_node->query_cur_task == NULL) return;

    DEBUG("%s\n",__func__);
    task_node = task_head_node->query_cur_task;
    task_head_node->query_cur_task = task_node->pre;
    if(task_head_node->query_cur_task == NULL)
    {
        task_head_node->task_head = task_node->next;
        if(task_head_node->task_head != NULL)
        {
            task_head_node->task_head->pre = NULL;
        }
    }
    else
    {
        task_head_node->query_cur_task->next = task_node->next;
        if(task_node->next != NULL)
        {
            task_node->next->pre = task_head_node->query_cur_task;
        }
    }
    
    task_head_node->node_amount--;
    /*free current node*/
    if(task_node->node_msg->evt != NULL)event_free(task_node->node_msg->evt);
    free(task_node->node_msg);
    if(task_node->list_timeout != NULL)free((list_timeout_t *)task_node->list_timeout);
    free(task_node);
}

/*******************************************************************
 ** 函数名:       list_node_del
 ** 函数描述:    链表节点删除
 ** 参数:      
 ** 返回:
 ** 注意:       
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
void list_node_del(list_head_node_t *task_head_node,list_node_t *list_node)
{
    if(task_head_node == NULL)  return;
    if(list_node == NULL)  return;

    DEBUG("%s\n",__func__);
    if(list_node == task_head_node->query_cur_task)
    {
        task_head_node->query_cur_task = task_head_node->query_cur_task->pre;
    }
    if(list_node->pre == NULL)
    {
        task_head_node->task_head = list_node->next;
        if(task_head_node->task_head != NULL)
        {
            task_head_node->task_head->pre = NULL;
        }
    }
    else
    {
        list_node->pre->next = list_node->next;
        if(list_node->next != NULL)
        {
            list_node->next->pre = list_node->pre;
        }
    }
    task_head_node->node_amount--;
    /*free current node*/
    if(list_node->node_msg->evt != NULL)event_free(list_node->node_msg->evt);
    free(list_node->node_msg);
    if(list_node->list_timeout != NULL)free((list_timeout_t *)list_node->list_timeout);
    free(list_node);
}

void list_node_msg_del(list_head_node_t *task_head_node,node_msg_t *node_msg)
{
    list_node_t *list_node;
    
    list_node = list_msg_to_node_query(task_head_node,node_msg);
    if(list_node == NULL)return;
    list_node_del(task_head_node,list_node);
}

/*******************************************************************
 ** 函数名:       list_note_all_del
 ** 函数描述:    删除该链表的所有节点
 ** 参数:      list_head_node:链表
               mode: 0:直接删除节点，1:释放应用payload数据，2:调用回调
 ** 返回:       0:成功，-1:失败       
 ** 注意:       
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
size_t list_note_all_del(list_head_node_t *list_head_node,uint8_t mode)
{
    node_msg_t *node_msg;
    
    if(list_head_node == NULL)return -1;
    /*清除所有节点*/
    list_msg_foreach_begin(list_head_node,node_msg)
    if(mode == 1 && node_msg->payload != NULL)
    {
        free(node_msg->payload);
    }
    else if(mode == 2 && node_msg->task_signal != NULL)
    {
        node_msg->task_signal(LIST_NODE_ALL_DEL,node_msg->payload,node_msg->payload_len);
    }
    list_cur_node_del(list_head_node);
    list_msg_foreach_end();    
    return 0;
}

/*******************************************************************
 ** 函数名:       list_module_timeout
 ** 函数描述:    节点超时处理函数
 ** 参数:      
 ** 返回:
 ** 注意:       
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
static void list_module_timeout(int fd, short event, void* arg)
{
    list_node_t * task_node;
    list_timeout_t *list_timeout = (list_timeout_t *)arg;
    uint8_t res = LIST_CB_NONE;

    DEBUG("%s,list_timeout:%p\n",__func__,list_timeout);
    task_node = list_timeout->task_node;
    task_node->times++;
    if(task_node->times > task_node->node_msg->retry_times)
    {
        if(task_node->node_msg->task_signal != NULL)
        {
             res = task_node->node_msg->task_signal(LIST_TIMEOUT_END,task_node->node_msg->payload,task_node->node_msg->payload_len);
        }
        if(res == LIST_CB_NONE)
        {
            res = LIST_CB_DEL;
        }
    }
    else
    {
        if(task_node->node_msg->task_signal != NULL)
        {
             res = task_node->node_msg->task_signal(LIST_TIMEOUT,task_node->node_msg->payload,task_node->node_msg->payload_len);
        }
        else
        {
            res= LIST_CB_DEL;
        }
    }
    switch(res)
    {
        case LIST_CB_NONE:
            break;
        case LIST_CB_DEL://删除节点
            list_node_del(list_timeout->task_head_node,list_timeout->task_node);
            //free(list_timeout);
            break;
        case LIST_CB_AGAIN:
            task_node->times = 0;
            break;
        default:
            DEBUG("undefine LIST_CMD!\n");
            list_node_del(list_timeout->task_head_node,list_timeout->task_node);
            break;
    }
    DEBUG("%s end\n",__func__);
} 

/*******************************************************************
 ** 函数名:       task_head_del
 ** 函数描述:    删除链表
 ** 参数:      
 ** 返回:
 ** 注意:       
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
size_t task_head_del(list_head_node_t *list_head_node)
{
    if(list_head_node == NULL)return -1;
    /*清除所有节点*/
    list_note_all_del(list_head_node,0);
    free(list_head_node);//删除链表头
    return 0;
}

/*******************************************************************
 ** 函数名:       task_head_m_del
 ** 函数描述:    多模式删除链表
 ** 参数:       list_head_node:链表
                mode: 0:直接删除节点，1:释放应用payload数据，2:调用回调
 ** 返回:
 ** 注意:       
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
size_t task_head_m_del(list_head_node_t *list_head_node,uint8_t mode)
{
    if(list_head_node == NULL)return -1;
    /*清除所有节点*/
    list_note_all_del(list_head_node,mode);
    free(list_head_node);//删除链表头
    return 0;
}

/*******************************************************************
 ** 函数名:       task_head_create
 ** 函数描述:    添加链表
 ** 参数:       base: libevent base
                unit_time:  0:不计时;  >0:计时单位 其中timer == 0xffffff不计时       
 ** 返回:
 ** 注意:       链表可以加入线程链表中，也可以不加变成一个单独的链表(不能设置超时)
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
list_head_node_t * task_head_create(struct event_base *base)
{
    list_head_node_t *task_head_node;
    
    task_head_node = (list_head_node_t*)malloc(sizeof(list_head_node_t));
    task_head_node->base = base;
    task_head_node->query_cur_task = NULL;
    task_head_node->task_head = NULL;
    task_head_node->node_amount = 0;
    return task_head_node;
    
}

/*---------------------------------------------------------------------------*/


