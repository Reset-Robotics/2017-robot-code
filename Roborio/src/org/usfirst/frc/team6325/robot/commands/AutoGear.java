package org.usfirst.frc.team6325.robot.commands;

import edu.wpi.first.wpilibj.command.Command;

import org.usfirst.frc.team6325.robot.Robot;

/**
 *
 */

// distance = targetHeightinInches (5 inches) * imageWidth(640) / (2*targetHeightPixels*tan(cameraHorizontalAngle/2)
public class AutoGear extends Command {
	boolean done = false;
	
	public AutoGear() {
		// Use requires() here to declare subsystem dependencies
		requires(Robot.mecanumDrive);
		requires(Robot.GearMechanism);
	}

	// Called just before this Command runs the first time
	@Override
	protected void initialize() 
	{
		
	}

	// Called repeatedly when this Command is scheduled to run
	@Override
	protected void execute() 
	{
		while(currentDistanceToPeg > neededDistanceToPeg)
		{
			Robot.mecanumDrive.cartesianDrive(0, 1, 0, 0.5);
		}
		//Should probably add code that lines up robot on X axis
		Robot.GearMechanism.open();
		Timer.delay(1);
		Robot.mecanumDrive.cartesianDrive(-0.25, 0, 0, 0.5);
		Timer.delay(0.25);
		Robot.mecanumDrive(cartesianDrive(0, -1, 0, 0.5);
		Timer.delay(2);
		Robot.GearMechanism.close();
		done = true;
				 
				   
			
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
