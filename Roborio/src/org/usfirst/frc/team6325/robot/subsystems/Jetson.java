package org.usfirst.frc.team6325.robot.subsystems;

import org.usfirst.frc.team6325.robot.RobotMap;
import org.zeromq.ZMQ;

public class Jetson implements Runnable{

	private static double leftDisplacement = 0.0;
	private static double rightDisplacement = 0.0;
	
	private Thread t;
	private String threadName;
	
	private ZMQ.Socket subscriber = null;
	
	public Jetson(String name){
		threadName = name;
	}
	
	public void run() {
		String data = subscriber.recvStr(0).trim();
    	String[] strings = data.split(" ");
    			
    	String str = strings[1].trim().replaceAll("[^\\d|\\.|-]", "");
    	double L = Double.parseDouble(str);
    	
    	String str2 = strings[1].trim().replaceAll("[^\\d|\\.|-]", "");
    	double R = Double.parseDouble(str2);
    	
    	setLeftDisplacement(L);
    	setRightDisplacement(R);
    	
    	System.out.println("vision displacement: L: " + L + " R: " + R);
	}
	
	public void start () {
		ZMQ.Context context = ZMQ.context(1);
		subscriber = context.socket(ZMQ.SUB);
		
		subscriber.connect("tcp://" + RobotMap.jetsonIP + ":5801");
		
		String filter = "displacement";
		subscriber.subscribe(filter.getBytes());
		
		System.out.println("Starting " +  threadName );
		
	    if (t == null) {
	    	t = new Thread (this, threadName);
	    	t.start ();
	    }
	}
	
	public static double getLeftDisplacement(){
		return leftDisplacement;
	}
	
	public static double getRightDisplacement(){
		return rightDisplacement;
	}
	
	synchronized private static void setLeftDisplacement(double l){
		leftDisplacement = l;
	}
	
	synchronized private static void setRightDisplacement(double r){
		rightDisplacement = r;
	}
}

