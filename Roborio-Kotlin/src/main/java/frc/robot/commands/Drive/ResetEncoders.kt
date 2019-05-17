// Reset Robotics 2019
package frc.robot.commands.Drive

// Libraries
import org.sertain.command.Command
// Subsystems
import frc.robot.subsystems.Drivetrain
// Robot Class
import frc.robot.Morpheus


public class ResetEncoders : Command()
{
	init { requires(Drivetrain) } // Make sure we require the Drivetrain subsystem

	override fun execute(): Boolean
	{
		Drivetrain.resetEncoders()
		
		return true;
	}
}