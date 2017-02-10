package org.usfirst.frc.team6325.robot;

/**
 * The RobotMap is a mapping from the ports sensors and actuators are wired into
 * to a variable name. This provides flexibility changing wiring, makes checking
 * the wiring easier and significantly reduces the number of magic numbers
 * floating around.
 */
public class RobotMap {
	// For example to map the left and right motors, you could define the
	// following variables to use with your drivetrain subsystem.
	// public static int leftMotor = 1;
	// public static int rightMotor = 2;

	// If you are using multiple modules, make sure to define both the port
	// number and the module. For example you with a rangefinder:
	// public static int rangefinderPort = 1;
	// public static int rangefinderModule = 1;
	
	//CAN IDs for the mecanum wheel motors
	public static int frontRight = 0;
	public static int frontLeft = 1;
	public static int backRight = 2;
	public static int backLeft = 3;
	
	//CAN ID for the shooter's aim controller
	public static int shooterAim = 4;
	//CAN ID for the shooter's launch wheel controller
	public static int shooterWheel = 5;
	
	public static int climber = 6;
}
