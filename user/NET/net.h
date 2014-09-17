#ifndef __NET_H
#define __NET_H

#include "config.h"

// 接收指令最大条数
#define RECV_CMDS_NUM 2
// 发送指令最大条数
#define SEND_CMDS_NUM 20

// 指令最大长度
#define CMD_LEN 50
// 指令数据区最大长度 指令长度减去头
#define CMD_DATA_LEN (CMD_LEN-10)

// 接收缓冲区大小
#define NET_BUFFSIZE 512

// 设备 最大条数
#define  MAX_DEVTABLE_NUM  20
// 策略表 最大条数
#define  MAX_DESTABLE_NUM  10

#define NET_CMD_HEAD 0xE0
#define NET_CMD_END 0x18


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
/*
struct  devTable
{
    uint8_t     state;
    uint8_t     protocol;
    uint16_t    netId;
    uint64_t    mac;
          union
    {
        uint32_t Sensor_State;
        struct
        {
                    uint16_t  Actuator_ActSt;
                    uint16_t  Actuator_curSt;
        } st;
    }status;

    uint8_t   type;
    uint8_t   name[10];
};
*/

struct sensorInfo
{

    uint8_t sensorId[8];  //设备ID 可选 网络号|MAC
    uint32_t sensorTrigger;

};

struct actuatorInfo
{

    uint8_t actuatorId[8];    //设备ID 可选 网络号|MAC
    uint8_t actuatorTrigger;

};
/**
  * @brief 全局策略结构体:
                        序号  1               2           3                   4                   5                   6                       7
                                    优先级       id      触发数量        触发类型    传感器*N         控制器*M         名称
                        字节  1字节     1字节 1字节         1字节     2字节+4字节 2字节+1字节 10字节
                                    0为禁用  最大255   高2位传感器（4个）  低6位 控制器数量（64个）  两位(2bit)标识一个传感器       传感器标识 触发条件    控制器标识 控制状态    设备名称

    * @pram  usable  0 未使用 1正在填充 2可用 >=3 定时自加
                     type 无效    0b00 开关 0b01 小触发  0b10 大触发  0b11
    * @writer liuzhishan
    * @modify
  */
struct strgytable
{
    uint8_t usable; //操作状态

    uint8_t first;  //优先级
    uint8_t id;         //
    uint8_t num;
    uint8_t type;
    struct sensorInfo sensor[4];
    struct actuatorInfo actuator[10];
    uint8_t name[10];

};


/**
  * @brief 单条指令存储结构体
    序号  1   2   3   4   5   6
    字节数   1   2   1   N   4   1
    指令头   随机号   数据长度    数据  校验  结束符

    *@parm usable   [netSendDataCMDS]0没有使用 1正在填充 2有效 n 超时递增
                    [netRecvDataCMDS]0没有使用 1正在填充 2有效
    *@parm head 起始符 NET_CMD_HEAD 0xE0
    *@parm sn   0 心跳包 [奇数] 指令包 [偶数] 应答包
    *@parm len  data 数据长度
    *@parm crc  sn+len+data
    *@parm endl 结束符NET_CMD_END0x18
    head    sn      len         Data    CRC         End
    * @writer liuzhishan
    * @modify lishoulei
  */
//0x56字节
struct msgStu
{
    uint8_t    usable;
    uint8_t    head  ;//0xE0
    uint8_t    len;//data 的长度
    uint8_t    sn[2]  ;//
    uint8_t    data[CMD_DATA_LEN];
    uint8_t    crc[4];
    uint8_t    endl;//0x18

};

extern struct devTable  devTbs[MAX_DEVTABLE_NUM];
extern uint8_t NET_buf[NET_BUFFSIZE];
extern uint8_t buf[NET_BUFFSIZE];
extern __IO uint16_t NET_write ;
extern __IO uint16_t NET_read ;

void Delay(__IO uint32_t nTime);

unsigned char calcfcs(unsigned char *pmsg, unsigned char len);

/**
  * @brief  获取可用的发送缓冲
  * @retval 返回发送结构体指针
  * @writer lishoulei
  * @modify
  */
extern struct msgStu *getSendBuf(void);

//将指令从缓冲池中解析出来，放入指令表中
extern void NET_fetchParseInstruction(void);

extern void NET_parseInstruction(struct msgStu *pNmsgR);
//将指令从缓冲池中解析出来，放入指令表中
extern void parseRcvBufToLst(void);
//从指令表中取出所有指令 解析 完成 心跳、应答、指令处理
extern void parseAllCmd(void);
//策略解析，修改对应 设备表
extern void policydecisions(void);
//重发机制
extern void Net_sendTimer(void);
extern uint8_t Net_send(struct msgStu *pNmsgS);

extern void print_CMDS(void);
extern void print_DEVS(void);
extern uint8_t Net_send_device(struct  devTable *devTbs, uint8_t CMD, uint8_t control);
extern void devInit(void);

extern struct devTable *getDevTbsByMac(uint8_t *mac);
extern struct devTable *getDevTbsByNetId(uint16_t id);
extern void Net_PutChar(uint8_t ch);
#endif
