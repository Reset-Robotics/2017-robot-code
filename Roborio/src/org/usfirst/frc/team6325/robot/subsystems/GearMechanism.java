package org.usfirst.frc.team6325.robot.subsystems;

import edu.wpi.first.wpilibj.Compressor;
import edu.wpi.first.wpilibj.DoubleSolenoid;
import edu.wpi.first.wpilibj.DoubleSolenoid.Value;
import edu.wpi.first.wpilibj.command.Subsystem;

public class GearMechanism extends Subsystem {

	Compressor compressor = new Compressor(21);
	
	DoubleSolenoid leftPiston = new DoubleSolenoid(21, 0, 1);
	DoubleSolenoid rightPiston = new DoubleSolenoid(21, 2, 3);
	DoubleSolenoid pusher = new DoubleSolenoid(21, 4, 5);
	
	public void init(){
		compressor.setClosedLoopControl(true);
		close();
	}
	
	public void open(){
		leftPiston.set(DoubleSolenoid.Value.kReverse);
		rightPiston.set(DoubleSolenoid.Value.kReverse);
	}
	
	public void close(){
		leftPiston.set(DoubleSolenoid.Value.kForward);
		rightPiston.set(DoubleSolenoid.Value.kForward);
	}
	
	public void extendPusher(){
		pusher.set(DoubleSolenoid.Value.kForward);
	}
	public void retractPusher(){
		pusher.set(DoubleSolenoid.Value.kReverse);
	}
	
	public String getValue() {
		Value val = leftPiston.get();
		
		if(val == Value.kForward){
			return "Closed";
		}else{
			return "Open";
		}
	}
	
	public void initDefaultCommand() {

		// setDefaultCommand(new MySpecialCommand());
	}
}

