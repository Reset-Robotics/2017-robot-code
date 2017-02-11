package org.usfirst.frc.team6325.robot.commands;

import edu.wpi.first.wpilibj.command.Command;

import org.usfirst.frc.team6325.robot.Robot;

/**
 *
 */
public class OpenGear extends Command {
	
	boolean done = false;
	
	public OpenGear() {
		// Use requires() here to declare subsystem dependencies
		requires(Robot.gearMechanism);
	}

	// Called just before this Command runs the first time
	@Override
	protected void initialize() {
		Robot.gearMechanism.open();
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
