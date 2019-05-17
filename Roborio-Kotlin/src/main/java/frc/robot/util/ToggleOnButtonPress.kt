// Reset Robotics 2019
package frc.robot.Util

// Libraries
import edu.wpi.first.wpilibj.GenericHID
import edu.wpi.first.wpilibj.Joystick
import edu.wpi.first.wpilibj.buttons.JoystickButton
import org.sertain.command.Command
import org.sertain.command.CommandBase


public fun GenericHID.toggleOnButtonPress(button: Int, command: CommandBase)
{
    JoystickButton(this, button).toggleWhenPressed(command.mirror)
}