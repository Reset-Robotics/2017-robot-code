package org.usfirst.frc.team6325.robot.commands;

import org.usfirst.frc.team6325.robot.Robot;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.command.Command;

/**
 *
 */
public class MoveTime extends Command {
	
	private double f;
	private double l;
	private double t;
	
	private double startTime;
	
	private boolean done = false;

    public MoveTime(double forward, double left, double time) {
    	f = forward;
    	l = left;
    	t = time;

        requires(Robot.mecanumDrive);
    }

    // Called just before this Command runs the first time
    protected void initialize() {
    	Robot.mecanumDrive.drive(f, l, 0, 1);
    	
    	startTime = Timer.getMatchTime();
    }

    // Called repeatedly when this Command is scheduled to run
    protected void execute() {
    	Robot.mecanumDrive.drive(f, l, 0, 1);
    	
    	if(Timer.getMatchTime() - startTime > t){
    		done = true;
    		Robot.mecanumDrive.killMotors();
    	}
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
