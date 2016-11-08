// Import the Clay package
var Clay = require('pebble-clay');
// Load our Clay configuration file
var clayConfig = require('./config');
// Initialize Clay
var clay = new Clay(clayConfig);

// api key
var myAPIKey = 'f1ec56c0595742ceccd56de618aa8aea';
var initialized = false;

// weather
Pebble.addEventListener('ready', 
    function(e) {
        console.log('PebbleKit JS ready!');
        initialized = true;

        // send init message
        dict = {
            'Init': 1
        };

        // Send to Pebble
        Pebble.sendAppMessage(dict,
            function(e) {
                console.log('Weather info sent to Pebble successfully!');
            },
            function(e) {
                console.log('Error sending weather info to Pebble!');
            }
        );
    }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
    function(e) {
        console.log('AppMessage received!');

        var dict = e.payload;
        console.log('Got message: ' + JSON.stringify(dict));

        // get weather
        getWeather(dict['Location']);
    }                     
);


var xhrRequest = function (url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        callback(this.responseText);
    };
    xhr.open(type, url);
    xhr.send();
};

function locationSuccess(pos) {
    // We will request the weather here
    var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
        pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + myAPIKey;

      // Send request to OpenWeatherMap
    xhrRequest(url, 'GET', 
        function(responseText) {
            // responseText contains a JSON object with weather info
            var json = JSON.parse(responseText);

            // Temperature in Kelvin requires adjustment
            var temperature = Math.round(json.main.temp - 273.15);
            console.log('Temperature is ' + temperature);

            // Conditions
            var conditions = json.weather[0].main;      
            console.log('Conditions are ' + conditions);

            var dictionary = {
                'Temperature': temperature,
                'Conditions': conditions
            };

            // Send to Pebble
            Pebble.sendAppMessage(dictionary,
                function(e) {
                    console.log('Weather info sent to Pebble successfully!');
                },
                function(e) {
                    console.log('Error sending weather info to Pebble!');
                }
            );
        }      
    );
}

function locationError(err) {
    console.log('Error requesting location!');
    var dictionary = {
        'Temperature': -300,
        'Conditions': -300
    };

    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
        function(e) {
            console.log('Weather info sent to Pebble successfully!');
        },
        function(e) {
            console.log('Error sending weather info to Pebble!');
        }
    );
}

function requestPosition(pos) {
    // We will request the weather here
    var url = 'http://api.openweathermap.org/data/2.5/weather?q=' + pos +
        '&appid=' + myAPIKey;

      // Send request to OpenWeatherMap
    xhrRequest(url, 'GET', 
        function(responseText) {
            // responseText contains a JSON object with weather info
            var json = JSON.parse(responseText);

            // Temperature in Kelvin requires adjustment
            var temperature = Math.round(json.main.temp - 273.15);
            console.log('Temperature is ' + temperature);

            // Conditions
            var conditions = json.weather[0].main;      
            console.log('Conditions are ' + conditions);

            var dictionary = {
                'Temperature': temperature,
                'Conditions': conditions
            };

            // Send to Pebble
            Pebble.sendAppMessage(dictionary,
                function(e) {
                    console.log('Weather info sent to Pebble successfully!');
                },
                function(e) {
                    console.log('Error sending weather info to Pebble!');
                }
            );
        }      
    );
}

function getWeather(locate) {
    if (!initialized) {
        setTimeout(6000, getWeather);
    } else {
        if (locate === "gps") {
            navigator.geolocation.getCurrentPosition(
                locationSuccess,
                locationError,
                {timeout: 15000, maximumAge: 60000}
            );
        } else {
            requestPosition(locate); 
        }
    }
}
