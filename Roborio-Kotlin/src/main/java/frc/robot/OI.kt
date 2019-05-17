// Reset Robotics 2019
package frc.robot

// Libraries
import org.sertain.command.*
import org.sertain.hardware.*
import edu.wpi.first.wpilibj.Joystick
import edu.wpi.first.wpilibj.XboxController
import edu.wpi.first.wpilibj.buttons.*
import edu.wpi.first.wpilibj.GenericHID
// Subsystems
//
// Miscellaneous Imports
import frc.robot.commands.Drive.*
import frc.robot.data.OIData
import frc.robot.Util.toggleOnButtonPress


public class OI 
{
    val oiData: OIData = OIData()

    // Joysticks/Controllers
    val joystickLeft by lazy { Joystick((oiData.leftUSBID.id)) }
	val joystickRight by lazy { Joystick((oiData.rightUSBID.id)) }
    val xboxController by lazy { XboxController((oiData.xboxUSBID.id)) }
	val xboxJoystickLeft by lazy { Joystick((oiData.xboxLeftJoystickYAxis.id)) }
    val xboxJoystickRight by lazy { Joystick((oiData.xboxRightJoystickYAxis.id)) }
    
    // Setting default trigger variable values
    var leftTrigger: Double = 0.0
    var rightTrigger: Double = 0.0
    var kLeft: Int = 0
    var kRight: Int = 1

   fun OI()    
   {
        //Driver 1
        //joystickLeft.whenActive(4, ToggleFieldOriented())//Toggles Field Oriented Drive ---- Top-Button-Bottom-Left
        //joystickLeft.whenActive(5, TogglePiston()) //Deploys R-Brake
        //joystickLeft.whenActive(3, Deploy()) //Deploys R-Brake
        //joystickRight.toggleOnButtonPress(oiData.rightTrigger.id, ToggleFieldOriented()) // Toggle whether the drivetrain is field oriented or normal
        //joystickLeft.toggleOnButtonPress(oiData.leftTrigger.id, Deploy()) // Toggle whether the drivetrain is field oriented or normal
    
        //Driver 2
        leftTrigger = xboxController.getTriggerAxis(GenericHID.Hand.kLeft) // 2
        rightTrigger = xboxController.getTriggerAxis(GenericHID.Hand.kRight) // 3
   }
}