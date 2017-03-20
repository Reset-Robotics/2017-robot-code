/*
 * I2C.cpp
 *
 *  Created on: Jan 31, 2015
 *      Author: trevor
 */

#include "I2C.h"

namespace jetsonhardware {
namespace i2c {


I2C::I2C() : _ten_bit_address(0),
			 _op_result(0),
			 _handle(0) {

}

I2C::~I2C() {

}

void I2C::i2c_open(std::string file, int mode) {
	_handle = open(file.c_str(), mode);
}


bool I2C::is_ten_bit_addr(unsigned int addr) const {
	unsigned int v = addr; // 32-bit word to find the log base 2 of
	unsigned int r = 0; // r will be lg(v)

	while (v >>= 1) // unroll for more speed...
	{
	  r++;
	}

	return r == 10;
}



void I2C::write_byte(unsigned int addr, uint8_t sub_addr, unsigned int data) {
	set_ten_bit(is_ten_bit_addr(addr));
	set_slave(addr);
	clear_buffers();
}

uint8_t I2C::read_byte(unsigned int addr, uint8_t sub_addr) {
	set_ten_bit(is_ten_bit_addr(addr));
	set_slave(addr);
	clear_buffers();
	set_tx(sub_addr);
	i2c_write(1);
	if (_op_result != 1) printf("No ACK bit!\n");
	i2c_read(1);
	printf("Part ID: %d\n", (int)_rx_buffer[0]); // should print 105
	return _rx_buffer[0];
}







void I2C::set_ten_bit(bool is_ten_bit) {
	if (is_ten_bit) {
		_ten_bit_address = 1;
	} else {
		_ten_bit_address = 0;
	}
	_op_result = ioctl(_handle, I2C_TENBIT, _ten_bit_address);
}

void I2C::set_slave(char addr) {
	_op_result = ioctl(_handle, I2C_SLAVE, addr);
}

char* I2C::command(char slave_addr, char tx_addr, size_t write_bytes, size_t read_bytes) {
	set_slave(slave_addr);
	return command(tx_addr, write_bytes, read_bytes);
}

char* I2C::command(char tx_addr, size_t write_bytes, size_t read_bytes) {
	clear_buffers();
	set_tx(tx_addr);
	i2c_write(write_bytes);
	i2c_read(read_bytes);
	return _rx_buffer;
}

void I2C::set_tx(char addr) {
	_tx_buffer[0] = addr;
}

void I2C::i2c_write(size_t bytes) {
	_op_result = write(_handle, _tx_buffer, bytes);
}

void I2C::i2c_read(size_t bytes) {
	_op_result = read(_handle, _rx_buffer, bytes);
}

const char* I2C::get_rx_buffer() const {
	return _rx_buffer;
}


void I2C::clear_tx() {
	memset(_tx_buffer, 0, sizeof(_tx_buffer));
}

void I2C::clear_rx() {
	memset(_rx_buffer, 0, sizeof(_rx_buffer));
}

void I2C::clear_buffers() {
	clear_tx();
	clear_rx();
}

} /* namespace i2c */
} /* namespace jetsonhardware */


