package org.usfirst.frc.team6325.robot.subsystems;

import org.usfirst.frc.team6325.robot.Robot;

import edu.wpi.first.wpilibj.command.Subsystem;
import edu.wpi.first.wpilibj.networktables.NetworkTable;

/**
 *
 */
public class JetsonInterlink extends Subsystem {
	
	NetworkTable table = Robot.table;
	
	public double getNumber(String key){
		double num = table.getNumber(key, 0);
		return num;
	}
	
	public String getString(String key){
		String s = table.getString(key, "");
		return s;
	}
	
	public boolean getBoolean(String key){
		boolean b = table.getBoolean(key, false);
		return b;
	}

	public void initDefaultCommand() {
		// Set the default command for a subsystem here.
		// setDefaultCommand(new MySpecialCommand());
	}
}
