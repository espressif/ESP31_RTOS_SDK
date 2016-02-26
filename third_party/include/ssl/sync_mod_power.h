#ifndef SYNC_MOD_POWER_HEADER
#define SYNC_MOD_POWER_HEADER

#ifdef SOFTWARE_CRYPTO

#else
int32 sync_bi_mode_power_enable(void);

int32 sync_bi_mode_power_disable(void);

int32 sync_bi_mode_power_check_len(int32 len);

void sync_bi_mode_power_write(uint32 * x, int32 len );

void sync_bi_mode_power_read(uint32 * x, int32 len );

int32 sync_bi_mode_power_setx( uint32 * x, int32 len );

int32 sync_bi_mode_power_sety( uint32 * y, int32 len );

int32 sync_bi_mode_power_setm( uint32 * m, uint32 m_dash, int32 len );

int32 sync_bi_mode_power_setrb( uint32 * rb, int32 len );

int32 sync_bi_mode_power_setmode( int32 len );

void sync_bi_mode_power_start(void);

int32 sync_bi_mode_power_getz( uint32 * z, int32 len );

int32 sync_bi_mode_power_calc( uint32 * x, uint32 * y, uint32 * m, uint32 m_dash, uint32 * rb, uint32 * z, int32 len, int32 again );

int32 sync_bi_mode_power_mult( uint32 * x, uint32 * y, uint32 * z, int32 len );

#endif

#endif

