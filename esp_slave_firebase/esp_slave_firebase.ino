#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "2MJ"
#define WIFI_PASSWORD "10000001"

//For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyC1c0Zikqmm7uFswzxIwRgAPprKfmHKVaA"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://test-c4221-default-rtdb.firebaseio.com" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "powerline@gmail.com"
#define USER_PASSWORD "powerline"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(3000);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h


  Firebase.begin(&config, &auth);

  //Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}
void loop(){
  if(Serial.available()){
    String rxString = "";
    String strArr[2]; //Set the size of the array to equal the number of values you will be receiveing.
  
    //Keep looping until there is something in the buffer.
    while (Serial.available()) {
      //Delay to allow byte to arrive in input buffer.
      delay(2);
      //Read a single character from the buffer.
      char ch = Serial.read();
      //Append that single character to a string.
      rxString+= ch;
    }
  
    int stringStart = 0;
    int arrayIndex = 0;
  
    for (int i=0; i < rxString.length(); i++){
      //Get character and check if it's our "special" character.
      if(rxString.charAt(i) == ','){
        //Clear previous values from array.
        strArr[arrayIndex] = "";
        //Save substring into array.
        strArr[arrayIndex] = rxString.substring(stringStart, i);
        //Set new string starting point.
        stringStart = (i+1);
        arrayIndex++;
      }
    }
  
    //Put values from the array into the variables.
    String wind_voltage = strArr[0];
    String solar_voltage = strArr[1];
    String totalVoltage = strArr[2];
    String current = strArr[3];
    String power = strArr[4];


if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    //Firebase.RTDB.set(&fbdo, F("/001/x/"), 80);
//    data_payload payload;

    Serial.printf("Set data... %s\n", Firebase.RTDB.set(&fbdo, F("/wind/w voltage/"), wind_voltage) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set data... %s\n", Firebase.RTDB.set(&fbdo, F("/solar/svoltage/"), solar_voltage) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set data... %s\n", Firebase.RTDB.set(&fbdo, F("/total/tvoltage/"), totalVoltage) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set data... %s\n", Firebase.RTDB.set(&fbdo, F("/total/tcurrent/"), current) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set data... %s\n", Firebase.RTDB.set(&fbdo, F("/total/tpower/"), power) ? "ok" : fbdo.errorReason().c_str());
   
    Serial.println();

    count++;
  }
    //Convert string to int if you need it.
//    int voltage = value1.toInt();
//    int current = value2.toInt();
  }
}
