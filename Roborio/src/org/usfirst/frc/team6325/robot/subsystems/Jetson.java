package org.usfirst.frc.team6325.robot.subsystems;

import edu.wpi.first.wpilibj.command.Subsystem;
import java.util.StringTokenizer;

import org.usfirst.frc.team6325.robot.RobotMap;
import org.zeromq.ZMQ;

/**
 *
 */
public class Jetson extends Subsystem {

	private double displacement = 0.0;
	
    public void init(){
    	ZMQ.Context context = ZMQ.context(1);
		ZMQ.Socket subscriber = context.socket(ZMQ.SUB);
		
		subscriber.connect("tcp://" + RobotMap.jetsonIP + ":5801");
		
		String filter = "displacement";
		subscriber.subscribe(filter.getBytes());
    }
    
    public void update(){
    	/*String data = subscriber.recvStr(0).trim();
		
		String[] strings = data.split(" ");
		
		String str = strings[1].trim().replaceAll("[^\\d|\\.|-]", "");
		double num = Double.parseDouble(str);*/
    }

    public void initDefaultCommand() {
        // Set the default command for a subsystem here.
        //setDefaultCommand(new MySpecialCommand());
    }
}

