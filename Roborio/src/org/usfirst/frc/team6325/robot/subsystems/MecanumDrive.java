package org.usfirst.frc.team6325.robot.subsystems;

import edu.wpi.first.wpilibj.command.Subsystem;

import org.usfirst.frc.team6325.robot.Robot;
import org.usfirst.frc.team6325.robot.RobotMap;
import org.usfirst.frc.team6325.robot.commands.JoystickDrive;

import com.ctre.CANTalon;

//this is the subsystem code for the mecanum drivetrain

public class MecanumDrive extends Subsystem {

	//motor controllers for all 4 wheels
	CANTalon[] motors = {
			new CANTalon(RobotMap.frontLeft),
			new CANTalon(RobotMap.backLeft),
			new CANTalon(RobotMap.backRight),
			new CANTalon(RobotMap.frontRight)
	};
	
	private DrivetrainOrientation orientation = DrivetrainOrientation.CLIMBER;
	private final double WHEEL_RADIUS = 3.0;
	
	//drive the robot using the joystick
	public void joystickDrive(){
		double x = Robot.oi.joyDriver.getX();
		double y = Robot.oi.joyDriver.getY();
		double r = Robot.oi.joyDriver.getTwist();
		double t = Robot.oi.joyDriver.getThrottle();
		
		double angle = Math.atan(y / x);
		double radius = Math.sqrt(Math.pow(x, 2) + Math.pow(y, 2));
		
		angle += orientation.angle();
		
		double s = radius * t;
		
		polarDrive(angle, s, r);
	}
	//control the robot using seperate inputs for x, y, and rotation
	public void cartesianDrive(double x, double y, double r, double throttle){
		
		//calculate the speed of each wheel
		double wheels [] = {
			(x + y + r) * 0.5,
			(x - y + r) * 0.5,
			-((x + y - r) * 0.5),
			-((x - y - r) * 0.5),	
		};
		
		//make sure motor values are never greater than 100% so we don't break the motors
		for(int i = 0; i < 4; i++){
			wheels[i] = Math.min(Math.max(wheels[i], -1.0), 1.0);
		}
		
		//send values to motors
		for(int i = 0; i < 4; i++){
			motors[i].set(wheels[i]);
		}
	}
	
	//control the robot using angle and speed inputs
	public void polarDrive(double angle, double speed, double r){
		angle += Math.PI / 2;
		
		double x = Math.cos(angle) * speed;
		double y = Math.sin(angle) * speed;
		
		cartesianDrive(x, y, r, 1.0);
	}
	
	//stop all motors in case shit gets real
	public void killMotors(){
		for(int i = 0; i < 4; i++){
			motors[i].set(0.0);
		}
	}
	
	public void changeOrientation(DrivetrainOrientation o){
		this.orientation = o;
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

}
