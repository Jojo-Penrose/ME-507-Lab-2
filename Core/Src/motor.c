/**
 * @file motor.c
 * @author Jojo Penrose
 * @date 18 Apr 2024
 * @brief Motor PWM control library for the STM32.
 *
 * Contains a motor struct to encapsulate a single motor object requiring two
 * PWM channels for control. Also contains methods useful to motor control.
 */
// motor.c
#include "motor.h"

/**
 * @brief Set motor duty cycle.
 *
 * Detailed description of what the function does.
 *
 * @param   mot     A pointer to the motor struct.
 * @param   duty    The requested duty cycle, saturated to +/-100 percent.
 * @return  Nothing.
 * @note    Something to note.
 * @warning Duty cycle is an INTEGER, please round all floats when passing, thx.
 */
 void motor_set_duty(Motor_Str* mot, int32_t duty)
{
    // Disabled motor check
    if (mot->enable != 1){ return; }

    // Duty cycle saturation
    if (duty > 100){
        duty = 100;
    } else if (duty < -100){
        duty = -100;
    }

    // ARR is unsigned, cast it to signed for math
    int32_t ARR = (int32_t) (mot->timer->Init.Period + 1);

    // Calculate duty CCR
    duty = duty*ARR/100;

    // Assign the duty cycle CCR to proper channel pair
    if (duty < 0){
        if(mot->ChannelPair == 1){
            mot->timer->Instance->CCR1 = ARR;
            mot->timer->Instance->CCR2 = ARR + duty;
        } else{
            mot->timer->Instance->CCR3 = ARR;
            mot->timer->Instance->CCR4 = ARR + duty;
        }
    } else{
        if(mot->ChannelPair == 1){
            mot->timer->Instance->CCR1 = ARR - duty;
            mot->timer->Instance->CCR2 = ARR;
        } else{
            mot->timer->Instance->CCR3 = ARR - duty;
            mot->timer->Instance->CCR4 = ARR;
        }
    }

    // Print to the console so we can see what's happening
    printf("Setting Motor %ld Duty Cycle to %ld\r\n", mot->ChannelPair, duty);
}

/**
 * @brief Enable motor.
 *
 * Detailed description of what the function does.
 * @param   mot     A pointer to the motor struct.
 * @return  Nothing.
 * @note    Something to note.
 * @warning Warning.
 */
 void motor_enable(Motor_Str* mot)
{
    // Retrieve ARR
    uint32_t ARR = (uint32_t) (mot->timer->Init.Period + 1);

    // Set motors to BRAKE using proper channel pair
    if(mot->ChannelPair == 1){
        mot->timer->Instance->CCR1 = ARR;
        mot->timer->Instance->CCR2 = ARR;
    } else{
        mot->timer->Instance->CCR3 = ARR;
        mot->timer->Instance->CCR4 = ARR;
    }

    // Set enable flag high
    mot->enable = 1;

    // Print to the console so we can see what's happening
    printf("Enabling Motor %ld\r\n", mot->ChannelPair);
}

/**
 * @brief Disable motor.
 *
 * Detailed description of what the function does.
 * @param   mot     A pointer to the motor struct.
 * @return  Nothing.
 * @note    Something to note.
 * @warning Warning.
 */
 void motor_disable(Motor_Str* mot)
{

    // Set motors to STOP using proper channel pair
    if(mot->ChannelPair == 1){
        mot->timer->Instance->CCR1 = 0;
        mot->timer->Instance->CCR2 = 0;
    } else{
        mot->timer->Instance->CCR3 = 0;
        mot->timer->Instance->CCR4 = 0;
    }

    // Set enable flag low
    mot->enable = 0;

    // Print to the console so we can see what's happening
    printf("Disabling Motor %ld\r\n", mot->ChannelPair);
}










