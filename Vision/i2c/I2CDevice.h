/*
 * I2CDevice.h
 *
 *  Created on: Feb 3, 2015
 *      Author: trevor
 */

#ifndef I2C_I2CDEVICE_H_
#define I2C_I2CDEVICE_H_


#include <map>
#include <string>

#include "I2CBus.h"
#include "I2CDeviceCommand.h"

namespace jetsonhardware {
namespace i2c {

/**
 * Hold data for a single I2C device. Every I2CDevice must be assigned to a bus.
 *
 */
class I2CDevice {

public:

	I2CDevice();
	I2CDevice(I2CBus &bus);

	~I2CDevice();

	/**
	 * Assign the device to a bus
	 */
	void set_i2c_bus(I2CBus &bus);

	/**
	 * Get the Bus with this device
	 */
	const I2CBus& get_i2c_bus() const;

	/**
	 * Send a command to this device
	 *
	 * TODO: check this const usage
	 */
	const char* send_command(unsigned int dev_addr, char tx_addr, size_t write_bytes, size_t read_bytes) const;

//	/**
//	 * Send a command to this device from the command set
//	 */
//	const char* send_command(std::string name) const;


	void add_command(I2CDeviceCommand& cmd);


	std::string to_string();


private:

	/**
	 * The bus this device is on
	 */
	I2CBus _bus;

	/**
	 * A set of regularly used commands for this device
	 */
	std::map<std::string, I2CDeviceCommand> _command_map;

};

} /* namespace i2c */
} /* namespace jetsonhardware */

#endif /* I2C_I2CDEVICE_H_ */
