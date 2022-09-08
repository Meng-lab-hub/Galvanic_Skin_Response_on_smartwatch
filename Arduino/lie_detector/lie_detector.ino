const int LED=13;
const int GSR=A0;
int threshold;
int sensorValue;
int counter = 0;
int question = 1;

void setup() {
  // put your setup code here, to run once:
  long sum=0;
  threshold = 0;
  Serial.begin(9600);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  delay(1000);
  
  for(int i=0;i<500;i++)
  {
    sensorValue=analogRead(GSR);
    sum += sensorValue;
    delay(5);
  }
  threshold = sum/500;
  Serial.print("threshold =");
  Serial.println(threshold);
  delay(500);
}

void lie(int ans){
  if (ans) Serial.print("Yes");
  else Serial.print("No");
  Serial.println(" is a lie!");
  delay(3000);
}

void notLie(int ans) {
  if (ans) Serial.print("Yes");
  else Serial.print("No");
  Serial.println("is not a lie!");
  delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:
  int tmp;
  int ans = -1;
  sensorValue = analogRead(GSR);

  Serial.print("sensorValue = ");
  Serial.println(sensorValue);
  
  if (counter == 500) {
    Serial.println("Question 1");
      Serial.println(" 0 - No");
      Serial.println(" 1 - Yes");
      ans = Serial.read();
  }

   tmp = threshold - sensorValue;
   if (abs(tmp) > 60 && counter == 501) {
    lie(ans);
    counter == 1501;
   }
   if (counter == 1500) {
    notLie(ans);
   }
  counter++;
}
