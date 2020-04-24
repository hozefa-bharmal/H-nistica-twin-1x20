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

#include <stdio.h>
#include <stdlib.h>
 
#include "nistica_wss_library.h"
 
 /************************************************
*************************************************
*		   Module Information
*************************************************
************************************************/

/**
* @brief Function will Get Vendor Name from Nistica WSS module
*
* This function reads the Module Information from WSS module and then the required
* data i.e., Vendor name is extracted. Function forms a command structure consisting
* of Message ID, Command, Object ID, Instance, Parameter and Checksum.
* The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
* '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
* synchronization between control module and WSS.
*
* Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
*		SUM (1 byte) = XOR from MID to PAR
*
* Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
*
* Function takes two arguments - UART port number and character pointer. UART port
* is used to transmit and receive data packets from/to Nistica WSS module. Certain
* validation in the receive packet such as Message ID, Length and Result is done
* by the function and the Vendor name data is extracted and returned.
* 
* @param uart_port_number
* an unsigned integer, represents the particular UART port to which Nistica WSS is connected
*
*	Example: 1
*
* @param vendor_name
* a character pointer, represents the data in which Vendor name is returned.
*
*	Example: Nistica
*
* @return an integer
*  <BR>  0  : Success
*  <BR> -1  : Failure
*/
int get_vendor_name_of_nistica_wss_module( unsigned int uart_port_number, unsigned char *vendor_name )
{
	char packet_to_transmit[11]={0};
	char uart_received_packet_return[110]={0};

	int transmit_packet=0,
		receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	packet_to_transmit = { 0xdd, 0x01, 0x01, 0x05, READ_CMD, 0x06, 0x01, 0x00, 0x01, 0xdd, 0x02 };
	length_of_packet_to_transmit = strlen(packet_to_transmit); 

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
		printf("Error : Failed to transmit packet via UART Port in get_vendor_name_of_nistica_wss_module()\n");
		return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
   		printf("Error : Failed to receive packet via UART Port in get_vendor_name_of_nistica_wss_module()\n");
    	return FAILURE;
	}

	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];
	Validate LEN -> 0x6c == uart_received_packet_return[3];
	
	If all validation passes ->
				extract Vendor Name from uart_received_packet_return from 5th byte to 11th byte
	
	If any validation fail -> 
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get Minimum Frequency Bound value of Nistica WSS module
 *
 * This function reads the lower possible channel edge value from WSS module.
 * The value remains the lower boundary to define the waveplan allowed for the flexible
 * waveplan management commands.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x19; OBJ=0x80; INS=0x04; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the
 * function and the data is extracted and returned.
 * 
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param minimum_frequency_bound
 * an unsigned short pointer, represents the data in which Minimum Frequency bound is returned.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/
int get_minimum_frequency_bound_of_nistica_wss_module( unsigned int uart_port_number, unsigned short* minimum_frequency_bound )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x19, 0x05, READ_CMD, 0x80, 0x04, 0x00, 0x9A, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
		receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit); 

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
		printf("Error : Failed to transmit packet via UART Port in get_minimum_frequency_bound_of_nistica_wss_module()\n");
		return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
   		printf("Error : Failed to receive packet via UART Port in get_minimum_frequency_bound_of_nistica_wss_module()\n");
    	return FAILURE;
	}

	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];
	
	If above validation passes ->
				extract Minimum Frequency bound value from uart_received_packet_return[5] and uart_received_packet_return[6], OR both bytes
    			multiply the result by 3.125 and store it in minimum_frequency_band
	
	If any validation fail -> 
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get Maximum Frequency Bound value of Nistica WSS module
 *
 * This function reads the upper possible channel edge value from WSS module.
 * The value remains the lower boundary to define the waveplan allowed for the flexible
 * waveplan management commands.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x19; OBJ=0x80; INS=0x05; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the
 * function and the data is extracted and returned.
 * 
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param maximum_frequency_bound
 * an unsigned short pointer, represents the data in which Maximum Frequency bound is returned.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/
int get_maximum_frequency_bound_of_nistica_wss_module( unsigned int uart_port_number, unsigned short* maximum_frequency_bound )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x19, 0x05, READ_CMD, 0x80, 0x05, 0x00, 0x9B, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
		receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit); 

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
		printf("Error : Failed to transmit packet via UART Port in get_maximum_frequency_bound_of_nistica_wss_module()\n");
		return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
   		printf("Error : Failed to receive packet via UART Port in get_maximum_frequency_bound_of_nistica_wss_module()\n");
    	return FAILURE;
	}

	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];
	
	If above validation passes ->
				extract Maximum Frequency bound value from uart_received_packet_return[5] and uart_received_packet_return[6], OR both bytes
    			multiply the result by 3.125 and store it in maximum_frequency_band
	
	If any validation fail -> 
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get Minimum Channel Bandwidth value of Nistica WSS module
 *
 * This function reads the smallest channel that can be defined in WSS module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x19; OBJ=0x80; INS=0x04; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function
 * and Data is extracted and returned.
 * The value returned by the module will be in terms of 3.125 GHz unit. Inorder to obtain
 * the actual Frequency in GHz, resultant is multiplied with 3.125.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param minimum_channel_bandwidth
 * an unsigned short pointer, represents the data in which minimum channel bandwidth is returned.
 *
 *	Example: Nistica
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/
int get_minimum_channel_bandwidth_of_nistica_wss_module( unsigned int uart_port_number, unsigned short* minimum_channel_bandwidth )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x19, 0x05, READ_CMD, 0x80, 0x06, 0x00, 0x9C, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
		receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit); 

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
		printf("Error : Failed to transmit packet via UART Port in get_minimum_channel_bandwidth_of_nistica_wss_module()\n");
		return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
   		printf("Error : Failed to receive packet via UART Port in get_minimum_channel_bandwidth_of_nistica_wss_module()\n");
    	return FAILURE;
	}

	Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
	Validate RES -> SUCCESS == uart_received_packet_return[4];
	
	If above validation passes ->
				extract Minimum channel bandwidth value from uart_received_packet_return[5] and uart_received_packet_return[6], OR both bytes
    			multiply the result by 3.125 and store it in minimum_channel_bandwidth
	
	If any validation fail -> 
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get Maximum WSS ID value present Nistica WSS module
 *
 * This function reads the maximum WSS ID value that is defined in WSS module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x19; OBJ=0x80; INS=0x0F; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param maximum_wss_id_value
 * a signed short pointer, represents the data in which Maximum WSS ID value is returned.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/
int get_maximum_wss_id_value_of_nistica_wss_module( unsigned int uart_port_number, short* maximum_wss_id_value )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x19, 0x05, READ_CMD, 0x80, 0x0F, 0x00, 0x91, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in get_minimum_channel_bandwidth_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in get_minimum_channel_bandwidth_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				extract Maximum WSS ID value from uart_received_packet_return[5] and uart_received_packet_return[6], OR it and store the result in maximum_wss_id_value

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get Maximum OCM ID value present Nistica WSS module
 *
 * This function reads the maximum OCM ID value that is defined in WSS module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x19; OBJ=0x80; INS=0x10; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param maximum_ocm_id_value
 * a signed short pointer, represents the data in which Maximum OCM ID value is returned.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/
int get_maximum_ocm_id_value_of_nistica_wss_module( unsigned int uart_port_number, short* maximum_ocm_id_value )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x19, 0x05, READ_CMD, 0x80, 0x10, 0x00, 0x8E, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in get_minimum_channel_bandwidth_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in get_minimum_channel_bandwidth_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				extract Maximum OCM ID value from uart_received_packet_return[5] and uart_received_packet_return[6], OR it and store the result in maximum_ocm_id_value

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}


/**
 * @brief Function will Get Maximum Waveplan ID value present Nistica WSS module
 *
 * This function reads the maximum Waveplan ID value that is defined in WSS module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x19; OBJ=0x80; INS=0x12; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param maximum_waveplan_id_value
 * a signed short pointer, represents the data in which Maximum Waveplan ID value is returned.
 *
 * @return an integer
 *  <BR>  0  : Success
 *  <BR> -1  : Failure
**/
int get_maximum_waveplan_id_value_of_nistica_wss_module( unsigned int uart_port_number, short* maximum_waveplan_id_value )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x19, 0x05, READ_CMD, 0x80, 0x12, 0x00, 0x8E, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in get_maximum_waveplan_id_value_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in get_maximum_waveplan_id_value_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				extract Maximum Waveplan ID value from uart_received_packet_return[5] and uart_received_packet_return[6], OR it and store the result in maximum_ocm_id_value

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get Multiple Commands Fearture value present Nistica WSS module
 *
 * This function reads the multiple commands fearture value that is defined in WSS module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x10; OBJ=0x80; INS=0x02; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param multicmd
 * a signed short pointer, represents the data in which multiple commands fearture value is returned.
 * This variable will have '1' if the module supports long commands feature, '0' if not.
 *
 * @return an integer
 *  <BR>  0 : Success
 *  <BR> -1  : Failure
**/
int get_module_supports_multi_commands_of_nistica_wss_module( unsigned int uart_port_number, short* multicmd )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x10, 0x05, READ_CMD, 0x80, 0x02, 0x00, 0x95 , 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in get_module_supports_multi_commands_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in get_module_supports_multi_commands_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				extract multicmd value from uart_received_packet_return[5] and uart_received_packet_return[6], OR it and store the result in maximum_ocm_id_value

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}

/**
 * @brief Function will Get Table and Rows Fearture value present Nistica WSS module
 *
 * This function reads the table and rows fearture value that is defined in WSS module.
 * Function forms a command structure consisting of Message ID, Command, Object ID,
 * Instance, Parameter and Checksum.
 * The Transmit and Receive packet is encoded with '0xdd 0x01' in the beginning and
 * '0xdd 0x02' in the end representing RS-232 frame marker required to ensure
 * synchronization between control module and WSS.
 *
 * Tx packet to WSS module : 0xdd 0x01 MID LEN CMD OBJ INS PAR SUM 0xdd 0x02
 *		MID=0x10; OBJ=0x80; INS=0x0E; PAR=0x00; SUM (1 byte) = XOR from MID to PAR
 *
 * Response from WSS : 0xdd 0x01 MID LEN RES DATA SUM 0xdd 0x02
 *		DATA (2 bytes)
 *
 * Function takes two arguments - UART port number and character pointer. UART port
 * is used to transmit and receive data packets from/to Nistica WSS module. Certain
 * validation in the receive packet such as Message ID and Result is done by the function.
 * Required Data is extracted and returned.
 *
 * @param uart_port_number
 * an unsigned integer, represents the particular UART port to which Nistica WSS is connected
 *
 *	Example: 1
 *
 * @param table_row_cmd
 * a signed short pointer, represents the data in which table and rows fearture value is returned.
 * This variable will have '1' if the module supports long commands feature, '0' if not.
 *
 * @return an integer
 *  <BR>  0 : Success
 *  <BR> -1  : Failure
**/
int get_module_supports_table_and_row_commands_of_nistica_wss_module( unsigned int uart_port_number,short* table_row_cmd )
{
	char packet_to_transmit[] = { 0xdd, 0x01, 0x11, 0x05, READ_CMD, 0x80, 0x0E, 0x00, 0x98, 0xdd, 0x02 };
	char uart_received_packet_return[255]={0};

	int transmit_packet=0,
	    receive_packet=0;

	unsigned int length_of_packet_to_transmit=0;
	unsigned int length_of_received_packet_return=0;

	length_of_packet_to_transmit = strlen(packet_to_transmit);

	transmit_packet = transmit_packet_via_uart_port(uart_port_number, packet_to_transmit, length_of_packet_to_transmit);
	if(SUCCESS != transmit_packet)
	{
        printf("Error : Failed to transmit packet via UART Port in get_module_supports_multi_commands_of_nistica_wss_module()\n");
        return FAILURE;
	}

	//usleep(WAIT_TIME_TO_RECEIVE_PACKET_FROM_MODULE);

	receive_packet = receive_packet_via_uart_port(uart_port_number, uart_received_packet_return, &length_of_received_packet_return);
	if(SUCCESS != receive_packet)
	{
        printf("Error : Failed to receive packet via UART Port in get_module_supports_multi_commands_of_nistica_wss_module()\n");
        return FAILURE;
	}

    Validate MID -> packet_to_transmit[2] == uart_received_packet_return[2];
    Validate RES -> SUCCESS == uart_received_packet_return[4];

    If above validation passes ->
				extract table_row_cmd value from uart_received_packet_return[5] and uart_received_packet_return[6], OR it and store the result in maximum_ocm_id_value

	If any validation fail ->
				Return error with appropriate error message/reason for failure
}
