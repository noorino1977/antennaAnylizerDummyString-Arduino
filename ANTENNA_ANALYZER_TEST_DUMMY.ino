/*
This sketch is used to test the antenna analyzer android application. 
You can develop this Sketch it using the Arduino circuit and have installed a generator and antenna signal reader hardware according to your creativity. 
Calibrate your circuit so that it can generate SWR data, impedance, and reactance. which will be sent to Android via Bluetooth serial.
*/

#include <SoftwareSerial.h> 
#include <EEPROM.h>
#include "math.h"

static const uint32_t bluetooth_baud = 115200;
SoftwareSerial bluetooth(3, 2); // RX | TX

int counter=0;
String message;String dt[10];int i;boolean parsing=false;//serialnya
byte led = 13; bool on_off_led=false;

unsigned long test_freq; 
unsigned long low_freq;
unsigned long high_freq;
unsigned long center_freq; 

int multi_scan=1; 
String data_grafik;
byte modeplay;
            void setup() {
              pinMode(led, OUTPUT);digitalWrite(led,on_off_led);
              Serial.begin(9600);            
              bluetooth.begin(bluetooth_baud);  
                pinMode(A0,INPUT);
                pinMode(A1,INPUT);
                analogReference(INTERNAL);                       
            }
    
//============ 
void loop(){
  from_android();  
  if (modeplay==1){Sweeper_on();}
  if (modeplay==3){stand();}
}
 
void from_android() {
    if(bluetooth.available()>0) {char inChar = (char)bluetooth.read(); message += inChar;if (inChar == '\n') { parsing = true;} 
    }                             
    if(parsing){parsingData(); parsing=false; message="";}
} 

void parsingData(){
int j=0; Serial.print("data masuk : ");Serial.println(message);dt[j]="";
String cekk=String(message[0])+String(message[1])+String(message[2]);
                    if  (cekk=="@1%"){modeplay=1;Serial.println("Sweeper On");}
                    if  (cekk=="@2%"){modeplay=2; Serial.println("Sweeper Off");digitalWrite(led,LOW);}                
                    if  (cekk=="@3%"){                                  
                              Serial.println("Setting Grid");                   
                              String convert = String(message).substring(3);
                              String pisah[3]; counter=0;int lastIndex = 0;
                              for (int i = 0; i < convert.length(); i++) {
                                if (convert.substring(i, i+1) == ":") {
                                  pisah[counter] = convert.substring(lastIndex, i);
                                  lastIndex = i + 1; counter++;
                                }
                                 if (i == convert.length() - 1) {
                                 pisah[counter] = convert.substring(lastIndex, i);
                                }
                              }
                              low_freq=(unsigned long)pisah[0].toInt();center_freq=(unsigned long)pisah[1].toInt(); high_freq=(unsigned long)pisah[2].toInt();
                              convert = ""; counter = 0;lastIndex = 0;                             
                              multi_scan=high_freq-low_freq; multi_scan=multi_scan/100;                                   
                    }
                    if  (cekk=="@4%"){modeplay=3; Serial.println("Stand Measurement");digitalWrite(led,LOW);}   
} 

void Sweeper_on(){
       data_grafik=""; double VSWR=1; double FWD5=0; double REV5=0;  int Z = 0 ; int R =0;  
       for (counter=0; counter <100;counter=counter+2){   
          for (int i=counter; i <counter+2; i++){  
              test_freq = low_freq + (i*multi_scan);                 
              
              // TEST VALUE 
              VSWR=VSWR+0.1; if (VSWR>6) VSWR=1;  //test dummy SWR
              Z = Z + 2; if (Z > 250) Z = 0;     //test dummy Impedancy
              R = R + 4; if (R > 250) R = 0;     //test dummy Reacktance
                    
              if (test_freq>(high_freq-multi_scan)){test_freq=low_freq;} 
              else {data_grafik = data_grafik + String(test_freq)+"|"+String(VSWR)+"|"+String(Z)+"|"+String(R)+"|";}               
          from_android();
          }
          on_off_led= not on_off_led;digitalWrite(led,on_off_led);
          Serial.println(data_grafik+"END");//test dummy
          bluetooth.println(data_grafik+"END");
          data_grafik=""; Serial.flush(); from_android();delay (100); 
       }
}

void stand(){
data_grafik="";  double VSWR=1; int Z=0 ; int R=0; 
              // TEST VALUE 
      for (counter=0; counter <30;counter=counter+1){  
      
                    VSWR=VSWR+0.1; if (VSWR>6) VSWR=1;  //test dummy SWR
                    Z = Z + 3; if (Z > 250) Z = 0;     //test dummy Impedancy
                    R = R + 5; if (R > 250) R = 0;     //test dummy Reacktance
       
         data_grafik =  String(center_freq)+"|"+String(VSWR)+"|"+String(Z)+"|"+String(R)+"|";               
         from_android();
         on_off_led= not on_off_led;digitalWrite(led,on_off_led);
         Serial.println(data_grafik+"END");
         bluetooth.println(data_grafik+"END");
         data_grafik=""; Serial.flush(); 
         delay (150); 
      }
}

//end Sketch
