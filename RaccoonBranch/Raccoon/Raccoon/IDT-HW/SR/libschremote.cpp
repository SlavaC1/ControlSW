#include "libschremote.h"

SR_IPLIST* sr_discover(const char *ip, unsigned short port)
{
	return NULL;
}

/* Free the resources used by sr_discover() function;
 *
 * Parameters:
 * list	Pointer to SR_IPLIST structure
 *
 * Return: none
 */
void sr_discover_free(SR_IPLIST *list)
{

}


/* Open a connection trough a ethernet interface
 *
 * Parameters:
 * ip	Zero terminated string to IP address or hostname of the target device
 * port	Port number of the target device. Default configuration uses port 3101
 *
 * Return:
 * NULL on failure
 * Handle on success
 *
 * Remarks:
 * When the connection is no more used, sr_close() must be called.
 */
SR_HANDLE sr_open_eth(const char *ip, unsigned short port)
{
	SR_HANDLE srh = (void*)new int*;
	return srh;
}

/* Close a active connection
 *
 * Parameters:
 * srh	Handle to connection
 *
 * Return: none
 */
void sr_close(SR_HANDLE srh)
{
	delete (int*)srh;
}

/* Send a NO OPERATION command to device
 *
 * Parameters:
 * srh	Handle to connection
 *
 * Return:
 * 0 on failure
 * non zero on success
 */
int sr_nop(SR_HANDLE srh)
{
	return 0; 
}

/* Return a message describing occurred error
 *
 * Parameters:
 * srh	Handle to connection
 *
 * Return:
 * Zero terminated string
 */
const char* sr_error_info(SR_HANDLE srh)
{
	return NULL;
}

/* Return library version
 *
 * Parameters: none
 *
 * Return:
 * version word
 *
 * Remarks:
 * Major version is in high order byte and minor version is in low order byte. Example: 0x0102 means version 1.2.
 */
unsigned short sr_version()
{
	return 0;
}

/* Configure a pin type
 *
 * Parameters:
 * srh	Handle to connection
 * pin	Pin number 0..11 to configure
 * pin_type	Type to assign
 * 
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * Only pins 4-9 can be analog input. Trying to set any of 1-3,10,11 pins to analog input, will be set as digital input.
 * Setup of any individual pin, change all digital output pins to their default level.
 */
int sr_pin_setup(SR_HANDLE srh, int pin, sr_pin_type pin_type)
{
	return 0;
}

/* Get type of a pin
 *
 * Parameters:
 * srh	Handle to connection
 * pin	Pin number 0..11 to configure
 * pin_type	Pointer where to store the type
 * 
 * Return:
 * 0 on failure
 * non zero on success
 */
int sr_pin_status(SR_HANDLE srh, int pin, sr_pin_type *pin_type)
{
	return 0;
}

/* Configure the type of all pins
 *
 * Parameters:
 * srh	Handle to connection
 * pin_type	Array of pin types to assign
 * 
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * Only pins 4-9 can be analog input. Trying to set any of 1-3,10,11 pins to analog input, will be set as digital input.
 */
int sr_pins_setup(SR_HANDLE srh, sr_pin_type pin_type[12])
{
	return 0;
}

/* Get type of all pins
 *
 * Parameters:
 * srh	Handle to connection
 * pin_type	Pointer where to store the types
 * 
 * Return:
 * 0 on failure
 * non zero on success
 */
int sr_pins_status(SR_HANDLE srh, sr_pin_type pin_type[12])
{
	return 0;
}

/* Set the state of digital out / open drain pin
 *
 * Parameters:
 * srh	Handle to connection
 * pin	Pin number 0..11 to configure
 * b	0/1 state
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * On pins setup as analog or digital input or used for communication this function have not effect.
 */
int sr_pin_set(SR_HANDLE srh, int pin, bool b)
{
	return 0;
}

/* Get the state of a pin
 *
 * Parameters:
 * srh	Handle to connection
 * pin	Pin number 0..11 to configure
 * b	Pointer where to store the state
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * On pins setup as analog input result is undetermined.
 */
int sr_pin_get(SR_HANDLE srh, int pin, bool *b)
{
	return 0;
}

/* Set the state of digital out / open drain pin
 *
 * Parameters:
 * srh	Handle to connection
 * b	Each bit 0..11 correspond to 0/1 state of related pin
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * On pins setup as analog or digital input or used for communication this have not effect.
 */
int sr_pins_set(SR_HANDLE srh, unsigned short b)
{
	return 0;
}

/* Get the state of a pin
 *
 * Parameters:
 * srh	Handle to connection
 * b	Pointer where to store the state. Each bit 0..11 correspond to 0/1 state of related pin.
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * On pins setup as analog input result is undetermined.
 */
int sr_pins_get(SR_HANDLE srh, unsigned short *b)
{
	return 0;
}

/* Get the value of analog pin
 *
 * Parameters:
 * srh	Handle to connection
 * pin	Pin number 4..9 to read
 * val	Pointer where to store the value. Range 0..1024.
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * On only digital pins function return error. On pins not setup as analog input result is undetermined.
 */
int sr_pin_get_analog(SR_HANDLE srh, int pin, unsigned short *val)
{
	return 0;
}

/* Setup the UART interface
 *
 * Parameters:
 * srh		Handle to connection
 * port		Uart module to query (0 or 1)
 * mode		Combination of UART_MODE_XXX flags
 * baud		UART baud rate
 * rx_pin	Pin number used for data receive
 * tx_pin	Pin number used for data transmit
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * Function will accept any baudrade, but will setup the device to the range of 16 .. 4 000 000.
 * Actual baud rate is: actual = 1 000 000 / (X+1), where X in 0 .. 65535 for a baudrates up to 1 000 000 and actual = 4 000 000 / (X+1), where X in 0 .. 3 for above.
 * As rx_pin and tx_pin can be selected from pins 0..2, 4..10 (pins 3 and 11 cannot be used). Function will return error if try to use unallowed pin.
 * Selected pins should be previously set as digital input, digital output, open drain output. Trying to use analog pin will not change the behavior of the pin.
 * Trying to use same pins for several functions have the effect of: last change override the previous purpose of the pin.
*/
int sr_uart_enable(SR_HANDLE srh, int port, int mode, int baud, int rx_pin, int tx_pin)
{
	return 0;
}

/* Read the setting of UART interface
 *
 * Parameters:
 * srh		Handle to connection
 * port		Uart module to query (0 or 1)
 * mode		Pointer where to store combination of UART_MODE_XXX flags
 * baud		Pointer where to store UART baud rate
 * rx_pin	Pointer where to store receive data pin number
 * tx_pin	Pointer where to store transmit pin number
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * If UART functionality is not enabled, returned data is: mode = 0, baud = 0, rx_pin = -1, tx_pin = -1
 */
int sr_uart_status(SR_HANDLE srh, int port, int *mode, int *baud, int *rx_pin, int *tx_pin)
{
	return 0;
}

/* Disable UART interface
 *
 * Parameters:
 * srh	Handle to connection
 * port	Uart module to query (0 or 1)
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * Assigned pins are turned to their state set by sr_pin_setup or sr_pins_setup.
 */
int sr_uart_disable(SR_HANDLE srh, int port)
{
	return 0;
}

/* Write a byte to the UART interface
 *
 * Parameters:
 * srh	Handle to connection
 * port	Uart module to query (0 or 1)
 * data	A byte to be written
 *
 * Return:
 * 0 on failure
 * non zero on success
 */
int sr_uart_write(SR_HANDLE srh, int port, unsigned char data)
{
	return 0;
}

/* Read a byte from the UART interface
 *
 * Parameters:
 * srh		Handle to connection
 * port		Uart module to query (0 or 1)
 * data		A byte pointer where to store readed data
 * readed	Pointer where to store actual number of readed bytes
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * If readed is NULL, function will wait for the data byte, else if not pending data readed will be 0
 */
int sr_uart_read(SR_HANDLE srh, int port, unsigned char *data, unsigned short *readed)
{
	return 0;
}

/* Write a array of bytes to the UART interface
 *
 * Parameters:
 * srh	Handle to connection
 * port	Uart module to query (0 or 1)
 * arr	A byte pointer to the data for write
 * cnt	Number if bytes to write
 *
 * Return:
 * 0 on failure
 * non zero on success
 */
int sr_uart_write_arr(SR_HANDLE srh, int port, unsigned char *arr, unsigned short cnt)
{
	return 0;
}

/* Read a array of bytes from the UART interface
 *
 * Parameters:
 * srh	Handle to connection
 * port	Uart module to query (0 or 1)
 * arr	A byte pointer where to store readed array
 * cnt	Number if bytes to read
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * If readed is NULL, function will wait for cnt data bytes to be available, else will return up to cnt bytes that are available, readed contain the actual number.
 */
int sr_uart_read_arr(SR_HANDLE srh, int port, unsigned char *arr, unsigned short cnt, unsigned short *readed)
{
	return 0;
}

/* Setup the SPI interface
 *
 * Parameters:
 * srh		Handle to connection
 * mode		Combination of SPI_MODE_XXX flags
 * baud		SPI clock baud rate
 * clk_pin	Pin number used for clock out
 * out_pin	Pin number used for data out
 * in_pin	Pin number used for data in
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * Function will accept any baudrade, but will setup the device to the nearest of the following baudrates: 8 000 000, 5 333 333, 4 000 000, 3 200 000,
 * 2 666 667, 2 285 714, 2 000 000, 1 333 333, 1 000 000, 800 000, 666 667, 571 429, 500 000, 333 333, 250 000, 200 000, 166 667, 142 857, 125 000, 83 333,
 * 62 500, 50 000, 41 667, 35 714, 31 250
 * As clk_pin, out_pin, in_pin can be selected from pins 0..2, 4..10 (pins 3 and 11 cannot be used). Function will return error if try to use unallowed pin.
 * Selected pins should be previously set as digital input, digital output, open drain output. Trying to use analog pin will not change the behavior of the pin.
 * Trying to use same pins for several functions have the effect of: last change override the previous purpose of the pin.
*/
int sr_spi_enable(SR_HANDLE srh, unsigned short mode, int baud, int clk_pin, int out_pin, int in_pin)
{
	return 0;
}

/* Read the setting of SPI interface
 *
 * Parameters:
 * srh		Handle to connection
 * mode		Pointer where to store combination of SPI_MODE_XXX flags
 * baud		Pointer where to store SPI clock baud rate
 * clk_pin	Pointer where to store clock out pin number
 * out_pin	Pointer where to store data out pin number
 * in_pin	Pointer where to store data in pin number
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * If SPI functionality is not enabled, returned data is: mode = 0, baud = 0, clk_pin = -1, out_pin = -1, in_pin = -1
 */
int sr_spi_status(SR_HANDLE srh, unsigned short *mode, int *baud, int *clk_pin, int *out_pin, int *in_pin)
{
	return 0;
}

/* Disable SPI interface
 *
 * Parameters:
 * srh	Handle to connection
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * Assigned pins are turned to their state set by sr_pin_setup or sr_pins_setup.
 */
int sr_spi_disable(SR_HANDLE srh)
{
	return 0;
}

/* Write a byte to the SPI interface
 *
 * Parameters:
 * srh	Handle to connection
 * data	A byte to be written
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * Typical SPI communication require a CS (chip select) signal. For that purpose can be used digital out pin and sr_pin_set, before and after this function is invoked.
 */
int sr_spi_write(SR_HANDLE srh, unsigned char data)
{
	return 0;
}


/* Read a byte from the SPI interface
 *
 * Parameters:
 * srh	Handle to connection
 * data	A byte pointer where to store readed data
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * Typical SPI communication require a CS (chip select) signal. For that purpose can be used digital out pin and sr_pin_set, before and after this function is invoked.
 */
int sr_spi_read(SR_HANDLE srh, unsigned char *data)
{
	return 0;
}

/* Write a array of bytes to the SPI interface
 *
 * Parameters:
 * srh	Handle to connection
 * arr	A byte pointer to the data for write
 * cnt	Number if bytes to write
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * Typical SPI communication require a CS (chip select) signal. For that purpose can be used digital out pin and sr_pin_set, before and after this function is invoked.
 */
int sr_spi_write_arr(SR_HANDLE srh, unsigned char *arr, unsigned short cnt)
{
	return 0;
}

/* Read a array of bytes from the SPI interface
 *
 * Parameters:
 * srh	Handle to connection
 * arr	A byte pointer where to store readed array
 * cnt	Number if bytes to read
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * Typical SPI communication require a CS (chip select) signal. For that purpose can be used digital out pin and sr_pin_set, before and after this function is invoked.
 */
int sr_spi_read_arr(SR_HANDLE srh, unsigned char *arr, unsigned short cnt)
{
	return 0;
}

/* Setup the I2C interface
 *
 * Parameters:
 * srh	Handle to connection
 * port	I2C module to setup: 0 - pin 0:SDA, pin 1:SCL or 1 - pin 8:SDA, pin 9:SCL
 * baud	I2C clock baud rate
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * Function will accept any baudrade, but will setup the device to the range of 62 112 .. 2 857 143.
 * Actual baud rate is: actual = 16 000 000 / (X+2.6), where X in 3 .. 255. Baud rate slightly depend from the I2C pullup resistors value.
 */
int sr_i2c_enable(SR_HANDLE srh, int port, int baud)
{
	return 0;
}

/* Read the setting of I2C interface
 *
 * Parameters:
 * srh	Handle to connection
 * port	I2C module to query
 * baud	Pointer where to store I2C clock baud rate
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * If I2C functionality is not enabled, returned data is: baud = 0
 */
int sr_i2c_status(SR_HANDLE srh, int port, int *baud)
{
	return 0;
}

/* Disable I2C interface
 *
 * Parameters:
 * srh	Handle to connection
 * port	I2C module to query
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * Used pins are turned to their state set by sr_pin_setup or sr_pins_setup.
 */
int sr_i2c_disable(SR_HANDLE srh, int port)
{
	return 0;
}

/* Write a byte to the I2C interface
 *
 * Parameters:
 * srh		Handle to connection
 * port		I2C module to query
 * i2c_addr	Address of target device
 * data		A byte to be written
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * I2C address is shifted 1 bit left: A6(MSB) A5 A4 A3 A2 A1 A0 <don't care bit>(LSB)
 */
int sr_i2c_write(SR_HANDLE srh, int port, unsigned char i2c_addr, unsigned char data)
{
	return 0;
}

/* Read a byte from the I2C interface
 *
 * Parameters:
 * srh		Handle to connection
 * port		I2C module to query
 * i2c_addr	Address of target device
 * data		A byte pointer where to store readed data
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * I2C address is shifted 1 bit left: A6(MSB) A5 A4 A3 A2 A1 A0 <don't care bit>(LSB)
 */
int sr_i2c_read(SR_HANDLE srh, int port, unsigned char i2c_addr, unsigned char *data)
{
	return 0;
}

/* Write a byte to the I2C interface, restart and read a byte from same I2C address
 *
 * Parameters:
 * srh		Handle to connection
 * port		I2C module to query
 * i2c_addr	Address of target device
 * data_write	A byte to be written
 * data_read	A byte pointer where to store readed data
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * I2C address is shifted 1 bit left: A6(MSB) A5 A4 A3 A2 A1 A0 <don't care bit>(LSB)
 */
int sr_i2c_write_read(SR_HANDLE srh, int port, unsigned char i2c_addr, unsigned char data_write, unsigned char *data_read)
{
	return 0;
}

/* Write a array of bytes to the I2C interface
 *
 * Parameters:
 * srh		Handle to connection
 * port		I2C module to query
 * i2c_addr	Address of target device
 * arr		A byte pointer to the data for write
 * cnt		Number if bytes to write
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * I2C address is shifted 1 bit left: A6(MSB) A5 A4 A3 A2 A1 A0 <don't care bit>(LSB)
 * Maximum size of arr is 128 bytes. If bigger, function return a error.
 */
int sr_i2c_write_arr(SR_HANDLE srh, int port, unsigned char i2c_addr, unsigned char *arr, unsigned short cnt)
{
	return 0;
}

/* Read a array of bytes from the I2C interface
 *
 * Parameters:
 * srh		Handle to connection
 * port		I2C module to query
 * i2c_addr	Address of target device
 * arr		A byte pointer where to store readed array
 * cnt		Number if bytes to read
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * I2C address is shifted 1 bit left: A6(MSB) A5 A4 A3 A2 A1 A0 <don't care bit>(LSB)
 * Maximum size of arr is 128 bytes. If bigger, function return a error.
 */
int sr_i2c_read_arr(SR_HANDLE srh, int port, unsigned char i2c_addr, unsigned char *arr, unsigned short cnt)
{
	return 0;
}

/* Write a array of bytes to the I2C interface, restart and read a array of bytes from same I2C address
 *
 * Parameters:
 * srh		Handle to connection
 * port		I2C module to query
 * i2c_addr	Address of target device
 * arr_write	A byte pointer to the data for write
 * cnt_write	Number if bytes to write
 * arr_read	A byte pointer where to store readed array
 * cnt_read	Number if bytes to read
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * I2C address is shifted 1 bit left: A6(MSB) A5 A4 A3 A2 A1 A0 <don't care bit>(LSB)
 * Maximum size of arr_write is 128 bytes. If bigger, function return a error.
 * Maximum size of arr_read is 128 bytes. If bigger, function return a error.
 */
int sr_i2c_write_read_arr(SR_HANDLE srh, int port, unsigned char i2c_addr, unsigned char *arr_write, unsigned short cnt_write, unsigned char *arr_read, unsigned short cnt_read)
{
	return 0;
}

/* Setup a counter module
 *
 * Parameters:
 * srh		Handle to connection
 * port		Counter module to query (range from 0 to 3)
 * in_pin	Pin number used for clock signals
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * As in_pin can be selected from pins 0..2, 4..10 (pins 3 and 11 cannot be used). Function will return error if try to use unallowed pin.
 * Selected pin should be previously set as digital input. Trying to use analog pin will not change the behavior of the pin.
*/
int sr_cnt_enable(SR_HANDLE srh, int port, int in_pin)
{
	return 0;
}

/* Read the setting of counter module
 *
 * Parameters:
 * srh		Handle to connection
 * port		Counter module to query (range from 0 to 3)
 * in_pin	Pointer where to store in pin number
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * If counter module is not enabled, returned data is: in_pin = -1
 */
int sr_cnt_status(SR_HANDLE srh, int port, int *in_pin)
{
	return 0;
}

/* Disable counter module
 *
 * Parameters:
 * srh		Handle to connection
 * port		Counter module to query (range from 0 to 3)
 *
 * Return:
 * 0 on failure
 * non zero on success
 */
int sr_cnt_disable(SR_HANDLE srh, int port)
{
	return 0;
}

/* Reset current count value to 0 of counter module
 *
 * Parameters:
 * srh		Handle to connection
 * port		Counter module to query (range from 0 to 3)
 *
 * Return:
 * 0 on failure
 * non zero on success
 */
int sr_cnt_reset(SR_HANDLE srh, int port)
{
	return 0;
}

/* Read current count value to 0 of counter module
 *
 * Parameters:
 * srh		Handle to connection
 * port		Counter module to query (range from 0 to 3)
 * cnt		Pointer where to store current count (0 to 65535)
 *
 * Return:
 * 0 on failure
 * non zero on success
 */
int sr_cnt_read(SR_HANDLE srh, int port, unsigned short *cnt)
{
	return 0;
}

/* Store current setup as default (start up) configuration
 *
 * Parameters:
 * srh		Handle to connection
 *
 * Return:
 * 0 on failure
 * non zero on success
 */
int sr_store_config(SR_HANDLE srh)
{
	return 0;
}

/* Store the default (start up) network configuration
 *
 * Parameters:
 * srh		Handle to connection
 * ip		Ip address
 * mask		Network mask
 * gateway	Gataway
 * dchp		Enable dhcp client (0 or 1)
 *
 * Return:
 * 0 on failure
 * non zero on success
 *
 * Remarks:
 * All addresses are in big endian format, for example 192.168.1.2 is 0x0201a8c0.
 * If dhcp client is enabled, device try to obtain the data from it and only if fail use the static address setup.
 */
int sr_store_ip(SR_HANDLE srh, unsigned long ip, unsigned long mask, unsigned long gateway, bool dhcp)
{
	return 0;
}

/* Read default (start up) network configuration
 *
 * Parameters:
 * srh		Handle to connection
 * ip		Pointer where to store ip address
 * mask		Pointer where to store network mask
 * gateway	Pointer where to store gateway
 * dchp		Pointer where to store is dhcp client in use
 *
 * Return:
 * 0 on failure
 * non zero on success
 */
int sr_read_ip(SR_HANDLE srh, unsigned long *ip, unsigned long *mask, unsigned long *gateway, bool *dhcp)
{
	return 0;
}
