#define READ_CMD 0x02
#define WRITE_CMD 0x01
#define ARRAY_WRITE 0x10

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

int get_active_partition_number_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *active_partition_number );

int get_booted_partition_number_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *booted_partition_number );

int get_partition_1_firmware_version_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *firmware_version );

int get_partition_2_firmware_version_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *firmware_version );

int get_bootloader_version_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *bootloader_version );

int get_module_uptime_low_word_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *module_uptime_low );

int get_module_uptime_high_word_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *module_uptime_high );

int get_module_supports_array_commands_of_nistica_wss_module( unsigned int uart_port_number, unsigned int status_of_array_cmd );

 /************************************************
*************************************************
*		Channel Configurations
*************************************************
************************************************/

int set_waveplan_of_nistica_wss_module( unsigned int uart_port_number, unsigned short number_of_channels, float center_frequency_in_Thz, float bandwidth_in_Ghz );
int set_channel_port_of_nistica_wss_module( unsigned int uart_port_number, unsigned short start_of_channel, 
										 unsigned short end_of_channel, char port_id);
int get_number_of_ports_of_nistica_wss_module( unsigned int uart_port_number,unsigned int wss_id, unsigned short *number_of_ports );
int get_active_configuration_of_nistica_wss_module( unsigned int uart_port_number,unsigned int wss_id, unsigned short *active_configuration_number );