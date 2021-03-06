
package org.usfirst.frc.team6325.robot;

import edu.wpi.first.wpilibj.IterativeRobot;
import edu.wpi.first.wpilibj.command.Command;
import edu.wpi.first.wpilibj.command.Scheduler;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.networktables.NetworkTable;
import edu.wpi.first.wpilibj.smartdashboard.SendableChooser;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

import org.usfirst.frc.team6325.robot.commands.Dashboard;
import org.usfirst.frc.team6325.robot.commands.LockAngle;
import org.usfirst.frc.team6325.robot.commands.VisionTest;
import org.usfirst.frc.team6325.robot.commands.AutoGearCenter;
import org.usfirst.frc.team6325.robot.commands.AutoGearLeft;
import org.usfirst.frc.team6325.robot.commands.AutoGearRight;
import org.usfirst.frc.team6325.robot.commands.AutoMoveForward;
import org.usfirst.frc.team6325.robot.subsystems.Climber;
import org.usfirst.frc.team6325.robot.subsystems.GearMechanism;
import org.usfirst.frc.team6325.robot.subsystems.Jetson;
import org.usfirst.frc.team6325.robot.subsystems.MecanumDrive;
import edu.wpi.cscore.UsbCamera;
import edu.wpi.first.wpilibj.CameraServer;
/**
 * The VM is configured to automatically run this class, and to call the
 * functions corresponding to each mode, as described in the IterativeRobot
 * documentation. If you change the name of this class or the package after
 * creating this project, you must also update the manifest file in the resource
 * directory.
 */
public class Robot extends IterativeRobot {

	public static final MecanumDrive mecanumDrive = new MecanumDrive();
	public static final GearMechanism gearMechanism = new GearMechanism();
	public static final Climber climber = new Climber();
	
	public static Jetson jetson = null;
	
	public static OI oi;
	
	public static NetworkTable table;

	Command autonomousCommand;
	SendableChooser<Command> chooser = new SendableChooser<>();
	UsbCamera camera0;
	//UsbCamera camera2;



	/**
	 * This function is run when the robot is first started up and should be
	 * used for any initialization code.
	 */
	@Override
	public void robotInit() {
		oi = new OI();

		chooser.addDefault("Move forward", new AutoMoveForward());
		chooser.addObject("Gear center peg", new AutoGearCenter());
		chooser.addObject("Gear left peg", new AutoGearLeft());
		chooser.addObject("Gear right peg", new AutoGearRight());
		chooser.addObject("gyro test", new LockAngle(90));
		chooser.addObject("vision test", new VisionTest());
		
		mecanumDrive.init();
  
		// chooser.addObject("My Auto", new MyAutoCommand());
		SmartDashboard.putData("Auto mode", chooser);
		UsbCamera camera0  = CameraServer.getInstance().startAutomaticCapture(); //http://roborio-6325-frc.local:1181/?action=stream
		//UsbCamera camera2 = CameraServer.getInstance().startAutomaticCapture();//http://roborio-6325-frc.local:1182/?action=stream
		camera0.setResolution(320, 240); // Jetson Gears
	
		
		table = NetworkTable.getTable("LiftTracker");
		
		jetson = new Jetson("jetsonThread");
		jetson.start();
		
		Dashboard.update();
	}

	/**
	 * This function is called once each time the robot enters Disabled mode.
	 * You can use it to reset any subsystem information you want to clear when
	 * the robot is disabled.
	 */
	@Override
	public void disabledInit() {
		mecanumDrive.unlockAngle();
		mecanumDrive.setFieldOriented(false);
	}

	@Override
	public void disabledPeriodic() {
		Scheduler.getInstance().run();
		Dashboard.update();
	}

	/**
	 * This autonomous (along with the chooser code above) shows how to select
	 * between different autonomous modes using the dashboard. The sendable
	 * chooser code works with the Java SmartDashboard. If you prefer the
	 * LabVIEW Dashboard, remove all of the chooser code and uncomment the
	 * getString code to get the auto name from the text box below the Gyro
	 *
	 * You can add additional auto modes by adding additional commands to the
	 * chooser code above (like the commented example) or additional comparisons
	 * to the switch structure below with additional strings & commands.
	 */
	@Override
	public void autonomousInit() {
		mecanumDrive.init();
		autonomousCommand = chooser.getSelected();

		/*
		 * String autoSelected = SmartDashboard.getString("Auto Selector",
		 * "Default"); switch(autoSelected) { case "My Auto": autonomousCommand
		 * = new MyAutoCommand(); break; case "Default Auto": default:
		 * autonomousCommand = new ExampleCommand(); break; }
		 */

		// schedule the autonomous command (example)
		if (autonomousCommand != null)
			autonomousCommand.start();
	}

	/**
	 * This function is called periodically during autonomous
	 */
	@Override
	public void autonomousPeriodic() {
		Scheduler.getInstance().run();
		
		Dashboard.update();
	}

	@Override
	public void teleopInit() {
		
		// This makes sure that the autonomous stops running when
		// teleop starts running. If you want the autonomous to
		// continue until interrupted by another command, remove
		// this line or comment it out.
		if (autonomousCommand != null)
			autonomousCommand.cancel();
	}

	/**
	 * This function is called periodically during operator control
	 */
	@Override
	public void teleopPeriodic() {
		Scheduler.getInstance().run();
		
		Dashboard.update();
			
	
	}

	/**
	 * This function is called periodically during test mode
	 */
	@Override
	public void testPeriodic() {
		LiveWindow.run();
	}
}
