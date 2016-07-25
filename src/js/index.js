var GeocodeMapquest = function() {

    this._xhrWrapper = function(url, type, callback) {
        var xhr = new XMLHttpRequest();
        xhr.onload = function () {
            callback(xhr);
        };
        xhr.open(type, url);
        xhr.send();
    };

    this.appMessageHandler = function(dict, options) {
        if (dict.payload['GM_REQUEST']) {
            console.log('geocode-mapquest: Got fetch request from C app');

            var apiKey = '';
            if (options && 'apiKey' in options) {
                apiKey = options['apiKey'];
            } else if (dict.payload && 'GM_APIKEY' in dict.payload) {
                apiKey = dict.payload['GM_APIKEY'];
            }

            var location = '';
            if (options && 'location' in options) {
                location = options['location'];
            } else if (dict.payload && 'GM_LOCATION' in dict.payload) {
                location = dict.payload['GM_LOCATION'];
            }

            var url = 'http://www.mapquestapi.com/geocoding/v1/address?maxResults=1&thumbMaps=false&key=' + apiKey + '&location=' + location;
            url = encodeURI(url);
            this._xhrWrapper(url, 'GET', function(req) {
                console.log('geocode-mapquest: Got API response');
                if (req.status == 200) {
                    var json = JSON.parse(req.response);

                    var latLng = json.results[0].locations[0].latLng;
                    var message = {
                        'GM_REPLY': 1,
                        'GM_LATITUDE': Math.floor(latLng.lat * 100000),
                        'GM_LONGITUDE': Math.floor(latLng.lng * 100000)
                    };
                    console.log(message['GM_LATITUDE']);
                    console.log(message['GM_LONGITUDE']);
                    Pebble.sendAppMessage(message);
                } else {
                    console.log('geocode-mapquest: Error fetching data (HTTP Status: ' + req.status + ')');
                    Pebble.sendAppMessage({ 'GM_BADKEY': 1 });
                }
            }.bind(this));
        }
    }

}

module.exports = GeocodeMapquest;

