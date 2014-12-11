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
                    if (attrs.ngChange) {
                        scope.$eval(attrs.ngChange, {$event: event, $ui: ui});
                    }
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

    $scope.X10_MDTx07__slide = function(event, ui) {
        $scope.ws.send(JSON.stringify({
            device: this.device['address'],
            command: 'PRESET_DIM',
            volume: this.device['volume']
        }));
    }

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
        if (message.type == 'devicesList') {
            scope.devices = message.devices;
        } else if (message.type == 'updateDevice') {
            scope.devices.forEach(function(e){
                if (e['address'] == message['device']['address']) {
                    $.extend(e, message['device']);
                }
            });
        }
        scope.$apply();
    };

    scope.ws.onerror = function(evt) {
        console.log("Error", evt);
        scope.connected = false;
        scope.connecting = false;
    };
}
