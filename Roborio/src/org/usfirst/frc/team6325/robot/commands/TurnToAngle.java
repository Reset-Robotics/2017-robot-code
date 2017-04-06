package org.usfirst.frc.team6325.robot.commands;

import org.usfirst.frc.team6325.robot.Robot;

import edu.wpi.first.wpilibj.command.Command;

/**
 *
 */
public class TurnToAngle extends Command {

	private double a;
	private double t;
	
	private boolean done = false;
	
    public TurnToAngle(double angle, double throttle) {
        a = angle;
        t = throttle;
        
        requires(Robot.mecanumDrive);
    }

    // Called just before this Command runs the first time
    protected void initialize() {
    	
    	Robot.mecanumDrive.turnToAngle(a, t);
    	
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
    }

    // Called when another command which requires one or more of the same
    // subsystems is scheduled to run
    protected void interrupted() {
    }
}
