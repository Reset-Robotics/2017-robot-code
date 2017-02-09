package org.usfirst.frc.team6325.robot.subsystems;

import org.usfirst.frc.team6325.robot.RobotMap;

import com.ctre.CANTalon;
import com.ctre.CANTalon.FeedbackDevice;
import com.ctre.CANTalon.TalonControlMode;

import edu.wpi.first.wpilibj.command.Subsystem;

/**
 *
 */
public class Shooter extends Subsystem {
	
	//motor controllers for the shooter wheel and the aiming motor
	CANTalon aim = new CANTalon(RobotMap.shooterAim);
	CANTalon wheel = new CANTalon(RobotMap.shooterWheel);
	
	//should be called when robot first turns on to activate motor controllers
	public void init(){
		aim.setFeedbackDevice(FeedbackDevice.AnalogEncoder);
		aim.changeControlMode(TalonControlMode.Position);
		aim.set(0);
		
		wheel.setFeedbackDevice(FeedbackDevice.QuadEncoder);
		wheel.changeControlMode(TalonControlMode.Speed);
		wheel.set(0);
	}
	
	public void startSpinning(){
		wheel.set(1.0);
	}
	
	public void stopSpinning(){
		wheel.set(0);
	}
	
	public void setAngle(double angle){
		aim.set(angle);
	}

	public void initDefaultCommand() {
		// Set the default command for a subsystem here.
		// setDefaultCommand(new MySpecialCommand());
	}
}
