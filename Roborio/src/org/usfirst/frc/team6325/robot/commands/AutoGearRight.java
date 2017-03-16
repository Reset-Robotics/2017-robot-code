package org.usfirst.frc.team6325.robot.commands;

import org.usfirst.frc.team6325.robot.Robot;
import org.usfirst.frc.team6325.robot.RobotMap;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.networktables.NetworkTable;

/**
 *
 */
public class AutoGearRight extends Command {
	
	private final double firstMoveTime = 1.0; //how long the robot will move before aligning
	private final double secondMoveTime = 2.0; //how long the robot will move after aligning
	private final double angle = -45.0; //the angle the robot will turn to face the peg.
	private final double visionMoveSensetivity = 0.001; //how fast the robot will try to align with the peg. should be very small
	private final double throttle = 0.5; //how fast the robot will move
	private final double turnThrottle = 0.5; //how fast the robot will turn
	
	boolean done = false;

    public AutoGearRight() {
        // Use requires() here to declare subsystem dependencies
        requires(Robot.mecanumDrive);
    }

    // Called just before this Command runs the first time
    protected void initialize() {
NetworkTable table = NetworkTable.getTable(RobotMap.networkTableName);
    	
    	Robot.mecanumDrive.lockAngle(0.0);
    	
    	//drive forward
    	Robot.mecanumDrive.drive(1.0, 0.0, 0.0, throttle);
    	Timer.delay(firstMoveTime);
    	Robot.mecanumDrive.killMotors();
    	
    	//wait for robot to stop
    	Timer.delay(0.5);
    	
    	//turn to face peg
    	Robot.mecanumDrive.turnToAngle(angle, turnThrottle);
    	
    	//wait for robot to stop
    	Timer.delay(0.5);
    	
    	//retrieve distance data from jetson
    	double offset = table.getNumber(RobotMap.distanceValueName, 0.0);
    	
    	//align with peg
    	while(Math.abs(offset) > 5.0){
    		Robot.mecanumDrive.drive(0.0, offset * visionMoveSensetivity, 0.0, throttle);
    		offset = table.getNumber(RobotMap.distanceValueName, 0.0);
    	}
    	
    	//drive into peg
    	Robot.mecanumDrive.drive(1.0, 0.0, 0.0, throttle);
    	Timer.delay(secondMoveTime);
    	Robot.mecanumDrive.killMotors();
    	
    	done = true;
    }

    // Called repeatedly when this Command is scheduled to run
    protected void execute() {
    }

    // Make this return true when this Command no longer needs to run execute()
    protected boolean isFinished() {
        return done;
    }

    // Called once after isFinished returns true
    protected void end() {
    	Robot.mecanumDrive.killMotors();
    }

    // Called when another command which requires one or more of the same
    // subsystems is scheduled to run
    protected void interrupted() {
    	Robot.mecanumDrive.killMotors();
    }
}
