// Reset Robotics 2019
package frc.robot.subsystems

// Libraries
import org.sertain.command.Subsystem
import com.ctre.phoenix.motorcontrol.*
import com.ctre.phoenix.motorcontrol.can.*
import com.kauailabs.navx.frc.AHRS
import edu.wpi.first.wpilibj.PIDController
import edu.wpi.first.wpilibj.PIDOutput
import edu.wpi.first.wpilibj.SPI
import edu.wpi.first.wpilibj.Timer
import edu.wpi.first.wpilibj.PWM
import edu.wpi.first.wpilibj.DigitalInput
// Robot Class
import frc.robot.Morpheus
// Miscellaneous Imports
import frc.robot.data.DrivetrainData
import frc.robot.commands.Drive.ArcadeJoystickDrive


public object Drivetrain : Subsystem(), PIDOutput
{
    val driveData: DrivetrainData = DrivetrainData()

    // Drive motors
    val driveFrontLeft: WPI_TalonSRX = WPI_TalonSRX(driveData.motorFrontLeft)
    val driveFrontRight: WPI_TalonSRX = WPI_TalonSRX(driveData.motorFrontRight)
    val driveBackLeft: WPI_TalonSRX = WPI_TalonSRX(driveData.motorBackLeft)
    val driveBackRight: WPI_TalonSRX = WPI_TalonSRX(driveData.motorBackRight)

    // PID values for turning to angles
    var turnRate: Double = driveData.turnRate
    val turnThreshold: Double = 2.0 // How many degrees the robot has to be within for it to stop looking for the required angle
    var driveAngle: Double = driveData.driveAngle
    val wheelCircumference: Double = 18.8495559215
    val wheelRadius: Double = 0.1524/2 // 6 inch mecanum wheel diamter in meters

    // Misc Variables/Objects
    var deltaT: Long = 0.0.toLong()
    //val ultrasonic1: UltrasonicBase = UltrasonicBase(0)
    var isPulsed: Boolean = false
    var prevSysTime: Long = 0.0.toLong()
    //var ultrasonicDistance: Long = 0.0.toLong()
    val navx: AHRS = AHRS(SPI.Port.kMXP) // "the robot knows where it is at all times."
    var turnController: PIDController = PIDController(driveData.pidP, driveData.pidI, driveData.pidD, driveData.pidF, navx, this, 0.05)
    var isFieldOriented: Boolean = false
    var isAngleLocked: Boolean = false
    var isDriftMode: Boolean = false 
   
    // Dead Reckon Values
    var deadReckonX: Double = 0.0
    var deadReckonY: Double = 0.0
    val rpmToRad: Double = 9.5492965964254
    var rotatedXVelocity: Double = 0.0
    var rotatedYVelocity: Double = 0.0
    //var isProfileFinished: Boolean = false
    //var angleDeadzone: Double = 3.0 
    

    override fun onCreate()
    {
        // Set up encoders
        //this.driveFrontLeft.configSelectedFeedbackSensor(FeedbackDevice.CTRE_MagEncoder_Relative, 0, 0)
        //this.driveFrontRight.configSelectedFeedbackSensor(FeedbackDevice.CTRE_MagEncoder_Relative, 0, 0)
        //this.driveBackLeft.configSelectedFeedbackSensor(FeedbackDevice.CTRE_MagEncoder_Relative, 0, 0)
        //this.driveBackRight.configSelectedFeedbackSensor(FeedbackDevice.CTRE_MagEncoder_Relative, 0, 0)   

        // Configure PID loop
        turnController.setInputRange(-180.0, 180.0)
        turnController.setOutputRange(-1.0, 1.0)
        turnController.setAbsoluteTolerance(driveData.turnThreshold)
        turnController.setContinuous(true)

        //ultrasonic1.enableInterrupts()
        
        // Zero gyro yaw
        resetGyro() 
    }


    fun Drivetrain()
    {
        // Set Talon Mode
        this.driveFrontLeft.setNeutralMode(NeutralMode.Coast)
        this.driveFrontRight.setNeutralMode(NeutralMode.Coast)
        this.driveBackLeft.setNeutralMode(NeutralMode.Coast)
        this.driveBackRight.setNeutralMode(NeutralMode.Coast)
		
        // Current Limiting
		this.driveFrontLeft.configContinuousCurrentLimit(40,0) // desired current after limit
		this.driveFrontLeft.configPeakCurrentLimit(35, 0) // max current
		this.driveFrontLeft.configPeakCurrentDuration(100, 0) // how long after max current to be limited (ms)
		this.driveFrontLeft.enableCurrentLimit(true)
		this.driveFrontRight.configContinuousCurrentLimit(40,0)
		this.driveFrontRight.configPeakCurrentLimit(35, 0)
		this.driveFrontRight.configPeakCurrentDuration(100, 0)
		this.driveFrontRight.enableCurrentLimit(true)
		this.driveBackLeft.configContinuousCurrentLimit(40,0)
		this.driveBackLeft.configPeakCurrentLimit(35, 0)
		this.driveBackLeft.configPeakCurrentDuration(100, 0)
		this.driveBackLeft.enableCurrentLimit(true)
		this.driveBackRight.configContinuousCurrentLimit(40,0)
		this.driveBackRight.configPeakCurrentLimit(35, 0)
		this.driveBackRight.configPeakCurrentDuration(100, 0)
		this.driveBackRight.enableCurrentLimit(true)   

        // Reset Sensors
		resetGyro()
        resetEncoders()
    }

    fun drive(yVal: Double, xVal: Double, spinVal: Double, throttleVal: Double)
    {
        var localYVal: Double = yVal
        var localXVal: Double = xVal
        var localSpinVal: Double = spinVal
        var localFieldOriented: Boolean = getFieldOriented()

        if(localFieldOriented)
        {
            var angle: Double = navx.getAngle() * Math.PI / 180
            var rotatedYVal: Double = yVal * Math.cos(angle) + xVal * Math.sin(angle)
            var rotatedXVal: Double = -yVal * Math.sin(angle) + xVal * Math.cos(angle)

            localYVal = rotatedYVal
            localXVal = rotatedXVal
        }
        
        if(isAngleLocked && !isDriftMode) 
            localSpinVal = turnRate

        cartesianDrive(localYVal, localXVal, localSpinVal, throttleVal)
    }

    fun cartesianDrive(yVal: Double, xVal: Double, spinVal: Double, throttleVal: Double)
    {
        var localDriftMode: Boolean = getDriftMode()

        val wheels: DoubleArray = doubleArrayOf(((-yVal - xVal + spinVal) * throttleVal), 
        (-((-yVal + xVal + spinVal) * throttleVal)), 
        ((-yVal + xVal - spinVal) * throttleVal), 
        (-((-yVal - xVal - spinVal) * throttleVal)))

        var max: Double = 0.0
        for(v: Double in wheels)
            if(Math.abs(v) > max) max = Math.abs(v)

        if(max > 1.0)
        {   
            wheels[0] = wheels[0] / max
            wheels[1] = wheels[1] / max
            wheels[2] = wheels[2] / max
            wheels[3] = wheels[3] / max
        }
        if(localDriftMode)
        {
            driveFrontLeft.set(wheels[0])
            driveFrontRight.set(-wheels[1])
            driveBackLeft.set(-yVal)
            driveBackRight.set(yVal)
        }
        if(!localDriftMode)
        {
            driveFrontLeft.set(wheels[0])
            driveFrontRight.set(-wheels[1])
            driveBackLeft.set(-wheels[2])
            driveBackRight.set(wheels[3])
        }
    }

    fun fieldOrientedDrive(yVal: Double, xVal: Double, spinVal: Double, throttleVal: Double)
    {
        val angle: Double = navx.getAngle() * Math.PI / 180
		val rotatedYVal: Double = yVal * Math.cos(angle) + xVal * Math.sin(angle)
		val rotatedXVal: Double = -yVal * Math.sin(angle) + xVal * Math.cos(angle)
		
        cartesianDrive(rotatedYVal, rotatedXVal, spinVal, throttleVal)
    }

    /* 
    fun driveAtAngle(yVal: Double, xVal: Double, angle: Double, throttleVal: Double)
    {
        //if(!turnController.isEnabled())
		//	turnController.enable()
		
		//if(turnController.getSetpoint() != angle)
		//	turnController.setSetpoint(angle)
		
		val spin: Double = -getAngle() * 0.02
		
        cartesianDrive(yVal, xVal, spin, throttleVal)
    }
    */

    fun fieldOrientedDriveAtAngle(yVal: Double, xVal: Double, angle: Double, throttleVal: Double)
    {
		val gyroAngle: Double = navx.getAngle() * Math.PI / 180 //convert degrees to radians
		
		//rotate the coordinates by the gyro angle
		val rotatedYVal: Double = yVal * Math.cos(gyroAngle) + xVal * Math.sin(gyroAngle)
		val rotatedXVal: Double = -yVal * Math.sin(gyroAngle) + xVal * Math.cos(gyroAngle)
		
		//driveAtAngle(rotatedYVal, rotatedXVal, angle, throttleVal)
    }

    fun turnToAngle(angle: Double, throttleVal: Double)
    {
	    killMotors()
	
	    lockAngle()
	
	    /*while(Math.abs(navx.getAngle() - turnController.getSetpoint()) > driveData.turnThreshold)
		    drive(0.0, 0.0, 0.0, throttleVal);*/
		
	    killMotors()
    }

	fun polarDrive(angle: Double, spin: Double, speed: Double)
    {
		var localAngle: Double = angle + 90
		localAngle = localAngle * Math.PI / 180
		
		val yVal: Double = Math.cos(localAngle) * speed
		val xVal: Double = Math.sin(localAngle) * speed
		
		cartesianDrive(yVal, xVal, spin, 1.0)
    }

    fun killMotors()
    {
        driveFrontLeft.set(0.0)
        driveFrontRight.set(0.0)
        driveBackLeft.set(0.0)
        driveBackRight.set(0.0)
    }

    fun getFieldOriented(): Boolean { return isFieldOriented; }
    
    fun toggleFieldOriented(): Boolean
    { 
        isFieldOriented = !isFieldOriented
        resetGyro()
        
        return isFieldOriented;
    }

    fun getDriftMode(): Boolean { return isDriftMode; }

    fun toggleDriftMode(): Boolean
    {
        isDriftMode = !isDriftMode

        return isDriftMode;
    }

    fun resetGyro()
    {
        navx.reset()
        navx.zeroYaw()
        if (isAngleLocked)
            unlockAngle()
    }

    fun resetMotorPositions()
    {
        driveFrontLeft.set(0.0)
        driveFrontRight.set(0.0)
        driveBackLeft.set(0.0)
        driveBackRight.set(0.0)
    }

    fun resetEncoders()
    {
        this.driveFrontLeft.setSelectedSensorPosition(0, 0, 0)
        this.driveFrontRight.setSelectedSensorPosition(0, 0, 0)
        this.driveBackLeft.setSelectedSensorPosition(0, 0, 0)
        this.driveBackRight.setSelectedSensorPosition(0, 0, 0)
    }

    fun lockAtAngle(targetPos: Double): Boolean
    {
        if (!isAngleLocked)
        {
            driveAngle = targetPos
            turnController.enable()
            turnController.setSetpoint(driveAngle)
            isAngleLocked = true
        }

        return isAngleLocked;
    }
    
    fun lockAngle(): Boolean
    {
        if (!isAngleLocked)
        {
            driveAngle = getAngle()
            turnController.enable()
            turnController.setSetpoint(driveAngle)
            isAngleLocked = true
        }
        else 
        {
            isAngleLocked = false
            turnController.disable()
        }

        return isAngleLocked;
    }

    fun unlockAngle(): Boolean
    {
        turnController.disable()
        isAngleLocked = false
        return isAngleLocked;
    }
    private fun velocityObject() =  object
    {

    }

    // deadReckon functions, outputs may need to be reversed
    public fun deadReckon()//when run in a loop. keeps track of robot posistion through encoder values gives robot speed in meters per second
    {
        val frontLeftVelocity = getEncoderVelocityFrontLeft().toDouble() / rpmToRad
        val frontRightVelocity = getEncoderVelocityFrontRight().toDouble() / rpmToRad
        val backLeftVelocity = getEncoderVelocityBackLeft().toDouble()  / rpmToRad
        val backRightVelocity = getEncoderVelocityBackRight().toDouble() /rpmToRad
        val currentGyroAngle = getAngle()
        //xVelocity = ((wheelRadius/4)*(frontLeftVelocity + frontRightVelocity + backLeftVelocity + backRightVelocity)*(1/Math.pow(2,.5))).toDouble()
        //yVelocity = (((wheelRadius/4)*(frontLeftVelocity - frontRightVelocity - backLeftVelocity + backRightVelocity)*(1/Math.pow(2,.5)))*.5).toDouble()
        val xVelocity: Double = ((wheelRadius/4.0)*(frontLeftVelocity + frontRightVelocity + backLeftVelocity + backRightVelocity)*(1/Math.pow(2.0,0.5)).toDouble())  
        val yVelocity: Double = (((wheelRadius/4.0)*(frontLeftVelocity - frontRightVelocity - backLeftVelocity + backRightVelocity)*(1/Math.pow(2.0,0.5).toDouble()))*0.5)
        rotatedXVelocity = yVelocity * Math.cos(currentGyroAngle) + xVelocity * Math.sin(currentGyroAngle)
        rotatedYVelocity = -yVelocity * Math.cos(currentGyroAngle) + xVelocity * Math.sin(currentGyroAngle)
        //deadReckonY = rotatedYVelocity * deltaT
        //val rotatedYVal: Double = yVal * Math.cos(angle) + xVal * Math.sin(angle)
		//val rotatedXVal: Double = -yVal * Math.sin(angle) + xVal * Math.cos(angle)
        return;
    }
    

    fun getAngle(): Double { return navx.getAngle() * Math.PI / 180; } 
    fun getEncoderRawFrontLeft(): Int { return driveFrontLeft.getSelectedSensorPosition(0); }
    fun getEncoderRawFrontRight(): Int { return driveFrontRight.getSelectedSensorPosition(0); }
    fun getEncoderRawBackLeft(): Int { return driveBackLeft.getSelectedSensorPosition(0); }
    fun getEncoderRawBackRight(): Int { return driveBackRight.getSelectedSensorPosition(0); }
    fun getEncoderVelocityFrontLeft(): Int { return driveFrontLeft.getSelectedSensorVelocity(0); }
    fun getEncoderVelocityFrontRight(): Int { return driveFrontRight.getSelectedSensorVelocity(0); }
    fun getEncoderVelocityBackLeft(): Int { return driveBackLeft.getSelectedSensorVelocity(0); }
    fun getEncoderVelocityBackRight(): Int { return driveBackRight.getSelectedSensorVelocity(0); }
    //fun getEncoderVelocityFrontLeftRad(): Double { return (driveFrontLeft.getSelectedSensorVelocity(0)).toDouble / rpmToRad; }
    //fun getEncoderVelocityFrontRightRad(): Double { return (driveFrontRight.getSelectedSensorVelocity(0)).toDouble / rpmToRad; }
    //fun getEncoderVelocityBackLeftRad(): Double { return (driveBackLeft.getSelectedSensorVelocity(0)).toDouble / rpmToRad; }
    //fun getEncoderVelocityBackRightRad(): Double { return (driveBackRight.getSelectedSensorVelocity(0)).toDouble / rpmToRad; }

    fun getSpeedFrontLeft(): Double { return driveFrontLeft.get(); }
    fun getSpeedFrontRight(): Double { return driveFrontRight.get(); }
    fun getSpeedBackLeft(): Double { return driveBackLeft.get(); }
    fun getSpeedBackRight(): Double { return driveBackRight.get(); }
    

    override fun pidWrite(output: Double){ turnRate = output }

    override val defaultCommand = ArcadeJoystickDrive()
}