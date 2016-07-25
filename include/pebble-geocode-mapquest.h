#pragma once

#define GEOCODE_MAPQUEST_BUFFER_SIZE 32
#define GEOCODE_MAPQUEST_MAX_LOCATION_LEN 32

typedef enum {
    GeocodeMapquestStatusNotYetFetched = 0,
    GeocodeMapquestStatusBluetoothDisconnected,
    GeocodeMapquestStatusPending,
    GeocodeMapquestStatusFailed,
    GeocodeMapquestStatusAvailable,
    GeocodeMapquestStatusBadKey
} GeocodeMapquestStatus;

typedef struct {
    //! Latitude of the coordinates x 100000 (eg : 42.123456 -> 4212345)
    int32_t latitude;
    //! Longitude of the coordinates x 100000 (eg : -12.354789 -> -1235478)
    int32_t longitude;
} GeocodeMapquestCoordinates;

typedef void(GeocodeMapquestCallback)(GeocodeMapquestCoordinates *coordinates, GeocodeMapquestStatus status);

void geocode_mapquest_init(void);
void geocode_mapquest_deinit(void);
void geocode_mapquest_set_api_key(const char *api_key);
bool geocode_mapquest_fetch(const char *location, GeocodeMapquestCallback *callback);
GeocodeMapquestCoordinates *geocode_mapquest_peek();
void geocode_mapquest_save(const uint32_t key);
void geocode_mapquest_load(const uint32_t key);
