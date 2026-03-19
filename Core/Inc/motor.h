#ifndef MOTOR_H
#define MOTOR_H

#include "main.h"
#include "tim.h"

// กำหนดทิศทางการหมุนให้เป็นมาตรฐาน
typedef enum {
    MOTOR_DIR_CW = 0, // ตามเข็มนาฬิกา
    MOTOR_DIR_CCW = 1 // ทวนเข็มนาฬิกา
} MotorDirection_t;

// โครงสร้าง Object ของ Motor
typedef struct {
    TIM_HandleTypeDef *htim; // Pointer ชี้ไปยัง Timer ที่ใช้ขับ PWM
    uint32_t channel;        // Channel ของ Timer (เช่น TIM_CHANNEL_1)

    GPIO_TypeDef *dir_port;  // พอร์ตของขา Direction (เช่น GPIOC)
    uint16_t dir_pin;        // ขา Direction (เช่น GPIO_PIN_7)

    uint32_t arr_value;      // ค่าสูงสุดของ Timer (Auto-Reload Register)
    float current_duty;      // เก็บค่า Duty cycle ปัจจุบัน (-1.0 ถึง 1.0)
} Motor_TypeDef;

// ประกาศฟังก์ชัน (Function Prototypes)
void Motor_Init(Motor_TypeDef *motor, TIM_HandleTypeDef *htim, uint32_t channel, GPIO_TypeDef *dir_port, uint16_t dir_pin, uint32_t arr_value);
void Motor_SetARR(Motor_TypeDef *motor, uint32_t new_arr);
void Motor_SetPWM(Motor_TypeDef *motor, float control_signal);
void Motor_Stop(Motor_TypeDef *motor);

#endif /* MOTOR_H */
