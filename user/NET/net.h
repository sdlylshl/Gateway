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
#define NET_BUFFSIZE 1512


#define NET_CMD_HEAD 0xE0
#define NET_CMD_END 0x18

// 服务器下发指令
// 查询所有设备
#define NET_CMD_REQ_ALL_DEV   0x00
// 查询单个设备信息
#define NET_CMD_REQ_ONE_DEV   0x02
// 设置设备名称
#define NET_CMD_SET_DEV_NAME  0x04
// 控制设备开关
#define NET_CMD_SET_ON_OFF    0x08

// 网关回应服务器指令
// 更新设备信息
#define NET_CMD_DEV_UPDATE    0x01

#define NET_CMD_PROCY_UPDATE    0x20


#define Tranverse16(X)                 ((((uint16_t)(X) & 0xff00) >> 8) |(((uint16_t)(X) & 0x00ff) << 8))
#define Tranverse32(X)                 ((((uint32_t)(X) & 0xff000000) >> 24) | (((uint32_t)(X) & 0x00ff0000) >> 8) |  (((uint32_t)(X) & 0x0000ff00) << 8) | (((uint32_t)(X) & 0x000000ff) << 24))


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


extern uint8_t NET_buf[];
extern uint8_t buf[];
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
extern struct msgStu *get_NetSendBuf(void);

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
extern void Net_send_Timer(void);
extern uint8_t Net_send_data(struct msgStu *pNmsgS);
extern void print_CMDS(void);
extern void print_DEVS(void);
//extern uint8_t Net_send_device(struct devTable *pdevTbs, uint8_t CMD, uint8_t control);
extern void devInit(void);

extern void Net_PutChar(uint8_t ch);
#endif
