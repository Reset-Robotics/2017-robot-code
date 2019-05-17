// Reset Robotics 2019
package frc.robot.subsystems

// Libraries
import org.sertain.command.Subsystem
import org.sertain.RobotLifecycle
import edu.wpi.cscore.UsbCamera
import com.ctre.phoenix.motorcontrol.*
import com.ctre.phoenix.motorcontrol.can.*
import edu.wpi.first.wpilibj.CameraServer
import frc.robot.data.ClimberData
import frc.robot.commands.Climber.ClimberJoystickDrive


public object Climber : Subsystem() 
{
    val climberData: ClimberData = ClimberData()

    val motor: WPI_TalonSRX = WPI_TalonSRX(climberData.motor)
	
	fun drive(power: Double)
    {
		motor.set(power)
	}

    override val defaultCommand = ClimberJoystickDrive()
}