#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#define PORT 8700
WiFiClient client;
WiFiUDP udp;
IPAddress ip(192, 168, 0, 172);
IPAddress broadcastIp(192, 168, 0, 255);
const char *ssid = "JONES-NEW";
const char *pass = "1133557799098";
static boolean clientActive = false;
char incomingByte;

void setup() {
  Serial.begin(115200);
  Serial.println("");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("link to : ");
  Serial.println(ssid);
  Serial.print("My IP : ");
  Serial.println(WiFi.localIP());
  delay(200);
  udp.begin(PORT);
  udp.beginPacket(broadcastIp,PORT);
  udp.write("2222222");
  udp.endPacket();
}

void loop() {

  
  if (client.connected()) { // Returns true if the client is connected, false if not
    
    if (!clientActive) 
    {
      Serial.println("New Client connection.");
      client.print("#");
      clientActive = true;
    }
    

    if (client.available()) { // Returns the number of bytes available for reading
      //incomingByte = client.read(); // Read the next byte recieved from the server the client is connected to
      Serial.print("From client: \"");
      while (client.available())
        Serial.print((char)client.read());
      Serial.println("\"");
      //delay(2000);
      client.connect(ip, PORT);
      client.print("tina");
    }
    //client.print("tina");
  }
  else {
    if (clientActive) {
      client.stop();       // Disconnected from the server
      Serial.println("Client disconnected.");
    }
    clientActive = false;
    client.connect(ip, PORT); // Connects to a specified IP address and port
  }

}
