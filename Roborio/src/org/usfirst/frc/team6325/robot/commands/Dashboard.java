package org.usfirst.frc.team6325.robot.commands;
import org.usfirst.frc.team6325.robot.Robot;
import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

import org.usfirst.frc.team6325.robot.Robot;

public class Dashboard extends Command {

        public Dashboard() {
        }
        
        @Override
        protected void initialize() {
        }
        
        @Override
        protected void execute() {
        	 SmartDashboard.putNumber("Front Left Motor Speed", Robot.mecanumDrive.getFrontLeftSpeed());
             SmartDashboard.putNumber("Front Right Motor Speed", Robot.mecanumDrive.getFrontRightSpeed());
             SmartDashboard.putNumber("Back Left Motor Speed", Robot.mecanumDrive.getBackLeftSpeed());
             SmartDashboard.putNumber("Back Right Motor Speed", Robot.mecanumDrive.getBackRightSpeed());

             SmartDashboard.putData(Robot.mecanumDrive);
             SmartDashboard.putData(Robot.gearMechanism);
             SmartDashboard.putData(Robot.climber);

             //SmartDashboard.putString("Gear Piston Position", Robot.gearMechanism.getValue());
        }
        
        @Override
        protected boolean isFinished() {
                return false;
        }
        
        @Override
        protected void interrupted() {
        }
        
        public static void update(){
        	SmartDashboard.putNumber("Front Left Motor Speed", Robot.mecanumDrive.getFrontLeftSpeed());
            SmartDashboard.putNumber("Front Right Motor Speed", Robot.mecanumDrive.getFrontRightSpeed());
            SmartDashboard.putNumber("Back Left Motor Speed", Robot.mecanumDrive.getBackLeftSpeed());
            SmartDashboard.putNumber("Back Right Motor Speed", Robot.mecanumDrive.getBackRightSpeed());
            
            SmartDashboard.putNumber("Gyro Angle", Robot.mecanumDrive.getAngle());

            SmartDashboard.putData(Robot.mecanumDrive);
            SmartDashboard.putData(Robot.gearMechanism);
            SmartDashboard.putData(Robot.climber);
            
            SmartDashboard.putData("Turn on field oriented drive", new DrivetrainGyroOn());
            SmartDashboard.putData("Turn off field oriented drive", new DrivetrainGyroOff());
            SmartDashboard.putData("Reset gyro", new DrivetrainGyroReset());

            //SmartDashboard.putNumber("Shooter Angle", Robot.shooter.getAngle());
            //SmartDashboard.putString("Gear Piston Position", Robot.gearMechanism.getValue());
        }
}
