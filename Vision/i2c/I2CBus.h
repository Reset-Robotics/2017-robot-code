/*
 * I2CBus.h
 *
 *  Created on: Feb 3, 2015
 *      Author: trevor
 */

#ifndef I2C_I2CBUS_H_
#define I2C_I2CBUS_H_

#include <string>
#include <vector>

#include "I2C.h"

/**
 *
 * Manage a set of slate I2C devices from a single
 * master source.
 *
 */

namespace jetsonhardware {
namespace i2c {

class I2CBus {

public:

	static const std::string GEN1_I2C;
	static const std::string GEN2_I2C_33V;
	static const std::string CAM1_I2C_33V;
	static const std::string PWR_I2C;


	/**
	 * Never instantiate this class directly. Instead use
	 * the I2CDeviceFactory to ensure that duplicate bus
	 * objects are not created.
	 */
	I2CBus();
	I2CBus(std::string file);
	~I2CBus();

	/**
	 * Return the I2C interface associated with this bus
	 */
	const I2C& get_bus_i2c() const;

	/**
	 * Return the file this bus communicates through
	 */
	const std::string& get_bus_file() const;


	std::string to_string();

private:

	std::string _bus_file;

	I2C _i2c;

};

} /* namespace i2c */
} /* namespace jetsonhardware */

#endif /* I2C_I2CBUS_H_ */
