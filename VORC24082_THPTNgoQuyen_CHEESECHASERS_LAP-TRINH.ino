//ver 1
#include <Wire.h> //thư viện I2c của Arduino, do PCA9685 sử dụng chuẩn giao tiếp i2c nên thư viện này bắt buộc phải khai báo 
#include <Adafruit_PWMServoDriver.h> // thư viện PCA9685
#include <PS2X_lib.h> // thư viện điều 
#include "controlmotor.h"//thư viện tự tạo để điều khiển 2 joystick di chuyển trong thư mục lib
#include "Adafruit_TCS34725.h"



const int SENSOR_PIN = 27;//khai báo chân cắm cảm biến hồng ngoại 
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);//cài đặt thông số cho color sensor TCS34725 về INTEGRATIONTIME (Thời gian tích hợp) và gain (giá trị nhận được)
int speed = 4096;//tốc độ tối đa của motor máy bắn
bool r1 = false;//biến của nút tròn
bool green = false;//biến của nút tam giác
bool pink = false;//biến của nút vuông nút
int degree = 90;


void setup() {
  tcs.begin(); //tìm địa chỉ cảm biến màu sắc
  pwm.setPWM(6, 0, map(degree,0,360,0,614)); //setup chân và góc ban đầu cho servo outtake phân tử nước
  pwm.setPWM(7, 0, 307);//setup chân và góc ban đầu cho servo outtake phân tử rác
  pwm.setPWM(5, 0, 307);//set chân và đặt góc ban đầu cho 2 servo 360 độ tự động của phân loại
  pwm.setPWM(4, 0, 307);//set chân và đặt góc ban đầu cho 2 servo 360 độ tự động của phân loại
  initMotors();
  setupPS2controller();//set up kết nối lặp vô hạn tới khi kết nối, thư viện controlmotor.h
  pinMode(SENSOR_PIN, INPUT);
}


void loop()
{
  ps2x.read_gamepad(0, 0); //class ps2x được khai báo trong thư viện controlmotor.h
  PS2control();//toàn bộ code di chuyển được đặt trong thư viện controlmotor.h
  if (ps2x.ButtonPressed(PSB_R1)) {r1 =! r1;}// đảo giá trị của biến mỗi khi nhấn nút
  if (ps2x.ButtonPressed(PSB_GREEN )) {green =! green;}// đảo giá trị của biến mỗi khi nhấn nút
  if (ps2x.ButtonPressed(PSB_PINK)) {pink =! pink;}// đảo giá trị của biến mỗi khi nhấn nút

  Intake();//Khởi động hàm Intake
  Color_Sensor();//Khởi động hàm Color sensor
  Cam_Bien_Hong_Ngoai();//Khởi động hàm cảm biến hồng ngoại
  Outake_Bong_Trang();//Khởi động hàm outake cho bóng trắng
  Outake_Bong_Den();//Khởi động hàm outake cho bóng đen
}

//------------------------------------------------------------------------------------------------

void Outake_Bong_Trang() 
{
  if (r1==true) 
  {
    pwm.setPWM(8, 0, speed);//chân số 8 set chiều dương tối đa
    pwm.setPWM(9, 0, 0); // chân số 9 set chiều âm
    // motor outtake quay ở tốc độ max
  }
  else 
  {
    pwm.setPWM(8, 0, 0);
    pwm.setPWM(9, 0, 0); 
    // ngưng không hoạt đông
  }//-Nếu nhấn nút circle 1 lần thì mortor sẽ quay và nếu nhấn lại lần nữa thì motor sẽ dừng
  unsigned long dem =0;
  dem=millis();
  while(r1==true)
  {
    pwm.setPWM(6, 0, 205);
    if ( (unsigned long) (millis() - dem) > 2000 ) //servo mở thanh chặn sau 2s
    {
      pwm.setPWM(6, 0, 256);
    }

    if ( (unsigned long) (millis() - dem) > 4000 ) //servo đóng thanh chặn sau 4s
    {
      pwm.setPWM(6, 0, 205); 
      dem=millis(); //set thời gian về 0s
    }//servo sẽ tự động đóng mở mỗi 2s từ khi ấn nút  

  }//-trong khi motor đang quay thì sẽ có 1 servo hỗ trợ chặn bóng lại để bắn từng quả 1
}//-nhấn lần đầu intake sẽ quay và nhấn lần 2 intake sẽ dừng lại

//--------------------------------------------------------------------------------------------------

void Outake_Bong_Den() 
{
  if(green==true)
  {
    pwm.setPWM(7, 0, 205); // servo quay 90 độ để mở cửa chặn phân tử rác
  }
  
  else
  {
    pwm.setPWM(7, 0, 307);  // servo về vị trí ban đầu để đóng cửa chặn
  }//-khi nhấn, cửa outtake phân tử rác sẽ được mở ra và nhấn lại lần nữa thì cổng sẽ đóng lại
}

//-------------------------------------------------------------------------------------------------

void Intake() 
{
  if(pink == true)
  {
    pwm.setPWM(14, 0, speed);//chân số 14 set chiều dương tối đa
    pwm.setPWM(15, 0, 0);// chân số 15 set chiều âm
    //motor intake quay ở tốc độ max
  }
  
  else
  {
    pwm.setPWM(14, 0, 0);
    pwm.setPWM(15, 0, 0);
    //motor intake dừng
  }
}

//----------------------------------------------------------------------------------------------------

void Color_Sensor() // tự động phân loại
{
  unsigned long time1 = 0;
  uint16_t r,g,b,c;
  //những giá trị  màu color sensor xác định bao gồm red, green, blue và clean
  tcs.getRawData(&r, &g, &b, &c);
  if ((c > 2000) && (r > 1000)) // nếu cảm biến màu thấy màu đen
  {
    if ( (unsigned long) (millis() - time1) > 300 ) // xoay servo trong 300ms
      time1=millis();
      pwm.setPWM(5, 0, 410); //servo quay lên 90 độ
    }
    //nếu đó là bóng đen và không có bóng thì servo sẽ quay lên 90 độ
  }
  
  else if ((c > 2000) && (r < 1000)) // cảm biến màu sắc thấy màu trắng
  {
    if ( (unsigned long) (millis() - time1) > 300 )// xoay servo trong 300ms
      time1=millis();
      pwm.setPWM(5, 0, 205);// servo quay xuống 90 độ
    }
  }
    //nếu đó là bóng trắng thì servo quay xuống 90 độ
}

//---------------------------------------------------------------------------------------

void Cam_Bien_Hong_Ngoai() // tự động phân loại
{
  unsigned long time = 0;
  int present = digitalRead(SENSOR_PIN);
  if (present == LOW) 
  {
    if ( (unsigned long) (millis() - time) > 300 )// xoay servo trong 300ms
    {
      time = millis();
      pwm.setPWM(5, 0, 205); //servo quay lên 90 độ
    }
    //nếu đó là bóng đen và không có bóng thì servo sẽ quay lên 90 độ
  }

  else
  {
    if ( (unsigned long) (millis() - time) > 300 )// xoay servo trong 300ms
    {
      time = millis();
      pwm.setPWM(5, 0, 410); //servo quay xuống 90 độ
    }
    //nếu đó là bóng trắng thì servo quay xuống 90 độ
  }
}
//Tuấn gay