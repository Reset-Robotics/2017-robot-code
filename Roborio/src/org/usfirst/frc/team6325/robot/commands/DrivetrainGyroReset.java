package org.usfirst.frc.team6325.robot.commands;

import edu.wpi.first.wpilibj.command.Command;

import org.usfirst.frc.team6325.robot.Robot;

/**
 *
 */
public class DrivetrainGyroReset extends Command {
	public DrivetrainGyroReset() {
		// Use requires() here to declare subsystem dependencies
		//requires(Robot.mecanumDrive);
	}

	// Called just before this Command runs the first time
	@Override
	protected void initialize() {
		Robot.mecanumDrive.resetGyro();
	}

	// Called repeatedly when this Command is scheduled to run
	@Override
	protected void execute() {
	}

	// Make this return true when this Command no longer needs to run execute()
	@Override
	protected boolean isFinished() {
		return true;
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
