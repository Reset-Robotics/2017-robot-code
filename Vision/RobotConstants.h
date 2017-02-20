/*
 * RobotConstants.h
 */

#ifndef SRC_ROBOTCONSTANTS_H_
#define SRC_ROBOTCONSTANTS_H_


//set these to correct values after drivetrain fixed
constexpr int TOLERANCE = 10;
constexpr int IMG_WIDTH = 640;
constexpr int IMG_HEIGHT = 480;
constexpr int targetHeight4FeetFromHighGoal = 39;
constexpr int targetHeight4FeetFromGearPeg = 121;
constexpr double cameraWidth2Feet = 29.9375;
constexpr double pi = 3.1415926535898;
constexpr double radiansPerSec = 7.0; //Just a guess. The number of radians
									  //the robot can turn a second by doing
									  //drivetrain->set(1.0,-1.0) or vice versa
constexpr int bestDistFromHighGoal = 36; //this value may be off
constexpr double robotSpeed = 120; //inches per second, may be off

bool isAiming = false;
bool isGearing = false;

//4 feet = height 121 for gear peg
//4 feet = height 39 for high goal 4 inch high tape

//24 in = 29.9375 in width


// constexpr int RANGE_FINDER_MODULE = 1;

#endif  // ROBOTMAP_H

#endif /* SRC_ROBOTCONSTANTS_H_ */
