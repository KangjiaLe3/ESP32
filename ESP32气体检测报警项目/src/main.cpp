#include <Arduino.h>
#include <U8g2lib.h>
#include <SensirionI2cScd4x.h>
#include <Wire.h>

#define buzzer 15
#define led_red 14
#define led_yellow 13
#define led_green 12
#define PIN_MQ9 34
bool buzzerstat = LOW;
bool warmup = false;
const float RL = 1.0;      // 负载电阻
const float Ro = 10.0;     //校准值
const float m = -1.5;      // 斜率
const float b = 2.0;       // 截距
unsigned long lastUpdateTime = 0;
unsigned long lastBuzzerTime = 0;
uint16_t co2_ppm=0;
float co_ppm = 0;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0,U8X8_PIN_NONE);
SensirionI2cScd4x scd4x;
void setup()
 {
  Serial.begin(115200);
  Wire.begin();
  u8g2.begin();
  u8g2.enableUTF8Print();           
  u8g2.setFont(u8g2_font_wqy12_t_gb2312b);
  pinMode(led_green,OUTPUT);
  pinMode(led_yellow,OUTPUT);
  pinMode(led_red,OUTPUT);
  pinMode(buzzer,OUTPUT);
  digitalWrite(buzzer,LOW);
  scd4x.begin((TwoWire&)Wire,SCD40_I2C_ADDR_62);
  scd4x.stopPeriodicMeasurement();
  scd4x.startPeriodicMeasurement();
}

void loop() 
{
  if (!warmup) {
    if (millis() < 180000)
     { 
      u8g2.clearBuffer();
      u8g2.setCursor(0, 30);
      u8g2.print("系统预热中...");
      u8g2.setCursor(0, 50);
      u8g2.print("倒计时: ");
      u8g2.print((180000 - millis()) / 1000);
      u8g2.print(" 秒");
      u8g2.sendBuffer();
      return; 
    } 
    else 
    {
      warmup = true; 
    }
  }
    if (millis() - lastUpdateTime >= 5000)
    {
    lastUpdateTime = millis();

    
    long sum = 0;
    for(int i=0; i<50; i++) { sum += analogRead(PIN_MQ9); delay(2); }
    float adcV = (sum / 50.0) * (3.3 / 4095.0); 

    if (adcV <= 0) adcV = 0.01; 
    float rs = ((3.3 - adcV) / adcV) * RL;

    float ratio = rs / Ro;
    co_ppm = 100.0 * pow(ratio, m);
    
    float temp, hum;
    uint16_t error;
    scd4x.readMeasurement(co2_ppm,temp,hum);
    digitalWrite(led_green,LOW);
    digitalWrite(led_red,LOW);
    digitalWrite(led_yellow,LOW);
    if(co_ppm>=100||co2_ppm>=2000)  
    {
      digitalWrite(led_red,HIGH);
    }
    else if(co_ppm>=60||co2_ppm>=1050)
    {
      digitalWrite(led_yellow,HIGH);
    }
    else
    {
      digitalWrite(led_green,HIGH);
    }
    u8g2.clearBuffer();
    u8g2.setCursor(0, 25);
    u8g2.print("CO浓度: ");
    u8g2.print(co_ppm, 1); 
    u8g2.print(" ppm");
    u8g2.setCursor(0, 45);
    u8g2.print("CO2浓度:  "); 
    u8g2.print(co2_ppm); 
    u8g2.print(" ppm");
    u8g2.sendBuffer();

    Serial.printf("CO: %.2f | CO2: %d\n", co_ppm, co2_ppm);
  }
     if (co_ppm >=100||co2_ppm>2000)
     {
    if (millis() - lastBuzzerTime >= 500) 
    { 
      lastBuzzerTime = millis();
      buzzerstat = !buzzerstat;
      digitalWrite(buzzer, buzzerstat); 
    }
  } 
  else if(co_ppm<100&&co2_ppm<=1900)
  {
    digitalWrite(buzzer, LOW);
    buzzerstat = LOW;
  }
}
  



