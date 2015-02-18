var smartHomeApp = angular.module('SmartHome', ['ngRoute']);

var WS;

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
                change: function(event, ui) {
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

smartHomeApp.filter('rateText', function($filter) {
    return function(input) {
        if (input < 1024) {
            return $filter('number')(input, 0) + ' b/s';
        } else if (input < 1024 * 1024) {
            return $filter('number')(input / 1024, 2) + ' kb/s';
        } else if (input < 1024 * 1024 * 1024) {
            return $filter('number')(input / 1024 / 1024, 2) + ' mb/s';
        };
    }
});

smartHomeApp.config(['$routeProvider',
  function($routeProvider) {
    $routeProvider.
      when('/devices', {
        templateUrl: 'view/devices.html',
        controller: 'DevicesList'
      }).
      when('/torrents', {
        templateUrl: 'view/torrents.html',
        controller: 'TorrentsList'
      }).
      otherwise({
        redirectTo: '/devices'
      });
  }
]);

smartHomeApp.controller('HeaderController', function ($scope, $location) {
    $scope.isActive = function (viewLocation) {
        return viewLocation === $location.path();
    };
});

smartHomeApp.controller('DevicesList', function ($scope) {
    $scope.connected = false;
    $scope.connecting = false;
    $scope.devices = [];

    $scope.reconnect = function() {
        initDevicesWS($scope);
    };

    $scope.X10_MDTx07__btn_click = function(event) {
        $scope.ws.send(JSON.stringify({
            device: this.device['address'],
            command: this.device['is_on'] ? 'OFF' : 'ON'
        }));
        this.device['is_on'] = !this.device['is_on'];
    };

    $scope.X10_MDTx07__change = function(event, ui) {
        $scope.ws.send(JSON.stringify({
            device: this.device['address'],
            command: 'PRESET_DIM',
            volume: this.device['volume']
        }));
    };

    initDevicesWS($scope);
});

smartHomeApp.controller('TorrentsList', function ($scope) {
    $scope.reconnect = function() {
        initTorrentsWS($scope);
    };

    $scope.addTorrentSsubmit = function(event) {
        var fileReader = new FileReader();

        fileReader.onload = function() {
            $scope.ws.send(fileReader.result);
            event.target.elements.torrentFile.value = '';
        };

        fileReader.readAsArrayBuffer(event.target.elements.torrentFile.files[0]);
        $(event.target.parentNode).modal('hide');
    };

    $scope.startTorrent = function() {
        $scope.ws.send(JSON.stringify({
            command: 'START_TORRENT',
            id: this.torrent.id
        }));
    };

    $scope.pauseTorrent = function() {
        $scope.ws.send(JSON.stringify({
            command: 'PAUSE_TORRENT',
            id: this.torrent.id
        }));
    };

    $scope.deleteTorrent = function() {
        $scope.ws.send(JSON.stringify({
            command: 'DELETE_TORRENT',
            id: this.torrent.id
        }));
    };

    initTorrentsWS($scope);
});

function _initWS(scope, url, onMessage) {
    scope.connecting = true;
    if (WS) {
        WS.close();
    }
    scope.ws = WS = new WebSocket(url);

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

    scope.ws.onmessage = onMessage;

    scope.ws.onerror = function(evt) {
        console.log("Error", evt);
        scope.connected = false;
        scope.connecting = false;
    };
}

function initDevicesWS(scope) {
    _initWS(scope, "ws://" + location.host + "/devices/", function(evt) {
        var message = JSON.parse(evt.data);
        //console.log("Message", message);
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
    });
}

function initTorrentsWS(scope) {
    _initWS(scope, "ws://" + location.host + "/torrents/", function(evt) {
        var message = JSON.parse(evt.data);
        //console.log("Message", message);
        scope.torrents = message.torrents;
        scope.$apply();
    });
}