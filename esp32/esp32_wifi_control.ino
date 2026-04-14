#include <WiFi.h>

HardwareSerial mySerial(2); // UART2 (GPIO16 RX, GPIO17 TX)

String statusMsg = "System Idle";

const char* ssid = "ESP32_Control";
const char* password = "12345678";

WiFiServer server(80);

void setup()
{
  Serial.begin(115200);
  mySerial.begin(115200, SERIAL_8N1, 16, 17);

  WiFi.softAP(ssid, password);
  server.begin();
}

void loop()
{
  // -------- READ FROM STM32 --------
  while(mySerial.available())
  {
    String data = mySerial.readStringUntil('\n');

    if(data.startsWith("M"))
      statusMsg = "Motor ON Time: " + data.substring(1) + " s";

    else if(data.startsWith("C"))
      statusMsg = "Conveyor ON Time: " + data.substring(1) + " s";

    else if(data.startsWith("I"))
      statusMsg = "System Idle";
  }

  // -------- WEB SERVER --------
  WiFiClient client = server.available();

  if (client)
  {
    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("/start") != -1)
      mySerial.write('1');

    if (request.indexOf("/stop") != -1)
      mySerial.write('0');

    // -------- HTML PAGE --------
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();

    client.println("<html>");
    client.println("<head>");
    client.println("<meta http-equiv='refresh' content='1'>"); // 🔥 auto refresh every 1 sec
    client.println("<style>");
    client.println("body { font-family: Arial; text-align: center; }");
    client.println("button { padding: 15px 30px; font-size: 18px; margin: 10px; }");
    client.println("</style>");
    client.println("</head>");

    client.println("<body>");
    client.println("<h2>STM32 CONTROL PANEL</h2>");
    client.println("<h3>Status:</h3>");
    client.println("<p>" + statusMsg + "</p>");

    client.println("<a href=\"/start\"><button>START</button></a><br>");
    client.println("<a href=\"/stop\"><button>STOP</button></a>");

    client.println("</body>");
    client.println("</html>");

    client.stop();
  }
}
