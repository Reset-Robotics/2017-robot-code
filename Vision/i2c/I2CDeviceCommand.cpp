/*
 * I2CDeviceCommand.cpp
 *
 *  Created on: Feb 13, 2015
 *      Author: trevor
 */

#include "I2CDeviceCommand.h"

namespace jetsonhardware {
namespace i2c {

I2CDeviceCommand::I2CDeviceCommand(std::string name, char tx_addr, size_t write, size_t read) :
	_name(name),
	_dev_tx_addr(tx_addr),
	_write_bytes(write),
	_read_bytes(read) {

}

I2CDeviceCommand::~I2CDeviceCommand() {
}

const std::string& I2CDeviceCommand::get_name() const {
	return _name;
}

const char I2CDeviceCommand::get_tx_addr() const {
	return _dev_tx_addr;
}

const size_t I2CDeviceCommand::get_write_bytes() const {
	return _write_bytes;
}

const size_t I2CDeviceCommand::get_read_bytes() const {
	return _read_bytes;
}


} /* namespace i2c */
} /* namespace jetsonhardware */
