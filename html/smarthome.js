var smartHomeApp = angular.module('SmartHome', []);

smartHomeApp.directive('slider', ['$parse', function($parse) {
    return {
        restrict: 'AE',
        link: function(scope, element, attrs) {
            element.slider({
                range: attrs.range,
                value: scope.$eval(attrs.ngModel),
                min: parseInt(attrs.min),
                max: parseInt(attrs.max),
                step: parseFloat(attrs.step),
                slide: function(event, ui) {
                    scope.$apply(function() {
                        $parse(attrs.ngModel).assign(scope, ui.value);
                    });
                }
            });
        }
    };
}]);

smartHomeApp.controller('DevicesList', function ($scope) {
    $scope.connected = false;
    $scope.connecting = false;
    $scope.devices = [];

    $scope.reconnect = function() {
        initWS($scope);
    };

    $scope.X10_MDTx07__btn_click = function(event) {
        $scope.ws.send(JSON.stringify({
            device: this.device['address'],
            command: this.device['is_on'] ? 'OFF' : 'ON'
        }));
        this.device['is_on'] = !this.device['is_on'];
    };

    initWS($scope);
});

function initWS(scope) {
    scope.connecting = true;
    scope.ws = new WebSocket("ws://localhost:38080/devices/");

    scope.ws.onopen = function(evt) {
        console.log("Opened", evt);
        scope.connected = true;
        scope.connecting = false;
        scope.$apply();
    };

    scope.ws.onclose = function(evt) {
        console.log("Closed", evt);
        scope.connected = false;
        scope.connecting = false;
        scope.$apply();
    };

    scope.ws.onmessage = function(evt) {
        var message = JSON.parse(evt.data);
        console.log("Message", message);
        if (message.type = 'devicesList') {
            scope.devices = message.devices;
        }
        scope.$apply();
    };

    scope.ws.onerror = function(evt) {
        console.log("Error", evt);
        scope.connected = false;
        scope.connecting = false;
    };
}

/*
      function drawDashboard(devices) {
          $('#dashboard').html('');
          devices.forEach(function(device) {
              console.log(device);
              if (device{'type' == 'X10::MDTx07'}) {
                  $('#dashboard').append('')
              }
          });
      }

      $('#reconnect-btn').on('click', function() {
          initWS();
      });

      $('UI-slider').slider({
          range: "min",
          value: 1,
          min: 1,
          max: 23,
          slide: function(event, ui) {
              $("#volumeA1-label").text(Math.round($('#volumeA1').slider('value') / ($('#volumeA1').slider('option', 'max') - 1) * 100));
          }
      });
      $('#btnA1').on('click', function() {
          if ($(this).hasClass('active')) {
              $('#volumeA1').slider('disable');
              ws.send(JSON.stringify({
                  device: 'A1',
                  command: 'OFF'
                })
              );
          } else {
              $('#volumeA1').slider('enable');
              ws.send(JSON.stringify({
                  device: 'A1',
                  command: 'ON'
                })
              );
          }
      });

      initWS();
*/