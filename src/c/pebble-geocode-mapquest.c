#include <pebble.h>
#include <pebble-events/pebble-events.h>
#include "pebble-geocode-mapquest.h"

static GeocodeMapquestCoordinates *s_coordinates;
static GeocodeMapquestStatus s_status;
static char s_api_key[33];
static GeocodeMapquestCallback *s_callback;

static EventHandle s_event_handle;

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
    Tuple *reply_tuple = dict_find(iter, MESSAGE_KEY_GM_REPLY);
    if (reply_tuple) {
        Tuple *lat_tuple = dict_find(iter, MESSAGE_KEY_GM_LATITUDE);
        s_coordinates->latitude = lat_tuple->value->int32;

        Tuple *lon_tuple = dict_find(iter, MESSAGE_KEY_GM_LONGITUDE);
        s_coordinates->longitude = lon_tuple->value->int32;

        s_status = GeocodeMapquestStatusAvailable;
        s_callback(s_coordinates, s_status);
    }

    Tuple *error_tuple = dict_find(iter, MESSAGE_KEY_GM_BADKEY);
    if (error_tuple) {
        s_status = GeocodeMapquestStatusBadKey;
        s_callback(s_coordinates, s_status);
    }
}

static bool fetch(const char *location) {
    DictionaryIterator *out;
    AppMessageResult result = app_message_outbox_begin(&out);
    if (result != APP_MSG_OK) {
        s_status = GeocodeMapquestStatusFailed;
        s_callback(s_coordinates, s_status);
        return false;
    }

    dict_write_uint8(out, MESSAGE_KEY_GM_REQUEST, 1);

    if (strlen(s_api_key) > 0) {
        dict_write_cstring(out, MESSAGE_KEY_GM_APIKEY, s_api_key);
    }

    if (strlen(location) > 0) {
        dict_write_cstring(out, MESSAGE_KEY_GM_LOCATION, location);
    }

    result = app_message_outbox_send();
    if (result != APP_MSG_OK) {
        s_status = GeocodeMapquestStatusFailed;
        s_callback(s_coordinates, s_status);
        return false;
    }

    s_status = GeocodeMapquestStatusPending;
    s_callback(s_coordinates, s_status);
    return true;
}

void geocode_mapquest_init(void) {
    if (s_coordinates) {
        free(s_coordinates);
    }

    s_coordinates = malloc(sizeof(GeocodeMapquestCoordinates));
    s_api_key[0] = 0;
    s_status = GeocodeMapquestStatusNotYetFetched;
    events_app_message_request_inbox_size(100);
    events_app_message_request_outbox_size(100);
    s_event_handle = events_app_message_register_inbox_received(inbox_received_handler, NULL);
}

void geocode_mapquest_deinit(void) {
    if (s_coordinates) {
        free(s_coordinates);
        s_coordinates = NULL;
        s_callback = NULL;
        events_app_message_unsubscribe(s_event_handle);
    }
}

void geocode_mapquest_set_api_key(const char *api_key) {
    if (!api_key) {
        s_api_key[0] = 0;
    } else {
        strncpy(s_api_key, api_key, sizeof(s_api_key));
    }
}

bool geocode_mapquest_fetch(const char *location, GeocodeMapquestCallback *callback) {
    if (!location || !callback) {
        return false;
    }

    s_callback = callback;

    if (!bluetooth_connection_service_peek()) {
        s_status = GeocodeMapquestStatusBluetoothDisconnected;
        s_callback(s_coordinates, s_status);
        return false;
    }

    return fetch(location);
}

GeocodeMapquestCoordinates *geocode_mapquest_peek() {
    return s_coordinates;
}

void geocode_mapquest_save(const uint32_t key) {
    if (!s_coordinates) {
        return;
    }

    persist_write_data(key, s_coordinates, sizeof(GeocodeMapquestCoordinates));
}

void geocode_mapquest_load(const uint32_t key) {
    if (!s_coordinates) {
        return;
    }

    if (persist_exists(key)) {
        persist_read_data(key, s_coordinates, sizeof(GeocodeMapquestCoordinates));
    }
}
