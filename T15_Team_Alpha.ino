#include <Servo.h>
#include<Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define tempPin A0
Servo myservo;
int pos=0;  // position of servo motor

LiquidCrystal_I2C lcd(0x3F,16,2);

char keys[4][3]={
 {'1','2','3'},
 {'4','5','6'},
 {'7','8','9'},
 {'*','0','#'}};
 
byte rowPin[4]={11,2,1,0};
byte colPin[3]={4,5,6};

String id_list[] = {"#001","#002","#003"};   //Extendable ID list
String password_list[] = {"145278","354691","789541"};  //Extendable corresponding password list

int redPin = 9;
int greenPin = 8;
int yellowPin = 7;
int buzzerPin = 12;

bool timeOut;   // timeout the system in case of no input from the user
bool sudden_door_close;  // kill switch pressed
String correct_password;
bool correct_id;
bool correct_pswd;
int attempts;
char pressed;

Keypad keypad=Keypad(makeKeymap(keys),rowPin,colPin,4,3); // mapping the Keypad

volatile boolean ledOn=false;
int temp;
void setup()
{
  
  pinMode(redPin,OUTPUT);
  pinMode(greenPin,OUTPUT);
  pinMode(yellowPin,OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(3,INPUT);
  attachInterrupt(1,door_open,RISING);  //interrupt
    
  myservo.attach(10); //Servo motor connection
  lcd.init(); //lcd startup
  lcd.backlight();
  lcd.setCursor(4,0);
  lcd.print("WELCOME!!!");
  delay(3000);
  lcd.clear();
}

void loop()
{ 
   //temp = analogRead(tempPin); 
   //float tempCel = (temp/1024.0)*500;
   //if (tempCel<=38 && tempCel>32){
   if (ledOn){//interrupt occured ; kill switch
    sudden_door_open();
   }
   led_setup(0,0,0); 
   correct_id = false;
   int attempts = 0; 
   while (!correct_id){
    if (ledOn){//check for interrupt occurance ; kill switch
    sudden_door_open();
    }
    lcd.setCursor(0,0);lcd.print("Enter ID and");  lcd.setCursor(4,1);lcd.print("Press [*]");
    int count = 0; String id="";
    pressed = keypad.getKey();   // get keypad input
    if (pressed){   // a user begins to enter the ID
      uint32_t start_timer1 = millis();   // the time user starts to enter his ID
      id.concat(pressed);lcd.clear();lcd.print(pressed);count++;delay(500);
      while (true){
        if (ledOn){//check for interrupt occurance ; kill switch
          sudden_door_open();
          sudden_door_close = true;
          break;
        }
        if((millis()-start_timer1)>30000){   //  check whether time is out for id entering
          timeOut=true;
          lcd.clear();lcd.print("......");delay(1000);lcd.clear();break;
        }
        if (id=="*"){  //When user enters the star symbol as the first digit of ID; system would proceed to the ID checking procedure
          lcd.clear();lcd.print("ID check");lcd.setCursor(3,1);lcd.print("in progress");delay(1000);lcd.clear();break;
        }
        pressed= keypad.getKey(); // get keypad input
        if (pressed){
          if(pressed=='*'){  //When user inputs the star symbol; system would start to check the ID is correct
            lcd.clear();lcd.print("ID check");lcd.setCursor(3,1);lcd.print("in progress");delay(1000);lcd.clear();break;
          }
          else{   //concat the pressed chars together to the 'id' string
            id.concat(pressed);lcd.setCursor(count,0);lcd.print(pressed);delay(500);count++;
          }
        }
        if(count==5){  //restrict user to input(from keypad) upto 5 digits only 
          lcd.clear();lcd.print("ID should not be");lcd.setCursor(0,1);lcd.print("more than four!");delay(1000);lcd.clear();break;
        }
      }
      if(timeOut){  //time out 
        timeOut=false;
        lcd.clear();lcd.print("Time Out....");delay(1000);lcd.clear();
        break;
      }
      if(sudden_door_close){  //if the kill switch pressed in the middle of user entering the ID, the system would ask hime to enter the ID again
        lcd.print("Please Enter ");lcd.setCursor(2,1);lcd.print("ID Again!");delay(1000);lcd.clear();pressed=false;
      }
      //lcd.clear();lcd.print("attempts ");lcd.print(attempts);delay(1000);
      //lcd.clear();lcd.print(id);delay(1000);
      if(!sudden_door_close){
        for(int index=0;index<3;index++){//check for interrupt occurance ; // check whether the given ID is within the id_list
          if (ledOn){
            sudden_door_open();
          }
          if (id_list[index]==id){  // if ID is in the id_list
            lcd.clear();lcd.setCursor(4,0);lcd.print("Valid ID!");correct_id = true;led_setup(0,0,1);delay(1000);lcd.clear();
            correct_password = password_list[index];  // obtain the corresponding correct password of the ID from the password_list
            break;
          }
        }
      
        if (!correct_id) {    // if ID is not in the list it is an incorrect ID
            lcd.clear();lcd.setCursor(4,0);lcd.print("Invalid ID!");led_setup(1,0,0);digitalWrite(buzzerPin,HIGH);delay(500);lcd.clear();led_setup(0,0,0);attempts++;digitalWrite(buzzerPin,LOW);delay(500);
            
        }
        if(attempts==3){//user is given only 3 attempts until he correctly enters the ID; by the end of 3 attempts if he still didn't enter the correct ID, the system will freeze for 30 sec
          lcd.clear();lcd.print("Try in 30sec...");lcd.setCursor(4,1);led_setup(1,0,0);
          uint32_t now = millis();
          while ((abs(millis()-now)/1000)<31){  //freeze the system for 30 sec
            lcd.setCursor(0,1);lcd.print(abs(millis()-now)/1000);
            if(ledOn){//check if interrupt occurs while system freezes
              sudden_door_open(); // the counter would increase while the door opens
              lcd.print("Try in 30sec...");
            }
            for(int i=0;i<2;i++){
              digitalWrite(buzzerPin,HIGH);delay(300);digitalWrite(buzzerPin,LOW);delay(200);
            }
          }
          lcd.clear();
          attempts=0;
          break;
        }
      }sudden_door_close=false;
    }
   }
   
   //lcd.clear();lcd.print(correct_password);delay(1000);lcd.clear();
   if (ledOn){     //check for interrupt occurance ; kill switch
    sudden_door_open();
   }
   correct_pswd = false;
   int pswd_attempts = 0; 
   uint32_t timer = millis();// start counting time after the id correct until the first enter of password enter
   while(correct_id && !correct_pswd){
    if (ledOn){    //check for interrupt occurance ; kill switch
    sudden_door_open();
    sudden_door_close=false;lcd.print("Please Enter");lcd.setCursor(2,1);lcd.print("ID Again!");delay(1000);lcd.clear();
    break;
    }
    if((millis()-timer)>30000){  //check if time is out
      lcd.clear();lcd.print("....");lcd.print("Time Out");delay(1000);break;
    }
    lcd.setCursor(0,0);lcd.print("Enter password ");lcd.setCursor(0,1);lcd.print("and press [*]");
    int pswd_count = 0; String password="";
    pressed = keypad.getKey();  //get teh keypad input
    if(pressed){  //user begins to input his password
      uint32_t start_timer2 = millis(); //the time user starts entering the password
      password.concat(pressed);lcd.clear();lcd.print('*');pswd_count++;delay(500);
      while (true){
        if (ledOn){  //check for interrupt occurance ; kill switch
          sudden_door_open();
          sudden_door_close = true;
          lcd.print("Please Enter");lcd.setCursor(2,1);lcd.print("ID Again!");delay(1000);lcd.clear();break;
         }
        if((millis()-start_timer2)>30000){ //check whether time is out for password entering
          timeOut=true;
          lcd.clear();lcd.print("......");delay(1000);lcd.clear();break;
        }
        if (password=="*"){ //When user enters the star symbol as the first digit of password; system would proceed to the password checking procedure
          lcd.clear();lcd.print("password check");lcd.setCursor(4,1);lcd.print("in progress");delay(1000);lcd.clear();break;
        }
        pressed= keypad.getKey();
        if (pressed){
          if(pressed=='*'){  //When user inputs the star symbol; system would start to check the password is correct
            lcd.clear();lcd.print("password check ");lcd.setCursor(4,1);lcd.print("in progress");delay(1000);break;
          }
          else{  //concat the pressed chars together to the 'password' string
            password.concat(pressed);lcd.setCursor(pswd_count,0);lcd.print('*');delay(500);pswd_count++;
          }
        }
        if(pswd_count==7){ //restrict user to input(from keypad) upto 7 digits only 
          lcd.clear();lcd.print("Password length");lcd.setCursor(0,1);lcd.print("exceeded!");delay(1000);break;
        }
      }
      if(timeOut){  //timeout
        timeOut=false;
        lcd.clear();lcd.print("Time Out....");delay(1000);lcd.clear();
        break;
      }
      if(sudden_door_close){ //if the kill switch pressed in the middle of user entering the password, the system would ask hime to enter from the ID again
        sudden_door_close=false;
        break;
      }
      //lcd.clear();lcd.print("attempts ");lcd.print(pswd_attempts);delay(1000);
      //lcd.clear();lcd.print(password);delay(1000);
      if (password==correct_password){  //check whether the input password is the corresponding password for the id
          lcd.clear();lcd.print("Password Correct!");correct_pswd = true;led_setup(0,1,0);delay(1000);
          lcd.clear();lcd.setCursor(6,1);lcd.print("Door Open!");  // if correct the door is opened
          for(pos = 180; pos>=0; pos-=5) // open the door; turn servo
          {           
            myservo.write(pos);digitalWrite(buzzerPin,HIGH);delay(50);digitalWrite(buzzerPin,LOW); delay(50);           
          }
          lcd.clear();lcd.setCursor(4,1);lcd.print("Door Close..");
          for(pos = 0; pos<=180; pos+=5) // close the door; turn the servo in the opposite direction
          {            
            myservo.write(pos);digitalWrite(buzzerPin,HIGH);delay(50); digitalWrite(buzzerPin,LOW);delay(50);      
          }
  
          lcd.clear();
          break;
      }
      else{ //if the password is incorrect
        lcd.clear();lcd.print("Invalid Password!");led_setup(1,0,1);digitalWrite(buzzerPin,HIGH);delay(500);led_setup(0,0,1);pswd_attempts++;lcd.clear();digitalWrite(buzzerPin,LOW);delay(500);
      }
      if (pswd_attempts==3){//user is given only 3 attempts until he correctly enters the password; by the end of 3 attempts if he still didn't enter the correct password, the system will freeze for 30 sec
        lcd.clear();lcd.print("Try in 30sec...");led_setup(1,0,0);
        uint32_t now = millis();
        while ((abs(millis()-now)/1000)<31){ //freeze the system for 30 sec
          lcd.setCursor(0,1);lcd.print(abs(millis()-now)/1000);
          if(ledOn){//check if interrupt occurs while system freezes
              sudden_door_open(); // the counter would increase while the door opens
              lcd.print("Try in 30sec...");
          }
          for(int i=0;i<2;i++){
            digitalWrite(buzzerPin,HIGH);delay(300);digitalWrite(buzzerPin,LOW);delay(200);
          }
        }
        
        lcd.clear();
        break;
      }
    }
   }
   //}
   //else if (tempCel>38){
   // lcd.clear();lcd.print("Not Allowed!!!");delay(1000);  
   //}
}

void sudden_door_open(){  // if the kill switch is pressed the door will suddenly open
  lcd.clear();lcd.print("Sudden door Open");delay(500); 
  for(pos = 180; pos>=0; pos-=5) // open the door
  { myservo.write(pos);
    digitalWrite(buzzerPin,HIGH);             
    delay(50);   
    digitalWrite(buzzerPin,LOW); 
    delay(50);         
  }
  lcd.clear();lcd.setCursor(4,1);lcd.print("Door Close..");
  for(pos = 0; pos<=180; pos+=5) // close the door
  {myservo.write(pos);
  digitalWrite(buzzerPin,HIGH);             
  delay(50);   
  digitalWrite(buzzerPin,LOW); 
  delay(50);              
  }
  
  ledOn = false;
  digitalWrite(13,LOW); lcd.clear();
}

void led_setup(int r,int g,int y)  //led array
{
  digitalWrite(redPin,r);
  digitalWrite(greenPin,g);
  digitalWrite(yellowPin,y);
}

void door_open(){  //interrup servise routine
  if (ledOn==false){
    ledOn = true;
    digitalWrite(13,HIGH); 
  }
}
