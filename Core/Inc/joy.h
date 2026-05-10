#ifndef JOY_H
#define JOY_H

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

// กำหนดสถานะของการกดปุ่ม
typedef enum {
	JOY_BTN_RELEASED = 0,
	JOY_BTN_PRESSED = 1
} JoyButtonState_t;

// โครงสร้างตัวแปรของ Joystick
typedef struct {
    float axes_X;            // แกน X ที่ถูกแปลงค่าแล้ว (-1.0 ถึง 1.0)
    float axes_Y;            // แกน Y ที่ถูกแปลงค่าแล้ว (-1.0 ถึง 1.0)

    JoyButtonState_t btn_A;  // ปุ่ม A (PB1)
    JoyButtonState_t btn_B;  // ปุ่ม B (PB15)
    JoyButtonState_t btn_C;  // ปุ่ม C (PB14)
    JoyButtonState_t btn_D;  // ปุ่ม D (PB13)
    JoyButtonState_t btn_E;
    JoyButtonState_t btn_F;
} Joy_TypeDef;

// ฟังก์ชันหลัก
void Joy_Init(Joy_TypeDef *joy);
void Joy_Update(Joy_TypeDef *joy, uint16_t raw_adc_X, uint16_t raw_adc_Y);

#endif /* JOY_H */
