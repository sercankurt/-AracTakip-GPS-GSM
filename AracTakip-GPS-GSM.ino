#include <Wire.h>             //-----> M P U  6 0 5 0   Kutuphaneler
#include <TinyMPU6050.h>
#include <SoftwareSerial.h>   //-----> S I M  8 0 0     Kutuphaneler
SoftwareSerial SIM800L(0,1);
#include <TinyGPS++.h>        //-----> N E O  6 M       Kutuphaneler
TinyGPSPlus gps;

String TELNO = "05542147154"; //-----> Alıcı telefon numarası
int aramasuresi = 16000;

double latitude, longitude;
String link, response, mesaj;
int lastStringLength = response.length();

float pos_offset = 70;   //-----> Hareket sensoru hassasiyet ayarı
float neg_offset = -70;  //-----> Hareket sensoru hassasiyet ayarı
long angle_x, angle_y, angle_z, offset_x, offset_y, offset_z;
MPU6050 mpu (Wire);
int b = 0;
int sns = 0; //Sensor durum
int mpusure = 0;

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
SIM800L.println("ATD+ +9"+TELNO+";");         delay(aramasuresi);
SIM800L.println("ATH");                       delay(1000);

SerialUSB.println("Aranıyor...");

SIM800L.println("AT");                        delay(1000); 
SIM800L.println("AT+CMGF=1");                 delay(1000);
SIM800L.println("AT+CNMI=2,2,0,0,0");}

void BLINK(int b){
digitalWrite(LED_BUILTIN, LOW);  delay(b); 
digitalWrite(LED_BUILTIN, HIGH); delay(b);}

void MPUBILGI(){
  
 for(int i=0; i<10; i++){
  mpu.Execute();
  angle_x = mpu.GetAngX();
  angle_y = mpu.GetAngY();
  angle_z = mpu.GetAngZ(); }

if (sns==1) { 
if (pos_offset < angle_x - offset_x || neg_offset > angle_x - offset_x || pos_offset < angle_y - offset_y || neg_offset > angle_y - offset_y || pos_offset < angle_z - offset_z || neg_offset > angle_z - offset_z)
    {
  
  BLINK(b=500);
  mpusure++;   SerialUSB.print("Mpu Süre = "); SerialUSB.println(mpusure);
  SerialUSB.print("AngX = ");       SerialUSB.print(angle_x - offset_x);
  SerialUSB.print("  /  AngY = ");  SerialUSB.print(angle_y - offset_y);
  SerialUSB.print("  /  AngZ = ");  SerialUSB.println(angle_z - offset_z);
  mpu.Execute();
  offset_x = mpu.GetAngX();
  offset_y = mpu.GetAngY();
  offset_z = mpu.GetAngZ(); }
   }
}
void setup() {
    mpu.Initialize();
    Serial1.begin(9600);  //-----> GPS UART PORTLARI
    SerialUSB.begin(9600); //-----> TERMINAL 
    //while(!SerialUSB){;}
    SIM800L.begin(9600); //-----> SIM800L
 
    delay(10000); //-----> SIM800L bağlanana kadar verilmiş olan zaman
    
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
 
  for(int i=0; i<200; i++){
   mpu.Execute();
  offset_x = mpu.GetAngX();
  offset_y = mpu.GetAngY();
  offset_z = mpu.GetAngZ();
  }
  SerialUSB.print("offset_x = ");     SerialUSB.print(offset_x);
  SerialUSB.print("  /  offsetY = "); SerialUSB.print(offset_y);
  SerialUSB.print("  /  offsetZ = "); SerialUSB.println(offset_z);

 //SMS(mesaj="GPS cihazi baslatildi");

}

void loop() {
GPS();
MPUBILGI();

SIM800L.listen();
if (SIM800L.available()>0){ response = SIM800L.readStringUntil('\n');}
  if(lastStringLength != response.length()){
      if(response.indexOf("konumat")!=-1){    
        SMS(mesaj=link);
        } GPS();
  }

SIM800L.listen();
if (SIM800L.available()>0){ response = SIM800L.readStringUntil('\n');}
   if(lastStringLength != response.length()){
      if(response.indexOf("sensorac")!=-1){    
         SMS(mesaj="Hareket Sensoru Aktif");   
         sns=1;
         }
  }

SIM800L.listen();
if (SIM800L.available()>0){ response = SIM800L.readStringUntil('\n');}
   if(lastStringLength != response.length()){     
      if(response.indexOf("sensorkapat")!=-1){ 
        SMS(mesaj="Hareket Sensoru Kapatildi");        
        sns=0;
       }
  }


 
 if (sns==1 && mpusure>=10) { ARA(); delay(aramasuresi); mpusure=0; }
          

}


