angular.module('Dashboard').directive("circleChart", function() {
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
      scope.canvas = element.find('canvas')[0];
      scope.ctx = scope.canvas.getContext('2d');

      console.log(scope);
      console.log('aa');

      scope.canvas.width = scope.width;
      scope.canvas.height = scope.height;
      var ctx = scope.ctx;
      var min = scope.min;
      var max = scope.max;
      var unit = scope.unit;

      scope.size = {
        width: scope.width,
        height: scope.height
      };

      var width = scope.canvas.offsetWidth;
      var height = scope.canvas.offsetHeight;
      var centerX = width / 2;
      var centerY = height / 2;

      var startAng = Math.PI * (3 / 4);
      var endAng = Math.PI * (9 / 4);
      var midAng = Math.PI * (3 / 2);

      var maxRadius = height < width ? centerY - 1 : centerX - 1;

      var startColor = '#3DD5E3';
      var midColor = '#7B67B7';
      var endColor = '#8D48AB';
      var backgroundColor = '#21323A';
      var thickness = 0.1;

      function drawGraph(value, min, max) {
        var inner = thickness * maxRadius;
        var radius = maxRadius - (inner / 2);

        var upGradient = ctx.createLinearGradient(0, height, 0, centerY - radius);
        upGradient.addColorStop(0, startColor);
        upGradient.addColorStop(1, midColor);

        var downGradient = ctx.createLinearGradient(0, centerY - radius, 0, height);
        downGradient.addColorStop(0, midColor);
        downGradient.addColorStop(1, endColor);

        var fontSize = width / 4;
        var font = fontSize + 'px sans-serif';

        var range = max - min;
        var ratio = 0;

        if (value > max)
          ratio = 1;
        else if (value < min)
          ratio = 0;
        else
          ratio = (value - min) / (max - min);

        var angRange = endAng - startAng;
        var ang = startAng + angRange * ratio;

        ctx.clearRect(0, 0, width, height);

        if (ang < midAng) {
          drawSlice(centerX, centerY, radius, inner, startAng, ang, upGradient);
          drawSlice(centerX, centerY, radius, inner, ang, endAng, backgroundColor);
        } else {
          drawSlice(centerX, centerY, radius, inner, startAng, midAng, upGradient);
          drawSlice(centerX, centerY, radius, inner, midAng, ang, downGradient);
          drawSlice(centerX, centerY, radius, inner, ang, endAng, backgroundColor);
        }

        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillStyle = '#FFFFFF';
        ctx.font = font;
        ctx.fillText('' + value.toFixed(1) + unit, centerX, centerY);
      }

      function drawSlice(centerX, centerY, radius, thickness, startAngle, endAngle, color) {
        ctx.strokeStyle = color;
        ctx.fillStyle = color;
        ctx.lineWidth = thickness;

        ctx.beginPath();
        ctx.arc(centerX, centerY, radius, startAngle, endAngle);
        ctx.stroke();
      }
      scope.$watch('value', function(newValue) {
        drawGraph(parseFloat(newValue), scope.min, scope.max);
      });
    }
  };
});
