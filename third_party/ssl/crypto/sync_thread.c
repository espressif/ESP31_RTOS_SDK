/*
 * user_test.c
 *
 * The testing of the crypto and ssl stuff goes here. Keeps the individual code
 * modules from being uncluttered with test code.
 *
 * This is test code - I make no apologies for the quality!
 *
 *  Created on: Oct 20, 2015
 *      Author: liuhan
 */

#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "freertos/semphr.h"

#include "ssl/ssl_os_port.h"
#include "ssl/ssl_crypto.h"

#ifdef SOFTWARE_CRYPTO
#else

void* SSL_CTX_mutex = NULL;
void* SSL_CTX_sem =NULL;

/*
 * Release APP CPU, APP CPU will run
 */
static void app_cpu_release()
{
    SET_PERI_REG_MASK(PRO_APPCPU_CTRL_REG_A, PRODPORT_APPCPU_CLKGATE_EN);
    CLEAR_PERI_REG_MASK(PRO_APPCPU_CTRL_REG_C, PRODPORT_APPCPU_RESETTING);
}

static void gpio_matrix_out(u32 gpio, u32 signal_idx)
{
    u32 addr = GPIO_FUNC_OUT_SEL0 + ((gpio/4)*4);
    u32 shift = (gpio%4)*8;

    SET_PERI_REG_BITS(addr, 0xff, signal_idx, shift);
}

/*
 * Swap uart1 to PIN GPIO2  
 */
static void uart1_swap_gpio2()
{
    gpio_matrix_out(2, U1TXD_OUT_IDX);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2_GPIO2_0);
    SET_PERI_REG_MASK(GPIO_ENABLE, BIT(2));
}

void sys_sync_resquest(void)
{
	SET_PERI_REG_BITS( GPIO_PIN31, GPIO_GPIO_PIN31_INT_TYPE, 4, GPIO_GPIO_PIN31_INT_TYPE_S );
	SET_PERI_REG_BITS( GPIO_PIN31, GPIO_GPIO_PIN31_INT_ENA, 2, GPIO_GPIO_PIN31_INT_ENA_S );
}

void IRAM_ATTR sys_interrupt_response(void *arg)
{
	SET_PERI_REG_BITS( GPIO_PIN30, GPIO_GPIO_PIN30_INT_TYPE, 0, GPIO_GPIO_PIN30_INT_TYPE_S );
	SET_PERI_REG_BITS( GPIO_PIN30, GPIO_GPIO_PIN30_INT_ENA, 0, GPIO_GPIO_PIN30_INT_ENA_S );
	SET_PERI_REG_MASK( GPIO_STATUS_W1TC, (1<<30) );
//	printf("pro_cpu interrupt\n");
//	REG_WRITE(REG_SYNC,SYNC_OPT_SUCCESS);
	SSL_CTX_SYNC_SEM(SSL_CTX_sem);
}

static void sys_interrupt_init(void)
{
    SET_PERI_REG_BITS(PRO_INTR_MAP_REG_C, PRODPORT_INTR_MAP_12, ETS_GPIO_NMI_INUM, PRODPORT_INTR_MAP_12_S);
	 xt_set_interrupt_handler(ETS_GPIO_NMI_INUM, sys_interrupt_response, NULL);
	 xt_ints_on(1 << ETS_GPIO_NMI_INUM); 
}

void sys_sync_opt_thread(uint8 opt)
{
	REG_WRITE(REG_SYNC, opt);
}

bool sys_sync_thread(void)
{
	if (REG_READ(REG_SYNC) == SYNC_OPT_SUCCESS)
		return true;
	else
		return false;
}

void sys_sync_enter_lock(void *arg)
{
	sys_mutex_unlock(&SSL_CTX_mutex);
}

void sys_sync_exit_lock(void *arg)
{
	sys_mutex_lock(&SSL_CTX_mutex);
}

void sys_sync_init(void *arg)
{
	sys_mutex_new(&SSL_CTX_mutex);
}

void sys_sync_sem_init(void *arg)
{
	vSemaphoreCreateBinary(SSL_CTX_sem);
	xSemaphoreTake(SSL_CTX_sem, 10);
}

void IRAM_ATTR sys_sync_sem_signal(void *arg)
{
	signed portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(SSL_CTX_sem, &xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

void sys_sync_sem_wait(void *arg)
{
	signed portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = xSemaphoreTake(SSL_CTX_sem, portMAX_DELAY);
//	ets_printf("%s, %d, %d\n",__FILE__, __LINE__, xHigherPriorityTaskWoken);
}

static void sys_sync_start(void)
{
	ets_printf("%s, %d, %d\n",__FILE__, __LINE__, system_get_free_heap_size());
	sys_interrupt_init();
	SSL_CTX_SYNC_INIT(NULL);
	SSL_CTX_SYNC_SEM_INIT(NULL);
}

void open_app_cpu()
{
    uart1_swap_gpio2();
    app_cpu_release();
	sys_sync_start();
}

#endif
