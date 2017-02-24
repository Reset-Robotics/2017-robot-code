package org.usfirst.frc.team6325.robot.subsystems;

import edu.wpi.first.wpilibj.Compressor;
import edu.wpi.first.wpilibj.DoubleSolenoid;
import edu.wpi.first.wpilibj.command.Subsystem;

public class GearMechanism extends Subsystem {

	Compressor compressor = new Compressor(0);
	
	DoubleSolenoid piston = new DoubleSolenoid(1, 2);
	
	public void init(){
		compressor.setClosedLoopControl(true);
		close();
	}
	
	public void open(){
		piston.set(DoubleSolenoid.Value.kForward);
	}
	
	public void close(){
		piston.set(DoubleSolenoid.Value.kReverse);
	}
	
	public Value getValue() {
		return piston.get();
	}
	
	public void initDefaultCommand() {

		// setDefaultCommand(new MySpecialCommand());
	}
}

