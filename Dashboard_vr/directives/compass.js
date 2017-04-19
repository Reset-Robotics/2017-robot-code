angular.module('Dashboard').directive("compass", function() {
  return {
    restrict: 'E',
    scope: {
      value: '=',
      width: '=',
      height: '='

    },
    template: "<canvas id='canvas' width='size.width' height='size.height'/>",
    link: function(scope, element, attrs) {
      scope.canvas = element.find('canvas')[0];
      scope.ctx = scope.canvas.getContext('2d');

      var ctx = scope.ctx;
      scope.canvas.width = scope.width;
      scope.canvas.height = scope.height;

      var width = scope.canvas.offsetWidth;
      var height = scope.canvas.offsetHeight;
      var centerX = width / 2;
      var centerY = height / 2;

      var size = 0;
      var textRadius = 0;

      if (width > height) {
        size = centerX * 0.7;
        textRadius = centerX * 0.85;
      } else {
        size = centerY * 0.7;
        textRadius = centerY * 0.85;
      }

      var tickColor = '#CCCCCC';

      ctx.textAlign = 'center';
      ctx.textBaseline = 'middle';
      ctx.font = '15px sans-serif';

      function drawTicks() {
        ctx.beginPath();
        ctx.strokeStyle = tickColor;
        ctx.fillStyle = tickColor;
        ctx.lineWidth = 2;

        for (var i = 0; i < 16; i++) {
          var angle = (i / 16) * (2 * Math.PI);
          ctx.moveTo(centerX + size * Math.cos(angle), centerY + size * Math.sin(angle));
          ctx.lineTo(centerX + size * 0.8 * Math.cos(angle), centerY + size * 0.8 * Math.sin(angle));
          ctx.stroke();
        }

        ctx.beginPath();
        ctx.lineWidth = 3;

        for (var i = 0; i < 4; i++) {
          var angle = (i / 4) * (2 * Math.PI);
          ctx.moveTo(centerX + size * Math.cos(angle), centerY + size * Math.sin(angle));
          ctx.lineTo(centerX + size * 0.7 * Math.cos(angle), centerY + size * 0.7 * Math.sin(angle));
          ctx.stroke();
        }

        ctx.fillText('0°', centerX, centerY - textRadius);
        ctx.fillText('90°', centerX + textRadius, centerY);
        ctx.fillText('180°', centerX, centerY + textRadius);
        ctx.fillText('270°', centerX - textRadius, centerY);
        ctx.stroke();

        ctx.fillText('45°', centerX + textRadius * Math.cos(Math.PI / 4), centerY - textRadius * Math.sin(Math.PI / 4));
        ctx.fillText('135°', centerX + textRadius * Math.cos(Math.PI / 4), centerY + textRadius * Math.sin(Math.PI / 4));
        ctx.fillText('225°', centerX - textRadius * Math.cos(Math.PI / 4), centerY + textRadius * Math.sin(Math.PI / 4));
        ctx.fillText('315°', centerX - textRadius * Math.cos(Math.PI / 4), centerY - textRadius * Math.sin(Math.PI / 4));
        ctx.stroke();
        /*
        ctx.translate(centerX, centerY);
        ctx.rotate(-Math.PI /4);
        ctx.fillText('45°', textRadius, 0);
        ctx.fillText('225°', -textRadius, 0);
        ctx.stroke();
        ctx.setTransform(1, 0, 0, 1, 0, 0);

        ctx.translate(centerX, centerY);
        ctx.rotate(Math.PI /4);
        ctx.fillText('135°', textRadius, 0);
        ctx.fillText('315°', -textRadius, 0);
        ctx.stroke();
        ctx.setTransform(1, 0, 0, 1, 0, 0);*/
      }

      function drawArrow(angle) {
        var arrowWidth = size * 0.2;

        ctx.translate(centerX, centerY);
        ctx.rotate(angle);

        ctx.strokeWidth = 1;
        ctx.strokeStyle = '#8D48AB';
        ctx.fillStyle = '#8D48AB';

        ctx.beginPath();
        ctx.moveTo(arrowWidth / 2, 0);
        ctx.lineTo(arrowWidth / 2, -size * 0.7);
        ctx.lineTo(0, -size * 0.9);
        ctx.lineTo(-arrowWidth / 2, -size * 0.7);
        ctx.lineTo(-arrowWidth / 2, 0);
        ctx.lineTo(arrowWidth / 2, 0);

        ctx.stroke();
        ctx.fill();

        ctx.strokeWidth = 1;
        ctx.strokeStyle = '#46C5DC';
        ctx.fillStyle = '#46C5DC';

        ctx.beginPath();
        ctx.moveTo(arrowWidth / 2, 0);
        ctx.lineTo(arrowWidth / 2, size * 0.7);
        ctx.lineTo(0, size * 0.9);
        ctx.lineTo(-arrowWidth / 2, size * 0.7);
        ctx.lineTo(-arrowWidth / 2, 0);
        ctx.lineTo(arrowWidth / 2, 0);

        ctx.stroke();
        ctx.fill();

        ctx.strokeStyle = '#000000';
        ctx.fillStyle = '#000000';

        ctx.beginPath();
        ctx.arc(0, 0, arrowWidth / 2 + 0.1, 0, 2 * Math.PI);
        ctx.fill();
        ctx.stroke();

        ctx.setTransform(1, 0, 0, 1, 0, 0);
      }

      function drawCompass(angle){
      	ctx.clearRect(0, 0, width, height);
        drawTicks();
        drawArrow(angle);
      }
      scope.$watch('value', function(newValue) {
        drawCompass(parseFloat(newValue));
      });
    }
  };
});
