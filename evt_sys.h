#ifndef _EVT_SYS_H_     
#define _EVT_SYS_H_    

typedef enum{
    EVT_SYS_TIME_ID,   //时间更新事件
    EVT_SYS_DATE_ID,   //日期更新事件
    EVT_SYS_INPUT_ID,  //系统输入事件 触摸输入等
    EVT_SYS_4G_SIGNAL_ID,   //4g事件
    EVT_SYS_NETWORK_STATUS_ID, //eth0网卡事件
    EVT_SYS_WIFI_SIGNAL_ID,   //wifi事件
    EVT_SYS_DATA_FLOW_ID,   //数据流量事件
    EVT_SYS_ANTI_DISMANTLING_ID,  //防拆事件
    EVT_SYS_LOGON_SERVER_ID,  //登录服务器事件
    EVT_SYS_COUNT_DOWN_ID,  //倒计时事件
    EVT_SYS_TIMING_ID, //定时器事件
    EVT_SYS_DIAL_ID, //拨号事件
    EVT_SYS_VOICE_ID, //声音事件
    EVT_SYS_NOTICE_ID, //通知事件
    EVT_SYS_BATTERY_ID, //电池电量事件
    EVT_SYS_RH_T_ID,   //温湿度
    EVT_SYS_ARMDISARM_STA_ID,   // 布撤防状态ID
    EVT_SYS_RINGING_ID,       // 来电事件ID
    EVT_SYS_ACCESS_NUM_ID,     // 配件数

    EVT_SYS_DETECTOR_DISP_ID,       // 防区设置显示
    EVT_SYS_SIREN_DISP_ID,          // 警号设置显示
    EVT_SYS_SOCKET_DISP_ID,         // 插座设置显示
    EVT_SYS_KEYPAD_DISP_ID,         // 检测线程
    EVT_SYS_ACCESSORIESSTATUS_ID,   // 配件状态
    EVT_SYS_MAX_ID
} evt_sys_e;


#endif // _EVT_SYS_H_
