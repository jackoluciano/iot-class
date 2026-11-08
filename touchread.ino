#define touchPin T0
#define ledPin 18

const int threshold = 800; 
int touchValue;

void setup(){
  Serial.begin(115200);
  delay(1000);
  pinMode(ledPin, OUTPUT);
}

void loop(){
  touchValue = touchRead(touchPin);
  Serial.println(touchValue);

  if(touchValue < threshold){
    digitalWrite(ledPin, HIGH);  // LED nyala saat disentuh
  }
  else{
    digitalWrite(ledPin, LOW);   // LED mati saat dilepas
  }

  delay(100);
}