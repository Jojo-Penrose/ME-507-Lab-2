/**
 * @file motor.h
 * @author Jojo Penrose
 * @date 18 Apr 2024
 * @brief Motor PWM control library for the STM32.
 *
 * Contains a motor struct to encapsulate a single motor object requiring two
 * PWM channels for control. Also contains methods useful to motor control.
 */
// motor.h
#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include <stdio.h>
#include <stdint.h>
#include "stm32l4xx_hal.h"

/**
 * @struct Motor_Str
 * @brief A PWM motor object to use with the STM32.
 *
 * A motor in this context uses two channels of a PWM-enabled timer that is pre-
 * configured at instantiation. The object assumes that the first and last two
 * channels of a timer are to be used as a pair (i.e. CH1 & CH2 or CH3 & CH4).
 * It contains a the signed duty cycle (fwd/bwd), a reference to the HAL timer,
 * and an indicator of which channels on the timer are used for this motor.
 *
 * @var Motor_Str::duty
 * Signed duty cycle integer saturated at +/-100, interpreted as percent duty.
 * @var Motor_Str::enable
 * Logical enabled flag. Disabled motors have their drivers forced to STOP mode.
 * @var Motor_Str::timer
 * A pointer to the HAL timer instance (htimx.Instance).
 * @var Motor_Str::ChannelPair
 * A value between 1 and 2, indicating the timer channels to use:
 *      1 = CH1 & CH2
 *      2 = CH3 & CH4
 */
 struct{
    int32_t                 duty;
    uint32_t                enable;
    TIM_HandleTypeDef *     timer;
    uint32_t                ChannelPair;
} typedef Motor_Str;


// Prototype for motor object "methods"
void motor_set_duty(Motor_Str* mot, int32_t duty);

void motor_enable(Motor_Str* mot);

void motor_disable(Motor_Str* mot);

#endif /* INC_MOTOR_H_ */
