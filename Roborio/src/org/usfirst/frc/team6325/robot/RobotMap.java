package org.usfirst.frc.team6325.robot;
import java.io.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;

/**
 * The RobotMap is a mapping from the ports sensors and actuators are wired into
 * to a variable name. This provides flexibility changing wiring, makes checking
 * the wiring easier and significantly reduces the number of magic numbers
 * floating around.
 */
public class RobotMap {
	// For example to map the left and right motors, you could define the
	// following variables to use with your drivetrain subsystem.
	// public static int leftMotor = 1;
	// public static int rightMotor = 2;

	// If you are using multiple modules, make sure to define both the port
	// number and the module. For example you with a rangefinder:
	// public static int rangefinderPort = 1;
	// public static int rangefinderModule = 1;
	
	//CAN IDs for the mecanum wheel motors
	public static int frontRight = 2;
	public static int frontLeft = 3;
	public static int backRight = 1;
	public static int backLeft = 0;
	
	//CAN ID for the shooter's aim controller
	public static int shooterAim = 4;
	//CAN ID for the shooter's launch wheel controller
	public static int shooterWheel = 7;
	public static int shooterIndexer = 6;
	
	public static int climber = 5;
	
	
	public static int maxRange = 10;
	
	public static String networkTableName = "LiftTracker"; //the name of the NetworkTable the jetson is sending data to
	public static String distanceValueName = "CenterDist"; //the name of the variable that contains the peg distance
	
	
	public static Map<Double, Double> shooterAngles = new HashMap<Double, Double>();
	public static Map<String, Double> customData = new HashMap<String, Double>();
	public static ArrayList<Double> distances;
	
	
	//some really crazy shit for the ball shooter that we don't have
	public static void readData(){
		FileInputStream file = null;
		ObjectInputStream in = null;
		
		try {
			file = new FileInputStream("/home/lvuser/shooterAngles.ser");
			in = new ObjectInputStream(file);
			
			shooterAngles = (HashMap<Double, Double>) in.readObject();
			
			in.close();
			file.close();
		} catch (FileNotFoundException e) {
			
		} catch (IOException e) {

		} catch (ClassNotFoundException e) {

		}
		
		try {
			file = new FileInputStream("/home/lvuser/customData.ser");
			in = new ObjectInputStream(file);
			
			customData = (HashMap<String, Double>) in.readObject();
			
			file.close();
			in.close();
		} catch (FileNotFoundException e) {
			
		} catch (IOException e) {

		} catch (ClassNotFoundException e) {

		}
	}
	
	public static void saveData(){
		FileOutputStream file = null;
		ObjectOutputStream out = null;
		
		try {
			file = new FileOutputStream("/home/lvuser/shooterAngles.ser");
			out = new ObjectOutputStream(file);
			
			out.writeObject(shooterAngles);
			
			out.close();
			file.close();
		} catch (FileNotFoundException e) {
			
		} catch (IOException e) {
			
		}
		
		try {
			file = new FileOutputStream("/home/lvuser/customData.ser");
			out = new ObjectOutputStream(file);
			
			out.writeObject(customData);
			
			out.close();
			file.close();
		} catch (FileNotFoundException e) {
			
		} catch (IOException e) {
			
		}
	}
	
	public static void updateDistances(){
		distances = new ArrayList(new TreeSet(shooterAngles.keySet()));
	}
	
	public static double getAngle(double distance){
		double lower = 0;
		double higher = 0;
		double ratio = 0;
		
		if(distance > distances.get(distances.size() - 1)){
			return shooterAngles.get(distances.get(distances.size() - 1));
		}
		
		if(distance < distances.get(0)){
			return shooterAngles.get(distances.get(0));
		}
		
		for(int i = 0; i < distances.size() - 1; i++){
			if(distances.get(i) < distance && distance < distances.get(i + 1)){
				lower = distances.get(i);
				higher = distances.get(i + 1);
				
				ratio = (distance - lower) / (higher - lower);
			}
		}
		
		return shooterAngles.get(lower) + (shooterAngles.get(higher) - shooterAngles.get(lower)) * ratio;
	}
}
