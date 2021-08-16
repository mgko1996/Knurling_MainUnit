#include <WiFi.h>
#include <WebSocketServer.h>
#include <ESP32SharpIR.h>

//포트 번호 정의(default 80)
WiFiServer server(80);

//WebSocketServer 라이브러리 사용을 위한 객체 선언
WebSocketServer webSocketServer;


//Sharp IR Sensor 신호선 33번 연결
ESP32SharpIR sensor( ESP32SharpIR::GP2Y0A21YK0F, 33);

int count = 0;
String machine_name = "LetPullDown";
static int distance;
static bool flag_up;
static bool flag_down;
static bool flag_machine;
static String data;


const char* ssid = "KT_GiGA_2G_Wave2_DC75";
const char* password = "db89kc5373";


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

 
  // esp32 서버 시작
  server.begin();
  delay(500);
}

void loop(){
  // 서버가 현재 사용 가능한 상태이면 클라이언트 요청 받을 준비
  WiFiClient client = server.available();

  // 클라이언트랑 연결되고, 소켓끼리 통신 가능 여부 확인
  if (client.connected() && webSocketServer.handshake(client)) {
    //클라이언트와 연결되어있는 동안, 클라이언트에서 보내는 정보를 받음 
    while (client.connected()){
      data = webSocketServer.getData();
      //클라이언트에서 받아지는 값이 NULL이 아니기 위해 조건을 걸어서 데이터를 처리
      if(data.length() > 0){
        //받아진 데이터가 "true"라는 String일 경우, 무한 루프 속에 갇힘(실시간 거리값에 의한 연산만 이루어질 수 있도록)
        if(data == "true"){
          //운동 기구에 대한 정보가 한 번만 보내질 수 있도록 flag_machine을 플래그로 박아두고 한 번만 보내지도록 코딩
          while(true){
            if(flag_machine == false){
                webSocketServer.sendData("운동 기구 : " + machine_name);
                flag_machine = true;
            }    
            distance = sensor.getDistance();
            delay(100);
            Serial.print("거리 : ");
            Serial.println(distance);
  
            //플레이트 까지의 거리가 20cm 를 넘을 때, flag_up을 true로 설정.
            if (distance<=20){
              flag_up=true;
            }
            
            //플레이트 까지의 거리가 40cm 를 넘을 때, flag_down를 true로 설정.
            if ((flag_up==true) && (40<=distance)){
              flag_down=true;
            }
            
            //flag_up과 flag_down가 모두 true일 때, count를 증가시킴.
            if ((flag_up==true) && (flag_down==true)){
              count += 1;
              Serial.print(count);
              Serial.println();  
              
              flag_up=false; //flag_up과 flag_down는 false로 초기화
              flag_down=false;

              Serial.println("ESP32 send data to TCP Client");
              webSocketServer.sendData(" 횟수 : " + String(count));
              continue;
            }

            //안드로이드에서 "ws.close()" 즉, 먼저 TCP 연결 끊는 코드 작성 후 이 조건문이 실행되는지 확인해야함
            if(!client.connected()){
              Serial.println("연결 끊어짐");
              continue;
            }
          }
        }
        // 스마트폰을 통해 받아진 데이터가 false일 경우, 모든 데이터들을 초기화
        else{
          Serial.println("스마트폰에게 false를 전달 받음");
          count = 0;
          flag_up = false;
          flag_down = false;
          flag_machine = false;
        }
      }
    }
    Serial.println("The client disconnected");
  }
  delay(10);
}