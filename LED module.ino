int before_voltage_level = 0;
int counter = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(9, OUTPUT);
  //pinMode(A0, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  
  int value;
  int voltage_level = 0;
  value = analogRead(0);
  //value = 0;
  Serial.print("Value : ");
  Serial.println(value);

  if(value <= 1000){
    voltage_level = 1;
  }
  
  if( voltage_level == 1){
    while(value < 1000){
      value = analogRead(0);
      delay(1);
      counter += 1;
    }

    if((50 < counter) && (counter < 150)){
      //LED点灯
      analogWrite(9, 200);
    }
    if((350 < counter) && (counter < 650)){
      analogWrite(9, 0);
    }
    voltage_level = 0;
    counter = 0;

    /*
    if(counter == 0){
      //1回目のときはつける
      analogWrite(9, 120);
      counter = 1;
    }else{
      //2回目の信号のときは消す
      analogWrite(9, 0);
      counter = 0;
    }
    */
  }

  //before_voltage_level = voltage_level;
  delay(1);

  /*
  int value;

  analogWrite(9, 120);
  delay(500);
  value = analogRead(0);
  Serial.print("Value(ON): ");
  Serial.println(value);
  delay(500);
  
  analogWrite(9, 0);
  delay(500);
  value = analogRead(0);
  Serial.print("Value(OFF): ");
  Serial.println(value);
  delay(500);
  */
}
