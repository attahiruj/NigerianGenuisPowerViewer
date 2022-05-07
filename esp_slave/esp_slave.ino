
void setup() {
  Serial.begin(115200);
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
    String value1 = strArr[0];
    String value2 = strArr[1];
    Serial.println("voltage =" + value1);
    Serial.println("current =" + value2);
    //Convert string to int if you need it.
//    int voltage = value1.toInt();
//    int current = value2.toInt();
  }
}
