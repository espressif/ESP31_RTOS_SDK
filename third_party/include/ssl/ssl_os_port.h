/*
 * Copyright (c) 2007, Cameron Rich
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, 
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice, 
 *   this list of conditions and the following disclaimer in the documentation 
 *   and/or other materials provided with the distribution.
 * * Neither the name of the axTLS project nor the names of its contributors 
 *   may be used to endorse or promote products derived from this software 
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file os_port.h
 *
 * Some stuff to minimise the differences between windows and linux/unix
 */

#ifndef HEADER_OS_PORT_H
#define HEADER_OS_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_common.h"

#if 0
#define ssl_printf(fmt, args...) os_printf(fmt,## args)
#else
#define ssl_printf(fmt, args...)
#endif

#define STDCALL
#define EXP_FUNC

//struct timeval {
//  long    tv_sec;         /* seconds */
//  long    tv_usec;        /* and microseconds */
//};

#define tls_htons(x) ((uint16)((((x) & 0xff) << 8) | (((x) >> 8) & 0xff)))
#define tls_ntohs(x) tls_htons(x)
#define tls_htonl(_n) ((uint32)( (((_n) & 0xff) << 24) | (((_n) & 0xff00) << 8) | (((_n) >> 8)  & 0xff00) | (((_n) >> 24) & 0xff) ))
#define tls_ntohl(x) tls_htonl(x)

#ifndef be16toh
#define be16toh(x) ((uint16)tls_ntohs((uint16)(x)))
#endif

#ifndef htobe16
#define htobe16(x) ((uint16)tls_htons((uint16)(x)))
#endif

#ifndef be32toh
#define be32toh(x) ((uint32)tls_ntohl((uint32)(x)))
#endif

#ifndef htobe32
#define htobe32(x) ((uint32)tls_htonl((uint32)(x)))
#endif

#ifndef be64toh
static __inline__ uint64 be64toh(uint64 __x);
static __inline__ uint64 be64toh(uint64 __x) {return (((uint64)be32toh(__x & (uint64)0xFFFFFFFFULL)) << 32) | ((uint64)be32toh((__x & (uint64)0xFFFFFFFF00000000ULL) >> 32));}
#define be64toh(x) be64toh(x)
#endif

#ifndef htobe64
#define htobe64(x) be64toh(x)
#endif


void *ax_malloc(size_t s, const char* file, int line);
void *ax_realloc(void *y, size_t s, const char* file, int line);
void *ax_calloc(size_t n, size_t s, const char* file, int line);
void *ax_zalloc(size_t s, const char* file, int line);
void ax_free(void *p, const char* file, int line);

#if 1
#define SSL_MALLOC(size) 		  ax_malloc(size, __FILE__, __LINE__)
#define SSL_REALLOC(mem_ref,size) ax_realloc(mem_ref, size, __FILE__, __LINE__)
#define SSL_CALLOC(element, size) ax_calloc(element, size, __FILE__, __LINE__)
#define SSL_ZALLOC(size) 		  ax_zalloc(size, __FILE__, __LINE__)
#define SSL_FREE(mem_ref) 	 	  ax_free(mem_ref, __FILE__, __LINE__)
#else
#define SSL_MALLOC(size) 		  malloc(size)
#define SSL_REALLOC(mem_ref,size) realloc(mem_ref, size)
#define SSL_CALLOC(element, size) calloc(element, size)
#define SSL_ZALLOC(size) 		  malloc(size)
#define SSL_FREE(mem_ref) 	 	  free(mem_ref)
#endif

#ifdef SOFTWARE_CRYPTO
#else

#define ETS_GPIO_NMI_INUM 22
#define REG_SYNC	0x3fff7F00

enum sync_opt{
	SYNC_START = 0x00,
	SYNC_KEY_SET = 0x01,
	SYNC_KEY_CONVER = 0x02,
	SYNC_DATA_DECRYPT = 0x04,
	SYNC_DATA_ENCRYPT = 0x08,
	SYNC_OPT_SUCCESS = 0x10,
	SYNC_POWER_ENABLE,
	SYNC_POWER_DISABLE,
	SYNC_POWER_SETX,
	SYNC_POWER_SETY,
	SYNC_POWER_SETRB,
	SYNC_POWER_SETM,
	SYNC_POWER_SET_MODE,
	SYNC_POWER_START,
	SYNC_POWER_GETZ,
	SYNC_POWER_MULT,
	SYNC_OPT_ERROR = 0x80,
	SYNC_END
};

void sys_sync_init(void *arg);
void sys_sync_enter_lock(void *arg);
void sys_sync_exit_lock(void *arg);

void sys_sync_sem_init(void *arg);
void sys_sync_sem_signal(void *arg);
void sys_sync_sem_wait(void *arg);

void sys_sync_resquest(void);

void sys_sync_opt_thread(uint8 opt);

bool sys_sync_thread(void);

#define SSL_CTX_SYNC_THREAD(A)		sys_sync_thread(A)
#define SSL_CTX_SYNC_OPT_THREAD(A)	sys_sync_opt_thread(A)
#define SSL_CTX_SYNC_REQUEST(A)		sys_sync_resquest(A)

#define SSL_CTX_SYNC_LOCK(A) 		sys_sync_enter_lock(A)
#define SSL_CTX_SYNC_UNLOCK(A)		sys_sync_exit_lock(A)
#define SSL_CTX_SYNC_INIT(A)		sys_sync_init(A)

#define SSL_CTX_SYNC_SEM(A) 		sys_sync_sem_signal(A)
#define SSL_CTX_SYNC_SEM_WAIT(A)	sys_sync_sem_wait(A)
#define SSL_CTX_SYNC_SEM_INIT(A)	sys_sync_sem_init(A)

#endif

#ifdef __cplusplus
}
#endif

#endif 
