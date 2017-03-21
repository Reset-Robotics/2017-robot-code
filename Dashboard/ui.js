
var app = angular.module("Dashboard", ['ngMaterial']);

app.config(function($mdThemingProvider) {
  $mdThemingProvider.theme('default')
    .dark();
});

app.factory('updateService', function(){
	var updateService = {};

	updateService.data = {
		commands: {
			fieldOrientedDrive: false,
			lockAngleFront: false,
			lockAnglePickup: false,
			lockAngleLeftGear: false,
			lockAngleRightGear: false,
			unlockAngle: false,
			autoAlignPickup: false,
			autoAlignGear: false
		},
		drivetrain: {
			motorFrontLeft: 0,
			motorBackLeft: 0,
			motorBackRight: 0,
			motorFrontRight: 0,
			encoderFrontLeft: 0,
			encoderBackLeft: 0,
			encoderBackRight: 0,
			encoderFrontRight: 0,
			climberMotor: 0
		},
		vision: {
			targetVisible: false,
			xDistance: 0
		},
		gear: {
			isOpen: false
		},
		power: {
			batteryVoltage: 0,
			totalPowerUse: 0,
			powerFrontLeft: 0,
			powerBackLeft: 0,
			powerBackRight: 0,
			powerFrontRight: 0,
			powerClimber: 0,
			powerRio: 0,
			powerJetson: 0,
			powerLed: 0,
		},
		match: {
			time: 0,
			phase: 'not started'
		},
		cameras: {

		},
		sensors: {
			gyroAngle: 0,
			ultrasonic1: 0,
			ultrasonic2: 0
		},
		autoMode:{
			selectedMode: 'none',
			availibleModes: {}
		},
		connected: false
	};

	updateService.sendValue = function(key, value){
		NetworkTables.putValue(key, value);
	};

	updateService.getValue = function(key){
		NetworkTables.getValue(key);
	};

	updateService.onValueChanged = function(key, value, isNew){
		if (value == 'true') {
			value = true;
		} else if (value == 'false') {
			value = false;
		}

		switch(key){

		}
	};

	updateService.onConnection = function(connected){
		updateService.data.connected = connected;
	};

	NetworkTables.addRobotConnectionListener(updateService.onConnection, true);
	NetworkTables.addGlobalListener(updateService.onValueChanged, true);

	return updateService;
});

app.controller("testCtrl", function($scope){
	$scope.text = "hello";
});
