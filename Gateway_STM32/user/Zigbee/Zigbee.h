#ifndef __ZIGBEE_H
#define __ZIGBEE_H
#include "config.h"
//#define ZIGBEE_SND_DATA   0x5f24
//#define ZIGBEE_REQ_NET_IO     0x5E24
//#define ZIGBEE_SET_NET_IO     0x6024
//#define ZIGBEE_REQ_NET_STAT 0x5B24
//#define ZIGBEE_REQ_NET_MAC    0x5D24
//#define ZIGBEE_REQ_NET_ADDR 0x5C24
//#define ZIGBEE_SWITCH_MODE    0x2A21
//小端模式 数据的低位 存放在低地址中
//         对于发送来说，低位先发送
#define ZIGBEE_API_FRAME_HEAD           0xfe
//Zigbee 指令回应
#define ZIGBEE_CMD_RSP                      0x8044
//数据发送指令
#define ZIGBEE_SND_DATA                 0x5F24
//发送回应指令
// #define ZIGBEE_SND_DATA_RSP          0x8044
//数据接收指令
#define ZIGBEE_RECV_DATA                    0x5f44
//远程查询IO 状态接收
#define ZIGBEE_REMOTE_REQ_IO            0x5e24
// #define ZIGBEE_REMOTE_REQ_IO_RSP     0x8044
#define ZIGBEE_REMOTE_REQ_RECV_DATA     0x5e44
//远程设置IO 状态接收
#define ZIGBEE_REMOTE_SET_IO            0x6024
// #define ZIGBEE_REMOTE_SET_IO_RSP     0x8044
#define ZIGBEE_REMOTE_SET_RECV_DATA     0x6044
//Zigbee 网络查询操作
//Zigbee 查询网内所有设备
#define ZIGBEE_GET_ALL                      0x5b24
//通过MAC地址查询网络地址
#define ZIGBEE_GET_NETID_ADDR       0x5c24
//通过网络号查询MAC地址
#define ZIGBEE_GET_MAC_ADDR             0x5d24
//网络操作回应
// #define ZIGBEE_NET_REQ_RSP               0x8044
// 网络设备状态数据接收
#define ZIGBEE_NET_RECV_DEVID      0x5d44
//Zigbee 模式切换
#define ZIGBEE_MODE_SWITCH              0x2a21
#define ZIGBEE_MODE_SWITCH_RSP      0x2a61
//#define ZIGBEE_REQ_SET_RSP            0x2b61
//Zigbee 设备参数设置[]
#define ZIGBEE_DEV_PAR_SET              0x2b21
//Zigbee 设备参数回应[]
#define ZIGBEE_DEV_PAR_SET_RSP      0x2b61
//Zigbee 系统参数设置[save reset start]
#define ZIGBEE_SYS_PAR_SET              0x2c21
//Zigbee 系统参数回应[save reset start]
#define ZIGBEE_SYS_PAR_SET_RSP      0x2c61
#define CMD_DEV_MODE 0x00
#define CMD_CHANNEL  0x01
#define CMD_DEV_TYPE 0x02
#define CMD_DEV_NET_ID 0x03
#define CMD_DEV_NET_ADDR 0x04
#define CMD_DEV_SERIAL_BAUDRATE 0x05
#define CMD_DEV_SERIAL_PARITY 0x06
#define CMD_DEV_SERIAL_STOP 0x07
#define CMD_DEV_SERIAL_FLOWCTL 0x08
#define CMD_DEV_SERIAL_FRAME_INTERVAL 0x09
#define CMD_DEV_DEBUG_GRADE 0x0A
#define CMD_DEV_SLEEP_MODE 0x0B
#define CMD_DEV_WAKE_TIME 0x0C
#define CMD_DEV_SLEEP_TIME 0x0D
#define CMD_DEV_D0_MODE 0x0e
#define CMD_DEV_D0_INTERVAL 0x0F
#define CMD_DEV_D0_VALUE 0x10
#define CMD_DEV_D1_MODE 0x11
#define CMD_DEV_D1_INTERVAL 0x12
#define CMD_DEV_D1_VALUE 0x13
#define CMD_DEV_D2_MODE 0x14
#define CMD_DEV_D2_INTERVAL 0x15
#define CMD_DEV_D2_VALUE 0x16
#define CMD_DEV_D3_MODE 0x17
#define CMD_DEV_D3_INTERVAL 0x18
#define CMD_DEV_D3_VALUE 0x19
#define CMD_DEV_D4_MODE 0x1A
#define CMD_DEV_D4_INTERVAL 0x1B
#define CMD_DEV_D4_VALUE 0x1C
#define CMD_DEV_NET_ID_2 0x1D
#define CMD_DEV_NET_ADDR_2 0x1E
#define CMD_DEV_NET_STAT 0x1F
#define CMD_DEV_AUTO_START_NET 0x20
#define CMD_DEV_OUTPUT_SERIAL 0x21
#define CMD_DEV_FIND_WAY 0x22
#define CMD_DEV_SND_RSP 0x23
#define CMD_DEV_API_DATA_STAT 0x24
#define CMD_DEV_D3_PULSE_COUNT 0x25
#define CMD_DEV_D4_PULSE_COUNT 0x26
#define CMD_DEV_TRANSPARANT_ADDR 0x27
#define CMD_SYS_PARA_SAVE 0x00
#define CMD_SYS_RESTORE_FACTORY 0x01
#define CMD_SYS_REBOOT 0x02
#define CMD_SYS_START_NET 0x03
//IO 接口定义
#define IO_D0 0
#define IO_D1 1
#define IO_D2 2
#define IO_D3 3
#define IO_D4 4
//IO 模式设置
#define IO_MODE_NOUSE           0
#define IO_MODE_ANALOG_INPUT    1
#define IO_MODE_GPIO_INPUT      2
#define IO_MODE_GPIO_OUTPUT_0   3   //实际情况和手册相反
#define IO_MODE_GPIO_OUTPUT_1   4   //实际情况和手册相反
#define IO_MODE_PULSE_COUNT     5
//Zigbee 设备类型
#define ZIGBEE_COORDINATOR  0
#define ZIGBEE_ROUTER       1
#define ZIGBEE_DEVICE       2
//Zigbee 传输模式
#define ZIGBEE_MODE_TRANSPARENT     0
#define ZIGBEE_MODE_AT              1
#define ZIGBEE_MODE_API             2
//中断接收缓冲区大小
#define ZIGBEE_BUFFSIZE 512
//中断接收缓冲区数据区大小
#define ZIGEBEE_DATA_LEN 17
//定义最大发送指令条数
#define ZIGEBE_SEND_CMD_NUM 0x150
#define ZIGEBE_RECV_CMD_NUM 2
#define ZIGEBE_IMMEDIATE 1
#define ZIGEBE_NONIMMEDIATE 0
#define ZIGEBE_FORCE 1
#define ZIGEBE_NOFORCE 0
struct Zigbee_msgStu
{
    __IO uint8_t    usable;
    uint8_t    head  ;    //0xFE
    uint8_t    len;       //data 的长度
    uint8_t    cmd[2]  ;  //命令cmd[0]存放低字节
    uint8_t    data[ZIGEBEE_DATA_LEN];
    uint8_t    chk;       //校验和
    uint8_t    endl;      //0x00
};
// 中断接收 缓冲区
extern uint8_t Zigbee_buf[ZIGBEE_BUFFSIZE] ;
// 数据发送缓冲区
extern struct Zigbee_msgStu Zigbee_SendBuff[ZIGEBE_SEND_CMD_NUM];
extern __IO uint16_t Zigbee_read;
extern __IO uint16_t Zigbee_write;
extern __IO uint16_t Zigbee_send_add;
extern __IO uint16_t Zigbee_send_rm;
void SendCMD(uint8_t data);
extern uint8_t Zigbee_send(struct Zigbee_msgStu *pZmsgS);
extern int8_t zigbee_send_data(uint8_t len, uint16_t netid, uint8_t buf[],uint8_t immediate);
extern void zigbee_remote_req_net_io(uint16_t netid, uint8_t IOn,uint8_t immediate );
extern uint8_t zigbee_remote_set_net_io(uint16_t netid, uint8_t IOn, uint8_t IOmode, uint8_t IOvalue,uint8_t immediate);
extern void Zigbee_getActstate_timer(uint32_t timeout);
extern void Zigbee_getBattery(uint32_t timeout);
extern void Zigbee_send_Timer(uint32_t timeout);
extern void Zigbee_ClearRequest(uint32_t timeout);
extern void zigbee_operate_default(uint32_t timeout);
extern void Zigbee_ClearPriority( void);
extern void zigbee_updateAllDevice(void);
extern void zigbee_updateMacByNetId(uint16_t netid);
extern void zigbee_updateNetIdByMac(uint8_t *mac);
//从数据缓冲池中将指令提取出来 放入Zigbee_msgStu[]
extern uint8_t Zigbee_fetchParseInstruction (void);
extern uint8_t Zigbee_parseInstruction(struct Zigbee_msgStu *pZmsgS);
#endif
