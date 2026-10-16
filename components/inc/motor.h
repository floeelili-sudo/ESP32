#ifndef _MOTOR_H
#define _MOTOR_H

#include "driver/gpio.h"

#define MOTOR_IA GPIO_NUM_46
#define MOTOR_IB GPIO_NUM_9

void Motor_Init(void);

void Motor_Run(void);   // 正转
void Motor_Back(void);  // 反转
void Motor_Stop(void);  // 停转

#endif