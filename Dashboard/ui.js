
var app = angular.module("Dashboard", ['ngMaterial', 'nvd3']);

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
		motors: {
			motorFrontLeft: 1,
			motorBackLeft: 1,
			motorBackRight: -1,
			motorFrontRight: -1,
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
			batteryVoltage: 10.8,
			totalPowerUse: 28.6,
			powerFrontLeft: 12.4,
			powerBackLeft: 7,
			powerBackRight: 9,
			powerFrontRight: 10,
			powerClimber: 0,
			powerRio: 0.8,
			powerJetson: 2.3,
			powerLed: 1.4,
      powerPcm: 0,
      RioCpu: 86,
      RioRam: 34
		},
		match: {
			time: 0,
			phase: 'not started'
		},
		cameras: {

		},
		sensors: {
			gyroAngle: 1,
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

  updateService.getProperty = function(o, s) {
    s = s.replace(/\[(\w+)\]/g, '.$1'); // convert indexes to properties
    s = s.replace(/^\./, '');           // strip a leading dot
    var a = s.split('/');
    for (var i = 0, n = a.length - 1; i < n; ++i) {
      var k = a[i];
      if (k in o) {
        o = o[k];
      } else {
        return;
      }
    }
    return o;
  };

	updateService.onValueChanged = function(key, value, isNew){
		if (value == 'true') {
			value = true;
		} else if (value == 'false') {
			value = false;
		}

    var a = key.split('/');
    updateService.getProperty(updateService.data, key)[a[a.length - 1]] = value;
	};

	updateService.onConnection = function(connected){
		updateService.data.connected = connected;
	};

	NetworkTables.addRobotConnectionListener(updateService.onConnection, true);
	NetworkTables.addGlobalListener(updateService.onValueChanged, true);

	return updateService;
});

app.controller('uiCtrl', function($scope, updateService){
  $scope.data = updateService.data;

  /*$scope.data.power.batteryVoltage = 10.3;
  $scope.data.power.totalPowerUse = 20.6;
  $scope.data.power.powerFrontLeft = 18;
  $scope.data.power.powerFrontright = 3;
  $scope.data.power.powerBackRight = 15;
  $scope.data.power.powerBackLeft = 4;*/

});

app.controller('clockCtrl', function($scope, updateService){
  var data = updateService.data;
  $scope.data = data;

  $scope.getTime = function(){
    var minutes = data.match.time / 60;
    var seconds = data.match.time % 60;

    return (minutes < 10 ? '0' : '') + minutes + ':' + (seconds < 10 ? '0' : '') + seconds;
  };
  $scope.getStatus = function(){
    if(data.connected)
      return 'connected';
    else
      return 'disconnected';
  };
});

app.controller('compassCtrl', function($scope, updateService){
  $scope.info = {
    value: 1
  };
});
