// Reset Robotics 2019
package frc.robot.commands.Drive

// Libraries
import org.sertain.command.Command
// Subsystems
import frc.robot.subsystems.Drivetrain
// Miscellaneous Imports
import frc.robot.data.DrivetrainData
import frc.robot.OI


public class ArcadeJoystickDrive : Command()
{
    init { requires(Drivetrain) } // Make sure we require the Drivetrain subsystem, as its necessary for default commands

    // Run all our code here
    override fun execute(): Boolean
    {              
        val driveData: DrivetrainData = DrivetrainData()

        var yDirection: Double = OI().joystickLeft.getY()
        var xDirection: Double = OI().joystickLeft.getX()
        var spin: Double = OI().joystickLeft.getTwist()
        var throttle: Double = ((OI().joystickLeft.getThrottle()*-1)+1)/2

        if (Math.abs(yDirection) < driveData.deadzone) yDirection = 0.0 
        if (Math.abs(xDirection) < driveData.deadzone) xDirection = 0.0 
        if (Math.abs(spin) < 0.17) 
        {
            spin = 0.0 
           Drivetrain.lockAngle()
        }
        else Drivetrain.unlockAngle()
        
        Drivetrain.drive(xDirection, -yDirection, spin, throttle)
        return false;
    }

    // Safely stop motors if the command is interrupted or destroyed
    override fun onDestroy() = Drivetrain.killMotors()
}