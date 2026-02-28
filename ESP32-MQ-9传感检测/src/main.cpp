#include <Arduino.h>
#include <U8g2lib.h>

#define buzzer 15
#define led_red 14
#define led_yellow 13
#define led_green 12
#define PIN_MQ9 34
bool buzzerstat = LOW;
const float RL = 1.0;      // 负载电阻为 1kOhm
const float Ro = 10.0;     //校准值
const float m = -1.5;      // 斜率（来自手册对数曲线）
const float b = 2.0;       // 截距
unsigned long lastUpdateTime = 0;
unsigned long lastBuzzerTime = 0;
float ppm = 0;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0,U8X8_PIN_NONE);
void setup()
 {
  Serial.begin(115200);
  u8g2.begin();
  u8g2.enableUTF8Print();           
  u8g2.setFont(u8g2_font_wqy12_t_gb2312b);
  pinMode(led_green,OUTPUT);
  pinMode(led_yellow,OUTPUT);
  pinMode(led_red,OUTPUT);
  pinMode(buzzer,OUTPUT);
  digitalWrite(buzzer,LOW);
}

void loop() 
{
    if (millis() - lastUpdateTime >= 5000)
    {
    lastUpdateTime = millis();

    // 1. 获取平滑的 ADC 原始值
    long sum = 0;
    for(int i=0; i<50; i++) { sum += analogRead(PIN_MQ9); delay(2); }
    float adcV = (sum / 50.0) * (3.3 / 4095.0); // 转换为电压

    // 2. 计算 Rs (传感器当前电阻)
    // 防止除以 0
    if (adcV <= 0) adcV = 0.01; 
    float rs = ((3.3 - adcV) / adcV) * RL;

    // 3. 计算 PPM (使用幂函数 pow)
    // 公式: PPM = 100 * (Rs/Ro)^m
    float ratio = rs / Ro;
    ppm = 100.0 * pow(ratio, m);
    
    digitalWrite(led_green,LOW);
    digitalWrite(led_red,LOW);
    digitalWrite(led_yellow,LOW);
    if(ppm<60)  
    {
      digitalWrite(led_green,HIGH);
    }
    else if(ppm<100)
    {
      digitalWrite(led_yellow,HIGH);
    }
    else
    {
      digitalWrite(led_red,HIGH);
    }
    u8g2.clearBuffer();
    u8g2.setCursor(0, 45);
    u8g2.print("CO浓度: ");
    u8g2.print(ppm, 1); 
    u8g2.print(" ppm");
    u8g2.sendBuffer();

    Serial.printf("Voltage: %.2fV | Rs: %.2f | PPM: %.2f\n", adcV, rs, ppm);
  }
     if (ppm >=100)
     {
    if (millis() - lastBuzzerTime >= 500) 
    { 
      lastBuzzerTime = millis();
      buzzerstat = !buzzerstat;
      digitalWrite(buzzer, buzzerstat); 
    }
  } 
  else 
  {
    digitalWrite(buzzer, LOW);
    buzzerstat = LOW;
  }
}
  



