#include "stm32f10x.h" // Device header
#include "Infrared.h"
#include "Car.h"

void TrackLine_Update(void)
{
    uint8_t ir = IR_Scan();
    switch (ir)
    {
    //  0001 0011 0010 右转
    case 0x01:
        Turn_Right();
        break;
    case 0x03:
        Turn_Right();
        break;
    case 0x02:
        Turn_Right();
        break;

    //  0100 1000 1100 左转
    case 0x04:
        Turn_Left();
        break;
    case 0x08:
        Turn_Left();
        break;
    case 0x0C:
        Turn_Left();
        break;

    //  1111 0110 直行
    case 0x0F:
        Go_Ahead();
        break;
    case 0x06:
        Go_Ahead();
        break;

    /* 其余未列情况，保守停车 */
    default:
        Car_Stop();
        break;
    }
}
