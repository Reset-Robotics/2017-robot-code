// Reset Robotics 2019
package frc.robot.commands.Climber

// Libraries
import org.sertain.command.Command
// Subsystems
import frc.robot.subsystems.Climber
// Miscellaneous Imports
import frc.robot.data.ClimberData
import frc.robot.OI


public class ClimberJoystickDrive : Command()
{
    init { requires(Climber) } // Make sure we require the Climber subsystem, as its necessary for default commands

    // Run all our code here
    override fun execute(): Boolean
    {              
        val climberData: ClimberData = ClimberData()

        var yDirection: Double = OI().joystickRight.getY()

        if (Math.abs(yDirection) < climberData.deadzone) yDirection = 0.0 
        
        Climber.drive(yDirection)
        return false;
    }
}