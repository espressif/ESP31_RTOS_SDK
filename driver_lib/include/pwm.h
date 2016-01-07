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


#ifndef __PWM_H__
#define __PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

//#include "Ledc_reg.h"
//#include "ets_sys.h"
//#include "eagle_soc.h"


/** \defgroup Driver_APIs Driver APIs
  * @brief Driver APIs
  */

/** @addtogroup Driver_APIs
  * @{
  */

/** \defgroup PWM_Driver_APIs PWM Driver APIs
  * @brief PWM driver APIs
  */

/** @addtogroup PWM_Driver_APIs
  * @{
  */

#define PWM_CHANNEL_NUM_MAX 8

#define TIMER0          0
#define TIMER1          1
#define TIMER2          2
#define TIMER3          3

#define CHANNEL0        0
#define CHANNEL1        1
#define CHANNEL2        2
#define CHANNEL3        3
#define CHANNEL4        4
#define CHANNEL5        5
#define CHANNEL6        6
#define CHANNEL7        7

#define OUTPUT_LOW      0
#define OUTPUT_HIGH     1

#define REF_TICK_CLK    0
#define APB_CLK         1

struct pwm_param {
    uint32 period;
    uint32 freq;
    uint32 duty[PWM_CHANNEL_NUM_MAX];  //PWM_CHANNEL<=8
};

/**
  * @brief     PWM function initialization, including GPIO, frequency and duty cycle.
  *
  * @attention This API can be called only once.
  *
  * @param     uint32 period : pwm frequency
  * @param     uint32 *duty : duty cycle
  * @param     uint32 pwm_channel_num : PWM channel number
  * @param     uint32 (*pin_info_list)[3] : GPIO parameter of PWM channel, it is a pointer
  *                                         of n x 3 array which defines GPIO register, IO
  *                                         reuse of corresponding pin and GPIO number.
  *
  * @return    null
  */
void pwm_init(uint32 period, uint32 *duty, uint32 pwm_channel_num, uint32(*pin_info_list)[3]);

/**
  * @brief     Set the duty cycle of a PWM channel.
  *
  *            Set the time that high level signal will last, duty depends on period,
  *            the maximum value can be period *1000 / 45.
  *            For example, 1KHz PWM, duty range is 0~22222
  *
  * @attention After set configuration, pwm_start needs to be called to take effect.
  *
  * @param     uint32 duty : duty cycle
  * @param     uint8 channel : PWM channel number
  *
  * @return    null
  */
void pwm_set_duty(uint32 duty, uint8 channel);

/**
  * @brief  Get the duty cycle of a PWM channel.
  *
  *         Duty cycle will be (duty * 45)/(period *1000).
  *
  * @param  uint8 channel : PWM channel number
  *
  * @return Duty cycle of PWM output.
  */
uint32 pwm_get_duty(uint8 channel);

/**
  * @brief     Set PWM period, unit : us.
  *
  *            For example, for 1KHz PWM, period is 1000 us.
  *
  * @attention  After set configuration, pwm_start needs to be called to take effect.
  *
  * @param     uint32 period : PWM period, unit : us.
  *
  * @return    null
  */
void pwm_set_period(uint32 period);

/**
  * @brief  Get PWM period, unit : us.
  *
  * @param  null
  *
  * @return PWM period, unit : us.
  */
uint32 pwm_get_period(void);

/**
  * @brief  Starts PWM. 
  *
  * @attention This function needs to be called after PWM configuration is changed.
  *
  * @param  null
  *
  * @return null
  */
void pwm_start(void);

/**
  * @brief     Set high_speed channel base clock.
  *
  * @param     uint8 timer_sel : timer to set
  * @param     uint8 apb_clk_sel : pick clock source for timer
  *
  * @return    null
  */
void ledc_set_base_hclk(uint8 timer_sel, uint8 apb_clk_sel);

/**
  * @brief     Set low_speed channel base clock.
  *
  * @param     uint8 timer_sel : timer to set
  * @param     uint8 apb_clk_sel :pick clock source for timer
  *
  * @return    null
  */
void ledc_set_base_lclk(uint8 timer_sel, uint8 apb_clk_sel);

/**
  * @brief     Set high_speed channel frequency.
  *
  *            frequency=base_clk_frequency*div_num*(2^timer_lim)/256
  *
  * @param     uint8 timer_sel : timer to set
  * @param     uint32 div_num : set first divider
  * @param     uint8 timer_lim : set second divider
  *
  * @return    null
  */
void ledc_set_hperiod(uint8 timer_sel, uint32 div_num, uint8 timer_lim);

/**
  * @brief     Set low_speed channel frequency.
  *
  *            frequency=base_clk_frequency*div_num*(2^timer_lim)/256
  *
  * @param     uint8 timer_sel : timer to set
  * @param     uint32 div_num : set first divider
  * @param     uint8 timer_lim : set second divider
  *
  * @return    null
  */
void ledc_set_lperiod(uint8 timer_sel, uint32 div_num, uint8 timer_lim);

/**
  * @brief     Select one timer for one low_speed channel.
  *
  * @param     uint8 chan_num : channel to pick
  * @param     uint8 timer_sel : timer to set
  *
  * @return    null
  */
void ledc_set_ltimer(uint8 chan_num, uint8 timer_sel);

/**
  * @brief     Select one timer for one high_speed channel.
  *
  * @param     uint8 chan_num : channel to pick
  * @param     uint8 timer_sel : timer to set
  *
  * @return    null
  */
void ledc_set_htimer(uint8 chan_num, uint8 timer_sel);

/**
  * @brief     Set high_speed channel output (as high or low) when idle.
  *
  * @param     uint8 chan_num : channel to pick
  * @param     uint8 idle_level : choose output as high or low
  *
  * @return    null
  */
void ledc_set_idle_hlevel(uint8 chan_num, uint8 idle_level);

/**
  * @brief     Set low_speed channel output (as high or low) when idle.
  *
  * @param     uint8 chan_num : channel to pick
  * @param     uint8 idle_level : choose output as high or low
  *
  * @return    null
  */
void ledc_set_idle_llevel(uint8 chan_num, uint8 idle_level);

/**
  * @brief     Set high_speed channel duty.
  *
  * @param     uint8 chan_num : 8 channels in total,value from 0~7
  * @param     uint32 hpoint_val : output high when counter equals this value
  * @param     uint32 duty_val : output low after counter equals this value
  * @param     uint8 increase : 1 - increase duty ratio, 0 - decrease duty ratio
  * @param     uint16 duty_num : generate interrupt after duty_num * duty_cycle outputs
  * @param     uint16 duty_cycle : increase or decrease duty ratio every duty_cycle outputs
  * @param     uint16 duty_scale : the range of changing on duty ratio
  *
  * @return    null
  */
void ledc_set_hduty(uint8 chan_num, uint32 hpoint_val, uint32 duty_val, uint8 increase, uint16 duty_num, uint16 duty_cycle, uint16 duty_scale);

/**
  * @brief     Set low_speed channel duty.
  *
  * @param     uint8 chan_num : 8 channels in total, value from 0~7
  * @param     uint32 hpoint_val : output high when counter equals this value
  * @param     uint32 duty_val : output low after counter equals this value
  * @param     uint8 increase : 1 - increase duty ratio, 0 - decrease duty ratio
  * @param     uint16 duty_num : generate interrupt after duty_num * duty_cycle outputs
  * @param     uint16 duty_cycle : increase or decrease duty ratio every duty_cycle outputs
  * @param     uint16 duty_scale : the range of changing on duty ratio
  *
  * @return    null
  */
void ledc_set_lduty(uint8 chan_num, uint32 hpoint_val, uint32 duty_val, uint8 increase, uint16 duty_num, uint16 duty_cycle, uint16 duty_scale);

/**
  * @brief     Enable one high_speed channel.
  *
  * @param     uint8 chan_num : channel to pick
  *
  * @return    null
  */
void ledc_hstart(uint8 chan_num);

/**
  * @brief     Enable one low_speed channel.
  *
  * @param     uint8 chan_num : channel to pick
  *
  * @return    null
  */
void ledc_lstart(uint8 chan_num);

/**
  * @brief  Pause one of the timers for high_speed channel.
  *
  * @param  uint8 timer_sel : timer to set
  *
  * @return null
  */
void ledc_timer_hpause(uint8 timer_sel);

/**
  * @brief     Pause one of the timers for low_speed channel.
  *
  * @param     uint8 timer_sel : timer to set
  *
  * @return    null
  */
void ledc_timer_lpause(uint8 timer_sel);

/**
  * @brief     Unpause one of the timers for high_speed channel.
  *
  * @param     uint8 timer_sel : timer to set
  *
  * @return    null
  */
void ledc_timer_hunpause(uint8 timer_sel);

/**
  * @brief     Unpause one of the timers for low_speed channel.
  *
  * @param     uint8 timer_sel : timer to set
  *
  * @return    null
  */
void ledc_timer_lunpause(uint8 timer_sel);

/**
  * @brief     Stop one of the timers for high_speed channel.
  *
  * @param     uint8 timer_sel : timer to set
  *
  * @return    null
  */
void ledc_timer_hstop(uint8 timer_sel);

/**
  * @brief     Stop one of the timers for low_speed channel.
  *
  * @param     uint8 timer_sel : timer to set
  *
  * @return    null
  */
void ledc_timer_lstop(uint8 timer_sel);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}

#endif

#endif
