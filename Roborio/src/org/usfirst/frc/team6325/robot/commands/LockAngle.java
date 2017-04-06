package org.usfirst.frc.team6325.robot.commands;

import org.usfirst.frc.team6325.robot.Robot;

import edu.wpi.first.wpilibj.command.Command;

/**
 *
 */
public class LockAngle extends Command {

	private double ang;
	
    public LockAngle(double angle) {
    	ang = angle;
        // Use requires() here to declare subsystem dependencies
        // eg. requires(chassis);
    }

    // Called just before this Command runs the first time
    protected void initialize() {
    	Robot.mecanumDrive.lockAngle(ang);
    }

    // Called repeatedly when this Command is scheduled to run
    protected void execute() {
    }

    // Make this return true when this Command no longer needs to run execute()
    protected boolean isFinished() {
        return true;
    }

    // Called once after isFinished returns true
    protected void end() {
    }

    // Called when another command which requires one or more of the same
    // subsystems is scheduled to run
    protected void interrupted() {
    }
}
