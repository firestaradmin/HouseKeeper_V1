
#include <ESP8266WiFi.h>
//#include <WiFi.h>
#define AP_ssid   "iu" //这里改成你的设备当前环境下要连接的接入点名字
#define password  "lizhienya"         //这里改成你的设备当前环境下要连接的接入点密码

const uint16_t port = 8888;
const char * host = "120.55.45.151"; // ip or dns
WiFiClient client;//创建一个tcp client连接

int rxBuffer_tail = 0;
unsigned char rxBuffer[100];
bool isCommand;
void setup(void)
{
  Serial.begin(115200);  // 启动串口通讯，波特率设置为9600
  WiFi.mode(WIFI_STA);
  //调用 WiFi.begin()函数，开始连接接入点
  WiFi.begin(AP_ssid, password);

  //这里的循环用来判断是否连接成功的。连接过程中每隔500毫秒会检查一次是否连接成功，，并打一个点表示正在连接中
  //连接成功后会给出提示，但是若60秒后还是没有连接上，则会提示超时
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("网络连接成功");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned char id;
  id = get_FP_ID();
  Serial.println(String("id:")+id);
  if(id < 10){
    send_to_server("帅气庚");
  }
  else if(id < 20){
    send_to_server("聪明明");
  }
  else if(id < 30){
    send_to_server("憨憨瑶");
  }
  else if(id < 40){
    send_to_server("老实峰");
  }
  else if(id < 50){
    send_to_server("快乐诚");
  }
  else {
    send_to_server("其他憨憨");
  }

}

unsigned char get_FP_ID()
{
  getValue();
  return rxBuffer[0];
    
}

unsigned char* getValue()
{
  unsigned char inByte;
  while (true)
  {
    if (Serial.available() > 0)
    {
      inByte = Serial.read();
      //Serial.print(inByte);
      if (inByte == '[')
      {
        isCommand = true;
        rxBuffer_tail = 0;
        //rxBuffer[0] = 0;
      }
      else if (inByte == ']')
      {
        isCommand = false;
        rxBuffer[rxBuffer_tail] = '\0';
        return rxBuffer;
      }
      else if (isCommand)
      {
        rxBuffer[rxBuffer_tail++] = inByte;
      }
    }
  }
}


void send_to_server( String Name)
{
  //Serial.println("connecting to ");
  Serial.println(host);
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }

  // 发送数据到Tcp server
  //Serial.println("Send this data to server");
  client.println(String("\"{\"opreation\":\"open\",\"Note\":\""+Name+"\"}\""));
  //读取从server返回到响应数据

  //String line = client.readStringUntil('\r');
  //Serial.println(line);
  Serial.println("closing connection");
  client.stop();

  
}
