

#include "config.h"


//全局设备表
struct devTable  devTbs[MAX_DEVTABLE_NUM];


void loadDevMsg()
{


}


/**
  * @brief  通过MAC地址表获取设备表
  * @param  None
  * @retval None
    * @writer liuzhishan
    *   @modify
  */

struct devTable *getDevTbsByMac(uint8_t *mac)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint8_t flag = 0;

    while (i < MAX_DEVTABLE_NUM)
    {

        flag = 1;

        for (j = 0; j < 8; j++)
        {
            if (devTbs[i].mac[j] != mac[j])
                flag = 0;
        }

        if ( flag)
        {
            return &devTbs[i];
        }
        i++;
    }
    return NULL;
}
/**
  * @brief  通过NetId地址表获取设备表
  * @param  None
  * @retval struct devTable *
  * @writer liuzhishan
  *   @modify
  */
struct devTable *getDevTbsByNetId(uint16_t id)
{
    uint32_t i = 0;
    while (i < MAX_DEVTABLE_NUM)
    {
        if (devTbs[i].netId == id)
        {
            return &devTbs[i];
        }
        i++;
    }
    return NULL;
}



void print_DEVS()
{
    uint16_t i = 0;
    uint16_t j = 0;
    //uint8_t *pcmd;

    while (i < MAX_DEVTABLE_NUM)
    {

        if (devTbs[i].devstate )
        {

            //pcmd = (uint8_t *)&devTbs[i];

            j++;

        }
        i++;
    }

    SendCMD(j);
    SendCMD(0x5B);


}


void devInit(void)
{
    //pwNr = 0;
    //C1 3A D7 02 00 4B 12 00
    devTbs[1].mac[0] = 0xC1;
    devTbs[1].mac[1] = 0x3A;
    devTbs[1].mac[2] = 0xD7;
    devTbs[1].mac[3] = 0x02;
    devTbs[1].mac[4] = 0x00;
    devTbs[1].mac[5] = 0x4B;
    devTbs[1].mac[6] = 0x12;
    devTbs[1].mac[7] = 0x00;
    devTbs[1].netId = 3600;
    devTbs[1].protocol = 1;
    devTbs[1].devstate = 1;

    devTbs[2].mac[0] = 0x02;
    devTbs[2].netId = 0x3600;
    devTbs[2].protocol = 4;
    devTbs[2].devstate = 2;

    devTbs[3].mac[0] = 0x03;
    devTbs[3].netId = 0x3603;
    devTbs[3].protocol = 3;
    devTbs[3].devstate = 1;

    devTbs[4].mac[0] = 0x04;
    devTbs[4].netId = 0x3604;
    devTbs[4].protocol = 3;
    devTbs[4].devstate = 1;

    devTbs[5].mac[0] = 0x05;
    devTbs[5].netId = 0x3605;
    devTbs[5].protocol = 3;
    devTbs[5].devstate = 1;

    devTbs[6].mac[0] = 0x06;
    devTbs[6].netId = 0x3606;
    devTbs[6].protocol = 3;
    devTbs[6].devstate = 1;

    devTbs[7].mac[0] = 0x07;
    devTbs[7].netId = 0x3607;
    devTbs[7].protocol = 3;
    devTbs[7].devstate = 1;

    devTbs[8].mac[0] = 0x08;
    devTbs[8].netId = 0x3608;
    devTbs[8].protocol = 3;
    devTbs[8].devstate = 1;

    devTbs[9].mac[0] = 0x09;
    devTbs[9].netId = 0x3609;
    devTbs[9].protocol = 3;
    devTbs[9].devstate = 1;

    devTbs[0].mac[0] = 0x10;
    devTbs[0].netId = 0x3610;
    devTbs[0].protocol = 3;
    devTbs[0].devstate = 1;

}