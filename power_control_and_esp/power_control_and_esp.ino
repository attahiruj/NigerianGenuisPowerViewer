#include <LiquidCrystal.h>    // include Arduino LCD library
#include <arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial espSerial(12, 13);
String data_out;

//const int rs=12, en=11, d4=5, d5=4, d6=3, d7=2;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7)// LCD module connections (RS, E, D4, D5, D6, D7)
;
float wind_voltage = 0.0;
float solar_voltage = 0.0;
float totalVoltage = 0.0;
float power = 0.0;
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
int R1 = 100000;
int R2 = 1000;

// Current sensor
unsigned int x=0;
float AcsValue=0.0,Samples=0.0,AvgCurrent=0.0,current=0.0;


// LDR motor
#include <Servo.h> //includes the servo library

Servo myservo;

#define ldr1 A3 // set ldr 1 Analog input pin of East ldr as an integer
#define ldr2 A4 // set ldr 2 Analog input pin of West ldr as an integer

int pos = 90; // initial position of the Horizontal movement controlling servo motor
int tolerance = 20; // allowable tolerance setting - so solar servo motor isn't constantly in motion


void setup(void) {
  lcd.begin(16, 2);           // set up the LCD's number of columns and rows
  lcd.setCursor(0, 0);
  Serial.begin(9600);
  espSerial.begin(115200);

  myservo.attach(9); // attaches the servo on digital pin 2 to the horizontal movement servo motor 
  pinMode(ldr1, INPUT); //set East ldr pin as an input
  pinMode(ldr2, INPUT); //set West ldr pin as an input
  myservo.write(pos); // write the starting position of the horizontal movement servo motor 
  delay(1000); // 1 second delay to allow the solar panel to move to its staring position before comencing solar tracking
}
 
// main loop
void loop() {
  byte Mastersend;
 
//Conversion formula for voltage
   int wind_value = analogRead(A0);
   int solar_value = analogRead(A1);
   int totalVoltage;
   wind_voltage = (wind_value * (5 / 1024.0) * ((R1+R2)/R2));
   solar_voltage = (solar_value * (5 / 1024.0) * ((R1+R2)/R2)); 
   
   if (wind_voltage < 0.1) 
   {
     wind_voltage=0.0;
   } 
   if (solar_voltage < 0.1) 
   {
     solar_voltage=0.0;
   } 
    Serial.print("vWind= ");
    Serial.println(wind_voltage);
    Serial.print("vSolar= ");
    Serial.println(solar_voltage);
    
    totalVoltage = wind_voltage + solar_voltage;
    
    lcd.setCursor(0, 1);
    lcd.print("Voltage= ");
    lcd.print(totalVoltage);

    delay (100);

    //CURRENT CALCULATION
    for (int x = 0; x < 150; x++){ //Get 150 samples
      AcsValue = analogRead(A2);     //Read current sensor values   
      Samples = Samples + AcsValue;  //Add samples together
      delay (3); // let ADC settle before next sample 3ms
    }
    AvgCurrent=Samples/150.0;//Taking Average of Samples
    
    //((AvgAcs * (5.0 / 1024.0)) is converitng the read voltage in 0-5 volts
    //2.5 is offset(I assumed that arduino is working on 5v so the viout at no current comes
    //out to be 2.5 which is out offset. If your arduino is working on different voltage than 
    //you must change the offset according to the input voltage)
    //0.066v(66mV) is rise in output voltage when 1A current flows at input
    current = (2.5 - (AvgCurrent * (5.0 / 1024.0)) )/0.066;
//    if (current < 0.01) 
//   {
//     current=0.0;
//   }
    Serial.print("current =");
    Serial.println(current);//Print the read current on Serial monitor
    
    lcd.setCursor(0, 0);
    lcd.print("Current= ");
    lcd.print(current);

  int power = (current * totalVoltage);
//LDR Servo
int val1 = analogRead(ldr1); // read the value of ldr 1
Serial.println(val1);

int val2 = analogRead(ldr2); // read the value of ldr 2
Serial.println(val2);
      
if((abs(val1 - val2) <= tolerance) || (abs(val2 - val1) <= tolerance)) {
  //no servo motor horizontal movement will take place if the ldr value is within the allowable tolerance
  }else {
    if(val1 > val2){
      pos = pos+1; // decrement the 90 degree poistion of the horizontal servo motor - this will move the panel position Eastward
    }
    if(val1 < val2){ 
      pos = pos-1; // increment the 90 degree position of the horizontal motor - this will move the panel position Westward
       }
    } 
    if(pos > 180) {pos = 180;} // reset the horizontal postion of the motor to 180 if it tries to move past this point
    if(pos < 0) {pos = 0;} // reset the horizontal position of the motor to 0 if it tries to move past this point
    myservo.write(pos); // write the starting position to the horizontal motor
    delay(300);

data_out =String(wind_voltage)+String(",")+String(solar_voltage)+String(",")+String(totalVoltage)+String(",")+String(current)+String(",")+String(power)+String(",");
espSerial.println(data_out);
if(espSerial.println(data_out)){
  Serial.println("data out");
}
delay(2000);

}
