#ifndef _LIST_MODULE_H_     
#define _LIST_MODULE_H_    
 
 
/*------------------------------- Includes ----------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <event2/event.h>
/*----------------------------- Global Defines ------------------------------*/


/*----------------------------- Global Typedefs -----------------------------*/

typedef enum
{
    LIST_TIMEOUT,//timer触发超时
    LIST_TIMEOUT_END,//达到超时的次数上限
    LIST_NODE_ALL_DEL,//所有链表节点被删除
}list_signal_e;

enum
{
    LIST_CB_NONE = 0,
    LIST_CB_DEL,
    LIST_CB_AGAIN
};
typedef int32_t (*fun_cb)(uint8_t status,void *payload,uint32_t payload_len);

typedef struct
{
    struct event *evt;
    uint64_t interval_timer;
    uint32_t retry_times;
    void *payload;
    uint32_t payload_len;
    fun_cb task_signal;
}node_msg_t;

typedef struct _list_node_t
{
    //struct timeval tm;
    uint32_t times;
    node_msg_t *node_msg;
    void *list_timeout;
    struct _list_node_t *pre;
    struct _list_node_t *next;
}list_node_t;

typedef struct _list_head_node_t
{
    list_node_t *task_head; 
    list_node_t *query_cur_task;
    //uint32_t unit_time;//0:不计时;  >0:计时单位时间 其中timer == 0xffffff不计时
    int32_t node_amount;
    struct event_base *base;
}list_head_node_t;

/*----------------------------- External Variables --------------------------*/
#define IS_LIST_EMPTY(list_head) (list_head->task_head == NULL)

/**一次性查询节点函数**/
#define list_msg_foreach_begin(task_head_node,list_node_msg)      for(task_head_node->query_cur_task = task_head_node->task_head;\
                                                                 task_head_node->query_cur_task != NULL;\
                                                                 task_head_node->query_cur_task=(task_head_node->query_cur_task==NULL?task_head_node->task_head:task_head_node->query_cur_task->next))\
                                                                 {\
                                                                        list_node_msg = task_head_node->query_cur_task->node_msg;
#define list_msg_foreach_end()                                   }

/*WARN:该查询宏只能用作查询链表使用，不能在内部进行节点删除*/
#define list_msg_p_foreach_begin(task_head_node,list_node_msg)      {list_node_t *query_cur_task;\
                                                                    for(query_cur_task = task_head_node->task_head;\
                                                                     query_cur_task != NULL;\
                                                                     query_cur_task=(query_cur_task==NULL?NULL:query_cur_task->next))\
                                                                     {\
                                                                            list_node_msg = query_cur_task->node_msg;
#define list_msg_p_foreach_end()                                    }}

#define list_node_amount(list_head)   (list_head->node_amount)
/*------------------------ Global Function Prototypes -----------------------*/

/*******************************************************************
 ** 函数名:       list_node_add
 ** 函数描述:    从头部添加节点
 ** 参数:      
 ** 返回:
 ** 注意:     
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
node_msg_t *list_node_add(list_head_node_t *task_head_node,uint64_t timer);

/*******************************************************************
 ** 函数名:       list_node_add_tail
 ** 函数描述:    从尾部添加节点
 ** 参数:      
 ** 返回:
 ** 注意:     
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
node_msg_t *list_node_add_tail(list_head_node_t *task_head_node,uint64_t timer);

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
node_msg_t *list_node_mid_add(list_head_node_t *task_head_node,list_node_t *task_node,uint8_t type,uint64_t timer);

/*******************************************************************
 ** 函数名:       list_node_retime
 ** 函数描述:    节点计时重置
 ** 参数:      
 ** 返回:       0:成功，-1:失败
 ** 注意:      允许查询中断后，下次继续查询 
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
size_t list_node_retime(node_msg_t *node_msg);

/*******************************************************************
 ** 函数名:       list_node_reset_time
 ** 函数描述:    节点超时时间重新设置
 ** 参数:      
 ** 返回:       0:成功，-1:失败
 ** 注意:      
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
size_t list_node_reset_time(list_node_t *list_node,uint64_t timer);

/*******************************************************************
 ** 函数名:       list_msg_to_node_query
 ** 函数描述:    用户信息转换成节点信息
 ** 参数:      
 ** 返回:      
 ** 注意:      允许查询中断后，下次继续查询 
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
list_node_t *list_msg_to_node_query(list_head_node_t *task_head_node,node_msg_t *node_msg);

/*******************************************************************
 ** 函数名:       list_node_query
 ** 函数描述:    查询节点
 ** 参数:      
 ** 返回:
 ** 注意:      允许查询中断后，下次继续查询 
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
list_node_t *list_node_query(list_head_node_t *task_head_node);

/*******************************************************************
 ** 函数名:       list_msg_query
 ** 函数描述:    查询节点
 ** 参数:      
 ** 返回:
 ** 注意:      允许查询中断后，下次继续查询 
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
node_msg_t *list_msg_query(list_head_node_t *task_head_node);

/*******************************************************************
 ** 函数名:       list_query_status_clean
 ** 函数描述:    清除查询状态
 ** 参数:      
 ** 返回:
 ** 注意:       该函数配合list_msg_query()或list_node_query()使用
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
void list_query_status_clean(list_head_node_t *task_head_node);

/*******************************************************************
 ** 函数名:       list_cur_node_del
 ** 函数描述:    删除当前节点
 ** 参数:      
 ** 返回:
 ** 注意:       该函数配合查询函数使用
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
void list_cur_node_del(list_head_node_t *task_head_node);

/*******************************************************************
 ** 函数名:       list_node_del
 ** 函数描述:    链表节点删除
 ** 参数:      
 ** 返回:
 ** 注意:       
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
void list_node_del(list_head_node_t *task_head_node,list_node_t *list_node);

/*******************************************************************
 ** 函数名:       list_node_msg_del
 ** 函数描述:    链表节点删除
 ** 参数:      
 ** 返回:
 ** 注意:       
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
void list_node_msg_del(list_head_node_t *task_head_node,node_msg_t *node_msg);

/*******************************************************************
 ** 函数名:       list_note_all_del
 ** 函数描述:    删除该链表的所有节点(不带删头结点)
 ** 参数:      list_head_node:链表
               mode: 0:直接删除节点，1:释放应用payload数据，2:调用回调
 ** 返回:       0:成功，-1:失败       
 ** 注意:       
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
size_t list_note_all_del(list_head_node_t *list_head_node,uint8_t mode);

/*******************************************************************
 ** 函数名:       task_head_del
 ** 函数描述:    删除链表
 ** 参数:      
 ** 返回:
 ** 注意:       
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
size_t task_head_del(list_head_node_t *list_head_node);

/*******************************************************************
 ** 函数名:       task_head_m_del
 ** 函数描述:    多模式删除链表(带删表头)
 ** 参数:       list_head_node:链表
                mode: 0:直接删除节点，1:释放应用payload数据，2:调用回调
 ** 返回:
 ** 注意:       
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
size_t task_head_m_del(list_head_node_t *list_head_node,uint8_t mode);

/*******************************************************************
 ** 函数名:       task_head_create
 ** 函数描述:    添加链表
 ** 参数:       task: 线程链表指针
                timing_mode:  0:不计时;  1:计时 其中timer == 0xffffff不计时 
 ** 返回:
 ** 注意:       链表可以加入线程链表中，也可以不加变成一个单独的链表(不能设置超时)
 ** 记录: 	 2017/3/9,  Liu Yongfei创建
 ********************************************************************/
list_head_node_t * task_head_create(struct event_base *base);

#endif //






