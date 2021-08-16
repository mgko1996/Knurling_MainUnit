#include <WiFi.h>
#include <WebSocketServer.h>
#include <ESP32SharpIR.h>
#include <time.h>


WiFiServer server(80);
WebSocketServer webSocketServer;

ESP32SharpIR sensor( ESP32SharpIR::GP2Y0A21YK0F, 33);

int count = 0;
String machine_name = "LetPullDown";
static int distance;
static bool flag1=false;
static bool flag2=false;
static String data;

const char* ntpServer = "pool.ntp.org";
uint8_t timeZone = 9; 
uint8_t summerTime = 0; // 3600 

const char* ssid = "yourNetworkName";
const char* password = "yourNetworkPassword";

void printLocalTime() {
  struct tm timeinfo; 
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time"); 
    return; 
  } 
  String year = String(timeinfo.tm_year+1900);
  String month = String(timeinfo.tm_mon+1);
  String day = String(timeinfo.tm_mday);
  String hour = String(timeinfo.tm_hour);
  String minute = String(timeinfo.tm_min);
  String sec = String(timeinfo.tm_sec);
  String date = year + "," + month + "." + day + " " + hour + ":" + minute + ":" + sec;
}

void setup(){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
  server.begin()
  delay(500);
}

void loop(){
  WiFiClient client = server.available();
  
  if (client.connected() && webSocketServer.handshake(client)) {

    while (client.connected()) {
      data = webSocketServer.getData();
      if(data == "ture"){
        printLocalTime();
        distance = sensor.getDistance();
        webSocket.Server.sendData(data);
        if (distance<=20){
          flag1=true;
        } //플레이트 까지의 거리가 20cm 를 넘을 때, flag1을 true로 설정.
        
        if ((flag1==true) && (40<=distance)){
          flag2=true;
        } //플레이트 까지의 거리가 40cm 를 넘을 때, flag2를 true로 설정.
  
        if ((flag1==true) && (flag2==true))//flag1과 flag2가 모두 true일 때, count를 증가시킴.
        {
          count+=1;
          Serial.print(" Count  :: ");
          Serial.print(count);
          Serial.println();   
      
          flag1=false; //flag1과 flag2는 false로 초기화
          flag2=false;
      
          Serial.println("ESP32 send data to TCP Client");
          webSocket.Server.sendData("운동 일자 : " + date + "운동 기구" + machine_name + "횟수 : " + String(count));
        }
        delay(10);
      }
      else{
        count = 0;
        flag1 = false;
        flag2 = false;
      }
    }
    Serial.println("The client disconnected");

    delay(10);
  }
  delay(10);
}
