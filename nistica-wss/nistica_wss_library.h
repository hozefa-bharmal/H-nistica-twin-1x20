#define READ_CMD 0x02
#define WRITE_CMD 0x01
#define ARRAY_WRITE 0x10

#define RS_232_START_BYTES {0xdd, 0x01}

#define SUCCESS 0

 /************************************************
*************************************************
*		   Module Information
*************************************************
************************************************/

int get_vendor_name_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *vendor_name );

int get_minimum_frequency_bound_of_nistica_wss_module( unsigned int uart_port_number, unsigned short* minimum_frequency_bound );

int get_maximum_frequency_bound_of_nistica_wss_module( unsigned int uart_port_number, unsigned short* maximum_frequency_bound );

int get_minimum_channel_bandwidth_of_nistica_wss_module( unsigned int uart_port_number, unsigned short* minimum_channel_bandwidth );

int get_maximum_wss_id_value_of_nistica_wss_module( unsigned int uart_port_number, short* maximum_wss_id_value );

int get_maximum_ocm_id_value_of_nistica_wss_module( unsigned int uart_port_number, short* maximum_ocm_id_value );

int get_maximum_waveplan_id_value_of_nistica_wss_module( unsigned int uart_port_number, short* maximum_ocm_id_value );

int get_power_on_self_test_results_of_nistica_wss_module( unsigned int uart_port_number, short* power_test_result );

int get_module_status_of_nistica_wss_module( unsigned int uart_port_number, short* module_status );

int get_boot_mode_of_nistica_wss_module( unsigned int uart_port_number, short* boot_mode );

int set_cold_boot_mode_for_nistica_wss_module( unsigned int uart_port_number, short* boot_mode );

int set_warm_boot_mode_for_nistica_wss_module( unsigned int uart_port_number, short* boot_mode );

int set_watchdog_reset_boot_mode_for_nistica_wss_module( unsigned int uart_port_number, short* boot_mode );

int set_hot_boot_mode_for_nistica_wss_module( unsigned int uart_port_number, short* boot_mode );

int get_module_supports_multi_commands_of_nistica_wss_module( unsigned int uart_port_number, short* multicmd );

int get_module_supports_table_and_row_commands_of_nistica_wss_module( unsigned int uart_port_number,short* table_row_cmd );

int get_module_supports_long_cmd_format_of_nistica_wss_module( unsigned int uart_port_number,short* longcmd );

int get_module_supports_broadcast_of_nistica_wss_module( unsigned int uart_port_number,short* broadcast );

 /************************************************
*************************************************
*		Channel Configurations
*************************************************
************************************************/

int set_waveplan_of_nistica_wss_module( unsigned int uart_port_number, unsigned short number_of_channels, float center_frequency_in_Thz, float bandwidth_in_Ghz );

int assign_particular_waveplan_of_nistica_wss_module( unsigned int uart_port_number, unsigned short wss_id, unsigned short altconf_id, unsigned short waveplan_id );

int set_channel_port_of_nistica_wss_module( unsigned int uart_port_number, unsigned short start_of_channel, unsigned short end_of_channel, char port_id);

int set_channel_port_and_attenuation_of_nistica_wss_module( unsigned int uart_port_number, unsigned short wss_id, unsigned short altconf_id, unsigned char *channel_numbers, unsigned char *port_data, unsigned char *attenuation_data );

int set_channel_port_and_attenuation_in_twin_wss_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *channel_numbers, unsigned char *port_data, unsigned char *attenuation_data );

