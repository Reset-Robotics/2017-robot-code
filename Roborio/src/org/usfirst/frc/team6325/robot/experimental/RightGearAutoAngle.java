package org.usfirst.frc.team6325.robot.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

/**
 * 
 */
public class RightGearAutoAngle extends CommandGroup {
	
	private final double distanceUntilTurn = 42;
	private final double turnTilVisionAngle = -20; // angle to turn to see vision tape
	
	public RightGearAutoAngle() {
        
    	addSequential(new DriveStraight(distanceUntilTurn)); // Need a drive straight
    	addSequential(new Turn(turnTilVisionAngle)); // turn to see tape (navX)
      addSequential(new AutoGear()); // Command to angle itself based off vision and drive straight onto peg
    }
}
