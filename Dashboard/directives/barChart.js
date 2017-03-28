angular.module('Dashboard').directive("barChart", function() {
  return {
    restrict: 'E',
    scope: {
      value: '=',
      min: '=',
      max: '=',
      width: '=',
      height: '=',
      unit: '@'

    },
    template: "<canvas id='canvas' width='size.width' height='size.height'/>",
    link: function(scope, element, attrs) {
      console.log(element);
      scope.canvas = element.find('canvas')[0];
      scope.ctx = scope.canvas.getContext('2d');
      console.log(scope.ctx);

      scope.canvas.width = scope.width;
      scope.canvas.height = scope.height;
      var ctx = scope.ctx;
      var min = scope.min;
      var max = scope.max;
      var unit = scope.unit;

      if(unit == undefined) unit = ''

      var width = scope.canvas.offsetWidth;
      var height = scope.canvas.offsetHeight;
      var centerX = width / 2;
      var centerY = height / 2;

      var startColor = '#3DD5E3';
      var midColor = '#7B67B7';
      var endColor = '#8D48AB';
      var backgroundColor = '#21323A';
      var thickness = 0.2;

      function drawGraph(value, min, max) {

        var gradient = ctx.createLinearGradient(0, 0, width, 0);
        gradient.addColorStop(0, startColor);
        gradient.addColorStop(0.5, midColor);
        gradient.addColorStop(1, endColor);

        var fontSize = height - 2;
        var font = fontSize + 'px sans-serif';

        var range = max - min;
        var ratio = 0;

        if (value > max)
          ratio = 1;
        else if (value < min)
          ratio = 0;
        else
          ratio = (value - min) / (max - min);

        ctx.clearRect(0, 0, width, height);

        ctx.strokeStyle = gradient;
        ctx.lineWidth = height;
        ctx.beginPath();
        ctx.moveTo(0, centerY);
        ctx.lineTo(width * ratio, centerY);
        ctx.stroke();

        ctx.strokeStyle = backgroundColor;
        ctx.beginPath();
        ctx.moveTo(width * ratio, centerY);
        ctx.lineTo(width, centerY);
        ctx.stroke();

        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillStyle = '#FFFFFF';
        ctx.font = font;
        ctx.fillText('' + value.toFixed(1) + unit, centerX, centerY);
      }

      scope.$watch('value', function(newValue) {
        drawGraph(parseFloat(newValue), scope.min, scope.max);
      });
    }
  };
});
