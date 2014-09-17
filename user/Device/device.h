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


/*
  1、与服务端通信的数据格式
    1[1]           2[1]      3[2]     4 [8]             5[4]       6 [1]      7[10+1]
 设备状态    接口类型     网络号   唯一设备MAC  执行状态 当前状态 设备类型  “名称”
 在线0x01    zigbee--0x01 0x8232   0x12345678   4字节状态值     烟感0x81 “烟雾传感器”
 离线0x00    wifi--0x02   0x0233   0x12312354   2字节    2字节  灯--0x01 “卧室灯2”
 休眠0x02    蓝牙-0x03    0x0234   0x32132122   2字节    2字节  开关0x02 “客厅开关”
*/
//28字节
struct  devTable
{
    uint8_t     devstate;
    uint8_t     protocol;
    uint16_t    netId;
    uint8_t     mac[8];
    uint16_t    ActSt;
    uint16_t    curSt;
    uint8_t     type;
    uint8_t     name[11];
};


//全局设备表
extern struct devTable devTbs[MAX_DEVTABLE_NUM];

extern struct devTable *getDevTbsByMac(uint8_t *mac);
extern struct devTable *getDevTbsByNetId(uint16_t id);

extern void devInit(void);

#endif
