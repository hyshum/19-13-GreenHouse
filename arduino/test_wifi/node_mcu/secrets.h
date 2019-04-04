#include <pgmspace.h>

#define THINGNAME "Greenhouse_final"
const char ssid[] = "DESKTOP-MTE7L3T 2124";
const char password[] = "V66%3e04";

int8_t TIME_ZONE = -5;

const char MQTT_HOST[] = "ap3wui1s0hiyp-ats.iot.us-east-1.amazonaws.com";

static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
.
-----END CERTIFICATE-----
)EOF";

static const char client_cert[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
.
-----END CERTIFICATE-----
)KEY";

static const char privkey[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
.
-----END RSA PRIVATE KEY-----
)KEY";
