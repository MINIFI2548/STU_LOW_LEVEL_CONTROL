#include "motor.h"
#include <math.h> // สำหรับฟังก์ชัน fabs()

// 1. ฟังก์ชันเริ่มต้นระบบ (Constructor)
void Motor_Init(Motor_TypeDef *motor, TIM_HandleTypeDef *htim, uint32_t channel, GPIO_TypeDef *dir_port, uint16_t dir_pin, uint32_t arr_value)
{
    // บันทึกค่าฮาร์ดแวร์ลงใน Struct
    motor->htim = htim;
    motor->channel = channel;
    motor->dir_port = dir_port;
    motor->dir_pin = dir_pin;
    motor->arr_value = arr_value;
    motor->current_duty = 0.0f;

    // มั่นใจว่ามอเตอร์หยุดนิ่งตอนเริ่มต้น
    Motor_Stop(motor);

    // เริ่มสร้างสัญญาณ PWM
    HAL_TIM_PWM_Start(motor->htim, motor->channel);
    __HAL_TIM_MOE_ENABLE(motor->htim);
}

// 2. ฟังก์ชันอัปเดตค่า ARR (ปรับความถี่ PWM)
void Motor_SetARR(Motor_TypeDef *motor, uint32_t new_arr)
{
    motor->arr_value = new_arr;
    __HAL_TIM_SET_AUTORELOAD(motor->htim, new_arr);

    // เมื่อเปลี่ยน ARR ต้องอัปเดต PWM ใหม่ให้สัดส่วนถูกต้องด้วย
    Motor_SetPWM(motor, motor->current_duty);
}

// 3. ฟังก์ชันสั่งงานหลัก (รองรับค่า -1.0 ถึง 1.0)
void Motor_SetPWM(Motor_TypeDef *motor, float control_signal)
{
    // A. จำกัดขอบเขตของสัญญาณ (Saturation) ป้องกันค่าเกิน
    if (control_signal > 1.0f) control_signal = 1.0f;
    if (control_signal < -1.0f) control_signal = -1.0f;

    motor->current_duty = control_signal;

    // B. แยกแยะทิศทางจากเครื่องหมาย (ตรรกะเชิงคณิตศาสตร์)
    if (control_signal >= 0.0f)
    {
        HAL_GPIO_WritePin(motor->dir_port, motor->dir_pin, GPIO_PIN_RESET); // กำหนดทิศ CW

    }
    else
    {
        HAL_GPIO_WritePin(motor->dir_port, motor->dir_pin, GPIO_PIN_SET);   // กำหนดทิศ CCW
    }

    // C. คำนวณค่า Duty Cycle เชิงฮาร์ดแวร์ (แปลงเป็นจำนวนเต็มบวก)
    float abs_duty = fabs(control_signal);
    uint32_t ccr_value = (uint32_t)(abs_duty * (float)(motor->arr_value));

    // D. อัปเดต Register
    __HAL_TIM_SET_COMPARE(motor->htim, motor->channel, ccr_value);
}

// 4. ฟังก์ชันหยุดมอเตอร์ฉุกเฉิน
void Motor_Stop(Motor_TypeDef *motor)
{
    motor->current_duty = 0.0f;
    __HAL_TIM_SET_COMPARE(motor->htim, motor->channel, 0);
}
