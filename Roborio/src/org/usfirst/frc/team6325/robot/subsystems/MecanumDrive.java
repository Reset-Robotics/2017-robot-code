package org.usfirst.frc.team6325.robot.subsystems;

import edu.wpi.first.wpilibj.ADXRS450_Gyro;
import edu.wpi.first.wpilibj.PIDController;
import edu.wpi.first.wpilibj.PIDOutput;
import edu.wpi.first.wpilibj.SerialPort;
import edu.wpi.first.wpilibj.command.Subsystem;

import org.usfirst.frc.team6325.robot.Robot;
import org.usfirst.frc.team6325.robot.RobotMap;
import org.usfirst.frc.team6325.robot.commands.JoystickDrive;

import com.ctre.CANTalon;
import com.ctre.CANTalon.FeedbackDevice;
import com.kauailabs.navx.frc.AHRS;

//this is the subsystem code for the mecanum drivetrain

public class MecanumDrive extends Subsystem implements PIDOutput{
	
	private final double wheelCircumfrence = 18.8495559215;
	private final double deadzone = 0.1;
	
	//the PID values used for turning to a specific angle
	private final double turnP = 0.1;
	private final double turnI = 0.0;
	private final double turnD = 0.0;
	private final double turnF = 0.0;
	//how close the robot will try to get to the target angle before stopping
	private final double turnThreshold = 2.0;
	
	private double driveAngle = 0.0;
	
	//if the robot is currently using field oriented driving
	private boolean fieldOriented = false;
	
	//if the robot is currently using a specified angle
	private boolean angleLocked = false;
	
	private double turnRate = 0.0;
	
	//motor controllers for all 4 wheels
	CANTalon[] motors = {
			new CANTalon(RobotMap.frontLeft),
			new CANTalon(RobotMap.backLeft),
			new CANTalon(RobotMap.backRight),
			new CANTalon(RobotMap.frontRight)
	};
	
	/*
	The robot knows where it is at all times. It knows this because it knows where it isn't. By 
	subtracting where it is from where it isn't, or where it isn't from where it is (whichever is 
	greater), it obtains a difference, or deviation. The guidance subsystem uses deviations to 
	generate corrective commands to drive the robot from a position where it is to a position 
	where it isn't, and arriving at a position where it wasn't, it now is. Consequently, the position 
	where it is, is now the position that it wasn't, and it follows that the position that it was, is now 
	the position that it isn't. In the event that the position that it is in is not the position that it 
	wasn't, the system has acquired a variation, the variation being the difference between 
	where the robot is, and where it wasn't. If variation is considered to be a significant factor, it 
	too may be corrected by the NavX. However, the robot must also know where it was.The 
	robot guidance computer scenario works as follows. Because a variation has modified some 
	of the information the robot has obtained, it is not sure just where it is. However, it is sure 
	where it isn't, within reason, and it knows where it was. It now subtracts where it should be 
	from where it wasn't, or vice-versa, and by differentiating this from the algebraic sum of 
	where it shouldn't be, and where it was, it is able to obtain the deviation and its variation, 
	which is called error.
	*/
	private AHRS navx = new AHRS(SerialPort.Port.kMXP);
	
	//pid controller for turning to a specific angle
	private PIDController turnController = new PIDController(turnP, turnI, turnD, turnF, navx, this);
	
	//sets up various values. should be called when the robot first turns on.
	public void init(){
		for(CANTalon talon : motors){
			talon.setFeedbackDevice(FeedbackDevice.CtreMagEncoder_Relative);
		}
		
		turnController.setInputRange(-180.0f,  180.0f);
	    turnController.setOutputRange(-1.0, 1.0);
	    turnController.setAbsoluteTolerance(turnThreshold);
	    turnController.setContinuous(true);
	    
	    resetGyro();
	}
	
	//drive the robot using the joystick
	public void joystickDrive(){
		double forward = -Robot.oi.joyDriver.getY();
		double left = Robot.oi.joyDriver.getX();
		double spin = Robot.oi.joyDriver.getTwist() * 0.75;
		double throttle = 1.0;  //Robot.oi.joyDriver.getThrottle();
		
		if(Math.abs(forward) < deadzone) forward = 0;
		if(Math.abs(left) < deadzone) left = 0;
		if(Math.abs(spin) < deadzone) spin = 0;

		drive(forward, left, spin, throttle);
		/*if(fieldOriented && angleLocked){
			fieldOrientedDriveAtAngle(forward, left, driveAngle, throttle);
		}
		else if(fieldOriented){
			fieldOrientedDrive(forward, left, spin, throttle);
		}
		else if(angleLocked){
			driveAtAngle(forward, left, driveAngle, throttle);
		}
		else{
			cartesianDrive(forward, left, spin, throttle);
		}*/
	}
	
	/**
	 * Drives the robot. will modify the coordinates if field oriented drive or angle lock are turned on.
	 * @param forward How fast the robot should move in the X direction (1.0 = full speed forward, -1.0 = full speed backwards)
	 * @param left How fast the robot should move in the Y direction (1.0 = full speed left, -1.0 = full speed right)
	 * @param spin  How fast the robot should spin along its center of mass (1.0 = full speed clockwise, -1.0 = full speed counter-clockwise)
	 * @param throttle Multiplies all inputs by a set value, usually just used for joystick throttle input
	 */
	public void drive(double forward, double left, double spin, double throttle){
		
		if(fieldOriented){
			double angle = navx.getAngle() * Math.PI / 180;
			
			double rotatedForward = forward * Math.cos(angle) + left * Math.sin(angle);
			double rotatedLeft = -forward * Math.sin(angle) + left * Math.cos(angle);
			
			forward = rotatedForward;
			left = rotatedLeft;
		}
		
		if(angleLocked){
			spin = turnRate;
		}
		
		cartesianDrive(forward, left, spin, throttle);
	}
	
	/**
	 * Just drives the robot directly, ignoring the field oriented and angle locked settings.
	 * @param forward How fast the robot should move in the X direction (1.0 = full speed forward, -1.0 = full speed backwards)
	 * @param left How fast the robot should move in the Y direction (1.0 = full speed left, -1.0 = full speed right)
	 * @param spin  How fast the robot should spin along its center of mass (1.0 = full speed clockwise, -1.0 = full speed counter-clockwise)
	 * @param throttle Multiplies all inputs by a set value, usually just used for joystick throttle input
	 */
	public void cartesianDrive(double forward, double left, double spin, double throttle){
		
		//calculate the speed of each wheel
		double wheels [] = {
			(-forward - left + spin) * throttle,
			(-forward + left + spin) * throttle,
			-((-forward - left - spin) * throttle),
			-((-forward + left - spin) * throttle),	
		};
		
		//make sure motor values are never greater than 100% so we don't break the motors
		double max = 0;
		for(double v : wheels){
			if(Math.abs(v) > max) max = Math.abs(v);
		}
		
		if(max > 1.0){
			for(double v : wheels){
				v = v / max;
			}
		}
		
		//send values to motors
		for(int i = 0; i < 4; i++){
			motors[i].set(wheels[i]);
		}
	}
	
	/**
	 * No matter which direction the robot is facing, it will move relative to its original position
	 * (e.g. setting forward to 1.0 will always make the robot move away from the driver)
	 * @param forward the robot's X direction velocity
	 * @param left the robot's Y direction velocity
	 * @param spin the robot's angular velocity
	 * @param throttle speed multiplier
	 */
	public void fieldOrientedDrive(double forward, double left, double spin, double throttle){
		double angle = navx.getAngle() * Math.PI / 180;
		
		double rotatedForward = forward * Math.cos(angle) + left * Math.sin(angle);
		double rotatedLeft = -forward * Math.sin(angle) + left * Math.cos(angle);
		
		cartesianDrive(rotatedForward, rotatedLeft, spin, throttle);
	}
	
	/**
	 * the robot's heading will be locked to a specific angle while still allowing the robot to drive in any other direction
	 * @param forward the robot's X direction velocity, relative to the front of the robot
	 * @param left the robot's Y directon velocity, relative to the front of the robot
	 * @param angle the angle in degrees that the robot will face, relative to the field (-180 to +180, 0 degrees is straight forward)
	 * @param throttle speed multiplier
	 */
	public void driveAtAngle(double forward, double left, double angle, double throttle){
		if(!turnController.isEnabled()){
			turnController.enable();
		}
		
		if(turnController.getSetpoint() != angle){
			turnController.setSetpoint(angle);
		}
		
		double spin = -getAngle() * 0.02;
		
		cartesianDrive(forward, left, spin, throttle);
	}
	
	/**
	 * the robot's heading will be locked to a specific angle while still allowing the robot to drive in any other direction, using field oriented drive
	 * @param forward the robot's X direction velocity, relative to the front of the driver station
	 * @param left the robot's Y directon velocity, relative to the front of the driver station
	 * @param angle the angle in degrees that the robot will face (-180 to +180, 0 degrees is straight forward)
	 * @param throttle speed multiplier
	 */
	public void fieldOrientedDriveAtAngle(double forward, double left, double angle, double throttle){
		double gyroAngle = navx.getAngle() * Math.PI / 180; //convert degrees to radians
		
		//rotate the coordinates by the gyro angle
		double rotatedForward = forward * Math.cos(gyroAngle) + left * Math.sin(gyroAngle);
		double rotatedLeft = -forward * Math.sin(gyroAngle) + left * Math.cos(gyroAngle);
		
		driveAtAngle(rotatedForward, rotatedLeft, angle, throttle);
	}
	
	/**
	 * the robot will turn to face a specific angle, then stop.
	 * @param angle the angle the robot will face
	 * @param throttle how fast the robot will turn towards that angle
	 */
	public void turnToAngle(double angle, double throttle){
		killMotors();
		
		lockAngle(angle);
		
		while(Math.abs(navx.getAngle() - turnController.getSetpoint()) > turnThreshold){
			drive(0.0, 0.0, angle, throttle);
		}
		
		killMotors();
	}
	
	/**
	 * the robot will drive for a certain amount of inches, then stop.
	 * WARNING: this method is currently very basic. it is not extremely accurate and only works when driving at 90 degree angles (straight forward, straight left, etc.)
	 * @param angle the direction the robot will drive (0 is straight forward, 90 is left, -90 is right)
	 * @param targetInches the distance the robot will move in inches
	 * @param speed how fast the robot will move
	 */
	public void driveDistance(double angle, double targetInches, double speed){
		motors[0].setPosition(0);
		double distance = 0;
		
		while(distance < targetInches){
			distance = motors[0].getPosition() * wheelCircumfrence;
			polarDrive(angle, 0.0, speed);
		}
		
		killMotors();
	}
	
	/**
	 * drive the robot in a specific direction at a specific speed
	 * NOTE: 
	 * @param angle the direction the robot will drive (0 is straight forward, 90 is left, -90 is right, 180 is backward, or anything in between)
	 * @param spin how fast the robot will spin. (don't use this. it is not field oriented and will accomplish nothing but confusion)
	 * @param speed how fast the robot will move
	 */
	public void polarDrive(double angle, double spin, double speed){
		angle += 90;
		angle = angle * Math.PI / 180;
		
		double forward = Math.cos(angle) * speed;
		double left = Math.sin(angle) * speed;
		
		cartesianDrive(forward, left, spin, 1.0);
	}
	
	//stop all motors in case shit gets real
	public void killMotors(){
		for(int i = 0; i < 4; i++){
			motors[i].set(0.0);
		}
	}
	
	public double getAngle(){
		return navx.getAngle();
	}
	
	public boolean getFieldOriented(){
		return fieldOriented;
	}
	
	//turn field oriented drive on or off
	public void setFieldOriented(boolean use){
		fieldOriented = use;
	}
	
	public void resetGyro(){
		navx.zeroYaw();
	}
	
	//locks the robot's heading to a specified angle
	public void lockAngle(double newAngle){
		driveAngle = newAngle;
		
		turnController.enable();
		turnController.setSetpoint(driveAngle);
		
		angleLocked = true;
	}
	
	//allows the robot to start spinning again
	public void unlockAngle(){
		turnController.disable();
		
		angleLocked = false;
	}
	
	public double getFrontLeftSpeed() {
		return motors[0].get();
	}
	
	public double getFrontRightSpeed() {
		return motors[3].get();
	}
	
	public double getBackLeftSpeed() {
		return motors[1].get();
	}
	
	public double getBackRightSpeed() {
		return motors[2].get();
	}
	
	@Override
	protected void initDefaultCommand() {
		setDefaultCommand(new JoystickDrive());
	}
	
	@Override
	public void pidWrite(double output) {
		turnRate = output;
	}

}
