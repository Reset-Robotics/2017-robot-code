package org.usfirst.frc.team6325.robot.subsystems;
import edu.wpi.first.wpilibj.command.Subsystem;

public class SmartDashboard extends Subsystem {

        public void frontLeftSpeed() {
                SmartDashboard.putNumber("Front Left Motor Speed", MecanumDrive.getFrontLeftSpeed());
        }
        public void frontRightSpeed() {
                SmartDashboard.putNumber("Front Right Motor Speed", MecanumDrive.getFrontRightSpeed());
        }
        public void backLeftSpeed() {
                SmartDashboard.putNumber("Back Left Motor Speed", MecanumDrive.getBackLeftSpeed());
        }
        public void backRightSpeed() {
                SmartDashboard.putNumber("Back Right Motor Speed", MecanumDrive.getBackRightSpeed());
        }
        
        public void mecanumDriveCommands() {
                SmartDashboard.putData(MecanumDrive);
        }
        public void gearMechanismCommands() {
                SmartDashboard.putData(GearMechanism);
        }
        public void shooterCommands() {
                SmartDashboard.putData(Shooter);
        }
        public void climberCommands() {
                SmartDashboard.putData(Climber);
        }
        
        public void shooterAngle() {
                SmartDashboard.putNumber("Shooter Angle", Shooter.getAngle());
        }
        public void pistonPosition() {
                SmartDashboard.putNumber("Gear Piston Position", GearMechanism.getValue());
        }
        
        public void initDefaultCommand() {
        }
}
