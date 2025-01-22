#include <Arduino.h>
#include <cstdio>
#include <SPI.h>
#include <SdFat.h>
#include <EEPROM.h>
#include <String>

#define BUZZER_PIN PA4
#define LED_PIN PC13
#define TF_MOSI_PIN PB5
#define TF_MISO_PIN PB4
#define TF_SCK_PIN PB3
#define TF_CS_PIN PA15

bool on_off = 0;
int open_num=114514;
String Name = "666"; 
bool fe;
int num;

File TF;
File config;
SdFat SD;
static SPIClass mySPI2(TF_MOSI_PIN,TF_MISO_PIN,TF_SCK_PIN,TF_CS_PIN);
#define SD2_CONFIG SdSpiConfig(TF_CS_PIN, DEDICATED_SPI, SD_SCK_MHZ(18), &mySPI2)
#define first_open EEPROM.read(9)

void LED(bool on_off){
    if(on_off){
        digitalWrite(LED_PIN,0x1);
    }
    else{
        digitalWrite(LED_PIN,0x0);
    }
}
void setup(){
    Serial.begin(115200);
    EEPROM.begin();
    pinMode(BUZZER_PIN,OUTPUT);
    pinMode(LED_PIN,OUTPUT);
    pinMode(PB11,INPUT);
    pinMode(TF_CS_PIN,OUTPUT);
    tone(BUZZER_PIN,532);
    delay(100);
    tone(BUZZER_PIN,587);
    delay(100);
    tone(BUZZER_PIN,659);
    delay(300);
    noTone(BUZZER_PIN);
    if(!(first_open == 88)){
        EEPROM.write(9,88);
        EEPROM.write(0,1);
        Serial.println("eeprom.write:9_88/0_1");
    }
    else{
        open_num = EEPROM.read(0);
        EEPROM.write(0,open_num+1);
        String out1 = String(open_num);
        String out2 = String(open_num+1);
        Serial.println("eeprom.read:0_" + out1 + "\neeprom.write:0_" + out2);
    }

    if(!SD.begin(SD2_CONFIG)){
        Serial.println("no!");
        tone(BUZZER_PIN,988);
        delay(3000);
        noTone(BUZZER_PIN);
    }
    else{
        Serial.println("yes!");
    }


    config = SD.open("config.ini");
    if (!config) {
        Serial.println("Error opening config.ini");
        return;
    }
    Serial.println("Reading config.ini:");
    while (config.available()) {
        String line = config.readStringUntil('\n'); // 读取每一行
        line.trim(); // 去除首尾空格
        if (line.startsWith("[") || line.startsWith(";") || line == "") {
            continue; // 跳过节标题、注释和空行
        }
        // 解析键值对
        int equalPos = line.indexOf('=');
        if (equalPos > 0) {
            String key = line.substring(0, equalPos);
            key.trim();
            String value = line.substring(equalPos + 1);
            value.trim();
            if (key == "功能1") {
                fe = value.equalsIgnoreCase("true"); // 转换为布尔值
            } else if (key == "num") {
                num = value.toInt(); // 转换为整数
            } else if (key == "Name") {
                Name = value; // 直接存储为字符串
            }
        }
    }
    config.close();
    Serial.println(Name);
    TF = SD.open(Name + ".csv", FILE_WRITE);
    if(TF){
        tone(BUZZER_PIN,659);
        delay(100);
        noTone(BUZZER_PIN);
    }
    else {
        tone(BUZZER_PIN,988);
        delay(1000);
        noTone(BUZZER_PIN);
    }
    TF.println("");
    TF.close();
}

void loop(){
    if(digitalRead(PB11)){
        delay(10);
        if(digitalRead(PB11)){
            if(on_off==1){
                on_off = 0;
            }
            else{
                on_off = 1;
            }
            while(digitalRead(PB11));
        }
    }
    LED(on_off);
    
}