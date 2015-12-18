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

#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_Pin_0              (BIT(0))  /* Pin 0 selected */
#define GPIO_Pin_1              (BIT(1))  /* Pin 1 selected */
#define GPIO_Pin_2              (BIT(2))  /* Pin 2 selected */
#define GPIO_Pin_3              (BIT(3))  /* Pin 3 selected */
#define GPIO_Pin_4              (BIT(4))  /* Pin 4 selected */
#define GPIO_Pin_5              (BIT(5))  /* Pin 5 selected */
#define GPIO_Pin_6              (BIT(6))  /* Pin 6 selected */
#define GPIO_Pin_7              (BIT(7))  /* Pin 7 selected */
#define GPIO_Pin_8              (BIT(8))  /* Pin 8 selected */
#define GPIO_Pin_9              (BIT(9))  /* Pin 9 selected */
#define GPIO_Pin_10             (BIT(10)) /* Pin 10 selected */
#define GPIO_Pin_11             (BIT(11)) /* Pin 11 selected */
#define GPIO_Pin_12             (BIT(12)) /* Pin 12 selected */
#define GPIO_Pin_13             (BIT(13)) /* Pin 13 selected */
#define GPIO_Pin_14             (BIT(14)) /* Pin 14 selected */
#define GPIO_Pin_15             (BIT(15)) /* Pin 15 selected */
#define GPIO_Pin_16             (BIT(16)) /* Pin 16 selected */
#define GPIO_Pin_17             (BIT(17)) /* Pin 17 selected */
#define GPIO_Pin_18             (BIT(18)) /* Pin 18 selected */
#define GPIO_Pin_19             (BIT(19)) /* Pin 19 selected */
#define GPIO_Pin_20             (BIT(20)) /* Pin 20 selected */
#define GPIO_Pin_21             (BIT(21)) /* Pin 21 selected */
#define GPIO_Pin_22             (BIT(22)) /* Pin 22 selected */
#define GPIO_Pin_23             (BIT(23)) /* Pin 23 selected */
//#define GPIO_Pin_24             (BIT(24)) /* Pin 24 selected */
#define GPIO_Pin_25             (BIT(25)) /* Pin 25 selected */
#define GPIO_Pin_26             (BIT(26)) /* Pin 26 selected */
#define GPIO_Pin_27             (BIT(27)) /* Pin 27 selected */
//#define GPIO_Pin_28             (BIT(28)) /* Pin 28 selected */
//#define GPIO_Pin_29             (BIT(29)) /* Pin 29 selected */
//#define GPIO_Pin_30             (BIT(30)) /* Pin 30 selected */
//#define GPIO_Pin_31             (BIT(31)) /* Pin 31 selected */
#define GPIO_Pin_32             (BIT(0))  /* Pin 32 selected */
#define GPIO_Pin_33             (BIT(1))  /* Pin 33 selected */
#define GPIO_Pin_34             (BIT(2))  /* Pin 34 selected */
#define GPIO_Pin_35             (BIT(3))  /* Pin 35 selected */
#define GPIO_Pin_36             (BIT(4))  /* Pin 36 selected */
#define GPIO_Pin_37             (BIT(5))  /* Pin 37 selected */
#define GPIO_Pin_38             (BIT(6))  /* Pin 38 selected */
#define GPIO_Pin_39             (BIT(7))  /* Pin 39 selected */

#define GPIO_PIN_REG_0          PERIPHS_IO_MUX_GPIO0_U
#define GPIO_PIN_REG_1          PERIPHS_IO_MUX_U0TXD_U
#define GPIO_PIN_REG_2          PERIPHS_IO_MUX_GPIO2_U
#define GPIO_PIN_REG_3          PERIPHS_IO_MUX_U0RXD_U
#define GPIO_PIN_REG_4          PERIPHS_IO_MUX_GPIO4_U
#define GPIO_PIN_REG_5          PERIPHS_IO_MUX_GPIO5_U
#define GPIO_PIN_REG_6          PERIPHS_IO_MUX_SD_CLK_U
#define GPIO_PIN_REG_7          PERIPHS_IO_MUX_SD_DATA0_U
#define GPIO_PIN_REG_8          PERIPHS_IO_MUX_SD_DATA1_U
#define GPIO_PIN_REG_9          PERIPHS_IO_MUX_SD_DATA2_U
#define GPIO_PIN_REG_10         PERIPHS_IO_MUX_SD_DATA3_U
#define GPIO_PIN_REG_11         PERIPHS_IO_MUX_SD_CMD_U
#define GPIO_PIN_REG_12         PERIPHS_IO_MUX_MTDI_U
#define GPIO_PIN_REG_13         PERIPHS_IO_MUX_MTCK_U
#define GPIO_PIN_REG_14         PERIPHS_IO_MUX_MTMS_U
#define GPIO_PIN_REG_15         PERIPHS_IO_MUX_MTDO_U
#define GPIO_PIN_REG_16         PERIPHS_IO_MUX_GPIO16_U
#define GPIO_PIN_REG_17         PERIPHS_IO_MUX_GPIO17_U
#define GPIO_PIN_REG_18         PERIPHS_IO_MUX_GPIO18_U
#define GPIO_PIN_REG_19         PERIPHS_IO_MUX_GPIO19_U
#define GPIO_PIN_REG_20         PERIPHS_IO_MUX_GPIO20_U
#define GPIO_PIN_REG_21         PERIPHS_IO_MUX_GPIO21_U
#define GPIO_PIN_REG_22         PERIPHS_IO_MUX_GPIO22_U
#define GPIO_PIN_REG_23         PERIPHS_IO_MUX_GPIO23_U
#define GPIO_PIN_REG_25         PERIPHS_IO_MUX_GPIO25_U
#define GPIO_PIN_REG_26         PERIPHS_IO_MUX_GPIO26_U
#define GPIO_PIN_REG_27         PERIPHS_IO_MUX_GPIO27_U
#define GPIO_PIN_REG_32         PERIPHS_IO_MUX_GPIO32_U
#define GPIO_PIN_REG_33         PERIPHS_IO_MUX_GPIO33_U
#define GPIO_PIN_REG_34         PERIPHS_IO_MUX_GPIO34_U
#define GPIO_PIN_REG_35         PERIPHS_IO_MUX_GPIO35_U
#define GPIO_PIN_REG_36         PERIPHS_IO_MUX_GPIO36_U
#define GPIO_PIN_REG_37         PERIPHS_IO_MUX_GPIO37_U
#define GPIO_PIN_REG_38         PERIPHS_IO_MUX_GPIO38_U
#define GPIO_PIN_REG_39         PERIPHS_IO_MUX_GPIO39_U

#define GPIO_REG_READ(reg)          READ_PERI_REG(reg)
#define GPIO_REG_WRITE(reg, val)    WRITE_PERI_REG(reg, val)
#define GPIO_PIN_COUNT              40
#define GPIO_ID_PIN0                0
#define GPIO_ID_PIN(n)              (GPIO_ID_PIN0 + (n))
#define GPIO_PIN_ADDR(i)            (GPIO_PIN0 + i * 4)

#define GPIO_ID_IS_PIN_REGISTER(reg_id) \
    ((reg_id >= GPIO_ID_PIN0) && (reg_id <= GPIO_ID_PIN(GPIO_PIN_COUNT - 1)))

#define GPIO_REGID_TO_PINIDX(reg_id) ((reg_id) - GPIO_ID_PIN0)

typedef enum {
    GPIO_PIN_INTR_DISABLE = 0,      /**< disable GPIO interrupt */
    GPIO_PIN_INTR_POSEDGE = 1,      /**< GPIO interrupt type : rising edge */
    GPIO_PIN_INTR_NEGEDGE = 2,      /**< GPIO interrupt type : falling edge */
    GPIO_PIN_INTR_ANYEDGE = 3,      /**< GPIO interrupt type : bothe rising and falling edge */
} GPIO_INT_TYPE;

typedef enum {
    GPIO_Mode_Input = 0x0,          /**< GPIO mode : Input */
    GPIO_Mode_Out_OD,               /**< GPIO mode : Output_OD */
    GPIO_Mode_Output ,              /**< GPIO mode : Output */
    GPIO_Mode_Sigma_Delta ,         /**< GPIO mode : Sigma_Delta */
} GPIOMode_TypeDef;

typedef enum {
    GPIO_PullUp_DIS = 0x0,          /**< disable GPIO pull up */
    GPIO_PullUp_EN  = 0x1,          /**< enable GPIO pull up */
} GPIO_Pullup_IF;

typedef enum {
    GPIO_PullDown_DIS = 0x0,        /**< disable GPIO pull down */
    GPIO_PullDown_EN  = 0x1,        /**< enable GPIO pull down */
} GPIO_Pulldown_IF;

typedef struct {
    uint32           GPIO_Pin;      /**< GPIO pin */
    uint32           GPIO_Pin_high; /**< GPIO pin */
    GPIOMode_TypeDef GPIO_Mode;     /**< GPIO mode */
    GPIO_Pullup_IF   GPIO_Pullup;   /**< GPIO pullup */
    GPIO_Pulldown_IF GPIO_Pulldown; /**< GPIO pulldown */
    GPIO_INT_TYPE    GPIO_IntrType; /**< GPIO interrupt type */
} GPIO_ConfigTypeDef;

/** \defgroup Driver_APIs Driver APIs
  * @brief Driver APIs
  */

/** @addtogroup Driver_APIs
  * @{
  */

/** \defgroup GPIO_Driver_APIs GPIO Driver APIs
  * @brief GPIO APIs
  */

/** @addtogroup GPIO_Driver_APIs
  * @{
  */

/**
  * @brief   Set GPIO pin output level.
  *
  * @param   gpio_no   : The GPIO sequence number.
  * @param   bit_value : GPIO pin output level.
  *
  * @return  null
  */
#define GPIO_OUTPUT_SET(gpio_no, bit_value) \
    ((gpio_no < 32) ? gpio_output_conf(bit_value << gpio_no, (bit_value ? 0 : 1) << gpio_no, 1 << gpio_no, 0) : \
         gpio_output_conf_high(bit_value << (gpio_no - 32), (bit_value ? 0 : 1) << (gpio_no - 32), 1 << (gpio_no - 32),0))

/**
  * @brief   Set GPIO pin output level,This function only config GPIO0-GPIO31 .
  *
  * @param   gpio_bits : The GPIO bit number.
  * @param   bit_value : GPIO pin output level.
  *
  * @return  null
  */
#define GPIO_OUTPUT(gpio_bits, bit_value) \
    if(bit_value) gpio_output_conf(gpio_bits, 0, gpio_bits, 0);\
    else gpio_output_conf(0, gpio_bits, gpio_bits, 0)

/**
  * @brief   Set GPIO pin output level,This function only config GPIO32-GPIO39.
  *
  * @param   gpio_bits : The GPIO bit number.
  * @param   bit_value : GPIO pin output level.
  *
  * @return  null
  */
#define GPIO_OUTPUT_HIGH(gpio_bits, bit_value) \
    if(bit_value) gpio_output_conf_high(gpio_bits, 0, gpio_bits, 0);\
    else gpio_output_conf_high(0, gpio_bits, gpio_bits, 0)

/**
  * @brief   Disable GPIO pin output.
  *
  * @param   gpio_no : The GPIO sequence number.
  *
  * @return  null
  */
#define GPIO_DIS_OUTPUT(gpio_no)    ((gpio_no < 32) ? \
    gpio_output_conf(0, 0, 0, 1 << gpio_no) : gpio_output_conf_high(0, 0, 0, 1 << gpio_no))

/**
  * @brief   Enable GPIO pin intput,This function only config GPIO0-GPIO31.
  *
  * @param   gpio_bits : The GPIO bit number.
  *
  * @return  null
  */
#define GPIO_AS_INPUT(gpio_bits)    gpio_output_conf(0, 0, 0, gpio_bits)

/**
  * @brief   Enable GPIO pin intput,This function only config GPIO32-GPIO39.
  *
  * @param   gpio_bits : The GPIO bit number.
  *
  * @return  null
  */
#define GPIO_AS_INPUT_HIGH(gpio_bits)    gpio_output_conf_high(0, 0, 0, gpio_bits)

/**
  * @brief   Enable GPIO pin output,This function only config GPIO0-GPIO31.
  *
  * @param   gpio_bits : The GPIO bit number.
  *
  * @return  null
  */
#define GPIO_AS_OUTPUT(gpio_bits)   gpio_output_conf(0, 0, gpio_bits, 0)

/**
  * @brief   Enable GPIO pin output,This function only config GPIO32-GPIO39.
  *
  * @param   gpio_bits : The GPIO bit number.
  *
  * @return  null
  */
#define GPIO_AS_OUTPUT_HIGH(gpio_bits)   gpio_output_conf_high(0, 0, gpio_bits, 0)

/**
  * @brief   GPIO init .
  *
  * @param   pGPIOConfig : through this structure initialization GPIO.
  *
  * @return  null
  */
void gpio_config(GPIO_ConfigTypeDef *pGPIOConfig);

/**
  * @brief   Sample the level of GPIO input.
  *
  * @param   gpio_no : The GPIO sequence number.
  *
  * @return  the level of GPIO input
  */
#define GPIO_INPUT_GET(gpio_no)     ((gpio_no < 32) ? \
    ((gpio_input_get() >> gpio_no) & BIT0) : ((gpio_input_get_high() >> (gpio_no - 32)) & BIT0))

/**
  * @brief   Configure GPIO pins out or input.
  *
  * @param   uint32 set_mask     : Set the output for the high bit, the
  *                                corresponding bit is 1, the output of high,
  *                                the corresponding bit is 0, do not change the state.
  * @param   uint32 set_mask     : Set the output for the high bit, the
  *                                corresponding bit is 1, the output of low,
  *                                the corresponding bit is 0, do not change the state.
  * @param   uint32 enable_mask  : Enable Output
  * @param   uint32 disable_mask : Enable Input
  *
  * @return  null
  */
void gpio_output_conf(uint32 set_mask, uint32 clear_mask, uint32 enable_mask, uint32 disable_mask);

/**
  * @brief   Configure GPIO pins out or input.
  *
  * @param   uint32 set_mask     : Set the output for the high bit, the
  *                                corresponding bit is 1, the output of high,
  *                                the corresponding bit is 0, do not change the state.
  * @param   uint32 set_mask     : Set the output for the high bit, the
  *                                corresponding bit is 1, the output of low,
  *                                the corresponding bit is 0, do not change the state.
  * @param   uint32 enable_mask  : Enable Output
  * @param   uint32 disable_mask : Enable Input
  *
  * @return  null
  */
void gpio_output_conf_high(uint32 set_mask, uint32 clear_mask, uint32 enable_mask, uint32 disable_mask);

/**
  * @brief   Register an application-specific interrupt handler for GPIO pin interrupts.
  *
  * @param   void *fn  : interrupt handler for GPIO pin interrupts.
  * @param   void *arg : interrupt handler's arg
  *
  * @return  null
  */
void gpio_intr_handler_register(void *fn, void *arg);

/**
  * @brief   Configure GPIO wake up to light sleep,Only level way is effective.
  *
  * @param   uint32 i : GPIO sequence number
  * @param   GPIO_INT_TYPE intr_state : the level of wake up to light sleep
  *
  * @return  null
  */
void gpio_pin_wakeup_enable(uint32 i, GPIO_INT_TYPE intr_state);

/**
  * @brief   Disable GPIO wake up to light sleep.
  *
  * @param   null
  *
  * @return  null
  */
void gpio_pin_wakeup_disable(void);

/**
  * @brief   Config interrupt types of GPIO pin.
  *
  * @param   uint32 i : The GPIO sequence number.
  * @param   GPIO_INT_TYPE intr_state : GPIO interrupt types.
  *
  * @return  null
  */
void gpio_pin_intr_state_set(uint32 i, GPIO_INT_TYPE intr_state);

/**
  * @brief   Sample the value of GPIO input pins and returns a bitmask. This function only get the level GPIO0-GPIO31.
  *
  * @param   null
  *
  * @return  bitmask of GPIO pins input
  */
uint32 gpio_input_get(void);

/**
  * @brief   Sample the value of GPIO input pins and returns a bitmask. This function only get the level GPIO32-GPIO39.
  *
  * @param   null
  *
  * @return  bitmask of GPIO pins input
  */
uint32 gpio_input_get_high(void);

/**
  * @brief   Enable GPIO sigmadelta function.
  *
  * @param   uint32 gpio_num    : The GPIO sequence number.
  * @param   uint32 sigma_num  : the sigmadelta source sequence number 0-7.
  * @param   uint32 prescale      : Clock divide factor.
  *
  * @return   null
  */
void gpio_output_sigmadelta_enable(uint32 gpio_num, uint32 sigma_num, uint32 prescale);

/**
  * @brief   Disable GPIO sigmadelta function.
  *
  * @param   uint32 gpio_num    : The GPIO sequence number
  *
  * @return  null
  */
void gpio_output_sigmadelta_disable(uint32 gpio_num);

/**
  * @brief   Configure GPIO interrupr.
  *
  * @param   uint32 gpio_num         : The GPIO sequence number.
  * @param   uint32 intr_num          : the interrupt  source sequence number 0-7.
  * @param   GPIO_INT_TYPE intr_type : The type of interrupt.
  *
  * @return  null
  */
void gpio_intr_config(uint32 gpio_num, uint32 intr_num, GPIO_INT_TYPE intr_type);

/**
  * @brief   The GPIO interrupt function.
  *
  * @param   null
  *
  * @return  null
  */
void gpio_intr_process(void);

/**
  * @brief   To bind GPIO input and a certain road input signal.
  *
  * @param   uint32 gpio_num   : The GPIO sequence number.
  * @param   uint32 signal_idx   : input signal sequence number.
  *
  * @return  null
  */
void gpio_matrix_in(uint32 gpio, uint32 signal_idx);

/**
  * @brief   To bind GPIO ouput and a certain road output signal.
  *
  * @param   uint32 gpio_num   : The GPIO sequence number.
  * @param   uint32 signal_idx   : out signal sequence number.
  *
  * @return  null
  */
void gpio_matrix_out(uint32 gpio, uint32 signal_idx);

/**
  * @brief   To bind mode interrupt and interrupt sequence number.
  *
  * @param   uint32 model_num : The mode sequence number.
  * @param   uint32 intr_num     : interrupt sequence number.
  *
  * @return  null
  */
void intr_matrix_set(uint32 model_num, uint32 intr_num);

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
