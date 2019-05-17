// Reset Robotics 2019
package frc.robot.commands.Drive

// Libraries
import org.sertain.command.Command
// Subsystems
import frc.robot.subsystems.Drivetrain


public class ToggleFieldOriented : Command()
{
    override fun execute(): Boolean
    {
        //Drivetrain.toggleFieldOriented()

        return true;
    }

    override fun onCreate()
    {
        Drivetrain.toggleFieldOriented()
    }
}