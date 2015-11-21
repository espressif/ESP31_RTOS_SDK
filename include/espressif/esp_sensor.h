/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP32 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __ESP_SENSOR_H__
#define __ESP_SENSOR_H__

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup Sensor_APIs Sensor APIs
  * @brief Temperature Sensor and Touch pad Sensor APIs
  */

/** @addtogroup Sensor_APIs
  * @{
  */


/**
  * @brief  Read value from temperature sensor.
  *
  * @param  null
  *
  * @return range [0, 255]
  */
uint8 temperature_sensor_read(void);

typedef enum {
    TOUCH_SENSOR_PAD0 = BIT0,
    TOUCH_SENSOR_PAD1 = BIT1,
    TOUCH_SENSOR_PAD2 = BIT2,
    TOUCH_SENSOR_PAD3 = BIT3,
    TOUCH_SENSOR_PAD4 = BIT4,
    TOUCH_SENSOR_PAD5 = BIT5,
    TOUCH_SENSOR_PAD6 = BIT6,
    TOUCH_SENSOR_PAD7 = BIT7,
    TOUCH_SENSOR_PAD8 = BIT8,
    TOUCH_SENSOR_PAD9 = BIT9
} touch_sensor_pad;

/**
  * @brief  Initialize touch pad sensor.
  *
  * @param  touch_sensor_pad pad : enable the corresponding touch_pad[9:0]
  *
  * @return  null
  */
void touch_sensor_init(touch_sensor_pad pad);

/**
  * @brief  Read value from touch pad sensor.
  *
  * Example:          
  * <pre> 
  *         uint16 pad_out[10];
  *         uint16 sample_num = 10000;
  *         rtc_touch_read(pad_out, sample_num);
  * </pre>
  *
  * @param  uint16 *pad_out : pointer of the start address of uint16 pad_out[10], to get the value  
  *                                        from touch pad sensor (touch_pad[9:0]).
  * @param  uint16 sample_num : range [0, 65535], 
  *                                        meaturing time of touch pad = sample_num*(1/RTC_CLK)
  *
  * @return null
  */
void touch_sensor_read(uint16 *pad_out, uint16 sample_num);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
