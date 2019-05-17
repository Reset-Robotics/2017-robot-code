// Reset Robotics 2019
package frc.robot.commands.Drive

// Libraries
import org.sertain.command.Command
// Subsystems
import frc.robot.subsystems.Drivetrain


public class ToggleAngleLock: Command()
{
	override fun execute(): Boolean
	{
		Drivetrain.lockAngle()
		
		return true;
	}
}