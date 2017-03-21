package org.usfirst.frc.team6325.robot.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

/**
 *Turns the bot via a camera, then drives to the gear, and then drops the gear
 *onto the peg.
 */
public class AutoGear extends CommandGroup {

    public AutoGear() {
        addSequential(new VisionTurn()); // use navX to turn off jetson data
        addSequential(new DriveStraight(60)); // drive onto peg
    }
}
