#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

#define PORT_UDP 8700
#define PORT_TCP 8701

WiFiClient client;
WiFiUDP udp;
IPAddress serverIp;
IPAddress localIp;
IPAddress broadcastIp;
const char *ssid = "JONES-NEW";
const char *pass = "1133557799098";
static boolean clientActive = false;
char incomingByte;
char buff[10];

void setup() {
  Serial.begin(115200);
  Serial.println("");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  localIp=WiFi.localIP();
  broadcastIp=WiFi.localIP();
  broadcastIp[3]=255;
  Serial.println("");
  Serial.print("link to : ");
  Serial.println(ssid);
  Serial.print("My IP : ");
  Serial.println(localIp);
  delay(200);
  udp.begin(PORT_UDP);
  udp.beginPacket(broadcastIp,PORT_UDP);
  itoa( broadcastIp[0], buff, 10);
  udp.write(buff);
  udp.write(buff);
  udp.endPacket();
  /*do
  {
    incomingByte=udp.read();
    Serial.print(incomingByte);
    }while(incomingByte==0);
  udp.endPacket();*/
}

void loop() {
  Serial.print(udp.parsePacket());
  incomingByte=udp.read();
  while(incomingByte!=255){
  Serial.print(incomingByte);
  incomingByte=udp.read();
  }
  
 
 
    

  
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
      client.connect(serverIp, PORT_TCP);
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
    client.connect(serverIp, PORT_TCP); // Connects to a specified IP address and port
  }

}
