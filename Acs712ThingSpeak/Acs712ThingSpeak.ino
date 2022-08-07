#include <SoftwareSerial.h>                                   //SoftwareSerial kütüphanimizi ekliyoruz.
#define VIN A0
const float VCC = 5.0;
const int model = 2;
float cutOffLimit = 1.01;
String agAdi = "acılıvayfi";                 //Ağımızın adını buraya yazıyoruz.    
String agSifresi = "12345678";           //Ağımızın şifresini buraya yazıyoruz.

int rxPin = 10;                                               //ESP8266 RX pini
int txPin = 11;                                               //ESP8266 TX pini
String ip = "184.106.153.149"; 

SoftwareSerial esp(rxPin, txPin);                             //Seri haberleşme pin ayarlarını yapıyoruz.
float sensitivity[]={0.185,0.100,0.066};

const float QOV = 0.5 * VCC;
float voltage;

void setup() {  
  
  Serial.begin(9600);  //Seri port ile haberleşmemizi başlatıyoruz.
  Serial.println("Başladı");
  esp.begin(115200);                                          //ESP8266 ile seri haberleşmeyi başlatıyoruz.
  esp.println("AT");                                          //AT komutu ile modül kontrolünü yapıyoruz.
  Serial.println("AT Yollandı");
  while(!esp.find("OK")){                                     //Modül hazır olana kadar bekliyoruz.
    esp.println("AT");
    Serial.println("ESP8266 Bulunamadı.");
  }
  Serial.println("OK Komutu Alındı");
  esp.println("AT+CWMODE=1");                                 //ESP8266 modülünü client olarak ayarlıyoruz.
  while(!esp.find("OK")){                                     //Ayar yapılana kadar bekliyoruz.
    esp.println("AT+CWMODE=1");
    Serial.println("Ayar Yapılıyor....");
  }
  Serial.println("Client olarak ayarlandı");
  Serial.println("Aga Baglaniliyor...");
  esp.println("AT+CWJAP=\""+agAdi+"\",\""+agSifresi+"\"");    //Ağımıza bağlanıyoruz.
  while(!esp.find("OK"));                                     //Ağa bağlanana kadar bekliyoruz.
  Serial.println("Aga Baglandi. Ag Adi "+agAdi);
  delay(1000);
}
void loop() {
  esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");           //Thingspeak'e bağlanıyoruz.
  if(esp.find("Error")){                                      //Bağlantı hatası kontrolü yapıyoruz.
    Serial.println("AT+CIPSTART Error");
  }
  String veri = "GET https://api.thingspeak.com/update?api_key=27L20R9EBNC6ENZ1";
  float voltage_raw = (5.0/1023.0) * analogRead(VIN);
  voltage = voltage_raw - QOV + 0.012;
  float current = voltage / sensitivity[model];
  veri += "&field1=";
  veri += String(voltage);
  veri += "&field2=";
  veri += String(current);
  veri += "\r\n\r\n"; 
  esp.print("AT+CIPSEND=");                                   //ESP'ye göndereceğimiz veri uzunluğunu veriyoruz.
  esp.println(veri.length()+2);
  delay(2000);
  if(esp.find(">")){                                          //ESP8266 hazır olduğunda içindeki komutlar çalışıyor.
    esp.print(veri);                                          //Veriyi gönderiyoruz.
    Serial.println(veri);
    Serial.println("Veri gonderildi.");
    delay(1000);
  }
  if(abs(current)>cutOffLimit ){
  Serial.print("V: ");
  Serial.print(voltage,3);
  Serial.print("V, I: ");
  Serial.print(current,2);
  Serial.println("A");
  }
  else{
    Serial.println("Akım Yok ");
  }
  
  delay(1000);                                               //Yeni veri gönderimi için 1 dakika bekliyoruz.
}