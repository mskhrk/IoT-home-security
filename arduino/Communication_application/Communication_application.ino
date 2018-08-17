#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

//以下為必要全域變數
#define PORT 12345
WiFiClient client;
IPAddress ip(192,168,0,123);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
WiFiServer server(PORT);
String dataClient;
String dataServer;
const char *ssid="Misaka_net";
const char *pass="fripside";
const int gasPin = A0;
String data="";

void setup() {
wifiInit();//請先呼叫這個程式一次
}

void loop() {
data=data+analogRead(gasPin);
  
sendMessage("MQ4"+data);//要傳送的字串直接輸入在此
data="";
sendMessage("MQ4");//要傳送的字串直接輸入在此

}

void wifiInit()
{
  Serial.begin(115200);
  Serial.println("");
  //Serial.print("Server Port : ");
  //Serial.println(PORT);
  //pinMode(BUILTIN_LED,OUTPUT);// output LED
  //pinMode(12,OUTPUT);//output buzzer
  //WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid,pass);
  while(WiFi.status()!=WL_CONNECTED){
    Serial.print(".");
    delay(500);}
  Serial.println("");
  Serial.print("link to : ");
  Serial.println(ssid);
  Serial.print("My IP : ");
  Serial.println(WiFi.localIP());
  delay(200);
  //server.begin();//當server被連 server 啟動
}
void sendMessage(String message)
{
   while(!client.connected()){
  client=server.available();
  }

 while(client.connected()){
  //Serial.println(" data from server to client : ");
  //dataServer = "#0 -true-;"; // 跌倒為true
  client.print(message);//server send data to client
  Serial.print("Send message:");
  Serial.println(message);
  delay(1000); //every 1 second
  client.stop();
  dataClient="";
 }
}

