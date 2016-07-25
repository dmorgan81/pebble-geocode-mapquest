pebble-geocode-mapquest
=============

pebble-geocode-mapquest provides an API to convert a location into coordinates using
[MapQuest's geocoding API](http://www.mapquestapi.com/geocoding/).

Installation
------------

```
pebble package pebble-geocode-mapquest
```

You must be using a new-style project; install the latest pebble tool and SDK
and run `pebble convert-project` on your app if you aren't.

Usage
-----

The following needs to be added to your app.js:

```js
var GeocodeMapquest = require('pebble-geocode-mapquest');
var geocodeMapquest = new GeocodeMapquest();

Pebble.addEventListener('appmessage', function(e) {
    geocodeMapquest.appMessageHandler(e);
 });
```

```c
// This is not a complete example, but should demonstrate the basic usage

#include <pebble-geocode-mapquest/pebble-geocode-mapquest.h>

static void geocode_fetch_callback(GeocodeMapquestCoordinates *coordinates, GeocodeMapquestStatus status) {
    APP_LOG(APP_LOG_LEVEL_INFO, "%ld", coordinates->latitude);
    APP_LOG(APP_LOG_LEVEL_INFO, "%ld", coordinates->longitude);
    ...
}

static void init(void) {
  geocode_mapquest_init();
  geocode_mapquest_set_api_key(GEOCODE_API_KEY);

  geocode_mapquest_fetch("Seattle, WA", geocode_fetch_callback)
  ...
}

static void deinit(void) {
  geocode_mapquest_deinit();
  ...
}
```

You must call `geocode_mapquest_init()` first followed by `geocode_mapquest_set_api_key()`, preferably
in your app's init function.

It is recommended to use a preprocessor define for your MapQuest API key instead of hard coding it, if
only to prevent it from being stored in your repo history:
`CFLAGS="-DGEOCODE_API_KEY=\"thisismyapikey\" pebble build"`

Before your app quits, you must call `geocode_mapquest_deinit()`.

`geocode_mapquest_fetch()` takes a location string and a callback with the results of the geocode operation.
If the operation failed for some reason that will be reflected in the status parameter to the callback.

`geocode_mapquest_save()` and `geocode_mapquest_load()` will save or load the most recent geocoding to
the specified key. This is a good way to avoid excessive calls to the MapQuest API. Save the results and refresh
only if the user specifies a different location.
