package org.usfirst.frc.team6325.robot.commands;
import edu.wpi.first.wpilibj.command.Command;
import org.usfirst.frc.team6325.robot.Robot;

public class SmartDashboard extends Command {

        public SmartDashboard() {
                requires(Robot.SmartDashboard);
        }
        
        @Override
        protected void initialize() {
        }
        
        @Override
        protected void execute() {
                Robot.SmartDashboard.displaySmartDashboard();
        }
        
        @Override
        protected boolean isFinished() {
                return false;
        }
        
        @Override
        protected void interrupted() {
        }
}
