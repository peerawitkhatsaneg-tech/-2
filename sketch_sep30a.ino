#include <WiFi.h>
#include "DHT.h"

// ========================================
// ESP32 WiFi
// ========================================
const char* ssid = "ESP32_WIFI";
const char* password = "12345678";

// ========================================
// DHT11
// ========================================
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// ========================================
// Web Server
// ========================================
WiFiServer server(80);

// ========================================
// GPIO
// ========================================
const int output26 = 26;
const int output27 = 27;

String output26State = "OFF";
String output27State = "OFF";

// ========================================
// SETUP
// ========================================
void setup() {

  Serial.begin(115200);

  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);

  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  dht.begin();

  // ========================================
  // Create WiFi Access Point
  // ========================================
  WiFi.mode(WIFI_AP);

  WiFi.softAP(ssid, password);

  Serial.println();
  Serial.println("==============================");
  Serial.println("ESP32 WiFi Started");
  Serial.print("WiFi Name: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  Serial.println("==============================");

  server.begin();
}

// ========================================
// LOOP
// ========================================
void loop() {

  WiFiClient client = server.available();

  if (!client) {
    return;
  }

  Serial.println("New Client");

  String request = "";

  unsigned long timeout = millis();

  while (client.connected() && millis() - timeout < 2000) {

    if (client.available()) {

      char c = client.read();

      request += c;

      if (c == '\n') {

        // ========================================
        // GPIO 26
        // ========================================

        if (request.indexOf("GET /26/on") >= 0) {

          digitalWrite(output26, HIGH);
          output26State = "ON";

          Serial.println("GPIO 26 ON");
        }

        if (request.indexOf("GET /26/off") >= 0) {

          digitalWrite(output26, LOW);
          output26State = "OFF";

          Serial.println("GPIO 26 OFF");
        }

        // ========================================
        // GPIO 27
        // ========================================

        if (request.indexOf("GET /27/on") >= 0) {

          digitalWrite(output27, HIGH);
          output27State = "ON";

          Serial.println("GPIO 27 ON");
        }

        if (request.indexOf("GET /27/off") >= 0) {

          digitalWrite(output27, LOW);
          output27State = "OFF";

          Serial.println("GPIO 27 OFF");
        }

        // ========================================
        // Read DHT11
        // ========================================

        float temperature = dht.readTemperature();
        float humidity = dht.readHumidity();

        if (isnan(temperature)) {
          temperature = 0;
        }

        if (isnan(humidity)) {
          humidity = 0;
        }

        // ========================================
        // Calculate Gauge Angles
        // Temperature: 0-50 C
        // Humidity: 0-100 %
        // ========================================

        int tempAngle = map(
          constrain((int)temperature, 0, 50),
          0,
          50,
          -90,
          90
        );

        int humAngle = map(
          constrain((int)humidity, 0, 100),
          0,
          100,
          -90,
          90
        );

        // ========================================
        // HTTP
        // ========================================

        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println("Connection: close");
        client.println();

        // ========================================
        // HTML
        // ========================================

        client.println("<!DOCTYPE html>");
        client.println("<html>");
        client.println("<head>");

        client.println(
          "<meta name='viewport' "
          "content='width=device-width, initial-scale=1'>"
        );

        client.println("<title>ESP32 DHT11 Monitor</title>");

        // ========================================
        // CSS
        // ========================================

        client.println("<style>");

        client.println("* { box-sizing: border-box; }");

        client.println("body {");
        client.println("font-family: Arial, sans-serif;");
        client.println("text-align: center;");
        client.println("background: #111827;");
        client.println("color: white;");
        client.println("margin: 0;");
        client.println("padding: 20px;");
        client.println("}");

        client.println("h1 {");
        client.println("margin-bottom: 30px;");
        client.println("}");

        client.println(".gauges {");
        client.println("display: flex;");
        client.println("justify-content: center;");
        client.println("align-items: center;");
        client.println("gap: 40px;");
        client.println("flex-wrap: wrap;");
        client.println("}");

        // ========================================
        // Gauge
        // ========================================

        client.println(".gauge-box {");
        client.println("width: 330px;");
        client.println("background: #1f2937;");
        client.println("padding: 25px;");
        client.println("border-radius: 20px;");
        client.println("}");

        client.println(".gauge {");
        client.println("position: relative;");
        client.println("width: 280px;");
        client.println("height: 160px;");
        client.println("margin: 20px auto;");
        client.println("border-radius: 280px 280px 0 0;");
        client.println("background:");
        client.println("conic-gradient(");
        client.println("#2196F3 0deg,");
        client.println("#00C853 45deg,");
        client.println("#FFC107 70deg,");
        client.println("#F44336 90deg,");
        client.println("#F44336 180deg");
        client.println(");");
        client.println("overflow: hidden;");
        client.println("}");

        client.println(".gauge::after {");
        client.println("content: '';");
        client.println("position: absolute;");
        client.println("width: 230px;");
        client.println("height: 115px;");
        client.println("background: #1f2937;");
        client.println("bottom: 0;");
        client.println("left: 25px;");
        client.println("border-radius: 230px 230px 0 0;");
        client.println("}");

        client.println(".needle {");
        client.println("position: absolute;");
        client.println("width: 6px;");
        client.println("height: 110px;");
        client.println("background: white;");
        client.println("bottom: 0;");
        client.println("left: 137px;");
        client.println("transform-origin: bottom center;");
        client.println("z-index: 5;");
        client.println("border-radius: 5px;");
        client.println("}");

        client.println(".center {");
        client.println("position: absolute;");
        client.println("width: 22px;");
        client.println("height: 22px;");
        client.println("background: white;");
        client.println("border-radius: 50%;");
        client.println("bottom: -5px;");
        client.println("left: 129px;");
        client.println("z-index: 6;");
        client.println("}");

        client.println(".value {");
        client.println("position: absolute;");
        client.println("width: 100%;");
        client.println("bottom: 20px;");
        client.println("left: 0;");
        client.println("font-size: 25px;");
        client.println("font-weight: bold;");
        client.println("z-index: 10;");
        client.println("}");

        client.println(".title {");
        client.println("font-size: 22px;");
        client.println("font-weight: bold;");
        client.println("}");

        // ========================================
        // Buttons
        // ========================================

        client.println(".control {");
        client.println("background: #1f2937;");
        client.println("margin: 30px auto;");
        client.println("padding: 20px;");
        client.println("max-width: 700px;");
        client.println("border-radius: 20px;");
        client.println("}");

        client.println(".button {");
        client.println("display: inline-block;");
        client.println("padding: 14px 30px;");
        client.println("margin: 5px;");
        client.println("font-size: 18px;");
        client.println("text-decoration: none;");
        client.println("color: white;");
        client.println("background: #16a34a;");
        client.println("border-radius: 10px;");
        client.println("}");

        client.println(".off {");
        client.println("background: #dc2626;");
        client.println("}");

        client.println(".state {");
        client.println("font-size: 18px;");
        client.println("}");

        client.println("</style>");

        client.println("</head>");

        // ========================================
        // BODY
        // ========================================

        client.println("<body>");

        client.println("<h1>ESP32 DHT11 Monitor</h1>");

        // ========================================
        // GAUGES
        // ========================================

        client.println("<div class='gauges'>");

        // Temperature Gauge
        client.println("<div class='gauge-box'>");

        client.println("<div class='title'>TEMPERATURE</div>");

        client.println("<div class='gauge'>");

        client.print(
          "<div class='needle' style='transform: rotate("
        );
        client.print(tempAngle);
        client.println("deg);'></div>");

        client.println("<div class='center'></div>");

        client.print("<div class='value'>");
        client.print(temperature, 1);
        client.println(" &deg;C</div>");

        client.println("</div>");

        client.println("<p>Range: 0 - 50 &deg;C</p>");

        client.println("</div>");

        // Humidity Gauge
        client.println("<div class='gauge-box'>");

        client.println("<div class='title'>HUMIDITY</div>");

        client.println("<div class='gauge'>");

        client.print(
          "<div class='needle' style='transform: rotate("
        );
        client.print(humAngle);
        client.println("deg);'></div>");

        client.println("<div class='center'></div>");

        client.print("<div class='value'>");
        client.print(humidity, 1);
        client.println(" %</div>");

        client.println("</div>");

        client.println("<p>Range: 0 - 100 %</p>");

        client.println("</div>");

        client.println("</div>");

        // ========================================
        // GPIO CONTROL
        // ========================================

        client.println("<div class='control'>");

        client.println("<h2>GPIO CONTROL</h2>");

        // GPIO 26
        client.println("<h3>GPIO 26</h3>");

        client.print("<div class='state'>State: <b>");
        client.print(output26State);
        client.println("</b></div>");

        if (output26State == "OFF") {

          client.println(
            "<a class='button' href='/26/on'>ON</a>"
          );

        } else {

          client.println(
            "<a class='button off' href='/26/off'>OFF</a>"
          );
        }

        // GPIO 27
        client.println("<h3>GPIO 27</h3>");

        client.print("<div class='state'>State: <b>");
        client.print(output27State);
        client.println("</b></div>");

        if (output27State == "OFF") {

          client.println(
            "<a class='button' href='/27/on'>ON</a>"
          );

        } else {

          client.println(
            "<a class='button off' href='/27/off'>OFF</a>"
          );
        }

        client.println("</div>");

        client.println("</body>");
        client.println("</html>");

        client.println();

        break;
      }
    }
  }

  client.stop();

  Serial.println("Client disconnected");
}