#include <WiFi.h>
#include <WebSocketServer.h>
#include <ESP32SharpIR.h>
#include "BluetoothSerial.h"

//machine_port : 기구들마다 다르게 부여할 예정
const int MACHINE_PORT = 80;
WiFiServer server(MACHINE_PORT); //TODO: server 동작 확인

//Sharp IR Sensor 신호선 33번 연결
const int IR_SENSOR_SIGNAL_LINE = 33;
ESP32SharpIR sensor( ESP32SharpIR::GP2Y0A21YK0F, IR_SENSOR_SIGNAL_LINE);

static int SHARP_IR_DETECT_DISTANCE_CM;
static bool FLAG_UP;
static bool FLAG_DOWN;
static String mobile_connect_status_with_nfc;
const int SHARP_IR_DETECT_INTERVAL_MILLISECONDS = 100;
static bool FLAG_DEBUG = true;
static String weight;
BluetoothSerial SerialBT;
String MACadd = "AA:BB:CC:11:22:33";
String name = "ESP32test";
const char *pin = "1234"; //<- standard pin would be provided by default
uint8_t address[6]  = {0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33};
bool connected;
static int COUNT_10 = 0;
static int COUNT_20 = 0;
static int COUNT_30 = 0;
static int COUNT_40 = 0;
static int COUNT_50 = 0;
static int COUNT_60 = 0;
static int COUNT_70 = 0;
static int COUNT_80 = 0;
static int COUNT_90 = 0;


void setup() {

  const int ESP32_BAUDRATE = 115200;
  const float SHARPIR_FILTERRATE = 0.4f;
  const char* ssid = "KT_GiGA_2G_Wave2_DC75";
  const char* wifi_password = "db89kc5373";

  Serial.begin(ESP32_BAUDRATE);
  sensor.setFilterRate(SHARPIR_FILTERRATE);

  //TODO: begin 동작 확인, 연결 실패했을 때,  예외처리 있는지 확인
  WiFi.begin(ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

  if(FLAG_DEBUG){
    Serial.println("[setup] server.begin() start");
  }

  // esp32 서버 시작
  server.begin();
  delay(500);

  if(FLAG_DEBUG){
    Serial.println("[setup] server.begin() success");
  }
  SerialBT.begin("ESP32test", true); 
  Serial.println("The device started in master mode, make sure remote BT device is on!");
  connected = SerialBT.connect(name);
  Serial.println("Trying Connection");
  while(connected != true){
    while(!SerialBT.connected(10000)){
      Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app."); 
    }
    if(SerialBT.connected() == false){
      connected = SerialBT.connect(name);
      Serial.println("Trying Connection");
    }
  }
 Serial.println("Connected Succesfully!");
}

void loop() {
  if(FLAG_DEBUG){
    Serial.println("[loop] start");
  }

  WiFiClient mobile_app = server.available();
  WebSocketServer webSocketServer;

  if (mobile_app.connected() && webSocketServer.handshake(mobile_app)) {
    if(FLAG_DEBUG){
      Serial.println("[loop] mobile_app.connected() && webSocketServer.handshake(mobile_app)");
    }
    while (mobile_app.connected()) {
      if(FLAG_DEBUG){
        Serial.println("[loop] mobile_app.connected()");
      }

      mobile_connect_status_with_nfc = webSocketServer.getData();
      UPDOWN_COUNT = 0;

      if (mobile_connect_status_with_nfc && mobile_connect_status_with_nfc.length() > 0 ) {
          while (true) {
            if (!mobile_app.connected()) {
              UPDOWN_COUNT = 0;
              FLAG_UP = FLAG_DOWN = false;

              break;
            }

            mobile_connect_status_with_nfc = webSocketServer.getData();
            if (mobile_connect_status_with_nfc == "reconnect") {
              break;
            }

            if (SerialBT.available()) {
            
              char tmp_weight = (char)SerialBT.read();
              Serial.print("무게 알파벳 : ");
              Serial.println(tmp_weight);
              weight = String(tmp_weight);
            }


            
            SHARP_IR_DETECT_DISTANCE_CM = sensor.getDistance();
            Serial.print("SHARP_IR_DETECT_DISTANCE_CM : ");
            Serial.println(SHARP_IR_DETECT_DISTANCE_CM);

            //플레이트 까지의 거리가 20cm 를 넘을 때
            if (SHARP_IR_DETECT_DISTANCE_CM <= 20) {
              FLAG_UP = true;
            }

            //플레이트 까지의 거리가 40cm 를 넘을 때
            if ((FLAG_UP == true) && (40 <= SHARP_IR_DETECT_DISTANCE_CM)) {
              FLAG_DOWN = true;
            }

            if ((FLAG_UP == true) && (FLAG_DOWN == true)) {

              if(weight == "a"){
                COUNT_10 += 1;
                webSocketServer.sendData(weight + "," + String(COUNT_10));
              }
              else if(weight == "b"){
                COUNT_20 += 1;
                webSocketServer.sendData(weight + "," + String(COUNT_20));
              }
              else if(weight == "c"){
                COUNT_30 += 1;
                webSocketServer.sendData(weight + "," + String(COUNT_30));
              }
              else if(weight == "d"){
                COUNT_40 += 1;
                webSocketServer.sendData(weight + "," + String(COUNT_40));
              }
              else if(weight == "e"){
                COUNT_50 += 1;
                webSocketServer.sendData(weight + "," + String(COUNT_50));
              }
              else if(weight == "f"){
                COUNT_60 += 1;
                webSocketServer.sendData(weight + "," + String(COUNT_60));
              }
              else if(weight == "g"){
                COUNT_70 += 1;
                webSocketServer.sendData(weight + "," + String(COUNT_70));
              }
              else if(weight == "h"){
                COUNT_80 += 1;
                webSocketServer.sendData(weight + "," + String(COUNT_80));
              }
              else if(weight == "i"){
                COUNT_90 += 1;
                webSocketServer.sendData(weight + "," + String(COUNT_90));
              }
              else{
                webSocketServer.sendData("무게 인식이 되지 않았습니다");
              }
//              
//              UPDOWN_COUNT += 1;
              FLAG_UP = FLAG_DOWN = false; //FLAG_UP과 FLAG_DOWN는 false로 초기화

//              webSocketServer.sendData("무게" + weight +"횟수: " + String(UPDOWN_COUNT));
            }
            
            delay(SHARP_IR_DETECT_INTERVAL_MILLISECONDS);
          }
      }
      delay(10);
    }
  }
  COUNT_10 = 0;
  COUNT_20 = 0;
  COUNT_30 = 0;
  COUNT_40 = 0;
  COUNT_50 = 0;
  COUNT_60 = 0;
  COUNT_70 = 0;
  COUNT_80 = 0;
  COUNT_90 = 0;
  weight = " ";
}