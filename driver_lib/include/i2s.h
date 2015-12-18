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

#ifndef I2S_H
#define I2S_H

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup Driver_APIs Driver APIs
  * @brief Driver APIs
  */

/** @addtogroup Driver_APIs
  * @{
  */

/** \defgroup I2S_Driver_APIs I2S Driver APIs
  * @brief I2S driver APIs
  */

/** @addtogroup I2S_Driver_APIs
  * @{
  */

//#define GPIO_PIN_REG_4          (PERIPHS_IO_MUX + 0x48)
//#define GPIO_PIN_REG_0          (PERIPHS_IO_MUX + 0x44)

#define TX_MASTER         0
#define TX_SLAVE          1
#define RX_MASTER         2
#define RX_SLAVE          3

struct sdio_queue {
    uint32  blocksize: 12;
    uint32  datalen  : 12;
    uint32  unused   :  5;
    uint32  sub_sof  :  1;
    uint32  eof      :  1;
    uint32  owner    :  1;

    uint32  buf_ptr;
    uint32  next_link_ptr;
};

#define ETS_SLC_INTR_ENABLE()       xt_ints_on(1 << ETS_SLC_INUM)

#define CONF_RXLINK_ADDR(addr)      CLEAR_PERI_REG_MASK(I2SRX_LINK, I2S_I2S_RXLINK_ADDR);\
    SET_PERI_REG_MASK(I2SRX_LINK, ((uint32)(addr)) & I2S_I2S_RXLINK_ADDR)
#define CONF_TXLINK_ADDR(addr)      CLEAR_PERI_REG_MASK(I2STX_LINK, I2S_I2S_TXLINK_ADDR);\
    SET_PERI_REG_MASK(I2STX_LINK, ((uint32)(addr)) & I2S_I2S_TXLINK_ADDR)

#define START_RXLINK()              SET_PERI_REG_MASK(I2SRX_LINK, I2S_I2S_RXLINK_START)
#define START_TXLINK()              SET_PERI_REG_MASK(I2STX_LINK, I2S_I2S_TXLINK_START)

/**
  * @brief     GPIO initialization, including config DATA, WS and BCK GPIO pin.
  *
  * @attention This API can be called only once per mode.
  *
  * @param     uint8 mode : i2s mode select between TX_MASTER, TX_SLAVE, RX_MASTER, RX_SLAVE;
  *
  * @return    null
  */
void i2s_GPIO_init(uint8 mode);

/**
  * @brief     I2S module initialization, including FIFO, M/S mode, data format, clock frequency.
  *
  * @attention This API can be called only once per mode.
  *
  * @param     null
  *
  * @return    null
  */
void i2s_init(void);

/**
  * @brief     DMA module initialization, including DMA mode and interrupt.
  *
  * @attention This API can be called only once per mode.
  *
  * @param     null
  *
  * @return    null
  */
void slc_init(void);

/**
  * @brief     Process data received/treansmitted when interrupt occurs.
  *
  * @attention This API can be called only once per mode.
  *
  * @param     void *para: pointer to parameter
  *
  * @return    null
  */
void slc_isr(void *para);

/**
  * @brief     Create DMA buffer descriptors.
  *
  * @param     uint8 own : select the owner of the current link to be either software or hardware
  * @param     uint8 eof : mark for end of file
  * @param     uint8 sub_sof : mark for sub start of file
  * @param     uint16 size : the actual size of the buffer
  * @param     uint16 length : the total size of the buffer
  * @param     uint32* buf_ptr : the start address of the buffer
  * @param     struct sdio_queue* nxt_ptr : the address of the next descriptor
  * @param     struct sdio_queue* i2s_queue : the address of the current descriptor
  *
  * @return    null
  */
void create_one_link(uint8 own, uint8 eof, uint8 sub_sof, uint16 size, uint16 length,
                    uint32 *buf_ptr, struct sdio_queue *nxt_ptr, struct sdio_queue *i2s_queue);

/**
  * @brief     Functional DEMO for i2s module.
  *
  * @param     null
  *
  * @return    null
  */
void i2s_test(void);

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
