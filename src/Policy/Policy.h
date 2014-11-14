#ifndef __POLICY_H
#define __POLICY_H
#include "config.h"
// 策略表 最大条数
#define  MAX_DESTABLE_NUM  0x40

#define PRIORITY_HIGHEST  0xFF
#define PRIORITY_DEFAULT  0xFE
#define PRIORITY_LOWEST   0x01
#define PRIORITY_NOUSE    0x00
// 策略优先级为0时表示当前策略没有设定

// 触发类型 type
#define TRIGGER_FLIP  0x00
// #define TRIGGER_EQUAL 0x01
#define TRIGGER_BIG_EQUAL   0x02
#define TRIGGER_SMALL 0x03

// 模式设置

#define MODE_DEFAULT  0x00
#define MODE_HOME     0x01
#define MODE_LEAVE    0x02
#define MODE_RELAX  0x03
#define MODE_PROTECT  0x04

extern uint8_t MODE;

struct deviceInfo
{
    uint16_t deviceId;  //设备ID 可选 网络号|MAC
    // pdevTbs->statetables[pdevTbs->ion].curstat
    // pdevTbs->statetables[pdevTbs->ion].iomode
    uint16_t deviceSta;
};
struct sensor
{
    uint16_t sensorId;  //设备ID 可选 网络号|MAC
    // pdevTbs->statetables[pdevTbs->ion].curstat
    uint16_t sensorState;
};
struct actuator
{
    uint16_t actuatorId;  //设备ID 可选 网络号|MAC
    // pdevTbs->statetables[pdevTbs->ion].iomode
    uint8_t actuatorIO;
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
    uint8_t usable; //操作状态 模式设置 MODE_HOME
    uint8_t priority; //优先级 不允许相同 优先级为0 表示无此策略
    uint8_t num;
    uint8_t type;   //触发类型 TRIGGER_FLIP
    struct sensor sensors[4];
    struct actuator actuators[10];
    // uint8_t name[10];
};
struct strgy_swtable
{
    uint8_t usable; //操作状态 启用/禁用
    uint8_t priority; //优先级 不允许相同
    // uint8_t id;         //
    struct sensor  button;
    struct actuator  actuate;
};
/**
  * @brief  控制策略执行 循环检测 设备状态表
  * @param  NETID:
  * @param  IOn: IO_D0 IO_D1 IO_D2 IO_D3 IO_D4
  * @retval 发送成功返回 OK
    * @writer lishoulei
    *   @modify
  */
//最大全局策略表大小
extern struct strgytable strategytable[];
// extern struct strgytable *getANewDES(void);
//策略解析
extern void strategy_implementation(void);
extern void strategy_init(void);
extern void policy_mode_switch(void);
extern void policy_init(void);
#endif