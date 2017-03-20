/*
 * I2CBus.cpp
 *
 *  Created on: Feb 3, 2015
 *      Author: trevor
 */

#include "I2CBus.h"

namespace jetsonhardware {
namespace i2c {

const std::string I2CBus::GEN1_I2C = "/dev/i2c-0";
const std::string I2CBus::GEN2_I2C_33V = "/dev/i2c-1";
const std::string I2CBus::CAM1_I2C_33V = "/dev/i2c-2";
const std::string I2CBus::PWR_I2C = "/dev/i2c-4";


I2CBus::I2CBus() {
}

I2CBus::I2CBus(std::string file) : _bus_file(file) {
	_i2c.i2c_open(file);
}

I2CBus::~I2CBus() {
}

const I2C& I2CBus::get_bus_i2c() const {
	return _i2c;
}

const std::string& I2CBus::get_bus_file() const {
	return _bus_file;
}

std::string I2CBus::to_string() {
	std::string s;
	s += "I2CBus [bus file: " + _bus_file + "]";
	return s;
}

} /* namespace i2c */
} /* namespace jetsonhardware */

