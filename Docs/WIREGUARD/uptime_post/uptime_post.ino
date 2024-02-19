#include <WiFi.h>
#include <WireGuard-ESP32.h>
#include <HTTPClient.h>

// WiFi configuration --- UPDATE this configuration for your WiFi AP
char ssid[] = "TP-Link_97D8";
char password[] = "67942015";

// WireGuard configuration --- UPDATE this configuration from JSON
char private_key[] = "(UI6nbFXDChGay7lAe3KmFA4UzzbWZw4UAHyX0Q1DiWU=) ";  // [Interface] PrivateKey
IPAddress local_ip(10,221,17,1);            // [Interface] Address
char public_key[] = "(2Wn1j4whk9pPwOfj4G/Qyv6+FFv5B+XYK2nE7RmppWg=)";     // [Peer] PublicKey
char endpoint_address[] = "104.248.103.171";    // [Peer] Endpoint
int endpoint_port = 30034;              // [Peer] Endpoint

static constexpr const uint32_t UPDATE_INTERVAL_MS = 5000;

static WireGuard wg;
static HTTPClient httpClient;

void setup()
{
    Serial.begin(115200);
    Serial.println("Connecting to the AP...");
    WiFi.begin(ssid, password);
    while( !WiFi.isConnected() ) {
        delay(1000);
    }
    Serial.println("Adjusting system time...");
    configTime(9 * 60 * 60, 0, "ntp.jst.mfeed.ad.jp", "ntp.nict.jp", "time.google.com");

    Serial.println("Connected. Initializing WireGuard...");
    wg.begin(
        local_ip,
        private_key,
        endpoint_address,
        public_key,
        endpoint_port);
}

void loop()
{
    WiFiClient client;

    if( !client.connect("uni.soracom.io", 80) ) {
        Serial.println("Failed to connect...");
        delay(5000);
        return;
    }
    
    uint64_t uptime_msec = millis();
    Serial.printf("Sending uptime %lu [ms]\r\n", uptime_msec);
    String json;
    json += "{\"uptime_msec\":";
    json.concat(static_cast<unsigned long>(uptime_msec));
    json += "}";
    Serial.printf("payload: %s\r\n", json.c_str());
    
    client.write("POST / HTTP/1.1\r\n");
    client.write("Host: harvest.soracom.io\r\n");
    client.write("Connection: Keep-Alive\r\n");
    client.write("Keep-Alive: timeout=5, max=2\r\n");
    client.write("Content-Type: application/json\r\n");
    client.write("Content-Length: ");
    client.write(String(json.length(), 10).c_str());
    client.write("\r\n\r\n");
    client.write(json.c_str());

    while(client.connected()) {
        auto line = client.readStringUntil('\n');
        Serial.write(line.c_str());
        Serial.write("\n");
        if( line == "\r" ) break;
    }
    if(client.connected()) {
        uint8_t buffer[256];
        size_t bytesToRead = 0;
        while((bytesToRead = client.available()) > 0) {
            bytesToRead = bytesToRead > sizeof(buffer) ? sizeof(buffer) : bytesToRead;
            auto bytesRead = client.readBytes(buffer, bytesToRead); 
            Serial.write(buffer, bytesRead);
        }
    }
    delay(UPDATE_INTERVAL_MS);
}
