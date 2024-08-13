//ver 1
#include <Wire.h> //thư viện I2c của Arduino, do PCA9685 sử dụng chuẩn giao tiếp i2c nên thư viện này bắt buộc phải khai báo 
#include <Adafruit_PWMServoDriver.h> // thư viện PCA9685
#include <PS2X_lib.h> // thư viện điều 
#include "controlmotor.h"//thư viện tự tạo để điều khiển 2 joystick di chuyển
#include "Adafruit_TCS34725.h"



const int SENSOR_PIN = 10;//khai báo chân cắm cảm biến hồng ngoại 
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);//cài đặt thông số cho color sensor TCS34725 về INTEGRATIONTIME (Thời gian tích hợp) và gain (giá trị nhận được)
int speed=4096;//tốc độ tối đa của motor máy bắn
bool r1 = false;//biến của nút tròn
bool green = false;//biến của nút tam giác
bool pink = false;//biến của nút vuông nút n lmj ??



void setup() {
  pwm.begin(); //khởi tạo PCA9685 
  tcs.begin(); //khởi tạo PCA9685 
  //pwm.setOscillatorFrequency(27000000); // cài đặt tần số dao động 
  //pwm.setPWMFreq(50);// cài đặt tần số PWM. Tần số PWM có thể được cài đặt trong khoảng 24-1600 HZ, tần số này được cài đặt tùy thuộc vào nhu cầu xử dụng. Để điều khiển được cả servo và động cơ DC cùng nhau, tần số PWM điều khiển được cài đặt trong khoảng 50-60Hz.
  //Wire.setClock(400000); // cài đặt tốc độ giao tiếp i2c ở tốc độ cao nhất(400 Mhz). Hàm này có thể bỏ qua nếu gặp lỗi hoặc không có nhu cầu tử dụng I2c tốc độ cao
  int degree = 90;
  pwm.setPWM(6, 0, map(degree,0,360,0,614));
  pwm.setPWM(7, 0, 307);//setup chân ban đầu 2 servo outtake
  initMotors();
  setupPS2controller();//dùng trong thư viện điều khiển
  pwm.setPWM(5, 0, 307);
  pwm.setPWM(4, 0, 307);//set chân và đặt góc ban đầu cho servo tự động
  pinMode(SENSOR_PIN,INPUT);
}


void loop()
{
  //di chuyển
  ps2x.read_gamepad(0, 0);
  PS2control();//toàn bộ code di chuyển được đặt trong thư viện controlmotor.h
  if (ps2x.ButtonPressed(PSB_R1)) {r1=!r1;}// chuyển ngược biến lại khi nhấn nút
  if (ps2x.ButtonPressed(PSB_GREEN )){green=!green;}// chuyển ngược biến lại khi nhấn nút)
  if (ps2x.ButtonPressed(PSB_PINK)){pink =! pink;}// chuyển ngược biến lại khi nhấn nút

  Intake();//Khởi động hàm Intake
  Color_Sensor();//Khởi động hàm Color sensor
  Cam_Bien_Hong_Ngoai();//Khởi động hàm cảm biến hồng ngoại
  Outake_Bong_Trang();//Khởi động hàm outake cho bóng trắng
  Outake_Bong_Den();//Khởi động hàm outake cho bóng đen
}

void Outake_Bong_Trang() 
{
  if (r1==true) 
  {
    pwm.setPWM(8, 0, speed);//chân số 8 set chiều dương tối đa
    pwm.setPWM(9, 0, 0); // chân số 9 set chiều âm
  }
  else 
  {
    pwm.setPWM(8, 0, 0);
    pwm.setPWM(9, 0, 0); 
    // ngưng không hoạt đông
  }//-Nếu nhấn nút circle1 lần thì mortor sẽ quay và nếu nhấn lại lần nữa thì motor sẽ hủy
  
  while(r1==true)
  {
    int dem=millis();
    pwm.setPWM(6, 0, 205);
    if(dem==1000)
    {
      pwm.setPWM(6, 0, 256);
    }

    if(dem==2000)
    {
      pwm.setPWM(6, 0, 205);
      dem=0;
    }//servo sẽ tự động đóng mở khi outtake bắt đc bật mỗi 3s

  }//-trong khi motor đang quay thì sẽ có 1 servo hỗ trợ chặn bóng lại để motor bắn từng quả 1
}//-nhấn lần đầu intake sẽ quay và nhấn lầ 2 intake sẽ đóng lại
  

void Outake_Bong_Den() 
{
  if(green==true)
  {
    pwm.setPWM(7, 0, 205);
  }
  
  else
  {
    pwm.setPWM(7, 0, 307);  
  }//-khi nhấn cổng outtake sẽ được mở ra và nhấn lại lần nữa thì cổng sẽ đóng lại
}


void Intake() 
{
  if(pink==true)
  {
    pwm.setPWM(14, 0, speed);//chân số 14 set chiều dương tối đa
    pwm.setPWM(15, 0, 0);// chân số 15 set chiều âm
  }
  
  else
  {
    pwm.setPWM(14, 0, 0);
    pwm.setPWM(15, 0, 0);
    //ngưng hoạt động
  }
}
  

void Color_Sensor() 
{
  uint16_t r,g,b,c;
  //những giá trị  màu color sensor xác định bao gồm red, green, blue và clean
  tcs.getRawData(&r,&g,&b,&c);
  if ((c>2000)&&(r>1000)) 
  {
    pwm.setPWM(5, 0, 307);
    //nếu đó là bóng đen thì servo sẽ đóng lại
  }
  
  else if ((c>2000)&&(r<1000)) 
  {
      pwm.setPWM(5, 0, 205);
    }
    //nếu đó là bóng trắng thì servo sẽ được mở ra
}


void Cam_Bien_Hong_Ngoai() 
{
  int present = digitalRead(SENSOR_PIN);
  if (present == LOW) 
  {
    pwm.setPWM(4, 0, 307);
    //nếu đó là bóng đen thì servo sẽ đóng lại 
  }

  else
  {
    pwm.setPWM(4, 0,205);
      //nếu đó là bóng trắng thì servo sẽ được mở ra
  }
}
