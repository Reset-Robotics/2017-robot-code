// Reset Robotics 2019
package frc.robot.data


public data class OIData(val doesMorpheusWork: Boolean = false)
{
    public data class NamedIDs(val name: String, val id: Int)

    // Driver Controls/OI
    val leftUSBID = NamedIDs("Left-Joystick-USB-ID", 0)
    val leftXAxis = NamedIDs("Left-Joystick-X-Axis", 0)
    val leftYAxis = NamedIDs("Left-Joystick-Y-Axis", 1)
    val leftZAxis = NamedIDs("Left-Joystick-Z-Axis", 2)
    val leftTrigger = NamedIDs("Left-Joystick-Trigger", 1)
    val leftSideThumb = NamedIDs("Left-Joystick-Side-Thumb", 2)
    val leftTopButtonBottomRight = NamedIDs("Left-Joystick-Top-Button-Bottom-Right", 3)
    val leftTopButtonBottomLeft = NamedIDs("Left-Joystick-Top-Button-Bottom-Left", 4)
    val leftTopButtonTopLeft = NamedIDs("Left-Joystick-Top-Button-Top-Left", 5)
    val leftTopButtonTopRight = NamedIDs("Left-Joystick-Top-Button-Top-Right", 6)
    val leftSliderAxis = NamedIDs("Left-Joystick-SliderAxis", 3)

    val rightUSBID = NamedIDs("Right-Joystick-USB-ID", 1)
    val rightXAxis = NamedIDs("Right-Joystick-X-Axis", 0)
    val rightYAxis = NamedIDs("Right-Joystick-Y-Axis", 1)
    val rightZAxis = NamedIDs("Right-Joystick-Z-Axis", 2)
    val rightTrigger = NamedIDs("Right-Joystick-Trigger", 1)
    val rightSideThumb = NamedIDs("Right-Joystick-Side-Thumb", 2)
    val rightTopButtonBottomRight = NamedIDs("Right-Joystick-Top-Button-Bottom-Right", 3)
    val rightTopButtonBottomLeft = NamedIDs("Right-Joystick-Top-Button-Bottom-Left", 4)
    val rightTopButtonTopLeft = NamedIDs("Right-Joystick-Top-Button-Top-Left", 5)
    val rightTopButtonTopRight = NamedIDs("Right-Joystick-Top-Button-Top-Right", 6)

    val xboxUSBID = NamedIDs("Xbox-USB-ID", 2)
    val xboxAButton = NamedIDs("Xbox-A-Button", 1)
    val xboxBButton = NamedIDs("Xbox-B-Button", 2)
    val xboxXButton = NamedIDs("Xbox-X-Button", 3)
    val xboxYButton = NamedIDs("Xbox-Y-Button", 4)
    val xboxLeftBumper = NamedIDs("Xbox-Left-Bumper", 5)
    val xboxRightBumper = NamedIDs("Xbox-Right-Bumper", 6)
    val xboxLeftBumperAxis = NamedIDs("Xbox-Left-Bumper-Axis", 2)
    val xboxRightBumperAxis = NamedIDs("Xbox-Right-Bumper-Axis", 3)
    var xboxLeftTriggerAxis = NamedIDs("Xbox-Left-Trigger-Axis", 3)
    var xboxRightTriggerAxis = NamedIDs("Xbox-Right-Trigger-Axis", 2)
    val xboxBackButton = NamedIDs("Xbox-Back-Button", 7)
    val xboxLeftJoystickButton = NamedIDs("Xbox-Left-Joystick-Button", 8)
    val xboxRightJoystickButton = NamedIDs("Xbox-Right-Joystick-Button", 9) 
    val xboxLeftJoystickYAxis = NamedIDs("Xbox-Left-Joystick-Y-Axis", 1)
    val xboxRightJoystickYAxis = NamedIDs("Xbox-Right-Joystick-Y-Axis", 5)
}