package org.usfirst.frc.team5262.robot.commands;

import edu.wpi.first.wpilibj.command.Command;
import org.usfirst.frc.team6325.robot.Robot;

public class Autonomous extends Command {

    public GearAutonomous() {
        requires(Robot.mecanumDrive);
        requires(Robot.GearMechanism);
    }

	protected void initialize() {
        int pos = 0; // position of robot at start of match: -1, 0, 1
        boolean done = false;
        if(pos == -1) {
            Robot.mecanumDrive.cartesianDrive(0, 1, 0, 0.5);
            Timer.delay(1);
            Robot.mecanumDrive.cartesiandrive(0, 0, 0.5, 1);
			Timer.delay(1.5);
            while(centerbetweentapesx < centerx) {
                Robot.mecanumDrive.cartesianDrive(-1, 0, 0, 0.5);
            }
        }
        if(pos == 1) {
            Robot.mecanumDrive.cartesianDrive(0, 1, 0, 0.5);
            Timer.delay(1);
            Robot.mecanumDrive.cartesianDrive(0, 0, -0.5, 1);
			Timer.delay(1.5)
            while(centerbetweentapesx > centerx) {
                Robot.mecanumDrive.cartesianDrive(1, 0, 0, 0.5);
            }
        }
        while(distancebetweentapes < certainamount) {
            Robot.mecanumDrive.cartesianDrive(0, 1, 0, 0.5);
            while(centerbetweentapesx < centerx) {
                Robot.mecanumDrive.cartesianDrive(-1, 0, 0, 0.5);
            }
            while(centerbetweentapesx > centerx) {
                Robot.mecanumDrive.cartesianDrive(1, 0, 0, 0.5);
            }
        }
        Robot.mecanumDrive.killMotors();
        Robot.GearMechanism.open();
        while(centerbetweentapesx < wherecenterissupposedright) {
            Robot.mecanumDrive.cartesianDrive(-1, 0, 0, 0.5);
        }
		Robot.mecanumDrive.cartesianDrive(0, -1, 0, 0.5);
		Timer.delay(1);
		Robot.mecanumDrive.killMotors();
        Robot.GearMechanism.close();
        done = true;
	}

	// Called repeatedly when this Command is scheduled to run
	@Override
	protected void execute() {
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


