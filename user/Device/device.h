#ifndef __DEVICE_H
#define __DEVICE_H
#include "config.h"
// 设备 最大条数
#define  MAX_DEVTABLE_NUM  20
#define DEVTAB_REQUEST 0x10
#define DEVTAB_UPDATE  0x11
#define DEV_STATE 0x01
#define DEV_PROTOCOL 0x02
#define DEV_NETID 0x04
#define DEV_MAC 0x08
#define DEV_ACTST 0x10
#define DEV_CURST 0x20
#define DEV_TYPE 0x40
#define DEV_NAME 0x80
// 每个设备 IO个数
#define DEV_IO_NUM 0x05
#define DEV_NAME_SIZE 12
// 设备类型 netId高字节
// 灯
#define DEV_ACT_LIGHT         0x0100
// 插座
#define DEV_ACT_JACK         0x0200
// 窗帘
#define DEV_ACT_CURTAIN      0x0300

#define IO_MODE_CURTAIN_FORTH 0x10
#define IO_MODE_CURTAIN_BACK 0x11
#define IO_MODE_CURTAIN_STOP 0x12
// 开关  开关
#define DEV_SENSOR_SW          0x8000
// 人体红外感应 开关
#define DEV_SENSOR_IR          0x8100
// 烟雾 开关
#define DEV_SENSOR_SMOKE       0x8200
// 燃气 模拟
#define DEV_SENSOR_GAS       0x8300
enum devTbCV
{
    STATE = 1,      //设备状态 0
    PROTOCOL = 2, //接口类型 1
    NETID = 0x04,   //网络号     2
    MAC = 0x08,         //唯一设备MAC 3
    ACTST = 0x10,   //执行状态
    CURST = 0x20,   //当前状态
    TYPE = 0x40,        //设备类型
    NAME = 0x80         //设备名称
};
struct stateTable
{
    uint8_t     iomode; //执行状态
    uint16_t    curstat;//当前状态
    uint8_t     tranSt; //暂存状态
    uint8_t     name[DEV_NAME_SIZE];
};
// 更改设备类型为 暂态类型，字节数更改为uint16_t
// i= sizeof(struct devTable);//0x6A =106
// i= sizeof(devTbs);//2120 =106*20
// i= sizeof(devTbs[0]);//106
// i= sizeof(devTbs[0].statetables);//90=18*5
// i= sizeof(devTbs[0].statetables[0]);//18
struct  devTable
{
    uint8_t     devstate;
    uint8_t     protocol;
    uint16_t    netId;
    uint8_t     mac[8];
    uint8_t    ion;
    uint8_t    operate;//允许操作
    uint8_t    priority; //当前设备 被哪个优先级的任务执行了
    uint8_t    request; //查询标准，每个IO一位，接受到反馈清零
    struct stateTable  statetables[DEV_IO_NUM];
};
//全局设备表
extern struct devTable devTbs[MAX_DEVTABLE_NUM];
extern void Device_operateFlag(struct devTable *pdevTbs);
extern struct devTable *getDevTbsByMac(uint8_t *mac);
extern struct devTable *getDevTbsByNetId(uint16_t id);
extern struct devTable *getNewDevTbs(void);
extern void devInit(void);
#endif
