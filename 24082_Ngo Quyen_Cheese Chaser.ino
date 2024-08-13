#include <Wire.h> //thư viện I2c của Arduino, do PCA9685 sử dụng chuẩn giao tiếp i2c nên thư viện này bắt buộc phải khai báo 
#include <Adafruit_PWMServoDriver.h> // thư viện PCA9685
#include <PS2X_lib.h> // thư viện điều 
#include "controlmotor.h"//thư viện tự tạo để điều khiển 2 joystick di chuyển
#include "Adafruit_TCS34725.h"
Servo outtake2;//servo cho outtake bóng đen
Servo outtake1;//servo cho outtake bóng trắng
Servo colorsensor1;//servo cho color sensor TCS34725
Servo colorsensor2;//servo cho ir sensor
const int SENSOR_PIN = 10;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);//cài đặt thông số cho color sensor TCS34725 về INTEGRATIONTIME (Thời gian tích hợp) và gain (giá trị nhận được)
int speed=4096;//tốc độ tối đa của motor máy bắn
bool r1 = false;//biến của nút tròn
bool green = false;//biến của nút tam giác
bool pink = false;//biến của nút vuông
int IN1pin = 7;
int IN2pin = 8; 
int ENApin = 9; 
//chân cắm cho color sensor và ir sensor
void setup() {
  pinMode(IN1pin, OUTPUT);
  pinMode(IN2pin, OUTPUT);
  pinMode(ENApin, OUTPUT);
  //setup color sensor và ir sensor
  pwm.begin(); //khởi tạo PCA9685 
  pwm.setOscillatorFrequency(27000000); // cài đặt tần số dao động 
  pwm.setPWMFreq(50);// cài đặt tần số PWM. Tần số PWM có thể được cài đặt trong khoảng 24-1600 HZ, tần số này được cài đặt tùy thuộc vào nhu cầu xử dụng. Để điều khiển được cả servo và động cơ DC cùng nhau, tần số PWM điều khiển được cài đặt trong khoảng 50-60Hz.
  Wire.setClock(400000); // cài đặt tốc độ giao tiếp i2c ở tốc độ cao nhất(400 Mhz). Hàm này có thể bỏ qua nếu gặp lỗi hoặc không có nhu cầu tử dụng I2c tốc độ cao
  pwm.setPWM(6, 0, 150);
  pwm.setPWM(7, 0, 375);//setup chân ban đầu 2 servo outtake
  initMotors();
  setupPS2controller();//dùng trong thư viện điều khiển
  pwm.setPWM(5, 0, 375);
  pwm.setPWM(4, 0, 375);//set chân và đặt góc ban đầu cho servo tự động
}


void loop() {
  //di chuyển
  ps2x.read_gamepad(0, 0);
  PS2control();//toàn bộ code di chuyển được đặt trong thư viện controlmotor.h
  if (ps2x.ButtonPressed(PSP_R1)) {r1=!r1;}// chuyển ngược biến lại khi nhấn nút
  if(ps2x.ButtonPressed(PSB_Green )){green=!green;}// chuyển ngược biến lại khi nhấn nút)
  if(ps2x.ButtonPressed(PSB_PINK)){pink =! pink;}// chuyển ngược biến lại khi nhấn nút
  Intake()//Khởi động hàm Intake
  Color_Sensor()//Khởi động hàm Color sensor
  Cam_Bien_Hong_Ngoai()//Khởi động hàm cảm biến hồng ngoại
  Outake_Bong_Trang()//Khởi động hàm outake cho bóng trắng
  Outake_Bong_Den()//Khởi động hàm outake cho bóng đen
  }
  const Outake_Bong_Trang() {
  if (r1==true) {
    pwm.setPWM(8, 0, speed);//chân số 8 set chiều dương tối đa
    pwm.setPWM(9, 0, 0); // chân số 9 set chiều âm
  }
  else {
    pwm.setPWM(8, 0, 0);
    pwm.setPWM(9, 0, 0); 
    // ngưng không hoạt đông
  }//-Nếu nhấn nút circle1 lần thì mortor sẽ quay và nếu nhấn lại lần nữa thì motor sẽ hủy
  
  while(r1==true){
    int dem=millis();
    pwm.setPWM(6, 0, 150);
    if(dem==1000){
      pwm.setPWM(6, 0, 188);
    }
    if(dem==2000){
      pwm.setPWM(6, 0, 150);
      dem=0;
  }//servo sẽ tự động đóng mở khi outtake bắt đc bật mỗi 3s
  }//-trong khi motor đang quay thì sẽ có 1 servo hỗ trợ chặn bóng lại để motor bắn từng quả 1
  }//-nhấn lần đầu intake sẽ quay và nhấn lầ 2 intake sẽ đóng lại
  

  const Outake_Bong_Den() {
  if(green==true){
  pwm.setPWM(7, 0, 150);
  }
  else{
  pwm.setPWM(7, 0, 375);  
  }//-khi nhấn cổng outtake sẽ được mở ra và nhấn lại lần nữa thì cổng sẽ đóng lại
  }


  const Intake() {
  if(pink==true){
    pwm.setPWM(14, 0, speed);//chân số 14 set chiều dương tối đa
    pwm.setPWM(15, 0, 0);// chân số 15 set chiều âm
  }
  else{
    pwm.setPWM(14, 0, 0);
    pwm.setPWM(15, 0, 0);
    //ngưng hoạt động
  }
  }
  

  const Color_Sensor() {
    
    uint16_t r,g,b,c;
    //những màu color sensor xác định bao gồm red, green, blue và clean
  tcs.getRawData(&r,&g,&b,&c);
  int dem2=millis();
  if ((c>2500)&&(r+g+b>1500)) {
    pwm.setPWM(5, 0, 375);
    if(dem2==3000){
    pwm.setPWM(5, 0, 600);
    }
    if(dem2==6000){
      pwm.setPWM(5, 0, 375);
      dem2=0;
    }
    //nếu đó là bóng đen thì servo sẽ đóng lại
  }
  else if ((c>2500)&&(r+g+b<1500)) {
    pwm.setPWM(5, 0, 375);
    if(dem2==3000){
      pwm.setPWM(5, 0, 600);
    }
    else if(dem2==6000){
      pwm.setPWM(5, 0, 375);
      dem2=0;
    }  
    }
    //nếu đó là bóng trắng thì servo sẽ được mở ra
  }


  const Cam_Bien_Hong_Ngoai() {
    int dem3=millis();
    int present = digitalRead(SENSOR_PIN);
    if (present == LOW) {
      pwm.setPWM(4, 0, 600);
      if(dem3==3000){
        pwm.setPWM(4, 0, 375);
      }
      if(dem3==3000){
        pwm.setPWM(4, 0, 600);
        dem3=0;
      }
      //nếu đó là bóng đen thì servo sẽ đóng lại 
    }
    else {
      pwm.setPWM(4, 0, 600);
      if(dem3==3000){
        pwm.setPWM(4, 0, 375);
      }
      if(dem3==3000){
        pwm.setPWM(4, 0, 600);
        dem3=0;
      }
      //nếu đó là bóng trắng thì servo sẽ được mở ra
    }
  }