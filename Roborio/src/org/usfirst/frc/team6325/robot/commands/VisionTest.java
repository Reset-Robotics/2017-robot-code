package org.usfirst.frc.team6325.robot.commands;

import edu.wpi.first.wpilibj.command.CommandGroup;

/**
 *
 */
public class VisionTest extends CommandGroup {

    public VisionTest() {
        addSequential(new VisionAlign());
    }
}
