
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

public static final int DRIVE_AND_SHOOT_BLUE_LEFT = 9;
public static final int DRIVE_AND_SHOOT_BLUE_CENTER = 10;
public static final int DRIVE_AND_SHOOT_BLUE_RIGHT = 11;
public static final int DRIVE_AND_SHOOT_RED_LEFT = 12;
public static final int DRIVE_AND_SHOOT_RED_CENTER = 13;
public static final int DRIVE_AND_SHOOT_RED_RIGHT = 14;


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

chooser.addObject("DEPOSIT_GEAR_AND_SHOOT_RED_CENT ER", new ModeSelection(DEPOSIT_GEAR_AND_SHOOT_RED_CENTER));
chooser.addObject("DEPOSIT_GEAR_AND_SHOOT_RED_RIGH T", new ModeSelection(DEPOSIT_GEAR_AND_SHOOT_RED_RIGHT));
chooser.addObject("DEPOSIT_GEAR_AND_SHOOT_BLUE_LEF T", new ModeSelection(DEPOSIT_GEAR_AND_SHOOT_BLUE_LEFT));
chooser.addObject("DEPOSIT_GEAR_AND_SHOOT_BLUE_CEN TER", new ModeSelection(DEPOSIT_GEAR_AND_SHOOT_BLUE_CENTER)) ;

chooser.addObject("DRIVE_AND_SHOOT_BLUE_LEFT", new ModeSelection(DRIVE_AND_SHOOT_BLUE_LEFT));
chooser.addObject("DRIVE_AND_SHOOT_BLUE_CENTER", new ModeSelection(DRIVE_AND_SHOOT_BLUE_CENTER));
chooser.addObject("DRIVE_AND_SHOOT_BLUE_RIGHT", new ModeSelection(DRIVE_AND_SHOOT_BLUE_RIGHT));	
chooser.addObject("DRIVE_AND_SHOOT_RED_LEFT", new ModeSelection(DRIVE_AND_SHOOT_RED_LEFT));
chooser.addObject("DRIVE_AND_SHOOT_RED_CENTER", new ModeSelection(DRIVE_AND_SHOOT_RED_CENTER));
chooser.addObject("DRIVE_AND_SHOOT_RED_RIGHT", new ModeSelection(DRIVE_AND_SHOOT_RED_RIGHT));


SmartDashboard.putData("Auto_Mode_Chooser", chooser);
}

public int getAutoChoice() {
ModeSelection selection = chooser.getSelected();
return selection.mode;
}

}
