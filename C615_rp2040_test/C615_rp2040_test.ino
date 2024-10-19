#include <Servo.h>
#include <math.h>

#undef SERIAL
#define SERIAL Serial




const int AservoPin = 4; 
const int BservoPin = 3;
const int CservoPin = 5;

uint32_t Aval = 0; 
uint32_t Bval = 0;
uint32_t Cval = 0;
uint32_t Aval_last = 1000;
uint32_t Bval_last = 1000;
uint32_t Cval_last = 1500;
uint32_t Aline_multiple = 1;
uint32_t Bline_multiple = 1;
uint32_t Cline_multiple = 1;
uint32_t S_esp_x_min = -5; //  S加速曲线函数 x 最小值
uint32_t S_esp_x_max = 10; //  S加速曲线函数 X 最大值
Servo Aservo;
Servo Bservo;
Servo Cservo;

void line();
void SERIAL_app();

void setup(){
  SERIAL.begin(115200);
  // put your setup code here, to run once:
  Aservo.attach(AservoPin);
  Aservo.write(1000); 
  Bservo.attach(BservoPin);
  Bservo.write(1000);
  Cservo.attach(CservoPin);
  Cservo.write(1500);

}

void loop() {
  // put your main code here, to run repeatedly:
  SERIAL_app();
}

void SERIAL_app(){//  上位机交互程序
  String inString ="" ; //串口接收命令存放区
  while(SERIAL.available()>0){ //检查缓冲区是否存在数据
    inString += char(SERIAL.read()); //读取缓冲区
    delay(10);      // 延时函数用于等待字符完全进入缓冲区
  }



  if(inString!=""){ //命令解析区
    String command_head= "" ;//命令头缓存
    String command_text= "" ; //  命令内容缓存
    String command_mid= "" ;
    command_head = inString.substring(0,3);// 命令头格式"/00"
    command_mid = inString.substring(4,4);
    command_text = inString.substring(4,inString.length());
    /*解析命令头
    SERIAL.printf("command_head:");
    SERIAL.println(command_head);
    SERIAL.printf("command_mid:");
    SERIAL.println(command_mid);
    SERIAL.printf("command_text:");
    SERIAL.println(command_text);
    */


    /* 对A口写入PWM匀加速变化 */
    if(command_head == "/11")
    {
      Aval = command_text.toInt();
      Aline();
    }

    /* 对B口写入PWM匀加速变化 */
    if(command_head == "/12")
    {
      Bval = command_text.toInt();
      Bline();
    }
    
    /* 对AB口写入PWM匀加速变化 */
    if(command_head == "/13")
    {
      Aval = command_text.toInt();
      ABline();
    }

    /* A口匀加速倍数设置 */
    if(command_head == "/21")
    {
      Aline_multiple = command_text.toInt();
      SERIAL.printf("\nNow line_multiple:%d",Aline_multiple);
    }

    /* A口匀加速倍数设置 */
    if(command_head == "/22")
    {
      Bline_multiple = command_text.toInt();
      SERIAL.printf("\nline_multiple:%d",Bline_multiple);
    }

    /* AB口匀加速倍数设置 */
    if(command_head == "/23")
    {
      Aline_multiple = command_text.toInt();
      Bline_multiple = command_text.toInt();
      SERIAL.printf("\nline_multiple:%d",Bline_multiple);
    }


    /* A口PWM立即生效设置 */
    if(command_head == "/31")
    {
      Aval = command_text.toInt();
      SERIAL.printf("A_PWM us=");
      SERIAL.printf("%d\n",Aval);
      Aval_last = Aval;
      Aservo.writeMicroseconds(Aval);
    }

    /* B口PWM立即生效设置 */
    if(command_head == "/32")
    {
      Bval = command_text.toInt();
      SERIAL.printf("B_PWM us=");
      SERIAL.printf("%d\n",Bval);
      Bval_last = Bval;
      Bservo.writeMicroseconds(Bval);
    }

  /* AB口PWM立即生效设置 */
    if(command_head == "/33")
    {
      Aval = command_text.toInt();
      Bval = command_text.toInt();
      SERIAL.printf("A_PWM us=");
      SERIAL.printf("%d\n",Aval);
      SERIAL.printf("B_PWM us=");
      SERIAL.printf("%d\n",Bval);
      Aval_last = Aval;
      Bval_last = Bval;
      Aservo.writeMicroseconds(Aval);
      Bservo.writeMicroseconds(Bval);
    }

      /* C口PWM立即生效设置 */
    if(command_head == "/34")
    {
      Cval = command_text.toInt();
      SERIAL.printf("C_PWM us=");
      SERIAL.printf("%d\n",Cval);
      Cval_last = Cval;
      Cservo.writeMicroseconds(Cval);
    }


  /* A口S型变速*/
  if(command_head == "/41")
    {
      Aval = command_text.toInt();
      A_S_Speed();
    }

  /* B口S型变速 */
  if(command_head == "/42")
    {
      Bval = command_text.toInt();
      B_S_Speed();
    }


  /* 自动停止 */
  if(command_head == "/50")
    {
      Cval = 1500;
      Cval_last = Cval;
      Cservo.writeMicroseconds(Cval);
      delay(500);
      Aval = 1000 ;
      ABline();
    }
  
  /* 自动启动至怠速 */
  if(command_head == "/51")
    {
      Aval = 1071 ;
      ABline();
    }

  /* 自动启动至半速 */
  if(command_head == "/52")
    {
      Aval = 1520 ;
      ABline();
      delay(300);
      Cval = 1600;
      Cval_last = Cval;
      Cservo.writeMicroseconds(Cval);
    }

  /* 自动启动至全速 */
  if(command_head == "/53")
    {
      Aval = 1700 ;
      ABline();
      delay(300);
      Cval = 1700;
      Cval_last = Cval;
      Cservo.writeMicroseconds(Cval);
    } 


    
  /* 自动退弹 */
  if(command_head == "/59")
    {
      Cval =1000 ;
      SERIAL.printf("C_PWM us=");
      SERIAL.printf("%d\n",Cval);
      Cval_last = Cval;
      Cservo.writeMicroseconds(Cval);
      delay(500);
      Cval = 1500 ;
      SERIAL.printf("C_PWM us=");
      SERIAL.printf("%d\n",Cval);
      Cval_last = Cval;
      Cservo.writeMicroseconds(Cval);
    } 







    /* 自动初始化 */
    if(command_head == "/99")
    {
      Aservo.writeMicroseconds(2200);
      Bservo.writeMicroseconds(2200);
      delay(2000);
      Aservo.writeMicroseconds(0);
      Bservo.writeMicroseconds(0);
    }

    
  }

}




/* A口线性匀加速函数 */
void Aline(){
  if(Aval>Aval_last){
    for(int i=Aval_last;i<=Aval;i++){
      Aservo.writeMicroseconds(i);
      SERIAL.printf("A_PWM us=");
      SERIAL.printf("%d\n",i);
      delay(Aline_multiple);
    }
     SERIAL.printf("FINISH\n");
  }
  if(Aval<Aval_last){
    for(int i=Aval_last;i>=Aval;i--){
      Aservo.writeMicroseconds(i);
      SERIAL.printf("A_PWM us=");
      SERIAL.printf("%d\n",i);
      delay(Aline_multiple);
    }
    SERIAL.printf("FINISH\n");
  }
  if(Aval==Aval_last)
    SERIAL.printf("NO CHANGE!\n");
  Aval_last = Aval;
}



/* B口线性匀加速函数 */
void Bline(){
  if(Bval>Bval_last){
    for(int i=Bval_last;i<=Bval;i++){
      Bservo.writeMicroseconds(i);
      SERIAL.printf("B_PWM us=");
      SERIAL.printf("%d\n",i);
      delay(Bline_multiple);
    }
     SERIAL.printf("FINISH\n");
  }
  if(Bval<Bval_last){
    for(int i=Bval_last;i>=Bval;i--){
      Bservo.writeMicroseconds(i);
      SERIAL.printf("B_PWM us=");
      SERIAL.printf("%d\n",i);
      delay(Bline_multiple);
    }
    SERIAL.printf("FINISH\n");
  }
  if(Bval==Bval_last)
    SERIAL.printf("NO CHANGE!\n");
  Bval_last = Bval;
}



/* AB口线性匀加速函数 */
void ABline(){
  SERIAL.printf("\n\n\n");
  if(Aval>Aval_last){
    for(int i=Aval_last;i<=Aval;i++){
      Aservo.writeMicroseconds(i);
      Bservo.writeMicroseconds(i);
      //SERIAL.printf(".");
      delay(Aline_multiple);
    }
     SERIAL.printf("FINISH\n");
     SERIAL.printf("AB_PWM us=");
     SERIAL.printf("%d\n",Aval);
  }
  if(Aval<Aval_last){
    for(int i=Aval_last;i>=Aval;i--){
      Aservo.writeMicroseconds(i);
      Bservo.writeMicroseconds(i);
      //SERIAL.printf(".");
      delay(Aline_multiple);
    }
    SERIAL.printf("\n\n\n");
    SERIAL.printf("FINISH\n");
    SERIAL.printf("AB_PWM us=");
    SERIAL.printf("%d\n",Aval);
  }
  if(Aval==Aval_last)
    SERIAL.printf("\n\n\n");
    SERIAL.printf("NO CHANGE!\n");
  Aval_last = Aval;
}



/* A口S加速函数 */
void A_S_Speed(){
  if(Aval>Aval_last){
    int i=Aval_last,A_x=0,A_time=0;
    int A_am = 0;
    while(i<=Aval)
    {
      Aservo.writeMicroseconds(i);
      A_x = map(i,Aval_last,Aval,S_esp_x_min,S_esp_x_max);
      A_am = (1.0/(1+exp(-A_x)))*10000000;
      A_time = map(A_am,0,10000000,50,20);
      SERIAL.printf("A_PWM us=");
      SERIAL.printf("%d\n",i);
      SERIAL.printf("A_Delay ms=");
      SERIAL.printf("%d\n",A_time);
      SERIAL.printf("A_am =");
      SERIAL.printf("%d\n",A_am);
      SERIAL.printf("A_x =");
      SERIAL.printf("%d\n",A_x);
      delay(A_time);
      i++;
    }
  }
  if(Aval<Aval_last){
    int i=Aval_last,A_x=0,A_time=0;
    float A_am = 0;
    while(i>=Aval)
    {
      Aservo.writeMicroseconds(i);
      A_x = map(i,Aval_last,Aval,S_esp_x_min,S_esp_x_max);
      A_am = (1.0/(1+exp(-A_x)))*10000000;
      A_time = map(A_am,0,10000000,50,20);
      SERIAL.printf("A_PWM us=");
      SERIAL.printf("%d\n",i);
      SERIAL.printf("A_Delay ms=");
      SERIAL.printf("%d\n",A_time);
      SERIAL.printf("A_am =");
      SERIAL.printf("%d\n",A_am);
      SERIAL.printf("A_x =");
      SERIAL.printf("%d\n",A_x);
      delay(A_time);
      i--;
    }
  }
  Aval_last = Aval;
}
/* B口S加速函数 */
void B_S_Speed(){
  if(Bval>Bval_last){
    int i=Bval_last,B_x=0,B_time=0;
    int B_am = 0;
    while(i<=Bval)
    {
      Bservo.writeMicroseconds(i);
      B_x = map(i,Bval_last,Bval,S_esp_x_min,S_esp_x_max);
      B_am = (1.0/(1+exp(-B_x)))*10000000;
      B_time = map(B_am,0,10000000,50,20);
      SERIAL.printf("B_PWM us=");
      SERIAL.printf("%d\n",i);
      SERIAL.printf("B_Delay ms=");
      SERIAL.printf("%d\n",B_time);
      SERIAL.printf("B_am =");
      SERIAL.printf("%d\n",B_am);
      SERIAL.printf("B_x =");
      SERIAL.printf("%d\n",B_x);
      delay(B_time);
      i++;
    }
  }
  if(Bval<Bval_last){
    int i=Bval_last,B_x=0,B_time=0;
    float B_am = 0;
    while(i>=Bval)
    {
      Bservo.writeMicroseconds(i);
      B_x = map(i,Bval_last,Bval,S_esp_x_min,S_esp_x_max);
      B_am = (1.0/(1+exp(-B_x)))*10000000;
      B_time = map(B_am,0,10000000,50,20);
      SERIAL.printf("B_PWM us=");
      SERIAL.printf("%d\n",i);
      SERIAL.printf("B_Delay ms=");
      SERIAL.printf("%d\n",B_time);
      SERIAL.printf("B_am =");
      SERIAL.printf("%d\n",B_am);
      SERIAL.printf("B_x =");
      SERIAL.printf("%d\n",B_x);
      delay(B_time);
      i--;
    }
  }
  Bval_last = Bval;
}
