const int redLed = 2;
const int blueLed = 3;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(redLed, OUTPUT);
  pinMode(blueLed, OUTPUT);


}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(blueLed, HIGH);
  digitalWrite(redLed, LOW);

  delay(200);

  digitalWrite(blueLed, LOW);
  digitalWrite(redLed, HIGH);

  delay(200);

}
