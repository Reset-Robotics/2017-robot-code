package org.usfirst.frc.team6325.robot.subsystems;

public enum DrivetrainOrientation {

	CLIMBER(0),
	SHOOTER(Math.PI / 2),
	GEAR(-Math.PI / 2),
	BACK(Math.PI);
	
	private final double driveAngle;
	DrivetrainOrientation(double ang){
		this.driveAngle = ang;
	}
	
	public double angle(){
		return driveAngle;
	}
}
