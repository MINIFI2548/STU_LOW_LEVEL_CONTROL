#include "joy.h"
#include <math.h>

// ตั้งค่าพารามิเตอร์ของฮาร์ดแวร์
#define ADC_CENTER_VALUE 2048.0f
#define ADC_MAX_DEVIATION 2048.0f
#define JOY_DEADBAND 150.0f // ค่าความกว้างที่ไม่ต้องการให้จอยตอบสนอง (ปรับจูนได้)

void Joy_Init(Joy_TypeDef *joy)
{
    // เคลียร์ค่าเริ่มต้นทั้งหมดให้เป็น 0 และสถานะปล่อยปุ่ม
    joy->axes_X = 0.0f;
    joy->axes_Y = 0.0f;
    joy->btn_A = JOY_BTN_RELEASED;
	joy->btn_B = JOY_BTN_RELEASED;
	joy->btn_C = JOY_BTN_RELEASED;
	joy->btn_D = JOY_BTN_RELEASED;
}

void Joy_Update(Joy_TypeDef *joy, uint16_t raw_adc_X, uint16_t raw_adc_Y)
{
    // ==========================================
    // 1. การคำนวณแกน X (พร้อม Deadband)
    // ==========================================
    float dev_X = (float)raw_adc_X - ADC_CENTER_VALUE;
    if (fabs(dev_X) <= JOY_DEADBAND) {
        joy->axes_X = 0.0f;
    } else {
        joy->axes_X = dev_X / ADC_MAX_DEVIATION;
    }

    // ==========================================
    // 2. การคำนวณแกน Y (พร้อม Deadband)
    // ==========================================
    float dev_Y = (float)raw_adc_Y - ADC_CENTER_VALUE;
    if (fabs(dev_Y) <= JOY_DEADBAND) {
        joy->axes_Y = 0.0f;
    } else {
        joy->axes_Y = dev_Y / ADC_MAX_DEVIATION;
    }

    // จำกัดขอบเขต (Saturation) ป้องกันค่าเกิน -1.0 ถึง 1.0
    if (joy->axes_X > 1.0f) joy->axes_X = 1.0f;
    if (joy->axes_X < -1.0f) joy->axes_X = -1.0f;
    if (joy->axes_Y > 1.0f) joy->axes_Y = 1.0f;
    if (joy->axes_Y < -1.0f) joy->axes_Y = -1.0f;

    // ==========================================
    // 3. การอัปเดตสถานะปุ่ม (แบบ Active-Low)
    // ==========================================
    // หมายเหตุ: ต้องมั่นใจว่าใน CubeMX มีการตั้งขา PB เป็น Pull-up (PU) ไว้แล้ว
    joy->btn_A = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET) ? JOY_BTN_PRESSED : JOY_BTN_RELEASED;
	joy->btn_B = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_RESET) ? JOY_BTN_PRESSED : JOY_BTN_RELEASED;
	joy->btn_C = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_RESET) ? JOY_BTN_PRESSED : JOY_BTN_RELEASED;
	joy->btn_D = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_RESET) ? JOY_BTN_PRESSED : JOY_BTN_RELEASED;
}
