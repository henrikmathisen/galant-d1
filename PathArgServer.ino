#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <uri/UriBraces.h>

#ifndef STASSID
#define STASSID "Tidelagets Hunddagis"
#define STAPSK "helvetessatan"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(8000);

void setup(void) {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D6, OUTPUT);
  digitalWrite(D0, HIGH);
  digitalWrite(D1, HIGH);
  digitalWrite(D3, HIGH);
  digitalWrite(D6, HIGH);
  WiFi.setOutputPower(3);
  delay(3000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }

  server.on(UriBraces("/direction/{}/time/{}"), []() {
    String direction = server.pathArg(0);
    String time = server.pathArg(1);
    uint8_t d_pin = 255;
    switch (atoi(direction.c_str())) {
      case 2:
        d_pin = D1;
        break;
      case 1:
        d_pin = D0;
        break;
      default:
        server.send(200, "text/plain", "Unknown direction :( " + direction);
        return;
    }
    if (d_pin == 255) {
      return;
    }
    digitalWrite(LED_BUILTIN, LOW);
    server.send(200, "text/plain", "Direction: " + direction + " for " + time + " ms. Using pin: " + d_pin);
    digitalWrite(d_pin, LOW);
    delay(atol(time.c_str()));
    digitalWrite(d_pin, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
  });

  server.begin();
  Serial.println("HTTP server started");
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop(void) {
  server.handleClient();
}
