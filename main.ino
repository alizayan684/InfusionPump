const int relayPin = 4;  
const int flowPin = 2;
const int alarmPin = 7;
volatile unsigned long flowPulseCount = 0; 
unsigned long oldTime = 0;
float flowRate = 0.0;
bool relayOn = true;
void flowISR() {
  flowPulseCount++;
}

void setup() {
  pinMode(relayPin, OUTPUT); 
  pinMode(alarmPin, OUTPUT);
  Serial.begin(9600);
  pinMode(flowPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(flowPin), flowISR, RISING);  

}

void loop() {
  //////////RELAY LOGIC//////////////////////////////////////////////
  if (relayOn && flowRate < 1  ){  // TRUE,  < 1
    digitalWrite(relayPin, HIGH);  
    Serial.println("Pump is delivering");
    delay(5000);
  }



  //////////FLOWMETER LOGIC//////////////////////////////////////////////////
  unsigned long currentTime = millis();
  if (currentTime - oldTime >= 1000) {
    noInterrupts();
    unsigned long flowPulseCountCopy = flowPulseCount;
    float frequency = flowPulseCountCopy / ((currentTime - oldTime)*0.001); // calc actual freq given n and time (n/t = f)
    flowPulseCount = 0;
    interrupts();

    // Pulse frequency (Hz) = 7.5 * Flow rate (L/min)
    flowRate = frequency / 7.5;
    Serial.print("Flow rate: ");
    Serial.print(flowRate);
    Serial.println(" L/min");
    oldTime = currentTime;
    if( flowRate < 0.1 && relayOn){

      Serial.print("BLOCKAGE in the tubing!!!!");
      digitalWrite(relayPin, LOW);  
      Serial.println("Turn off");
      relayOn = false;

      while(true){
      
        digitalWrite(alarmPin, HIGH);

        if (Serial.available() > 0) {

          String input = Serial.readString();
          Serial.print("You entered: "); Serial.println(input);
          if (input == 1) { 
            Serial.print("You entered: "); Serial.println(input);
            Serial.println("Turning on the pump...");
            relayOn = true;
          } else { 
            Serial.println("Unknown command"); 
          }

        }

      } 

    } else if ( flowRate >  1 ){

      digitalWrite(relayPin, LOW);
      Serial.println("Desired Rate reached");
      relayOn = false;

    } else {

       digitalWrite(relayPin, HIGH);
       relayOn = true;
    }
  }
}
