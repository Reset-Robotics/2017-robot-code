// Reset Robotics 2019
package frc.robot

// Libraries
import org.sertain.*
import org.sertain.command.and
import edu.wpi.first.wpilibj.command.Scheduler
import edu.wpi.first.wpilibj.I2C
import edu.wpi.first.wpilibj.PWM
import edu.wpi.cscore.UsbCamera;
import edu.wpi.first.wpilibj.CameraServer;
// Subsystems
import frc.robot.subsystems.Drivetrain

// Miscellaneous Imports
import frc.robot.commands.Drive.ToggleFieldOriented


public class Morpheus : Robot()
{
    public fun main(args: String)
    {
    }

    // Initialize subsystem instance objects for this script
    public val drivetrain: Drivetrain = Drivetrain

    // OI Initialization
    public var oi: OI = OI()

    // Runs on robot initialization; WPILib robotInit() equivalent
    override fun onCreate()
    {
        drivetrain.onCreate()
    	//camera0.setResolution(320, 240)
        //camera0.setFPS(30)
   
        // put any data to dashboard here
    }

    // Runs periodically when the robot is disabled; WPILib disabledPeriodic() equivalent
    override fun executeDisabled()
    {
        drivetrain.unlockAngle()

        // any dashboard data populatin here too
    }

    // Runs on autonomous(sandstorm) initialization; WPILib autonomousInit() equivalent
    override fun onAutoStart()
    {
        drivetrain.onCreate()
    }

    // Runs periodically during autonomous(sandstorm); WPILib autonomousPeriodic() equivalent
    override fun executeAuto()
    {
        // put any dashboard data
    }

    // Runs on teleop initialization; WPILib teleopInit() equivalent
    override fun onStart()
    {
        drivetrain.onCreate()
    }
     

    // Runs periodically during teleop; WPILib teleopPeriodic() equivalent
    override fun executeTeleop()
    {
        oi.OI()

        // put dashboard data here
    }
}