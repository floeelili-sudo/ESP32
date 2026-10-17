#ifndef _MOTOR_H
#define _MOTOR_H

#include "driver/gpio.h"
#include "driver/ledc.h"

#define MOTOR_IA GPIO_NUM_46
#define MOTOR_IB GPIO_NUM_9

void Motor_Init(void);

void Motor_IA_PWM(uint16_t speed);
void Motor_IB_PWM(uint16_t speed);
void Motor_Control(int speed);
#endif
