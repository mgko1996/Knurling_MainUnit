#include <WiFi.h>
#include <WebSocketServer.h>
#include <ESP32SharpIR.h>
#include <time.h>

//포트 번호 정의(default 80)
WiFiServer server(80);
WebSocketServer webSocketServer;


//Sharp IR Sensor 신호선 33번 연결
ESP32SharpIR sensor( ESP32SharpIR::GP2Y0A21YK0F, 33);

int count = 0;
String machine_name = "LetPullDown";
static int distance;
static bool time_flag;
static bool flag_up = false;
static bool flag_down = false;
static String data;

const char* ntpServer = "pool.ntp.org";
uint8_t timeZone = 9; 
uint8_t summerTime = 0; // 3600 

const char* ssid = "KT_GiGA_2G_Wave2_DC75";
const char* password = "db89kc5373";


// 시간 관련 구조체에서 현재 시간과 관련된 정보인 date 추출
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

  //보드 레이트 115200 설정
  Serial.begin(115200);

  //센서 filter 하이퍼파라미터 정의
  sensor.setFilterRate(0.4f);

  //와이파이 연결
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
  
  //시간 재설정 및 정보 가져오기
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  //esp32 서버 시작
  server.begin()
  delay(500);
  time_flag = true
}

void loop(){
  WiFiClient client = server.available();
  
  if (client.connected() && webSocketServer.handshake(client)) {

    while (client.connected()) {
      data = webSocketServer.getData();
      if(data == "true"){
        if(time_flag==true){
          printLocalTime();
          time_flag=false;
        }
        distance = sensor.getDistance();
        webSocket.Server.sendData(data);
        if (distance<=20){
          flag_up=true;
        } //플레이트 까지의 거리가 20cm 를 넘을 때, flag_up을 true로 설정.
        
        if ((flag_up==true) && (40<=distance)){
          flag_down=true;
        } //플레이트 까지의 거리가 40cm 를 넘을 때, flag_down를 true로 설정.
  
        if ((flag_up==true) && (flag_down==true))//flag_up과 flag_down가 모두 true일 때, count를 증가시킴.
        {
          count+=1;
          Serial.print(" Count  :: ");
          Serial.print(count);
          Serial.println();   
      
          flag_up=false; //flag_up과 flag_down는 false로 초기화
          flag_down=false;
      
          Serial.println("ESP32 send data to TCP Client");
          webSocket.Server.sendData("운동 일자 : " + date + "운동 기구" + machine_name + "횟수 : " + String(count));
        }
        delay(10);
      }
      else{
        count = 0;
        flag_up = false;
        flag_down = false;
        time_flag = true;
      }
    }
    Serial.println("The client disconnected");

    delay(10);
  }
  delay(10);
}
