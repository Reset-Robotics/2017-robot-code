package org.usfirst.frc.team6325.robot.commands;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.command.Command;

import org.usfirst.frc.team6325.robot.Robot;

/**
 *
 */
public class AutoMoveForward extends Command {
	boolean done = false;
	
	public AutoMoveForward() {
		// Use requires() here to declare subsystem dependencies
		requires(Robot.mecanumDrive);
	}

	// Called just before this Command runs the first time
	@Override
	protected void initialize() {
		Robot.mecanumDrive.cartesianDrive(0.5, 0.0,  0.0, 1.0);
		Timer.delay(2.5);
		done = true;
	}

	// Called repeatedly when this Command is scheduled to run
	@Override
	protected void execute() {
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
