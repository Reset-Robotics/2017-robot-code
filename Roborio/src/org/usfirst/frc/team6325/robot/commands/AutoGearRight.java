package org.usfirst.frc.team6325.robot.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

/**
 *
 */
public class AutoGearRight extends CommandGroup {

    public AutoGearRight() {
        addSequential(new AutoGearSide(-60));
    }
}
