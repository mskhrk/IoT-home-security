#include <MsTimer2.h>
#include <Servo.h>
/*馬達*/
Servo myservo;

/*移動參數*/
int OneFloorDelay = 890;    //一片木地板行走時間900ns
int FloorDeviation = 50;    //前進時煞車距離誤差
int TurnAround15 =  130;    //旋轉15度所需時間(ns)
int TurnAround90 =  660;    //旋轉90度所需時間(ns)
int TurnAround180 = 1200;   //旋轉180度所需時間(ns)
int TurnAround360 = 2400;   //旋轉360度所需時間(ns)
int CarSpeed = 200;         //移動速度0~255,初始值200

/*控制旗標*/
int stopFlag = 0;           //停止移動旗標
int stopScan = 0;           //停止掃描旗標

/*腳位設定*/
int rb1 = 12;
int rb2 = 11;
int rf1 = 8;
int rf2 = 3;
int lf1 = 7;
int lf2 = 5;
int lb1 = 4;
int lb2 = 6;

/*
int lf1 = 12;
int lf2 = 11;
int lb1 = 8;
int lb2 = 3;
int rb1 = 7;
int rb2 = 5;
int rf1 = 4;
int rf2 = 6;
*/

/*計時器timer參數(5.0版後版本移除timer功能)*/
unsigned long CountTime = 0;
int CountRst = 0;

/*uart外部命令存放字串*/
String s1 = " ";
String s2 = " ";
String s3 = " ";
String s4 = " ";
String s5 = " ";
String s6 = " ";

/*超音波參數*/
int Trig_pin;
int Echo_pin;
long Time_out;
long duration;

//----------------------------

void setup() {

  /*arduino 對應腳位*/
  pinMode(lf1, OUTPUT); //左前
  pinMode(lf2, OUTPUT); //左前
  pinMode(lb1, OUTPUT); //左後
  pinMode(lb2, OUTPUT); //左後
  pinMode(rb1, OUTPUT); //右後
  pinMode(rb2, OUTPUT); //右後
  pinMode(rf1, OUTPUT); //右前
  pinMode(rf2, OUTPUT); //右前
  
  hcrs04_init(18, 19, 3000);        //超音波初始腳位 & timeout設定
  Serial.begin(9600);               //uart通訊速度
  myservo.attach(10, 500, 2400);   //修正伺服脈衝寬度範圍
  myservo.write(90);               //伺服馬達起始角度
  

}


void loop() {

  CommunicationSerial();                  //解析uart送來的指令、並執行
  
  if (stopFlag == 1)  Stop();             //當停止旗標被設定就停止移動
  
  if (stopScan == 1)  Ultrasound_sacn();  //掃描車子超音波範圍內是否有障礙
}

//-超音波---------------------------

/*初始設定函式*/
void hcrs04_init(int TP, int EP, long TO)
{
  pinMode(TP, OUTPUT);
  pinMode(EP, INPUT);
  Trig_pin = TP;
  Echo_pin = EP;
  Time_out = TO;
}

/*超音波掃描函式 回傳值為距離所跑的時間*/
long Timing()
{
  digitalWrite(Trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(Trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig_pin, LOW);
  duration = pulseIn(Echo_pin, HIGH, Time_out);
  if ( duration == 0 ) {
    duration = Time_out;
  }
  return duration;
}

/*超音波掃描函式 回傳值為距離(cm)*/
long Ranging()
{
  Timing();
  delayMicroseconds(15);
  return (duration / 29 / 2) ;
}

/*  
 *  超音波掃描函式 測量前方20cm是否有障礙物
 *  有障礙物會設定停止旗標讓車停止(stopFlag)
 *  此函式只有在stopScan被設為1時執行
 *  本函式由loop()來啟動
*/
void Ultrasound_sacn()
{
  int scanAns[6];
  int average = 0;
  int newAverage = 0;
  int count2 = 0;
  int value = 0;
  int i;
    for (i = 0; i < 5; i++) {
      scanAns[i] = Ranging();
      average += scanAns[i];
    }

    average = average / 5;
    for (i = 0; i < 5; i++) {
      if (average - scanAns[i] > 20 || average - scanAns[i] < -20) {}
      else {
        newAverage += scanAns[i];
        count2++;
      }
    }
    if ((newAverage / count2) < 20) {
      stopFlag = 1;
      stopScan = 0;
    }
    average = 0;
    newAverage = 0;
    count2 = 0;
    value = 0;
}

//----------------------------------
/*
 * timer計時
 * 因 timer2 library 會和pwm搶timer所以暫時不使用 
 */
void CountTimeing()
{
  CountTime++;
}

//-----------------------------------

/*uart轉換字串程式*/
String Rx2String()
{
  String s = "";
  char c;
  while (Serial.available() == 0) {}
  while (Serial.available() > 0) {

    c = Serial.read();
    //3層if 有換行空白enter就 break
    if (c == ';')break;
    if (c != '\r') {
      if (c != '\n') {
        if (c != ' ') s += c;
        else {
          break;
        }
      }
      else {
        break;
      }
    }
    else {
      break;
    }

    while (Serial.available() == 0) {} //等下一個字來
  }
  return s;
}

/*uart 指令分析程式*/
void CommunicationSerial()
{
  int value = 0;
  if (Serial.available() > 0) {
    value = Serial.read();
    switch (value) {
      case 10:
        break;

      case 13:
        break;

      case 65:
        Serial.println("in modeA");
        modeA();
        break;

      case 66:
        Serial.println("in modeB");
        modeB();
        break;

      case 67:
      Serial.println("in modeC");
        modeC();
        break;

      case 68:
      Serial.println("in modeD");
        modeD();
        break;

      case 69:
      Serial.println("in modeE");
        modeE();
        break;

      case 70:
      Serial.println("in modeF");
        modeF();
        break;

      case 71:
      Serial.println("in modeG");
        modeG();
        break;

      case 72:
      Serial.println("in modeH");
        modeH();
        break;

      case 73:
      Serial.println("in modeI");
        modeI();
        break;

      case 74:
      //Serial.println("in modeJ");
        modeJ();
        break;

      case 75:
      //Serial.println("in modeK");
        modeK();
        break;

      case 80:
      Serial.println("in modeP");
        modeP();
        break;

      //default:
        //Serial.write(value);
        //Serial.println("  error mode select");
    }
  }
}



void modeA()
{
  int value = 0;
  while (Serial.available() == 0) {}
  value = Serial.read();
  while (Serial.available() == 0) {}
  switch (value) {
    case 49:
      value = Serial.read();
      if (value == 49) {
        go(1);
      }
      else if (value == 50)go(2);
      else if (value == 51)go(3);
      else if (value == 52)go(4);
      else if (value == 53)go(5);
      else if (value == 54)go(6);
      else if (value == 55)go(7);
      else if (value == 56)go(8);
      else if (value == 57)go(9);
      break;

    case 50:
      value = Serial.read();
      if (value == 49)goback(1);
      else if (value == 50)goback(2);
      else if (value == 51)goback(3);
      else if (value == 52)goback(4);
      else if (value == 53)goback(5);
      else if (value == 54)goback(6);
      else if (value == 55)goback(7);
      else if (value == 56)goback(8);
      else if (value == 57)goback(9);
      break;

    default:
      Serial.println("error input in modeA");
  }
}


void modeB()
{
  int value = 0;
  while (Serial.available() == 0) {}
  value = Serial.read();
  while (Serial.available() == 0) {}
  switch (value) {
    case 49:
      value = Serial.read();
      while (Serial.available() == 0) {}
      if (value == 49) {
        value = Serial.read();
        if (value == 49)goRight(1000, 1);
        else if (value == 50)goRight(1000, 2);
        else if (value == 51)goRight(1000, 3);
      }
      else if (value == 50) {
        value = Serial.read();
        if (value == 49)goRight(2000, 1);
        else if (value == 50)goRight(2000, 2);
        else if (value == 51)goRight(2000, 3);
      }
      else if (value == 51) {
        value = Serial.read();
        if (value == 49)goRight(3000, 1);
        else if (value == 50)goRight(3000, 2);
        else if (value == 51)goRight(3000, 3);
      }
      else if (value == 52) {
        value = Serial.read();
        if (value == 49)goRight(4000, 1);
        else if (value == 50)goRight(4000, 2);
        else if (value == 51)goRight(4000, 3);
      }
      else if (value == 53) {
        value = Serial.read();
        if (value == 49)goRight(5000, 1);
        else if (value == 50)goRight(5000, 2);
        else if (value == 51)goRight(5000, 3);
      }
      else if (value == 54) {
        value = Serial.read();
        if (value == 49)goRight(6000, 1);
        else if (value == 50)goRight(6000, 2);
        else if (value == 51)goRight(6000, 3);
      }
      else if (value == 55) {
        value = Serial.read();
        if (value == 49)goRight(7000, 1);
        else if (value == 50)goRight(7000, 2);
        else if (value == 51)goRight(7000, 3);
      }
      else if (value == 56) {
        value = Serial.read();
        if (value == 49)goRight(8000, 1);
        else if (value == 50)goRight(8000, 2);
        else if (value == 51)goRight(8000, 3);
      }
      else if (value == 57) {
        value = Serial.read();
        if (value == 49)goRight(9000, 1);
        else if (value == 50)goRight(9000, 2);
        else if (value == 51)goRight(9000, 3);
      }
      break;

    case 50:
      value = Serial.read();
      while (Serial.available() == 0) {}
      if (value == 49) {
        value = Serial.read();
        if (value == 49)goLeft(1000, 1);
        else if (value == 50)goLeft(1000, 2);
        else if (value == 51)goLeft(1000, 3);
      }
      else if (value == 50) {
        value = Serial.read();
        if (value == 49)goLeft(2000, 1);
        else if (value == 50)goLeft(2000, 2);
        else if (value == 51)goLeft(2000, 3);
      }
      else if (value == 51) {
        value = Serial.read();
        if (value == 49)goLeft(3000, 1);
        else if (value == 50)goLeft(3000, 2);
        else if (value == 51)goLeft(3000, 3);
      }
      else if (value == 52) {
        value = Serial.read();
        if (value == 49)goLeft(4000, 1);
        else if (value == 50)goLeft(4000, 2);
        else if (value == 51)goLeft(4000, 3);
      }
      else if (value == 53) {
        value = Serial.read();
        if (value == 49)goLeft(5000, 1);
        else if (value == 50)goLeft(5000, 2);
        else if (value == 51)goLeft(5000, 3);
      }
      else if (value == 54) {
        value = Serial.read();
        if (value == 49)goLeft(6000, 1);
        else if (value == 50)goLeft(6000, 2);
        else if (value == 51)goLeft(6000, 3);
      }
      else if (value == 55) {
        value = Serial.read();
        if (value == 49)goLeft(7000, 1);
        else if (value == 50)goLeft(7000, 2);
        else if (value == 51)goLeft(7000, 3);
      }
      else if (value == 56) {
        value = Serial.read();
        if (value == 49)goLeft(8000, 1);
        else if (value == 50)goLeft(8000, 2);
        else if (value == 51)goLeft(8000, 3);
      }
      else if (value == 57) {
        value = Serial.read();
        if (value == 49)goLeft(9000, 1);
        else if (value == 50)goLeft(9000, 2);
        else if (value == 51)goLeft(9000, 3);
      }
      break;

    default:
      Serial.println("error input in modeB");
  }
}


void modeC()
{
  int value = 0;
  while (Serial.available() == 0) {}
  value = Serial.read();
  switch (value) {
    case 48:
      Stop();
      break;

    case 49:
      forward();
      break;

    case 50:
      back();
      break;

    default:
      Serial.println("error input in modeC");
  }
}

void modeD()
{
  int value = 0;
  while (Serial.available() == 0) {}
  value = Serial.read();
  while (Serial.available() == 0) {}
  switch (value) {
    case 49:
      value = Serial.read();
      if (value == 48)right(0);
      else if (value == 49)right(1);
      else if (value == 50)right(2);
      else if (value == 51)right(3);
      else if (value == 52)right(4);
      break;

    case 50:
      value = Serial.read();
      if (value == 48)left(0);
      else if (value == 49)left(1);
      else if (value == 50)left(2);
      else if (value == 51)left(3);
      else if (value == 52)left(4);
      break;

    default:
      Serial.println("error input in modeD");
  }
}


void modeE()
{
  s1 = "";
  int value = 0;
  while (Serial.available() == 0) {}
  value = Serial.read();
  while (Serial.available() == 0) {}
  switch (value) {
    case 49:
      value = Serial.read();
      if (value == 49)SetCarSpeed(1);
      else if (value == 50)SetCarSpeed(2);
      else if (value == 51)SetCarSpeed(3);
      break;
    case 50:
      value = Serial.read();
      s1 = Rx2String();
      CarSpeed = atoi(s1.c_str());//c_str()string->char[] | atoi() is char[] to int
      break;
    case 51:
      value = Serial.read();
      if (value == 49) CarSpeed = CarSpeed + 10;
      else if (value == 50)CarSpeed = CarSpeed - 10;
      if (CarSpeed > 255)CarSpeed = 255;
      if (CarSpeed < 70)CarSpeed = 70;
      break;
    case 52:
      value = Serial.read();
      if (value == 48)stopFlag = 0;
      else if (value == 49)stopFlag = 1;
      break;

    default:
      Serial.println("error input in modeE");
  }
}


void modeF()
{
  s1 = "";
  int value = 0;
  while (Serial.available() == 0) {}
  value = Serial.read();
  while (Serial.available() == 0) {}
  switch (value) {
    case 49:
      value = Serial.read();
      s1 = Rx2String();
      OneFloorDelay = atoi(s1.c_str());//c_str()string->char[] | atoi() is char[] to int
      break;
    case 50:
      value = Serial.read();
      s1 = Rx2String();
      FloorDeviation = atoi(s1.c_str());//c_str()string->char[] | atoi() is char[] to int
      break;
    case 51:
      value = Serial.read();
      s1 = Rx2String();
      TurnAround90 = atoi(s1.c_str());//c_str()string->char[] | atoi() is char[] to int
      break;
    case 52:
      value = Serial.read();
      s1 = Rx2String();
      TurnAround180 = atoi(s1.c_str());//c_str()string->char[] | atoi() is char[] to int
      break;
    case 53:
      value = Serial.read();
      s1 = Rx2String();
      CarSpeed = atoi(s1.c_str());//c_str()string->char[] | atoi() is char[] to int
      break;
    case 54:
      value = Serial.read();
      s1 = Rx2String();
      TurnAround15 = atoi(s1.c_str());//c_str()string->char[] | atoi() is char[] to int
      break;
    default:
      Serial.println("error input in modeF");
  }
}

void modeG()
{
  s1 = "";
  s2 = "";
  s3 = "";
  int value = 0;
  int addScan = 1; //MAX415 不可大於4988/12  增加掃描的筆數100
  unsigned int sampleId = 1;
  unsigned long count;
  unsigned long overtime = 0;
  while (Serial.available() == 0) {}
  value = Serial.read();
  while (Serial.available() == 0) {}
  switch (value) {
    case 49:
      value = Serial.read();
      s1 = Rx2String();
      s2 = Rx2String();
      s3 = Rx2String();

      CarSpeed = atoi(s1.c_str());//c_str()string->char[] | atoi() is char[] to int
      TurnAround360 = atoi(s2.c_str());//c_str()string->char[] | atoi() is char[] to int
      Time_out = atol(s3.c_str());//c_str()string->char[] | atol() is char[] to long
      //overtime = TurnAround360/50;
      overtime = TurnAround360 / 100;
      sampleId = 1;
      int i;
      /*數值測試
         Serial.print("overtime ");Serial.println(overtime);
        Serial.print("TurnAround360 ");Serial.println(TurnAround360);
        Serial.print("CarSpeed ");Serial.println(CarSpeed);
        Serial.print("Time_out ");Serial.println(Time_out);*/
      Serial.println("#0 -star-");//輸出開始訊號
      right(0);//開始迴旋
      for (count = 0; count <= overtime; count++) {
        for (i = 0; i < addScan; i++) {
          Serial.print("#");
          Serial.print(sampleId);
          sampleId++;
          Serial.print(" -");
          Serial.print(Ranging());
          Serial.println("-");
        }
        delayMicroseconds(4988 - (12 * addScan));
        delay(95);//每一次循環總和12+988+99000=100000us=100ms
      }

      Stop();
      Serial.println("#0 -over-;");//輸出結束訊號
      break;

    default:
      Serial.println("error input in modeG");
  }
}


void modeH()
{
  s1 = "";
  s2 = "";
  s3 = "";
  s4 = "";
  int rfSpeed, rbSpeed, lbSpeed, lfSpeed;
  int value = 0;
  while (Serial.available() == 0) {}
  value = Serial.read();
  while (Serial.available() == 0) {}
  switch (value) {
    case 49:
      value = Serial.read();
      s1 = Rx2String();
      s2 = Rx2String();
      s3 = Rx2String();
      s4 = Rx2String();

      rfSpeed = atoi(s1.c_str());//c_str()string->char[] | atoi() is char[] to int
      rbSpeed = atoi(s2.c_str());//c_str()string->char[] | atoi() is char[] to int
      lbSpeed = atoi(s3.c_str());//c_str()string->char[] | atol() is char[] to int
      lfSpeed = atoi(s4.c_str());//c_str()string->char[] | atol() is char[] to int

      if (rfSpeed <= 255)
      {
        analogWrite(rf2, rfSpeed);//rf
        digitalWrite(rf1, LOW);
      }
      else {
        analogWrite(rf2, 255 - (rfSpeed - 1000));
        digitalWrite(rf1, HIGH);
      }

      if (rbSpeed <= 255)
      {
        analogWrite(rb2, 255 - rbSpeed); //11
        digitalWrite(rb1, HIGH);
      }
      else {
        analogWrite(rb2, rbSpeed - 1000); //11
        digitalWrite(rb1, LOW);
      }

      if (lbSpeed <= 255)
      {
        analogWrite(lb2, 255 - lbSpeed); //11
        digitalWrite(lb1, HIGH);
      }
      else {

        analogWrite(lb2, lbSpeed - 1000); //11
        digitalWrite(lb1, LOW);
      }

      if (lfSpeed <= 255)
      {
        analogWrite(lf2, lfSpeed);//11
        digitalWrite(lf1, LOW);
      }
      else {
        analogWrite(lf2, 255 - (lfSpeed - 1000)); //11
        digitalWrite(lf1, HIGH);
      }

    default:
      Serial.println("error input in modeH");
  }
}

void modeI()
{
  
  /*原始modeI timer程式
   * if(CountRst==1){CountRst=0;CountTime=0;}
    if(CountTime!=0)
    {MsTimer2::stop( );CountRst=1;}
    else
    { MsTimer2::set(10,CountTimeing); // INTERVAL 1ms 0.001 秒
    MsTimer2::start( );}*/
  
  /*因timer 不使用 modeI 改為設定stopScan 掃描旗標
    當呼叫後就改變旗標狀態
  */
  if (stopScan == 1) {
    stopScan = 0;
  }
  else {
    stopScan = 1;
  }
}


void modeJ()
{
  s1 = "";
  s2 = "";
  s3 = "";
  s4 = "";
  s5 = "";
  int scanAns[20];
  int average = 0;
  int newAverage = 0;
  int count2 = 0;
  int starAngle, overAngle, oneAngle, scanTime;
  int value = 0;
  int i;
  unsigned int sampleId = 1;
  unsigned long count;
  while (Serial.available() == 0) {}
  value = Serial.read();
  while (Serial.available() == 0) {}
  switch (value) {
    case 49:
      value = Serial.read();
      s1 = Rx2String();//起始角度
      s2 = Rx2String();//結束角度
      s3 = Rx2String();//每一次轉動的角度
      s4 = Rx2String();//每一次轉動的超音波發射次數
      s5 = Rx2String();//超音波timeout

      starAngle = atoi(s1.c_str());//c_str()string->char[] | atoi() is char[] to int
      overAngle = atoi(s2.c_str());//c_str()string->char[] | atoi() is char[] to int
      oneAngle = atoi(s3.c_str());//c_str()string->char[] | atoi() is char[] to int
      scanTime = atoi(s4.c_str());//c_str()string->char[] | atoi() is char[] to int
      Time_out = atol(s5.c_str());//c_str()string->char[] | atol() is char[] to long

      if (starAngle > 180 || starAngle < 0) {
        Serial.println("starAngle error");
        break;
      }
      if (overAngle > 180 || overAngle < 0) {
        Serial.println("overAngle error");
        break;
      }
      if (oneAngle > 180 || oneAngle < 0) {
        Serial.println("oneAngle error");
        break;
      }
      if (scanTime > 20 || scanTime < 0) {
        Serial.println("scanTime error");
        break;
      }
      if (Time_out < 0) {
        Serial.println("Time_out error");
        break;
      }

      Serial.println("#0 -star-");//輸出開始訊號
      for (count = starAngle; count <= overAngle; count += oneAngle) {
        myservo.write(count);
        delay(300);//10ms電源共用間隔延遲
        for (i = 0; i < scanTime; i++) {
          Serial.print("#");
          Serial.print(sampleId);
          sampleId++;
          Serial.print(" -");
          Serial.print(Ranging());
          Serial.println("-");
        }
        delay(300);//10ms電源共用間隔延遲
      }
      myservo.write(90); //轉回中間
      Serial.println("#0 -over-;");//輸出結束訊號
      break;

    case 50:
      value = Serial.read();
      s1 = Rx2String();//目標角度
      s2 = Rx2String();//每一次轉動的超音波發射次數
      s3 = Rx2String();//超音波timeout

      starAngle = atoi(s1.c_str());//c_str()string->char[] | atoi() is char[] to int
      scanTime = atoi(s2.c_str());//c_str()string->char[] | atoi() is char[] to int
      Time_out = atol(s3.c_str());//c_str()string->char[] | atol() is char[] to long

      if (starAngle > 180 || starAngle < 0) {
        Serial.println("starAngle error");
        break;
      }
      if (scanTime > 20 || scanTime < 0) {
        Serial.println("scanTime error");
        break;
      }
      if (Time_out < 0) {
        Serial.println("Time_out error");
        break;
      }

      Serial.println("#0 -star-");//輸出開始訊號

      myservo.write(starAngle);
      delay(100);//10ms電源共用間隔延遲
      for (i = 0; i < scanTime; i++) {
        scanAns[i] = Ranging();
        average += scanAns[i];
      }
      average = average / scanTime;
      for (i = 0; i < scanTime; i++) {
        if (average - scanAns[i] > 30 || average - scanAns[i] < -30) {}
        else
        {
          newAverage += scanAns[i];
          count2++;
        }
      }
      if ((newAverage / count2) < 20)stopFlag = 1;
      Serial.print("#"); Serial.print(sampleId); Serial.print(" -"); Serial.print((newAverage / count2)); Serial.println("-");
      average = 0;
      newAverage = 0;
      count2 = 0;
      delay(100);//10ms電源共用間隔延遲
      myservo.write(90); //轉回中間
      Serial.println("#0 -over-;");//輸出結束訊號
      break;

    default:
      //Serial.println("error input in modeJ");
      //Serial.write(value);
      {}
  }
}

void modeK()
{
  s1 = "";
  int value = 0;
  int rotationAngle=0;//外部輸入旋轉角度
  while (Serial.available() == 0) {}
  value = Serial.read();
  while (Serial.available() == 0) {}
  switch (value) {
    case 49:
      value = Serial.read();
      s1 = Rx2String();
      rotationAngle = atoi(s1.c_str());//c_str()string->char[] | atoi() is char[] to int
      if(rotationAngle>360 ||rotationAngle<0){Serial.println("rotationAngle error");}
      rotationAngle=rotationAngle/15;
      if(rotationAngle==6){turnAround(1); }
      if(rotationAngle==12){turnAround(2); }
      if(rotationAngle!=6&&rotationAngle!=12){
      for(;rotationAngle!=0;rotationAngle--)
      {turnAround(5); }
      }
      break;
    case 50:
      value = Serial.read();
      s1 = Rx2String();
      rotationAngle = atoi(s1.c_str());//c_str()string->char[] | atoi() is char[] to int
      if(rotationAngle>360 ||rotationAngle<0){Serial.println("rotationAngle error");}
      rotationAngle=rotationAngle/15;
      if(rotationAngle==6){turnAround(3); }
      if(rotationAngle==12){turnAround(4); }
      if(rotationAngle!=6&&rotationAngle!=12){
      for(;rotationAngle!=0;rotationAngle--)
      {turnAround(6); }
      }
      break;
      
    default:
      Serial.println("error input in modeK");
  }
}



void modeP()
{
  Serial.println("");
  Serial.print("OneFloorDelay "); Serial.println(OneFloorDelay);
  Serial.print("FloorDeviation "); Serial.println(FloorDeviation);
  Serial.print("TurnAround90 "); Serial.println(TurnAround90);
  Serial.print("TurnAround180 "); Serial.println(TurnAround180);
  Serial.print("TurnAround360 "); Serial.println(TurnAround360);
  Serial.print("CarSpeed "); Serial.println(CarSpeed);
  Serial.print("lf1 "); Serial.println(lf1);
  Serial.print("lf2 "); Serial.println(lf2);
  Serial.print("lb1 "); Serial.println(lb1);
  Serial.print("lb2 "); Serial.println(lb2);
  Serial.print("rb1 "); Serial.println(rb1);
  Serial.print("rb2 "); Serial.println(rb2);
  Serial.print("rf1 "); Serial.println(rf1);
  Serial.print("rf2 "); Serial.println(rf2);
  Serial.print("s1 "); Serial.println(s1);
  Serial.print("s2 "); Serial.println(s2);
  Serial.print("s3 "); Serial.println(s3);
  Serial.print("s4 "); Serial.println(s4);
  Serial.print("s5 "); Serial.println(s5);
  Serial.print("s6 "); Serial.println(s6);
  Serial.print("Trig_pin "); Serial.println(Trig_pin);
  Serial.print("Echo_pin "); Serial.println(Echo_pin);
  Serial.print("Time_out "); Serial.println(Time_out);
  Serial.print("duration "); Serial.println(duration);
  Serial.print("CountTime "); Serial.println(CountTime);
  Serial.print("CountRst "); Serial.println(CountRst);
  Serial.print("stopScan "); Serial.println(stopScan);
  Serial.print("stopFlag "); Serial.println(stopFlag);
  Serial.print(";");
}


void SetCarSpeed(int i)//1:高速 2:中速 3:低速
{

  switch (i)
  {
    case 1: {
        OneFloorDelay = 950;//一片木地板行走時間900ns
        FloorDeviation = 50; //前進時煞車距離誤差
        CarSpeed = 255;
        break;
      }

    case 2: {
        OneFloorDelay = 1300;//一片木地板行走時間900ns
        FloorDeviation = 50; //前進時煞車距離誤差
        CarSpeed = 180;
        break;
      }

    case 3: {
        OneFloorDelay = 3000;//一片木地板行走時間900ns
        FloorDeviation = 0; //前進時煞車距離誤差
        CarSpeed = 100;
        break;
      }

    default: {
        Stop();
      }
  }

}


void goRight(int n1, int n2) {
  //輸入向右走的時間(nsec)和程度(1急轉 2快速右轉 3右偏)
  //程度1 急轉:轉90度後直走，此時n1為轉完後直走時間
  //程度2 快速右轉，此時n1為右轉全程時間
  //程度3 右偏，此時n1為右偏全程時間
  int tem;
  if (n2 == 1)
  {
    tem = CarSpeed; //轉彎強置全速
    CarSpeed = 200; //轉彎強置全速
    turnAround(1);//轉彎強置全速
    CarSpeed = tem; //轉彎強置全速
    //forward();
    //delay(n1);
    Stop();

  }
  else if (n2 == 2)
  {
    right(3);
    delay(n1);
    Stop();
  }
  else if (n2 == 3)
  {
    right(1);
    delay(n1);
    Stop();
  }
  else Stop();
}


void goLeft(int n1, int n2) {
  //輸入向左走的時間(nsec)和程度(1急轉 2快速左轉 3左偏)
  //程度1 急轉:轉90度後直走，此時n1為轉完後直走時間
  //程度2 快速左轉，此時n1為左轉全程時間
  //程度3 左偏，此時n1為左偏全程時間
  int tem;
  if (n2 == 1)
  {
    tem = CarSpeed; //轉彎強置全速
    CarSpeed = 200; //轉彎強置全速
    turnAround(3);//轉彎強置全速
    CarSpeed = tem; //轉彎強置全速
    //forward();
    //delay(n1);
    Stop();
  }
  else if (n2 == 2)
  {
    left(3);
    delay(n1);
    Stop();
  }
  else if (n2 == 3)
  {
    left(1);
    delay(n1);
    Stop();
  }
  else Stop();
}


void go(int n1) { //輸入向前走的格子數 允許數值1~10

  //誤差修正
  if (n1 < 1) Stop();
  else if (n1 == 1) FloorDeviation = 0;
  else if (n1 == 2) FloorDeviation = 45;
  else if (n1 == 3) FloorDeviation = 110;
  else if (10 >= n1 >= 4) FloorDeviation = 145;
  else Stop();

  forward();
  delay((OneFloorDelay - FloorDeviation)*n1); //一格距離*一格行走時間
  Stop();
}

void goback(int n1) { //輸入向後走的格子數 允許數值1~10

  //誤差修正
  if (n1 < 1) Stop();
  else if (n1 == 1) FloorDeviation = 0;
  else if (n1 == 2) FloorDeviation = 45;
  else if (n1 == 3) FloorDeviation = 110;
  else if (10 >= n1 >= 4) FloorDeviation = 145;
  else Stop();

  back();
  delay((OneFloorDelay - FloorDeviation)*n1); //一格距離*一格行走時間
  Stop();
}

void turnAround(int i)//1:右90度 2:右180度 3:左90度 4:左180度 5:右15度 6:左15度
{

  switch (i)
  {
    case 1: {
        right(0);
        delay(TurnAround90);
        Stop();
        break;
      }

    case 2: {
        right(0);
        delay(TurnAround180);
        Stop();
        break;
      }

    case 3: {
        left(0);
        delay(TurnAround90);
        Stop();
        break;
      }

    case 4: {
        left(0);
        delay(TurnAround180);
        Stop();
        break;
      }

    case 5: {
        right(0);
        delay(TurnAround15);
        Stop();
        break;
      }

    case 6: {
        left(0);
        delay(TurnAround15);
        Stop();
        break;
      }
    default: {
        Stop();
      }
  }

}


void Stop(int t)
{
  delay(t);
  lb(0);
  lf(0);
  rb(0);
  rf(0);
}

void Stop()
{
  lb(0);
  lf(0);
  rb(0);
  rf(0);
}

void back()
{
  lb(2);
  lf(2);
  rb(2);
  rf(2);
}

void forward()
{
  lb(1);
  lf(1);
  rb(1);
  rf(1);
}
void left(int i)//left<->right
{
  //0右迴旋
  //1慢速右前
  //2慢速右後
  //3快速右前
  //4快速右後
  switch (i)
  {
    case 1:
      {
        lb(1);
        lf(1);
        rb(1);
        rf(0);
        break;
      }

    case 2:
      {
        lb(2);
        lf(2);
        rb(0);
        rf(2);
        break;
      }

    case 3:
      {
        lb(1);
        lf(1);
        rb(0);
        rf(0);
        break;
      }

    case 4:
      {
        lb(2);
        lf(2);
        rb(0);
        rf(0);
        break;
      }

    default:
      {
        lb(1);
        lf(1);
        rb(2);
        rf(2);
        break;
      }
  }

}
void right(int i)////left<->right
{
  //0左迴旋
  //1慢速左前
  //2慢速左後
  //3快速左前
  //4快速左後
  switch (i)
  {
    case 1:
      {
        lb(1);
        lf(0);
        rb(1);
        rf(1);
        break;
      }

    case 2:
      {
        lb(0);
        lf(2);
        rb(2);
        rf(2);
        break;
      }

    case 3:
      {
        lb(0);
        lf(0);
        rb(1);
        rf(1);
        break;
      }

    case 4:
      {
        lb(0);
        lf(0);
        rb(2);
        rf(2);
        break;
      }

    default:
      {
        lb(2);
        lf(2);
        rb(1);
        rf(1);
        break;
      }
  }

}

void rf(int i)
{
  //單輪 0不動 1前 2後
  switch (i)
  {
    case 1: {
        //digitalWrite(rf2, HIGH);
        analogWrite(rf2, CarSpeed);//11
        digitalWrite(rf1, LOW);
        break;
      }

    case 2: {
        //digitalWrite(rf2, LOW);
        analogWrite(rf2, 255 - CarSpeed);
        digitalWrite(rf1, HIGH);
        break;
      }

    default: {
        digitalWrite(rf2, LOW);
        digitalWrite(rf1, LOW);
      }
  }
}


void lb(int i)
{
  //單輪 0不動 1前 2後
  switch (i)
  {
    case 2: {
        analogWrite(rb2, CarSpeed);//11
        //  digitalWrite(rb2, HIGH);
        digitalWrite(rb1, LOW);
        break;
      }

    case 1: {
        //digitalWrite(rb2, LOW);
        analogWrite(rb2, 255 - CarSpeed); //11
        digitalWrite(rb1, HIGH);
        break;
      }

    default: {
        digitalWrite(rb2, LOW);
        digitalWrite(rb1, LOW);
      }
  }
}


void rb(int i)
{
  //單輪 0不動 1前 2後
  switch (i)
  {
    case 2: {
        //digitalWrite(lb2, HIGH);
        analogWrite(lb2, CarSpeed);//11
        digitalWrite(lb1, LOW);
        break;
      }

    case 1: {
        //digitalWrite(lb2, LOW);
        analogWrite(lb2, 255 - CarSpeed); //11
        digitalWrite(lb1, HIGH);
        break;
      }

    default: {
        digitalWrite(lb2, LOW);
        digitalWrite(lb1, LOW);
      }
  }
}


void lf(int i)
{
  switch (i)
  {
    //單輪 0不動 1前 2後
    case 1: {
        //digitalWrite(lf2, HIGH);
        analogWrite(lf2, CarSpeed);//11
        digitalWrite(lf1, LOW);
        break;
      }

    case 2: {
        analogWrite(lf2, 255 - CarSpeed); //11
        //digitalWrite(lf2, LOW);
        digitalWrite(lf1, HIGH);
        break;
      }

    default: {
        digitalWrite(lf2, LOW);
        digitalWrite(lf1, LOW);
      }
  }
}

