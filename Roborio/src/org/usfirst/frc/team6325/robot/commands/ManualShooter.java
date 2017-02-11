package org.usfirst.frc.team6325.robot.commands;

import edu.wpi.first.wpilibj.command.Command;

import org.usfirst.frc.team6325.robot.Robot;

/**
 *
 */
public class ManualShooter extends Command {
	public ManualShooter() {
		requires(Robot.shooter);
	}

	// Called just before this Command runs the first time
	@Override
	protected void initialize() {
	}

	// Called repeatedly when this Command is scheduled to run
	@Override
	protected void execute() {
		if(Robot.oi.joySecondary.getRawButton(1)){
			Robot.shooter.startSpinning();
			Robot.shooter.startIndexer();
		} else {
			Robot.shooter.stopSpinning();
			Robot.shooter.stopIndexer();
		}
		
		Robot.shooter.setAngle(Robot.shooter.getAngle() + Robot.oi.joyDriver.getY() * 0.1);
	}

	// Make this return true when this Command no longer needs to run execute()
	@Override
	protected boolean isFinished() {
		return false;
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
