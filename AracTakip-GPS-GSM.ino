#include <Wire.h>             //-----> M P U  6 0 5 0   Kutuphaneler
#include <TinyMPU6050.h>
#include <SoftwareSerial.h>   //-----> S I M  8 0 0     Kutuphaneler
SoftwareSerial SIM800L(0,1);
#include <TinyGPS++.h>        //-----> N E O  6 M       Kutuphaneler
TinyGPSPlus gps;

const String TELNO = "05532941477"; //-----> Alıcı telefon numarası
double latitude, longitude;
String link, response, mesaj;
int lastStringLength = response.length();

float pos_offset = 20, neg_offset = -20;  //-----> Hareket sensoru hassasiyet ayarı
long angle_x, angle_y, angle_z, offset_x, offset_y, offset_z;
MPU6050 mpu (Wire);
int b, i=0;
bool j = false;

void setup() {
    mpu.Initialize();
    Serial1.begin(9600);  //-----> GPS UART PORTLARI
    SerialUSB.begin(9600); //-----> TERMINAL 
    //while(!SerialUSB){;}
    SIM800L.begin(9600); //-----> SIM800L
 
    delay(15000); //-----> SIM800L bağlanana kadar verilmiş olan zaman
    
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); delay(500);  
  digitalWrite(LED_BUILTIN, HIGH);delay(500);
  digitalWrite(LED_BUILTIN, LOW); delay(400); 
  digitalWrite(LED_BUILTIN, HIGH);delay(400);
  digitalWrite(LED_BUILTIN, LOW); delay(300); 
  digitalWrite(LED_BUILTIN, HIGH);delay(300);
  digitalWrite(LED_BUILTIN, LOW); delay(200); 
  digitalWrite(LED_BUILTIN, HIGH);delay(200);
  digitalWrite(LED_BUILTIN, LOW); delay(100); 
  digitalWrite(LED_BUILTIN, HIGH);delay(100);
  
    SIM800L.println("AT");        delay(1000); 
    SIM800L.println("AT+CMGF=1"); delay(1000);
    SIM800L.println("AT+CNMI=2,2,0,0,0");

  SerialUSB.println("=====================================");
  SerialUSB.println("hareket sensoru kalibrasyonu bekleniyor...");
  mpu.Calibrate();
  SerialUSB.println("hareket sensoru kalibre edildi.");
 
  for(int i=0; i<50;i++){
   mpu.Execute();
  offset_x = mpu.GetAngX();
  offset_y = mpu.GetAngY();
  offset_z = mpu.GetAngZ();
  }
  SerialUSB.print("offset_x = ");     SerialUSB.print(offset_x);
  SerialUSB.print("  /  offsetY = "); SerialUSB.print(offset_y);
  SerialUSB.print("  /  offsetZ = "); SerialUSB.println(offset_z);

// SMS(mesaj="GPS cihazi baslatildi");

}

void loop() {
  
 if (j==false){ goto x;  }
 if (j==true) { MPUSMS();}
 x:
  
GPS();
//MPUBILGI();

SIM800L.listen();
if (SIM800L.available()>0){ response = SIM800L.readStringUntil('\n');}
  if(lastStringLength != response.length()){
      if(response.indexOf("konumat")!=-1){    
        SMS(mesaj=link);}
  }

SIM800L.listen();
if (SIM800L.available()>0){ response = SIM800L.readStringUntil('\n');}
   if(lastStringLength != response.length()){
      if(response.indexOf("sensorac")!=-1){       
         j==true;  }
  }

SIM800L.listen();
if (SIM800L.available()>0){ response = SIM800L.readStringUntil('\n');}
   if(lastStringLength != response.length()){     
      if(response.indexOf("sensorkapa")!=-1){         
       j==false;  }
  }

 /*switch (j) {
  case true:
    
   break; 
  case false:
      
    goto devam;
  break;
  }*/
 
}

void GPS(){
  if(Serial1.available()) {
    gps.encode(Serial1.read());}
        
  if(gps.location.isUpdated()) {
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    link = "www.google.com/maps/place/" + String(latitude, 6) + "," + String(longitude, 6) ;
    SerialUSB.println(link);}
}

void SMS(String mesaj){
SerialUSB.println("Sms gonderiliyor...");
SIM800L.println("AT+CMGF=1");                 delay(1000); 
SIM800L.println("AT+CMGS=\""+TELNO+"\"\r");   delay(1000);
SIM800L.println(mesaj);                       delay(1000);
SIM800L.println((char)26);                    delay(1000);
SerialUSB.println("Sms gonderildi.");}

void ARA(){ 
SIM800L.println("AT");                        delay(1000);
SIM800L.println("ATD+ "+TELNO+";");           delay(7000);
SIM800L.println("ATH");                       delay(1000);}

void BLINK(int b){
digitalWrite(LED_BUILTIN, LOW);  delay(b); 
digitalWrite(LED_BUILTIN, HIGH); delay(b);}

void MPUSMS(){
  
  mpu.Execute();
  angle_x = mpu.GetAngX();
  angle_y = mpu.GetAngY();
  angle_z = mpu.GetAngZ();
    
if ( pos_offset < angle_x - offset_x || neg_offset > angle_x - offset_x || pos_offset < angle_y - offset_y || neg_offset > angle_y - offset_y || pos_offset < angle_z - offset_z || neg_offset > angle_z - offset_z){

  BLINK(b=500);
  SerialUSB.print("AngX = ");       SerialUSB.print(angle_x - offset_x);
  SerialUSB.print("  /  AngY = ");  SerialUSB.print(angle_y - offset_y);
  SerialUSB.print("  /  AngZ = ");  SerialUSB.println(angle_z - offset_z);
  SMS(mesaj = link);
  mpu.Execute();
  offset_x = mpu.GetAngX();
  offset_y = mpu.GetAngY();
  offset_z = mpu.GetAngZ();
 }
}

void MPUBILGI(){
   
  mpu.Execute();
  angle_x = mpu.GetAngX();
  angle_y = mpu.GetAngY();
  angle_z = mpu.GetAngZ();
    
if ( pos_offset < angle_x - offset_x || neg_offset > angle_x - offset_x || pos_offset < angle_y - offset_y || neg_offset > angle_y - offset_y || pos_offset < angle_z - offset_z || neg_offset > angle_z - offset_z){

  BLINK(b=500);
  SerialUSB.print("AngX = ");       SerialUSB.print(angle_x - offset_x);
  SerialUSB.print("  /  AngY = ");  SerialUSB.print(angle_y - offset_y);
  SerialUSB.print("  /  AngZ = ");  SerialUSB.println(angle_z - offset_z);
 
  mpu.Execute();
  offset_x = mpu.GetAngX();
  offset_y = mpu.GetAngY();
  offset_z = mpu.GetAngZ();
 }
}
