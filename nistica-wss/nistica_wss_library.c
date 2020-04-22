/** nistica_wss_library.c
 * @file nistica_wss_library.c
 * 
 * @author S.Prasanna, Gattu Shivakrishna, Premsai, Shanmukha
 * 
 * @version 1.0
 * 
 * @brief This Module is Linux User Space Driver which communicates with Nistica WSS Module via UART.
 *
 * @details
 * The Nistica WSS library consists of functions to assign/switch/attenuate channels in Nistica WSS 
 * module. It acts as an interface to communicate and obtain several information from the WSS module.
 * General module configurations related to boot, firmware related operations, PIN controls, Waveplan
 * configuration and management, OCM power monitoring, querying performance and alarms, etc,. have been
 * provided APIs with which necessary operations can be done in WSS and OCM module.
 *
 * The UART configuration for Nistica WSS module is :
 *		115200 Baud rate, 8 bits, non-parity and 1 stop-bit.
 *
 * The Nistica WSS LUD categorizes APIs into the following types :
 *		1. General Module Information
 *		2. Channel Configurations
 *		3. Module Operation status
 *		4. Alarm Information
 *		5. Firmware Upgradation
 *
 * @date 22nd April 2020.
 */

