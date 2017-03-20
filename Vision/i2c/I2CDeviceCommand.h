/*
 * I2CDeviceCommand.h
 *
 *  Created on: Feb 13, 2015
 *      Author: trevor
 */

#ifndef I2C_I2CDEVICECOMMAND_H_
#define I2C_I2CDEVICECOMMAND_H_

#include <string>

namespace jetsonhardware {
namespace i2c {

class I2CDeviceCommand {

public:

	I2CDeviceCommand(std::string name, char tx_addr, size_t write, size_t read);
	~I2CDeviceCommand();

	/**
	 * Command name, must be unique
	 */
	const std::string& get_name() const;

	/**
	 * The address to transmit data to
	 */
	const char get_tx_addr() const;

	/**
	 * The number of bytes to write
	 */
	const size_t get_write_bytes() const;

	/**
	 * The number of byts to read
	 */
	const size_t get_read_bytes() const;


private:

	std::string _name;
	char _dev_tx_addr;
	size_t _write_bytes;
	size_t _read_bytes;


};

} /* namespace i2c */
} /* namespace jetsonhardware */

#endif /* I2C_I2CDEVICECOMMAND_H_ */
