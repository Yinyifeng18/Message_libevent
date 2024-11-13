#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <event2/event.h>


#include "lv_create_pipe.h"
#include "lv_message_cache.h"

typedef struct {
	evt_node_t sys_time_evt;       
} vis_t;

static vis_t *m_vis = NULL;

int cb_cnt = 0;

static void sys_time_evt_cb(evt_node_t *evt_node, void *p)
{
	cb_cnt++;
	printf("sys_time_evt_cb: %d\n", cb_cnt);
}

static void *lv_message_loop(void *task)
{
    fd_set rd;
    struct timeval tv;
    int err;
	int pipe_read_fd = lv_get_read_pipe_fd();
    int loop_cnt = 0;

    while(1)
    {
      FD_ZERO(&rd);
      FD_SET(pipe_read_fd, &rd);
      
      tv.tv_sec = 0;
      tv.tv_usec = 5000;
      err = select(pipe_read_fd+1, &rd, NULL, NULL, &tv);
      
      if(err == 0) //超时
      {
        // printf("select time out!\n");
      }
      else if(err == -1)  //失败
      {
        printf("fail to select!\n");
      }
      else  //成功
      {
        Ui_Message_Queue_Pop();
      }

      usleep(1000);
    }
}


int main(int argc, char **argv)
{
	struct event_base *base = event_base_new();
    Ui_Message_Queue_Init(25);
    lv_create_pipe();
    //lv_leelen_create_theme();
    event_init(base);
    evt_config_init();
	printf("evt_config_init\n");
	
	m_vis = malloc(sizeof(*m_vis));
    memset(m_vis, 0, sizeof(*m_vis));
	
	
	{
        m_vis->sys_time_evt.event_id       = EVT_SYS_TIME_ID;
        m_vis->sys_time_evt.module_id      = EVT_SYS;
        m_vis->sys_time_evt.evt_cb         = sys_time_evt_cb;
        m_vis->sys_time_evt.p              = NULL;
		printf("sys_time_evt\n");
        event_listen(&m_vis->sys_time_evt);
    }
	printf("event_listen\n");
	
	pthread_t tid;
    pthread_create(&tid, NULL, lv_message_loop, NULL);
	
	while(1)
	{
	  printf("Ui_Message_Queue_Push\n"); 
	  Ui_Message_Queue_Push(EVT_SYS,EVT_SYS_TIME_ID,NULL,NULL);
	  sleep(1);	 
	}
	
	
	event_unlisten(&m_vis->sys_time_evt);
	event_base_dispatch(base);
	
	return 0;
}