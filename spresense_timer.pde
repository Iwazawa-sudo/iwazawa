import processing.serial.*;

Serial serial;
String Text = "";

//Alerm set
int alarm_hour = 23;
int alarm_minute = 7;

void setup() {
  size(800, 356);
  textSize(220);
  serial = new Serial(this, "COM15", 9600);
}

void draw() {
  
  int h, m;
  String t;
  
  background(0, 0, 0);
  fill(255, 255, 255);
  textSize(220);
  text(nf(hour(), 2)+":"+nf(minute(), 2), 10, 200);
  textSize(100);
  text("Alarm " + nf(alarm_hour,2) + ":" + nf(alarm_minute, 2), 10, 300);
  
  //t = "00:00";
  //serial.write(t);
  //serial.write(0);
}

void serialEvent(Serial serial){
  int arduino_data;
  int h, m;
  String t;
  delay(5);
  arduino_data = serial.read();
  
  h = hour();
  m = minute();
  t = h + ":" + nf(m ,2);
  //println(t);
  //serial.write("a");
  //serial.write(t);
  //serial.write(0);
  
  //delay(100);
  
  //t = nf(alarm_hour,2) + ":" + nf(alarm_minute,2);
  
  t = nf(h,2) + ":" + nf(m ,2) + nf(alarm_hour,2) + ":" + nf(alarm_minute,2);
  //serial.write("b");
  serial.write(t);
  serial.write(0);
  
  
  /*
  //reverse //specification??
  h = hour();
  m = minute();
  t = h + ":" + nf(m ,2);
  //println(t);
  serial.write("a");
  serial.write(t);
  serial.write(0);
  
  delay(100);
  
  t = nf(alarm_hour,2) + ":" + nf(alarm_minute,2);
  serial.write("b");
  serial.write(t);
  serial.write(0);
  
  /*
  if (arduino_data == 1){
    h = hour();
    m = minute();
    t = "1" + h + ":" + nf(m ,2);
    //println(t);
    //serial.write('1');
    serial.write(t);
    serial.write(0);
  }
  else if(arduino_data == 2){
    t = "2" + nf(alarm_hour,2) + ":" + nf(alarm_minute,2);
    //serial.write('2');
    serial.write(t);
    serial.write(0);
  }
  */
}
