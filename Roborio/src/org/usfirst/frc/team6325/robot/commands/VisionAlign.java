package org.usfirst.frc.team6325.robot.commands;

import org.usfirst.frc.team6325.robot.Robot;
import org.usfirst.frc.team6325.robot.RobotMap;
import org.zeromq.ZMQ;

import edu.wpi.first.wpilibj.Timer;
import edu.wpi.first.wpilibj.command.Command;

/**
 *
 */
public class VisionAlign extends Command {

	private final double visionMoveSensetivity = 0.001;
	private ZMQ.Socket subscriber = null;
	
	private boolean done = false;
	
    public VisionAlign() {
        // Use requires() here to declare subsystem dependencies
        requires(Robot.mecanumDrive);
    }

    // Called just before this Command runs the first time
    protected void initialize() {
    	Robot.mecanumDrive.lockAngle(0.0);
    	
    	ZMQ.Context context = ZMQ.context(1);
		subscriber = context.socket(ZMQ.SUB);
		
		subscriber.connect("tcp://" + RobotMap.jetsonIP + ":5801");
		
		String filter = "displacement";
		subscriber.subscribe(filter.getBytes());
    }

    // Called repeatedly when this Command is scheduled to run
    protected void execute() {
		String data = subscriber.recvStr(0).trim();
    	String[] strings = data.split(" ");
    			
    	String str = strings[1].trim().replaceAll("[^\\d|\\.|-]", "");
    	double offset = Double.parseDouble(str);
    	
    	Robot.mecanumDrive.drive(0.0, offset * visionMoveSensetivity, 0.0, 0.5);
    	
    	if(Math.abs(offset) < 5.0){
    		done = true;
    		Robot.mecanumDrive.killMotors();
    	}
    }

    // Make this return true when this Command no longer needs to run execute()
    protected boolean isFinished() {
        return done;
    }

    // Called once after isFinished returns true
    protected void end() {
    }

    // Called when another command which requires one or more of the same
    // subsystems is scheduled to run
    protected void interrupted() {
    }
}
