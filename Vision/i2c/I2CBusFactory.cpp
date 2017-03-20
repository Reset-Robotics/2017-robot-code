/*
 * I2CBusFactory.cpp
 *
 *  Created on: Feb 5, 2015
 *      Author: trevor
 */

#include "I2CBusFactory.h"

namespace jetsonhardware {
namespace i2c {

I2CBusFactory::I2CBusFactory() {
	// TODO Auto-generated constructor stub

}

I2CBusFactory::~I2CBusFactory() {
	// TODO Auto-generated destructor stub
}

const I2CBus& I2CBusFactory::get_i2c_bus(std::string file) {
	// Check if I2CBus already exists, if not create it
	if (_i2c_bus_map.find(file) != _i2c_bus_map.end()) {
		return _i2c_bus_map[file];
	}

	// create new i2c bus for file
	I2CBus i2c_bus(file);
	_i2c_bus_map[file] = i2c_bus;
	return _i2c_bus_map[file];
}


} /* namespace i2c */
} /* namespace jetsonhardware */
