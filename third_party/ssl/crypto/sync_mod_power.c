#include "esp_common.h"
#include "ssl/ssl_os_port.h"

extern uint32 *share_buffer;

#ifdef SOFTWARE_CRYPTO

#else
//#define SYNC_MOD_LOG

#ifdef SYNC_MOD_LOG
#define SYNC_MOD_MSG ets_printf
#else
#define SYNC_MOD_MSG
#endif

void sync_bi_to_hex(char* src, uint32 len)
{
	uint32 i = 0;
	for (i = 0; i < len; i ++){
		SYNC_MOD_MSG("%x ", src[i]);
		if ((i + 1) % 16 == 0)
			SYNC_MOD_MSG("\n");
	}
	SYNC_MOD_MSG("\n");	
}

int32 sync_bi_mode_power_enable(void)
{
   SSL_CTX_SYNC_LOCK(NULL);
  
   SSL_CTX_SYNC_OPT_THREAD(SYNC_POWER_ENABLE);
   SSL_CTX_SYNC_REQUEST();
//   for(;;){
//		if (SSL_CTX_SYNC_THREAD()){
//			break;
//		}
//   }
   SSL_CTX_SYNC_SEM_WAIT(NULL);
   SSL_CTX_SYNC_UNLOCK(NULL);
   return 0;
}

int32 sync_bi_mode_power_disable(void)
{
   SSL_CTX_SYNC_LOCK(NULL);
   
   SSL_CTX_SYNC_OPT_THREAD(SYNC_POWER_DISABLE);
   SSL_CTX_SYNC_REQUEST();
//   for(;;){
//		if (SSL_CTX_SYNC_THREAD()){
//			break;
//		}
//   }
   SSL_CTX_SYNC_SEM_WAIT(NULL);
   SSL_CTX_SYNC_UNLOCK(NULL);
   return 0;
}

int32 sync_bi_mode_power_check_len(int32 len)
{
	if( len!=512 && len!=1024 && len!=1536 && len!=2048 && len!=2560 && len!=3072 && len!=3584 && len!=4096 ) {
		return 1;
	} else {
		return 0;
	}
}

void sync_bi_mode_power_write(uint32 * x, int32 len )
{
	uint32* sync_buffer = share_buffer;
	*sync_buffer++ = len;
	if (len != 0)
		memcpy(sync_buffer, x, len);
	sync_buffer += len;
	sync_bi_to_hex((char*)x, len);
}

void sync_bi_mode_power_read(uint32 * x, int32 len )
{
	uint32* sync_buffer = share_buffer;
	len = *sync_buffer++;
	if (len != 0)
		memcpy(x, sync_buffer, len);
	sync_buffer += len;
	SYNC_MOD_MSG("%s, %d, len[%x]\n",__FILE__, __LINE__, len);
	sync_bi_to_hex((char*)x, len);
}

int32 sync_bi_mode_power_setx( uint32 * x, int32 len )
{  
   SSL_CTX_SYNC_LOCK(NULL);
   SYNC_MOD_MSG("%s, %d, len[%x]\n",__FILE__, __LINE__, len);
   sync_bi_mode_power_write(x, len);
   SSL_CTX_SYNC_OPT_THREAD(SYNC_POWER_SETX);
   SSL_CTX_SYNC_REQUEST();
//   for(;;){ 
//   	
//		if (SSL_CTX_SYNC_THREAD()){
//			SYNC_MOD_MSG("%s, %d\n",__FILE__, __LINE__);
//			sync_bi_mode_power_read(x, len);
//			break;
//		}
//   }
   SSL_CTX_SYNC_SEM_WAIT(NULL);
   sync_bi_mode_power_read(x, len);
   SSL_CTX_SYNC_UNLOCK(NULL);
   return 0;	
}

int32 sync_bi_mode_power_sety( uint32 * y, int32 len )
{
   SSL_CTX_SYNC_LOCK(NULL);
   SYNC_MOD_MSG("%s, %d, len[%x]\n",__FILE__, __LINE__, len);
   sync_bi_mode_power_write(y, len);
   SSL_CTX_SYNC_OPT_THREAD(SYNC_POWER_SETY);
   SSL_CTX_SYNC_REQUEST();
//   for(;;){  	
//   	
//		if (SSL_CTX_SYNC_THREAD()){
//			SYNC_MOD_MSG("%s, %d\n",__FILE__, __LINE__);
//			sync_bi_mode_power_read(y, len);
//			break;
//		}
//   }
   SSL_CTX_SYNC_SEM_WAIT(NULL);
   sync_bi_mode_power_read(y, len);
   SSL_CTX_SYNC_UNLOCK(NULL);
   return 0;

	return 0;	
}

void sync_bi_mode_power_setm_write(uint32 * x, uint32 m_dash, int32 len )
{
	uint32* sync_buffer = share_buffer;
	*sync_buffer++ = len;
	if (len != 0)
		memcpy(sync_buffer, x, len);
	sync_buffer += len;

	*sync_buffer++ = m_dash;
	sync_bi_to_hex((char*)x, len);
}

void sync_bi_mode_power_setm_read(uint32 * x, uint32 m_dash, int32 len )
{
	uint32* sync_buffer = share_buffer;
	len = *sync_buffer++;
	if (len != 0)
		memcpy(x, sync_buffer, len);
	sync_buffer += len;

	m_dash = *sync_buffer++;
	SYNC_MOD_MSG("%s, %d, m_dash[%x], len[%x]\n",__FILE__, __LINE__, m_dash, len);
	sync_bi_to_hex((char*)x, len);
}

int32 sync_bi_mode_power_setm( uint32 * m, uint32 m_dash, int32 len )
{   
   SSL_CTX_SYNC_LOCK(NULL);
   SYNC_MOD_MSG("%s, %d, m_dash[%x], len[%x]\n",__FILE__, __LINE__, m_dash, len);
   sync_bi_mode_power_setm_write(m, m_dash, len);
   SSL_CTX_SYNC_OPT_THREAD(SYNC_POWER_SETM);
   SSL_CTX_SYNC_REQUEST();
//   for(;;){
//		if (SSL_CTX_SYNC_THREAD()){
//			SYNC_MOD_MSG("%s, %d\n",__FILE__, __LINE__);
//			sync_bi_mode_power_setm_read(m, m_dash, len);
//			break;
//		}
//   }
   SSL_CTX_SYNC_SEM_WAIT(NULL);
   sync_bi_mode_power_setm_read(m, m_dash, len);
   SSL_CTX_SYNC_UNLOCK(NULL);
   return 0;
}

int32 sync_bi_mode_power_setrb( uint32 * rb, int32 len )
{
   SSL_CTX_SYNC_LOCK(NULL);
   SYNC_MOD_MSG("%s, %d, len[%x]\n",__FILE__, __LINE__, len);
   sync_bi_mode_power_write(rb, len);
   SSL_CTX_SYNC_OPT_THREAD(SYNC_POWER_SETRB);
   SSL_CTX_SYNC_REQUEST();
//   for(;;){ 
//   		
//		if (SSL_CTX_SYNC_THREAD()){
//			SYNC_MOD_MSG("%s, %d\n",__FILE__, __LINE__);
//			sync_bi_mode_power_read(rb, len);
//			break;
//		}
//   }
   SSL_CTX_SYNC_SEM_WAIT(NULL);
   sync_bi_mode_power_read(rb, len);
   SSL_CTX_SYNC_UNLOCK(NULL);
   return 0;
}


int32 sync_bi_mode_power_setmode( int32 len )
{ 
   SSL_CTX_SYNC_LOCK(NULL);
   SYNC_MOD_MSG("%s, %d, len[%x]\n",__FILE__, __LINE__, len);
   uint32* sync_buffer = share_buffer;
   *sync_buffer++ = len;
   SSL_CTX_SYNC_OPT_THREAD(SYNC_POWER_SET_MODE);
   SSL_CTX_SYNC_REQUEST();
//   for(;;){ 		
//		if (SSL_CTX_SYNC_THREAD()){
//			SYNC_MOD_MSG("%s, %d\n",__FILE__, __LINE__);
//			break;
//		}
//   }
   SSL_CTX_SYNC_SEM_WAIT(NULL);
   SSL_CTX_SYNC_UNLOCK(NULL);
   return 0;	
}

void sync_bi_mode_power_start(void)
{   
   SSL_CTX_SYNC_LOCK(NULL);
   SYNC_MOD_MSG("%s, %d\n",__FILE__, __LINE__);
   
   SSL_CTX_SYNC_OPT_THREAD(SYNC_POWER_START);
   SSL_CTX_SYNC_REQUEST();
//   for(;;){
//   	
//		if (SSL_CTX_SYNC_THREAD()){
//			SYNC_MOD_MSG("%s, %d\n",__FILE__, __LINE__);
//			break;
//		}
//   }
   SSL_CTX_SYNC_SEM_WAIT(NULL);
   SSL_CTX_SYNC_UNLOCK(NULL);
}

int32 sync_bi_mode_power_getz( uint32 * z, int32 len )
{   
   SSL_CTX_SYNC_LOCK(NULL);
   SYNC_MOD_MSG("%s, %d, len[%x]\n",__FILE__, __LINE__, len);
   sync_bi_mode_power_write(z, len);
   SSL_CTX_SYNC_OPT_THREAD(SYNC_POWER_GETZ);
   SSL_CTX_SYNC_REQUEST();
//   for(;;){ 	
//   	
//		if (SSL_CTX_SYNC_THREAD()){
//			SYNC_MOD_MSG("%s, %d\n",__FILE__, __LINE__);
//			sync_bi_mode_power_read(z, len);
//			break;
//		}
//   }
   SSL_CTX_SYNC_SEM_WAIT(NULL);
   sync_bi_mode_power_read(z, len);
   SSL_CTX_SYNC_UNLOCK(NULL);
   return 0;
}


int32 sync_bi_mode_power_calc( uint32 * x, uint32 * y, uint32 * m, uint32 m_dash, uint32 * rb, uint32 * z, int32 len, int32 again )
{
	if( sync_bi_mode_power_check_len( len ) ) {
		return 1;
	}

	if( again == 0 ) {
		sync_bi_mode_power_setm( m, m_dash, len );
		sync_bi_mode_power_sety( y, len );
		sync_bi_mode_power_setmode( len );
	}
	sync_bi_mode_power_setx( x, len );
	sync_bi_mode_power_setrb( rb, len );

	sync_bi_mode_power_start();

	sync_bi_mode_power_getz( z, len );

	return 0;
}

void sync_bi_mode_power_mult_write(uint32 * x, uint32 * y, uint32 * z, int32 len )
{
	uint32* sync_buffer = share_buffer;
	uint32 mult_len = 0;
	mult_len = len;
	*sync_buffer++ = mult_len;
	if (mult_len != 0)
		memcpy(sync_buffer, x, mult_len);
	sync_buffer += mult_len;

	mult_len = len;
	*sync_buffer++ = mult_len;
	if (mult_len != 0)
		memcpy(sync_buffer, y, mult_len);
	sync_buffer += mult_len;

	mult_len = len;
	*sync_buffer++ = mult_len;
	if (mult_len != 0)
		memcpy(sync_buffer, z, mult_len);
	sync_buffer += mult_len;

	*sync_buffer++ = len;
}

void sync_bi_mode_power_mult_read(uint32 * x, uint32 * y, uint32 * z, int32 len)
{
	uint32* sync_buffer = share_buffer;
	uint32 mult_len = 0;
	mult_len = *sync_buffer++;
	if (mult_len != 0)
		memcpy(x, sync_buffer, mult_len);
	sync_buffer += mult_len;

	mult_len = *sync_buffer++;
	if (mult_len != 0)
		memcpy(y, sync_buffer, mult_len);
	sync_buffer += mult_len;

	mult_len = *sync_buffer++;
	if (mult_len != 0)
		memcpy(z, sync_buffer, mult_len);
	sync_buffer += mult_len;

	len = *sync_buffer++;
}

int32 sync_bi_mode_power_mult( uint32 * x, uint32 * y, uint32 * z, int32 len )
{
   SSL_CTX_SYNC_LOCK(NULL);
   SYNC_MOD_MSG("%s, %d, len[%x]\n",__FILE__, __LINE__, len);
   sync_bi_mode_power_mult_write(x, y, z, len);
   SSL_CTX_SYNC_OPT_THREAD(SYNC_POWER_MULT);
   SSL_CTX_SYNC_REQUEST();
//   for(;;){ 
//   	
//		if (SSL_CTX_SYNC_THREAD()){
//			SYNC_MOD_MSG("%s, %d\n",__FILE__, __LINE__);
//			sync_bi_mode_power_mult_read(x, y, z, len);
//			break;
//		}
//   }
   SSL_CTX_SYNC_SEM_WAIT(NULL);
   sync_bi_mode_power_mult_read(x, y, z, len);
   SSL_CTX_SYNC_UNLOCK(NULL);
   return 0;
}
#endif

