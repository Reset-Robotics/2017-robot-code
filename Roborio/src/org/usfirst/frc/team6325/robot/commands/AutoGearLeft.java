package org.usfirst.frc.team6325.robot.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

/**
 *
 */
public class AutoGearLeft extends CommandGroup {

    public AutoGearLeft() {
        addSequential(new AutoGearSide(60));
    }
}
