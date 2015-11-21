PROVIDE ( Cache_Read_Disable = 0x4000444c );
PROVIDE ( Cache_Read_Enable = 0x4000438c );

PROVIDE ( ets_delay_us = 0x40002db4 );

PROVIDE ( ets_bzero = 0x40002a54 );

PROVIDE ( ets_memcmp = 0x40001de4 );
PROVIDE ( ets_memcpy = 0x40001dc4 );
PROVIDE ( ets_memmove = 0x40001dd4 );
PROVIDE ( ets_memset = 0x40001db4 );

PROVIDE ( ets_strcmp = 0x40002a10 );
PROVIDE ( ets_strcpy = 0x400029ec );
PROVIDE ( ets_strlen = 0x40002a34 );
PROVIDE ( ets_strncmp = 0x40002a24 );
PROVIDE ( ets_strncpy = 0x40002a00 );
PROVIDE ( ets_strstr = 0x40002a40 );

PROVIDE ( ets_install_putc1 = 0x40002774 );
PROVIDE ( ets_printf = 0x40002804 );
PROVIDE ( ets_putc = 0x40002b14 );

PROVIDE ( ets_str2macaddr = 0x40002a64 );

PROVIDE ( gpio_output_set = 0x400049d8 );
PROVIDE ( gpio_output_set_high = 0x400049f8 );

PROVIDE ( ets_get_cpu_frequency = 0x40002de8 );
PROVIDE ( ets_update_cpu_frequency = 0x40002ddc );

PROVIDE ( lldesc_build_chain = 0x40004c8c );

PROVIDE ( multofup = 0x400068a0 );
PROVIDE ( roundup2 = 0x40006890 );

PROVIDE ( SPIEraseSector = 0x40004708 );
PROVIDE ( SPIRead = 0x40004898 );
PROVIDE ( SPIWrite = 0x40004738 );

PROVIDE ( uart_div_modify = 0x400034e8 );
PROVIDE ( uart_tx_one_char = 0x4000362c );

PROVIDE ( __divsi3 = 0x40006888 );
PROVIDE ( __udivdi3 = 0x40006c30 );
PROVIDE ( __umoddi3 = 0x40006e64 );

PROVIDE ( _xtos_set_intlevel = 0x40006670 );

