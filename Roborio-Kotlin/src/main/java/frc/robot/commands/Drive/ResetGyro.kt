// Reset Robotics 2019
package frc.robot.commands.Drive

// Libraries
import org.sertain.command.Command
// Subsystems
import frc.robot.subsystems.Drivetrain
// Robot Class
import frc.robot.Morpheus


public class ResetGyro : Command()
{
	override fun execute(): Boolean
	{
		Drivetrain.resetGyro()

		return true;
	}
}