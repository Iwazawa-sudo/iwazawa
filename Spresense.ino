#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Fonts/FreeSans24pt7b.h>
#include <SDHCI.h>
#include <Audio.h>

#define TFT_CS -1
#define TFT_RST 8
#define TFT_DC 9

SDClass theSD;
AudioClass *theAudio;
File myFile;

int audio_mode = 0;
int ir_led_counter = 0;

bool ErrEnd = false;

char buff1[255] = {'1'};
char buff2[255] = {'2'};
int counter = 0;
int count = 0;

int before_now_time;
int before_alerm_time;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

static void audio_attention_cb(const ErrorAttentionParam *atprm){
  puts("Attention!");
  
  if (atprm->error_code >= AS_ATTENTION_CODE_WARNING){
      ErrEnd = true;
   }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("ILI9341 Test!"); 

  tft.begin(40000000);
  tft.setRotation(3);
  
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.setFont( &FreeSans24pt7b );

  pinMode(LED0, OUTPUT);

  /////////////////////////////////////
  //Audio set
  /////////////////////////////////////
  // start audio system
  theAudio = AudioClass::getInstance();
  
  theAudio->begin(audio_attention_cb);
  
  puts("initialization Audio Library");
  
  // Set clock mode to normal
  theAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);
  theAudio->setPlayerMode(AS_SETPLAYER_OUTPUTDEVICE_SPHP, AS_SP_DRV_MODE_LINEOUT);
  
  err_t err = theAudio->initPlayer(AudioClass::Player0, AS_CODECTYPE_MP3, "/mnt/sd0/BIN", AS_SAMPLINGRATE_AUTO, AS_CHANNEL_STEREO);
  
  // Verify player initialize
  if (err != AUDIOLIB_ECODE_OK)
    {
      printf("Player0 initialize error\n");
      exit(1);
    }
  
  // Open file placed on SD card
  myFile = theSD.open("Sound.mp3");
  
  // Verify file open
  if (!myFile)
  {
    printf("File open error\n");
    exit(1);
  }
  printf("Open! %d\n",myFile);

  // Send first frames to be decoded
  err = theAudio->writeFrames(AudioClass::Player0, myFile);

  if ((err != AUDIOLIB_ECODE_OK) && (err != AUDIOLIB_ECODE_FILEEND))
  {
      printf("File Read Error! =%d\n",err);
      myFile.close();
      exit(1);
  }

  //赤外線LED発光用
  pinMode(2, INPUT);
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);

  //LEDモジュールを消灯させる
  digitalWrite(3, HIGH);
  delay(500);
  digitalWrite(3, LOW);

}

void loop() {
  // put your main code here, to run repeatedly:
  String s;
  int current_time;
  int alerm_time;
  static int oktk_flag = 0;
  int sensor_value;

  //現在時刻・アラームタイマの取得
  Serial.write(1);
  //delay(10);
  //while(Serial.available() == 0);
  while(Serial.available()){
    char inChar = char(Serial.read());
    //現在時刻
    if(counter <= 4){  
      buff1[counter] = inChar;
    }
    //アラームタイマ
    else if ( counter > 4){
      buff2[counter - 5] = inChar;
    }
    counter++;

    if (inChar == '\0'){
      //Serial.println(buff);
      counter = 0;
    }else{
    }
  }

  //時刻をint型に変換
  char buff1_2[5];
  buff1_2[0] = buff1[0];
  buff1_2[1] = buff1[1];
  buff1_2[2] = buff1[3];
  buff1_2[3] = buff1[4];
  buff1_2[4] = buff1[5];
  sscanf(buff1_2, "%d", &current_time);

  //時刻をint型に変換
  char buff2_2[5];
  buff2_2[0] = buff2[0];
  buff2_2[1] = buff2[1];
  buff2_2[2] = buff2[3];
  buff2_2[3] = buff2[4];
  buff2_2[4] = buff2[5];
  sscanf(buff2_2, "%d", &alerm_time);
  
  //Serial.print("current time : ");
  //Serial.println(current_time);
  //Serial.print("alerm time : ");
  //Serial.println(alerm_time);


  //アラームを鳴らす
  if(current_time == alerm_time){
    digitalWrite(LED0, HIGH);
    if(oktk_flag == 0){
      oktk_flag = 1;
      theAudio->setVolume(-200);
      theAudio->startPlayer(AudioClass::Player0);
      
      //LEDモジュールを点灯させる
      digitalWrite(3, HIGH);
      delay(100);
      digitalWrite(3, LOW);
    }
  }else{
    //if(current_time > alerm_time + 4){
      oktk_flag = 0;
    //}
  }

  //ベッドの上に人がいるか
  sensor_value = digitalRead(2);
  if( sensor_value == 0 ){
    //続行
    Serial.println(" human is on the bed ");
  }
  if(sensor_value == 1 ){
    if(oktk_flag == 1){
      //count++;
      //if(count == 2){ 
        //count = 0;
        oktk_flag = -1;
        digitalWrite(LED0, LOW);
        theAudio->stopPlayer(AudioClass::Player0);
        
        //LEDモジュールを消灯させる
        digitalWrite(3, HIGH);
        delay(500);
        digitalWrite(3, LOW);
      //}
    }
  }

  //時刻やアラーム時間に変化があれば
  if( (before_now_time != current_time) || (before_alerm_time != alerm_time)){
    //時刻表示
    tft.fillScreen(0x0000);
    tft.setTextSize(2);
    //tft.setCursor(0, 40);
    //tft.print("Now");
    
    tft.setCursor(0, 80);
    tft.print(buff1);

    tft.setTextSize(1);
    tft.setCursor(0, 140);
    tft.print("Alarm ");
    
    //tft.setCursor(0, 160);
    tft.print(buff2);
  }

  before_now_time = current_time;
  before_alerm_time = alerm_time; 

  
  /********************************************************/
  //Audio
  /********************************************************/
  
  //puts("loop!!");

  // Send new frames to decode in a loop until file ends
  int err = theAudio->writeFrames(AudioClass::Player0, myFile);

  //  Tell when player file ends
  if (err == AUDIOLIB_ECODE_FILEEND)
    {
      //printf("Main player File End!\n");
      //printf("Loop player\n");

      myFile.close();
      myFile = theSD.open("Sound.mp3");
      theAudio->writeFrames(AudioClass::Player0, myFile);
      theAudio->startPlayer(AudioClass::Player0);
      theAudio->setVolume(-200);
      //theAudio->startPlayer(AudioClass::Player0);
    }

  // Show error code from player and stop
  if (err){
      //printf("Main player error code: %d\n", err);
      //goto stop_player;
    }

  if (ErrEnd){
      //printf("Error End\n");
      //goto stop_player;
    }
  
  delay(1000);
}
