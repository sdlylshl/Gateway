#ifndef __POLICY_H
#define __POLICY_H

#include "config.h"

// 策略表 最大条数
#define  MAX_DESTABLE_NUM  10

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
  * @brief  控制策略执行 循环检测 设备状态表
  * @param  NETID:
  * @param  IOn: IO_D0 IO_D1 IO_D2 IO_D3 IO_D4
  * @retval 发送成功返回 OK
    * @writer lishoulei
    *   @modify
  */
//最大全局策略表大小
extern struct strgytable strategytable[];


extern struct strgytable *getANewDES(void);
//策略解析
extern void  policydecisions(void);
extern void strategy_implementation(void);
#endif
