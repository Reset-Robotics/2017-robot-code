
public class AutoChooser {
public static final int DO_NOTHING = 0;
public static final int DRIVE_FORWARD = 1;

public static final int DEPOSIT_GEAR_LEFT = 2;
public static final int DEPOSIT_GEAR_CENTER = 3;
public static final int DEPOSIT_GEAR_RIGHT = 4;

public static final int DEPOSIT_GEAR_AND_SHOOT_RED_CENTER = 5;
public static final int DEPOSIT_GEAR_AND_SHOOT_RED_RIGHT = 6;
public static final int DEPOSIT_GEAR_AND_SHOOT_BLUE_LEFT = 7;
public static final int DEPOSIT_GEAR_AND_SHOOT_BLUE_CENTER = 8;


// internal selection class used for SendableChooser only
public class ModeSelection {
public int mode = DO_NOTHING;
ModeSelection(int mode) {
this.mode = mode;
}
}

int mode;
private SendableChooser<ModeSelection> chooser;

public AutoChooser() {
chooser = new SendableChooser<ModeSelection>();

chooser.addDefault("DO_NOTHING", new ModeSelection(DO_NOTHING));
chooser.addObject("DRIVE_FORWARD", new ModeSelection(DRIVE_FORWARD));

chooser.addObject("DEPOSIT_GEAR_LEFT", new ModeSelection(DEPOSIT_GEAR_LEFT));
chooser.addObject("DEPOSIT_GEAR_CENTER", new ModeSelection(DEPOSIT_GEAR_CENTER));
chooser.addObject("DEPOSIT_GEAR_RIGHT", new ModeSelection(DEPOSIT_GEAR_RIGHT));

chooser.addObject("DEPOSIT_GEAR_AND_SHOOT_RED_CENTER", new ModeSelection(DEPOSIT_GEAR_AND_SHOOT_RED_CENTER));
chooser.addObject("DEPOSIT_GEAR_AND_SHOOT_RED_RIGHT", new ModeSelection(DEPOSIT_GEAR_AND_SHOOT_RED_RIGHT));
chooser.addObject("DEPOSIT_GEAR_AND_SHOOT_BLUE_LEFT", new ModeSelection(DEPOSIT_GEAR_AND_SHOOT_BLUE_LEFT));
chooser.addObject("DEPOSIT_GEAR_AND_SHOOT_BLUE_CENTER", new ModeSelection(DEPOSIT_GEAR_AND_SHOOT_BLUE_CENTER)) ;



SmartDashboard.putData("Auto_Mode_Chooser", chooser);
}

public int getAutoChoice() {
ModeSelection selection = chooser.getSelected();
return selection.mode;
}

}
