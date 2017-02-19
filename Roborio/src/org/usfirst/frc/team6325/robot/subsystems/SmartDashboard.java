package org.usfirst.frc.team6325.robot.subsystems;
import edu.wpi.first.wpilibj.command.Subsystem;

public class SmartDashboard extends Subsystem {

        public void displaySmartDashboard() {
                SmartDashboard.putNumber("Front Left Motor Speed", MecanumDrive.getFrontLeftSpeed());
                SmartDashboard.putNumber("Front Right Motor Speed", MecanumDrive.getFrontRightSpeed());
                SmartDashboard.putNumber("Back Left Motor Speed", MecanumDrive.getBackLeftSpeed());
                SmartDashboard.putNumber("Back Right Motor Speed", MecanumDrive.getBackRightSpeed());

                SmartDashboard.putData(MecanumDrive);
                SmartDashboard.putData(GearMechanism);
                SmartDashboard.putData(Shooter);
                SmartDashboard.putData(Climber);

                SmartDashboard.putNumber("Shooter Angle", Shooter.getAngle());
                SmartDashboard.putNumber("Gear Piston Position", GearMechanism.getValue());
        public void initDefaultCommand() {
        }
}
