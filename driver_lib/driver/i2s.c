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


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "espressif/esp_common.h"
#include "i2s.h"
#include "gpio.h"
#include <stdio.h>

#define IIS_RX_BUF_LEN  512  //unit Byte
#define IIS_TX_BUF_LEN  512  //unit Byte
#define n  2

#define RX_NUM 128 //unit word
//#define

uint32 i2s_rx_buff1[IIS_RX_BUF_LEN / 4];
uint32 i2s_rx_buff2[IIS_RX_BUF_LEN / 4];
uint32 i2s_tx_buff1[IIS_TX_BUF_LEN / 4];
uint32 i2s_tx_buff2[IIS_TX_BUF_LEN / 4];
uint32 i2s_tx_buff3[IIS_TX_BUF_LEN / 4];

uint32 i2s_tx_test[IIS_TX_BUF_LEN * n];
uint32 i2s_rx_test[IIS_TX_BUF_LEN * n];

//uint32 i2s_tx_test_test[IIS_TX_BUF_LEN*n];

int rx_buff1_cnt = 0;
int rx_buff2_cnt = 0;
int tx_cnt = 0;

uint32 triang_tab1[IIS_RX_BUF_LEN / 2];
uint32 triang_tab2[IIS_RX_BUF_LEN / 2];

int8 tab_idx = 0;

struct sdio_queue i2s_rx_queue1, i2s_rx_queue2, i2s_tx_queue1, i2s_tx_queue2, i2s_tx_queue3;

//create fake audio data
void generate_data(void)
{
    uint16 i, j;
    uint32 val, val1;
    val = 0x100;
    val1 = 0x100;

    //generate data

    for (i = 0; i < 256; i++) {
        triang_tab1[i] = val;
        val += 0x100;
    }

    for (; i < 512; i++) {
        triang_tab2[i - 256] = val;
        val += 0x100;
    }

    for (j = 0; j < IIS_TX_BUF_LEN * n; j++) {
        i2s_rx_test[j] = val1;
        val1 += 0x100;
    }
}

//load data into buffer
void load_buffer1_1(uint32 *buffer, uint32 buff_len)
{
    uint32 i;
    uint32 *pbuff = buffer;

    for (i = 0; i < buff_len; i++) {
        *pbuff = triang_tab1[i];
        pbuff++;
    }
}

void load_buffer1_2(uint32 *buffer, uint32 buff_len)
{
    uint32 i;
    uint32 *pbuff = buffer;

    for (i = 0; i < buff_len; i++) {
        *pbuff = triang_tab1[buff_len + i];
        pbuff++;
    }
}

void load_buffer2_1(uint32 *buffer, uint32 buff_len)
{
    uint32 i;
    uint32 *pbuff = buffer;

    for (i = 0; i < buff_len; i++) {
        *pbuff = triang_tab2[i];
        pbuff++;
    }
}

void load_buffer2_2(uint32 *buffer, uint32 buff_len)
{
    uint32 i;
    uint32 *pbuff = buffer;

    for (i = 0; i < buff_len; i++) {
        *pbuff = triang_tab2[buff_len + i];
        pbuff++;
    }
}

void clear_rx_buff(void)
{
    uint32 i;

    for (i = 0; i < 128; i++) {
        i2s_tx_buff1[i] = 0;
        i2s_tx_buff2[i] = 0;
        i2s_tx_buff3[i] = 0;
    }
}

void copyarray(uint8 *dest, uint8 *src, uint32 nbyte)
{
    int i;

    for (i = 0; i < nbyte; i++) {
        dest[i] = src[i];
    }
}

//create DMA buffer descriptors, unit of either size or length here is byte.
//More details in I2S documents.
void create_one_link(uint8 own, uint8 eof, uint8 sub_sof, uint16 size, uint16 length,
                    uint32 *buf_ptr, struct sdio_queue *nxt_ptr, struct sdio_queue *i2s_queue)
{
    unsigned int link_data0;
    unsigned int link_data1;
    unsigned int link_data2;
    unsigned int start_addr;

    i2s_queue->owner = own;
    i2s_queue->eof = eof;
    i2s_queue->sub_sof = sub_sof;
    i2s_queue->datalen = length;
    i2s_queue->blocksize = size;
    i2s_queue->buf_ptr = (uint32)buf_ptr;
    i2s_queue->next_link_ptr = (uint32)nxt_ptr;
    i2s_queue->unused = 0;
}

void i2s_GPIO_init(uint8 mode)
{
    //GPIO_PIN_PAD_DRIVER
    CLEAR_PERI_REG_MASK(GPIO_PIN16, GPIO_GPIO_PIN19_PAD_DRIVER);
    CLEAR_PERI_REG_MASK(GPIO_PIN17, GPIO_GPIO_PIN20_PAD_DRIVER);
    CLEAR_PERI_REG_MASK(GPIO_PIN18, GPIO_GPIO_PIN21_PAD_DRIVER);
    CLEAR_PERI_REG_MASK(GPIO_PIN19, GPIO_GPIO_PIN19_PAD_DRIVER);
    CLEAR_PERI_REG_MASK(GPIO_PIN20, GPIO_GPIO_PIN20_PAD_DRIVER);
    CLEAR_PERI_REG_MASK(GPIO_PIN21, GPIO_GPIO_PIN21_PAD_DRIVER);

    if (mode == TX_MASTER) {
        //CONFIG I2S TX_master PIN FUNC
        //I2S0_TX module
        //I2S0_DATA_out-->GPIO_OUT_DATA[21]-->io_mux_core/GPIO21
        //I2S0_BCK_out/in-->GPIO_OUT_DATA[20]-->io_mux_core/GPIO20
        //I2S0_WS_out-->GPIO_OUT_DATA[19]-->io_mux_core/GPIO19
        SET_PERI_REG_BITS(GPIO_FUNC_OUT_SEL4, GPIO_GPIO_FUNC19_OUT_SEL, 25, GPIO_GPIO_FUNC19_OUT_SEL_S);
        SET_PERI_REG_BITS(GPIO_FUNC_OUT_SEL5, GPIO_GPIO_FUNC20_OUT_SEL, 24, GPIO_GPIO_FUNC20_OUT_SEL_S);
        SET_PERI_REG_BITS(GPIO_FUNC_OUT_SEL5, GPIO_GPIO_FUNC21_OUT_SEL, 23, GPIO_GPIO_FUNC21_OUT_SEL_S);

        WRITE_PERI_REG(0x60009074, FUN_PU);
        WRITE_PERI_REG(0x60009078, FUN_PU);
        WRITE_PERI_REG(0x6000907c, FUN_PU);

        //GPIO19_mcu_sel=0
        //GPIO20_mcu_sel=0
        //GPIO21_mcu_sel=0
        SET_PERI_REG_BITS(0x60009074, MCU_SEL, 0, MCU_SEL_S);
        SET_PERI_REG_BITS(0x60009078, MCU_SEL, 0, MCU_SEL_S);
        SET_PERI_REG_BITS(0x6000907c, MCU_SEL, 0, MCU_SEL_S);

        //SET_PERI_REG_BITS(0x60009074,FUN_DRV,3,FUN_DRV_S);
        //SET_PERI_REG_BITS(0x60009078,FUN_DRV,3,FUN_DRV_S);
        //SET_PERI_REG_BITS(0x6000907c,FUN_DRV,3,FUN_DRV_S);

        SET_PERI_REG_MASK(GPIO_ENABLE, BIT(19) | BIT(20) | BIT(21));
    } else if (mode == TX_SLAVE) {
        //I2S0_DATA_out-->GPIO_OUT_DATA[21]-->io_mux_core/GPIO21
        SET_PERI_REG_BITS(GPIO_FUNC_OUT_SEL5, GPIO_GPIO_FUNC21_OUT_SEL, 23, GPIO_GPIO_FUNC21_OUT_SEL_S);

        WRITE_PERI_REG(0x60009074, FUN_PU | FUN_IE);
        WRITE_PERI_REG(0x60009078, FUN_PU | FUN_IE);
        WRITE_PERI_REG(0x6000907c, FUN_PU);

        //I2S0_BCK_in
        //I2S0_WS_in
        SET_PERI_REG_BITS(GPIO_FUNC_IN_SEL4, GPIO_GPIO_FUNC24_IN_SEL, 20, GPIO_GPIO_FUNC24_IN_SEL_S);
        SET_PERI_REG_BITS(GPIO_FUNC_IN_SEL5, GPIO_GPIO_FUNC25_IN_SEL, 19, GPIO_GPIO_FUNC25_IN_SEL_S);

        SET_PERI_REG_MASK(GPIO_ENABLE, BIT(21));
        CLEAR_PERI_REG_MASK(GPIO_ENABLE, BIT(19) | BIT(20));
    } else if (mode == RX_MASTER) {
        //I2S1_WS_out
        //I2S1_BCK_out
        SET_PERI_REG_BITS(GPIO_FUNC_OUT_SEL4, GPIO_GPIO_FUNC16_OUT_SEL, 28, GPIO_GPIO_FUNC16_OUT_SEL_S);
        SET_PERI_REG_BITS(GPIO_FUNC_OUT_SEL4, GPIO_GPIO_FUNC17_OUT_SEL, 27, GPIO_GPIO_FUNC17_OUT_SEL_S);

        WRITE_PERI_REG(0x6000904c, FUN_PU);
        WRITE_PERI_REG(0x60009050, FUN_PU);
        WRITE_PERI_REG(0x60009070, FUN_IE | FUN_PU);

        SET_PERI_REG_MASK(GPIO_ENABLE, BIT(16) | BIT(17));
        CLEAR_PERI_REG_MASK(GPIO_ENABLE, BIT(18));

        //I2S1_DATA_in
        //I2S1_WS_in
        SET_PERI_REG_BITS(GPIO_FUNC_IN_SEL5, GPIO_GPIO_FUNC26_IN_SEL, 18, GPIO_GPIO_FUNC26_IN_SEL_S);
        SET_PERI_REG_BITS(GPIO_FUNC_IN_SEL5, GPIO_GPIO_FUNC28_IN_SEL, 16, GPIO_GPIO_FUNC28_IN_SEL_S);
    } else if (mode == RX_SLAVE) {
        WRITE_PERI_REG(0x6000904c, FUN_IE | FUN_PU);
        WRITE_PERI_REG(0x60009050, FUN_IE | FUN_PU);
        WRITE_PERI_REG(0x60009070, FUN_IE | FUN_PU);

        CLEAR_PERI_REG_MASK(GPIO_ENABLE, BIT(16) | BIT(17) | BIT(18));
        //I2S1_DATA_in
        //I2S1_BCK_in
        //I2S1_WS_in
        SET_PERI_REG_BITS(GPIO_FUNC_IN_SEL5, GPIO_GPIO_FUNC26_IN_SEL, 18, GPIO_GPIO_FUNC26_IN_SEL_S);
        SET_PERI_REG_BITS(GPIO_FUNC_IN_SEL5, GPIO_GPIO_FUNC27_IN_SEL, 17, GPIO_GPIO_FUNC27_IN_SEL_S);
        SET_PERI_REG_BITS(GPIO_FUNC_IN_SEL5, GPIO_GPIO_FUNC28_IN_SEL, 16, GPIO_GPIO_FUNC28_IN_SEL_S);
    }

    //I2S0_DATA_in
    //I2S0_BCK_in
    //I2S0_WS_in
    //SET_PERI_REG_BITS(GPIO_FUNC_IN_SEL4,GPIO_GPIO_FUNC23_IN_SEL,21,GPIO_GPIO_FUNC23_IN_SEL_S);
    //SET_PERI_REG_BITS(GPIO_FUNC_IN_SEL4,GPIO_GPIO_FUNC24_IN_SEL,20,GPIO_GPIO_FUNC24_IN_SEL_S);
    //SET_PERI_REG_BITS(GPIO_FUNC_IN_SEL5,GPIO_GPIO_FUNC25_IN_SEL,19,GPIO_GPIO_FUNC25_IN_SEL_S);

    //CONFIG I2S RX PIN FUNC
    //I2S1 module
    //I2S1_DATA_in-->GPIO_OUT_DATA[18]-->io_mux_core/GPIO18
    //I2S1_BCK_out/in-->GPIO_OUT_DATA[17]-->io_mux_core/GPIO17
    //I2S1_WS_in-->GPIO_OUT_DATA[16]-->io_mux_core/GPIO16
    //SET_PERI_REG_BITS(GPIO_FUNC_OUT_SEL4,GPIO_GPIO_FUNC16_OUT_SEL,28,GPIO_GPIO_FUNC16_OUT_SEL_S);
    //SET_PERI_REG_BITS(GPIO_FUNC_OUT_SEL4,GPIO_GPIO_FUNC17_OUT_SEL,27,GPIO_GPIO_FUNC17_OUT_SEL_S);
    //SET_PERI_REG_BITS(GPIO_FUNC_OUT_SEL4,GPIO_GPIO_FUNC18_OUT_SEL,26,GPIO_GPIO_FUNC18_OUT_SEL_S);

    //GPIO_PIN_PAD_DRIVER
    //CLEAR_PERI_REG_MASK(GPIO_PIN16, GPIO_GPIO_PIN19_PAD_DRIVER);
    //CLEAR_PERI_REG_MASK(GPIO_PIN17, GPIO_GPIO_PIN20_PAD_DRIVER);
    //CLEAR_PERI_REG_MASK(GPIO_PIN18, GPIO_GPIO_PIN21_PAD_DRIVER);
    //WRITE_PERI_REG(GPIO_PIN19,0x4);
    //WRITE_PERI_REG(GPIO_PIN20,0x4);

    //GPIO16_mcu_sel=0
    //GPIO17_mcu_sel=0
    //GPIO18_mcu_sel=0
    //SET_PERI_REG_BITS(0x6000904c,MCU_SEL,0,MCU_SEL_S);
    //SET_PERI_REG_BITS(0x60009050,MCU_SEL,0,MCU_SEL_S);
    //SET_PERI_REG_BITS(0x60009070,MCU_SEL,0,MCU_SEL_S);
}

//Initialize the I2S module
//More Registor details in I2S documents.
void i2s_init(void)
{
    //reset I2S
    CLEAR_PERI_REG_MASK(I2SCONF, I2S_I2S_RESET_MASK);
    SET_PERI_REG_MASK(I2SCONF, I2S_I2S_RESET_MASK);
    CLEAR_PERI_REG_MASK(I2SCONF, I2S_I2S_RESET_MASK);

    //Enable FIFO in i2s module
    SET_PERI_REG_MASK(I2S_FIFO_CONF, I2S_I2S_DSCR_EN);

    //set I2S_FIFO
    //set rx,tx data size, both are "24-bit full data discountinue" here
    SET_PERI_REG_BITS(I2S_FIFO_CONF, I2S_I2S_RX_FIFO_MOD, 2, I2S_I2S_RX_FIFO_MOD_S);
    SET_PERI_REG_BITS(I2S_FIFO_CONF, I2S_I2S_TX_FIFO_MOD, 2, I2S_I2S_TX_FIFO_MOD_S);

    //set I2S_CHAN
    //set rx,tx channel mode, both are "two channel" here

    SET_PERI_REG_BITS(I2S_FIFO_CONF, I2S_TX_CHAN_MOD, 0, I2S_TX_CHAN_MOD_S);
    SET_PERI_REG_BITS(I2S_FIFO_CONF, I2S_RX_CHAN_MOD, 0, I2S_RX_CHAN_MOD_S);

    //set RX eof num
    WRITE_PERI_REG(I2SRXEOF_NUM, RX_NUM);

    //trans master&rece slave mode,
    //MSB_shift,right_first,MSB_right,
    //use I2S clock divider to produce a 32KHz Sample Rate
    CLEAR_PERI_REG_MASK(I2SCONF, I2S_TRANS_SLAVE_MOD |
                        (I2S_BITS_MOD << I2S_BITS_MOD_S) |
                        (I2S_BCK_DIV_NUM << I2S_BCK_DIV_NUM_S) |
                        (I2S_CLKM_DIV_NUM << I2S_CLKM_DIV_NUM_S));

    SET_PERI_REG_MASK(I2SCONF, I2S_RIGHT_FIRST | I2S_MSB_RIGHT | I2S_RECE_SLAVE_MOD |
                      I2S_RECE_MSB_SHIFT | I2S_TRANS_MSB_SHIFT |
                      ((26 & I2S_BCK_DIV_NUM) << I2S_BCK_DIV_NUM_S) |
                      ((4 & I2S_CLKM_DIV_NUM) << I2S_CLKM_DIV_NUM_S) |
                      (8 << I2S_BITS_MOD_S));

    /*
     //trans slave&rece master mode,
     //MSB_shift,right_first,MSB_right,
     //use I2S clock divider to produce a 32KHz Sample Rate
    CLEAR_PERI_REG_MASK(I2SCONF, I2S_RECE_SLAVE_MOD|
    					(I2S_BITS_MOD<<I2S_BITS_MOD_S)|
    					(I2S_BCK_DIV_NUM <<I2S_BCK_DIV_NUM_S)|
                                     	(I2S_CLKM_DIV_NUM<<I2S_CLKM_DIV_NUM_S));

    SET_PERI_REG_MASK(I2SCONF, I2S_RIGHT_FIRST|I2S_MSB_RIGHT|I2S_TRANS_SLAVE_MOD|
    					I2S_RECE_MSB_SHIFT|I2S_TRANS_MSB_SHIFT|
    					(( 26&I2S_BCK_DIV_NUM )<<I2S_BCK_DIV_NUM_S)|
    					((4&I2S_CLKM_DIV_NUM)<<I2S_CLKM_DIV_NUM_S)|
    					(8<<I2S_BITS_MOD_S));

    */

    CLEAR_PERI_REG_MASK(I2SCONF1, I2S_BITS_MOD_MSB | I2S_I2S_RECE_SHORT_SYNC | I2S_I2S_TRAN_SHORT_SYNC);
    SET_PERI_REG_MASK(I2SCONF1, I2S_BITS_MOD_MSB);

    /*
    //clear int
    SET_PERI_REG_MASK(I2SINT_CLR,   I2S_I2S_TX_REMPTY_INT_CLR|I2S_I2S_TX_WFULL_INT_CLR|I2S_I2S_RX_REMPTY_INT_CLR|
    I2S_I2S_RX_WFULL_INT_CLR|I2S_I2S_PUT_DATA_INT_CLR|I2S_I2S_TAKE_DATA_INT_CLR);
    CLEAR_PERI_REG_MASK(I2SINT_CLR,   I2S_I2S_TX_REMPTY_INT_CLR|I2S_I2S_TX_WFULL_INT_CLR|I2S_I2S_RX_REMPTY_INT_CLR|
    I2S_I2S_RX_WFULL_INT_CLR|I2S_I2S_PUT_DATA_INT_CLR|I2S_I2S_TAKE_DATA_INT_CLR);

    //enable int
    SET_PERI_REG_MASK(I2SINT_ENA,   I2S_I2S_TX_REMPTY_INT_ENA|I2S_I2S_TX_WFULL_INT_ENA|I2S_I2S_RX_REMPTY_INT_ENA|
    I2S_I2S_RX_WFULL_INT_ENA|I2S_I2S_TX_PUT_DATA_INT_ENA|I2S_I2S_RX_TAKE_DATA_INT_ENA);
    */

    WRITE_PERI_REG(I2SINT_CLR, 0xffffffff);
    //Start transmitter and receiver
    SET_PERI_REG_MASK(I2SCONF, I2S_I2S_TX_START | I2S_I2S_RX_START);
}

//Initialize the SLC module for DMA function
void slc_init(void)
{
    //Reset DMA
    SET_PERI_REG_MASK(I2S_LC_CONF, I2S_I2S_RX_RST | I2S_I2S_TX_RST_S);
    CLEAR_PERI_REG_MASK(I2S_LC_CONF, I2S_I2S_RX_RST | I2S_I2S_TX_RST_S);

    //Enable and configure DMA
    SET_PERI_REG_MASK(I2S_LC_CONF, I2S_I2S_CHECK_OWNER | I2S_I2S_TO_EOF_MODE |
                      I2S_I2S_RX_LOOP_TEST | I2S_I2S_TX_LOOP_TEST);
    //SET_PERI_REG_MASK(SLC_RX_DSCR_CONF,SLC_INFOR_NO_REPLACE|SLC_TOKEN_NO_REPLACE);//|0xfe
    //CLEAR_PERI_REG_MASK(SLC_RX_DSCR_CONF, SLC_RX_FILL_EN|SLC_RX_EOF_MODE | SLC_RX_FILL_MODE);

    //os_printf("INTR_MAP_REG_B:%08x\n\r",READ_PERI_REG(INTR_MAP_REG_B));

    SET_PERI_REG_BITS(INTR_MAP_REG_B, PRODPORT_INTR_MAP_9, 1, PRODPORT_INTR_MAP_9_S);

    //os_printf("INTR_MAP_REG_B:%08x\n\r",READ_PERI_REG(INTR_MAP_REG_B));

    intr_matrix_set(ETS_SLC_SOURCE, ETS_SLC_INUM);
    xt_set_interrupt_handler(ETS_SLC_INUM, slc_isr, NULL);

    /////enable sdio operation intr
    WRITE_PERI_REG(I2SINT_ENA, I2S_I2S_TO_EOF_INT_ENA | I2S_I2S_FROM_SUC_EOF_INT_ENA);
    /////clear sdio initial random active intr signal
    WRITE_PERI_REG(I2SINT_CLR, 0xffffffff);
    /////enable sdio intr in cpu
    ETS_SLC_INTR_ENABLE();
}

//interrupt
//write new buff data into RX_DMA at the transmitter side
//or load buff data out from TX_DMA at the receiver side
void slc_isr(void *para)
{
    uint32 slc_intr_status;

    slc_intr_status = READ_PERI_REG(I2SINT_ST);

    if (slc_intr_status == 0) {
        //No interested interrupts pending
        return;
    }

    //clear all intrs
    WRITE_PERI_REG(I2SINT_CLR, 0xffffffff);

    //process every intr

    //Transimitter side
    if (slc_intr_status & I2S_I2S_TO_EOF_INT_ST) {
        //find the DMA which sends the interrupt signal
        if (READ_PERI_REG(I2S_TO_EOF_DES_ADDR) == (((uint32)&i2s_rx_queue1))) {
            //replace data in the buff
            if ((rx_buff1_cnt % 2) == 0) {
                load_buffer2_1(i2s_rx_buff1, IIS_RX_BUF_LEN / 4);
            } else if ((rx_buff1_cnt % 2) == 1) {
                load_buffer1_1(i2s_rx_buff1, IIS_RX_BUF_LEN / 4);
            }

            rx_buff1_cnt++;
        } else if (READ_PERI_REG(I2S_TO_EOF_DES_ADDR) == (((uint32)&i2s_rx_queue2))) {
            if ((rx_buff2_cnt % 2) == 0) {
                load_buffer2_2(i2s_rx_buff2, IIS_RX_BUF_LEN / 4);
            } else if ((rx_buff2_cnt % 2) == 1) {
                load_buffer1_2(i2s_rx_buff2, IIS_RX_BUF_LEN / 4);
            }

            rx_buff2_cnt++;
        }
    }

    //Receiver side
    if (slc_intr_status & I2S_I2S_FROM_SUC_EOF_INT_ST) {
        //find the DMA which sends the interrupt signal
        if (READ_PERI_REG(I2S_TX_EOF_DES_ADDR) == (((uint32)&i2s_tx_queue1))) {
            //load out data in the buff
            if (tx_cnt < 4 * n) {
                memcpy((uint8 *)i2s_tx_test + IIS_TX_BUF_LEN * tx_cnt, (uint8 *)i2s_tx_buff1, IIS_TX_BUF_LEN);
            }

            //reset DMA discrpiter
            i2s_tx_queue1.next_link_ptr = (uint32)(&i2s_tx_queue2);
            i2s_tx_queue1.eof = 0;
            i2s_tx_queue1.owner = 1;
            i2s_tx_queue1.datalen = 0;
        } else if (READ_PERI_REG(I2S_TX_EOF_DES_ADDR) == (((uint32)&i2s_tx_queue2))) {
            if (tx_cnt < 4 * n) {
                memcpy((uint8 *)i2s_tx_test + IIS_TX_BUF_LEN * tx_cnt, (uint8 *)i2s_tx_buff2, IIS_TX_BUF_LEN);
            }

            i2s_tx_queue2.next_link_ptr = (uint32)(&i2s_tx_queue3);
            i2s_tx_queue2.eof = 0;
            i2s_tx_queue2.owner = 1;
            i2s_tx_queue2.datalen = 0;
        } else if (READ_PERI_REG(I2S_TX_EOF_DES_ADDR) == (((uint32)&i2s_tx_queue3))) {
            if (tx_cnt < 4 * n) {
                memcpy((uint8 *)i2s_tx_test + IIS_TX_BUF_LEN * tx_cnt, (uint8 *)i2s_tx_buff3, IIS_TX_BUF_LEN);
            }

            i2s_tx_queue3.next_link_ptr = (uint32)(&i2s_tx_queue1);
            i2s_tx_queue3.eof = 0;
            i2s_tx_queue3.owner = 1;
            i2s_tx_queue3.datalen = 0;
        }

        tx_cnt++;
    }
}

void i2s_test(void)
{
    int i;
    rx_buff1_cnt = 0;
    rx_buff2_cnt = 0;
    tx_cnt = 0;
    /*
    	os_printf("rx_buff1_cnt is : %x	\n\r",(rx_buff1_cnt));
    	SET_PERI_REG_BITS(GPIO_PIN_REG_4,MCU_SEL,0,MCU_SEL_S);
    	SET_PERI_REG_MASK(GPIO_ENABLE,BIT(4));
    	SET_PERI_REG_BITS(GPIO_PIN_REG_0,MCU_SEL,0,MCU_SEL_S);
    	SET_PERI_REG_MASK(GPIO_ENABLE,BIT(0));
    	SET_PERI_REG_MASK(GPIO_OUT,BIT(4));
    	os_delay_us(2000);
    	CLEAR_PERI_REG_MASK(GPIO_OUT,BIT(4));
    */
    generate_data();
    load_buffer1_1(i2s_rx_buff1, IIS_RX_BUF_LEN / 4);
    load_buffer1_2(i2s_rx_buff2, IIS_RX_BUF_LEN / 4);
    clear_rx_buff();

    slc_init();

    os_printf(" %08x	\n\r", i2s_tx_buff1[0]);
    os_printf(" %08x	\n\r", i2s_tx_buff1[1]);
    os_printf(" %08x	\n\r", i2s_tx_buff1[2]);
    os_printf(" %08x	\n\r", i2s_tx_buff1[3]);
    os_printf(" %08x	\n\r", i2s_tx_buff1[4]);

    //To receive data from the I2S module, counter-intuitively we use the TXLINK part, not the RXLINK part.
    //Vice versa.
    //Note:At the transimitter side,the size of the DMAs can not be smaller than 128*4 bytes which are the
    //size of the I2S FIFO.
    //Note:At the receiver side,the number of the DMAs can not be smaller than 3 which is limited by the
    //hardware.
    create_one_link(1, 0, 0, IIS_TX_BUF_LEN, 0, i2s_tx_buff1, &i2s_tx_queue2, &i2s_tx_queue1);
    create_one_link(1, 0, 0, IIS_TX_BUF_LEN, 0, i2s_tx_buff2, &i2s_tx_queue3, &i2s_tx_queue2);
    create_one_link(1, 0, 0, IIS_TX_BUF_LEN, 0, i2s_tx_buff3, &i2s_tx_queue1, &i2s_tx_queue3);

    create_one_link(1, 1, 0, IIS_RX_BUF_LEN, IIS_RX_BUF_LEN, i2s_rx_buff1, &i2s_rx_queue2, &i2s_rx_queue1);
    create_one_link(1, 1, 0, IIS_RX_BUF_LEN, IIS_RX_BUF_LEN, i2s_rx_buff2, &i2s_rx_queue1, &i2s_rx_queue2);

    os_printf("================DMA descripter built==============\r\n");

    os_printf("==========debug RX descripter for I2S transmitter========\r\n");
    os_printf("i2s_rx_queue1_address: %08x\r\n", &i2s_rx_queue1);
    os_printf("i2s_rx_queue1_word0:%08x\n\r", READ_PERI_REG(&i2s_rx_queue1));
    os_printf("i2s_rx_queue1_buf_ptr: %08x\r\n", i2s_rx_queue1.buf_ptr);
    os_printf("i2s_rx_queue1_next_link_ptr: %08x\r\n", i2s_rx_queue1.next_link_ptr);
    os_printf("\r\n");
    os_printf("i2s_rx_queue2: %08x\r\n", &i2s_rx_queue2);
    os_printf("i2s_rx_queue2_word0:%08x\n\r", READ_PERI_REG(&i2s_rx_queue2));
    os_printf("i2s_rx_queue2_buf_ptr: %08x\r\n", i2s_rx_queue2.buf_ptr);
    os_printf("i2s_rx_queue2_next_link_ptr: %08x\r\n", i2s_rx_queue2.next_link_ptr);
    os_printf("--------------------------------------------------\r\n");

    os_printf("==========debug TX descripter for I2S receiver===========\r\n");
    os_printf("i2s_tx_queue1_address: %08x\r\n", &i2s_tx_queue1);
    os_printf("i2s_tx_queue1_word0:%08x\n\r", READ_PERI_REG(&i2s_tx_queue1));
    os_printf("i2s_tx_queue1_buf_ptr: %08x\r\n", i2s_tx_queue1.buf_ptr);
    os_printf("i2s_tx_queue1_next_link_ptr: %08x\r\n", i2s_tx_queue1.next_link_ptr);
    os_printf("\r\n");
    os_printf("i2s_tx_queue2_address: %08x\r\n", &i2s_tx_queue2);
    os_printf("i2s_tx_queue2_word0:%08x\n\r", READ_PERI_REG(&i2s_tx_queue2));
    os_printf("i2s_tx_queue2_buf_ptr: %08x\r\n", i2s_tx_queue2.buf_ptr);
    os_printf("i2s_tx_queue2_next_link_ptr: %08x\r\n", i2s_tx_queue2.next_link_ptr);
    os_printf("\r\n");
    os_printf("i2s_tx_queue3_address: %08x\r\n", &i2s_tx_queue3);
    os_printf("i2s_tx_queue3_word0:%08x\n\r", READ_PERI_REG(&i2s_tx_queue3));
    os_printf("i2s_tx_queue3_buf_ptr: %08x\r\n", i2s_tx_queue3.buf_ptr);
    os_printf("i2s_tx_queue3_next_link_ptr: %08x\r\n", i2s_tx_queue3.next_link_ptr);
    os_printf("--------------------------------------------------\r\n");

    //config rx&tx link to hardware
    CONF_RXLINK_ADDR(&i2s_rx_queue1);
    CONF_TXLINK_ADDR(&i2s_tx_queue1);

    os_printf("=============Descripter linked=================\r\n");

    os_printf("SLC_RX_LINK:%08x\n\r", READ_PERI_REG(I2SRX_LINK));
    os_printf("SLC_TX_LINK:%08x\n\r", READ_PERI_REG(I2STX_LINK));
    os_printf("--------------------------------------------------\r\n");
    //os_printf("SLC_RXLINK_DESCADDR_MASK:%08x\n\r",READ_PERI_REG(SLC_RX_LINK)&0x000FFFFF);

    //config rx control, start
    START_RXLINK();
    START_TXLINK();

    os_printf("=================set SLC start=================\r\n");
    os_printf("SLC_RX_LINK:%08x\n\r", READ_PERI_REG(I2SRX_LINK));
    os_printf("SLC_TX_LINK:%08x\n\r", READ_PERI_REG(I2STX_LINK));
    os_printf("--------------------------------------------------\r\n");

    i2s_GPIO_init(TX_MASTER);
    i2s_GPIO_init(RX_SLAVE);

    //i2s_GPIO_init(TX_SLAVE);
    //i2s_GPIO_init(RX_MASTER);

    os_printf("=================DEBUG GPIO_PAD=================\r\n");
    os_printf("GPIO_FUNC_IN_SEL4:%08x\n\r", READ_PERI_REG(GPIO_FUNC_IN_SEL4));
    os_printf("GPIO_FUNC_IN_SEL5:%08x\n\r", READ_PERI_REG(GPIO_FUNC_IN_SEL5));
    os_printf("GPIO16:%08x\n\r", READ_PERI_REG(0x6000904c));
    os_printf("GPIO17:%08x\n\r", READ_PERI_REG(0x60009050));
    os_printf("GPIO18:%08x\n\r", READ_PERI_REG(0x60009070));
    os_printf("GPIO19:%08x\n\r", READ_PERI_REG(0x60009074));
    os_printf("GPIO20:%08x\n\r", READ_PERI_REG(0x60009078));
    os_printf("GPIO21:%08x\n\r", READ_PERI_REG(0x6000907c));
    os_printf("GPIO_OUT:%08x\n\r", READ_PERI_REG(GPIO_ENABLE));
    os_printf("--------------------------------------------------\r\n");

    i2s_init();	//start


    os_printf("================I2S Registor State================\r\n");
    os_printf("I2SCONF:%08x\n\r", READ_PERI_REG(I2SCONF));
    os_printf("I2SCONF1:%08x\n\r", READ_PERI_REG(I2SCONF1));
    os_printf("I2S_FIFO_CONF:%08x\n\r", READ_PERI_REG(I2S_FIFO_CONF));
    os_printf("I2SCONF_CHAN:%08x\n\r", READ_PERI_REG(I2SCONF_CHAN));
    os_printf("I2SRXEOF_NUM:%08x\n\r", READ_PERI_REG(I2SRXEOF_NUM));
    os_printf("I2SCONF1:%08x\n\r", READ_PERI_REG(I2S_LC_CONF));
    os_printf("--------------------------------------------------\r\n");

    os_printf("\n\r");
    os_printf("\n\r");

    //As mentioned about the use of TXLINK & RXLINK part above,
    //rx_buff contains data sented and tx_buff contains data reveived.
    //Delay exists at the receiver side, and under this test condition the delay is 2 words.
    //However, delay may change depends on different I2S mode(like master_slave, data length, FIFO size etc).
    for (i = 0; i < IIS_TX_BUF_LEN * n; i += 32) {
        os_printf("the %dth number of data received: %08x   \n\r", i + 1, i2s_tx_test[i]);
    }

    os_printf("--------------------------------------------------\r\n");

    os_printf("the %dth number of data received: %08x	 \n\r", 1, i2s_tx_test[0]);
    os_printf("the %dth number of data received: %08x   \n\r", 2, i2s_tx_test[1]);
    os_printf("the %dth number of data received: %08x   \n\r", 3, i2s_tx_test[2]);
    os_printf("the %dth number of data received: %08x   \n\r", 4, i2s_tx_test[3]);
    os_printf("the %dth number of data received: %08x   \n\r", 5, i2s_tx_test[4]);

    os_printf("--------------------------------------------------\r\n");

    os_printf("the %dth number of data received: %08x   \n\r", 512, i2s_tx_test[511]);
    os_printf("the %dth number of data received: %08x   \n\r", 513, i2s_tx_test[512]);
    os_printf("the %dth number of data received: %08x   \n\r", 514, i2s_tx_test[513]);
    os_printf("the %dth number of data received: %08x   \n\r", 515, i2s_tx_test[514]);
    os_printf("the %dth number of data received: %08x   \n\r", 516, i2s_tx_test[515]);


    for (i = 0; i < 512 - 2; i += 1) {
        if (!(i2s_tx_test[i + 2] == i2s_rx_test[i])) {
            //printf("test failed");

            os_printf("the %dth data unmatch: %08x	 %08x \n\r", i + 1, i2s_rx_test[i], i2s_tx_test[i + 2]);
        }
    }
}
