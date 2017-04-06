package org.usfirst.frc.team6325.robot.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

/**
 *
 */
public class AutoGearSide extends CommandGroup {

	private final double firstMoveTime = 1.0; //how long the robot will move before aligning
	private final double secondMoveTime = 2.0; //how long the robot will move after aligning
	private final double visionMoveSensetivity = 0.001; //how fast the robot will try to align with the peg. should be very small
	private final double throttle = 0.5; //how fast the robot will move
	private final double turnThrottle = 0.5; //how fast the robot will turn
	
    public AutoGearSide(double angle) {
    	addSequential(new LockAngle(0.0));
        addSequential(new MoveTime(throttle, 0.0, firstMoveTime));
        addSequential(new TurnToAngle(angle, turnThrottle));
        addSequential(new LockAngle(angle));
        addSequential(new VisionAlign());
        addSequential(new MoveTime(throttle, 0.0, secondMoveTime));
        addSequential(new UnlockAngle());
    }
}
