/*
 * I2CBusFactory.h
 *
 *  Created on: Feb 5, 2015
 *      Author: trevor
 */

#ifndef I2C_I2CBUSFACTORY_H_
#define I2C_I2CBUSFACTORY_H_

#include <map>

#include "I2CBus.h"

namespace jetsonhardware {
namespace i2c {

class I2CBusFactory {

public:

	I2CBusFactory();
	~I2CBusFactory();

	const I2CBus& get_i2c_bus(std::string file);

private:

	std::map<std::string, I2CBus> _i2c_bus_map;

};

} /* namespace i2c */
} /* namespace jetsonhardware */

#endif /* I2C_I2CBUSFACTORY_H_ */
