package org.usfirst.frc.team6325.robot.commands;

import edu.wpi.first.wpilibj.command.Command;

import org.usfirst.frc.team6325.robot.Robot;
import org.usfirst.frc.team6325.robot.RobotMap;

/**
 *
 */
public class AutoShooter extends Command {
	boolean done = false;
	
	public AutoShooter() {
		// Use requires() here to declare subsystem dependencies
		requires(Robot.mecanumDrive);
		requires(Robot.shooter);
	}

	// Called just before this Command runs the first time
	@Override
	protected void initialize() {
		boolean inRange = Robot.jetsonInterlink.getBoolean("inRange");
		
		if(!inRange){
			done = true;
		}
	}

	// Called repeatedly when this Command is scheduled to run
	@Override
	protected void execute() {
		if(Robot.oi.joySecondary.getRawButton(1)){
			Robot.shooter.startSpinning();
			Robot.shooter.startIndexer();
		} else {
			Robot.shooter.stopIndexer();
			Robot.shooter.stopSpinning();
		}
		
		if(Robot.oi.joyDriver.getX() > 0.2 || Robot.oi.joyDriver.getY() > 0.02){
			done = true;
			return;
		}
		
		double angle = Robot.jetsonInterlink.getNumber("angle");
		double distance = Robot.jetsonInterlink.getNumber("distance");
		
		if(distance > RobotMap.maxRange){
			done = true;
			return;
		}
		
		Robot.shooter.setAngle(RobotMap.getAngle(distance));
		
		Robot.mecanumDrive.cartesianDrive(0.0, 0.0, angle * 0.1, 0.5);
	}

	// Make this return true when this Command no longer needs to run execute()
	@Override
	protected boolean isFinished() {
		return done;
	}

	// Called once after isFinished returns true
	@Override
	protected void end() {
	}

	// Called when another command which requires one or more of the same
	// subsystems is scheduled to run
	@Override
	protected void interrupted() {
	}
}
