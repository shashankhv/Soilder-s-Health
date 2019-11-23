// GSM Tx goes to RX pin of Arduino, Rx pin goes to TX pin of Arduino
#include<LiquidCrystal.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
#include <SoftwareSerial.h>
SoftwareSerial gps(10,11); //(Rx,Tx) Tx of GPS, Rx of GPS
//String str="";
char str[70];
String gpsString="";
char *test="$GPGGA";
String latitude="No Range      ";
String longitude="No Range     ";
int temp=0,i;
int button1 = 8;
int button2=9;
int value1 = 0;
int value2 = 0;
int sense=12;
int temp1;                               // Variable to store the temperature sensor values from analog pin 0
 int tempPin = 0;

boolean gps_status=0;
void setup() 
{
  pinMode(button1,INPUT_PULLUP);
  pinMode(button2,INPUT_PULLUP);
  pinMode(sense,OUTPUT);
  lcd.begin(16,2);
  Serial.begin(9600);
  gps.begin(9600);
  lcd.print("    Real Time   ");
  lcd.setCursor(0,1);
  lcd.print("Soldier Tracking");
  delay(2000);
  gsm_init();
  lcd.clear();
  Serial.println("AT+CNMI=2,2,0,0,0");
  lcd.print("GPS Initializing");
  lcd.setCursor(0,1);
  lcd.print("  No GPS Range  ");
  get_gps();
  delay(2000);
  lcd.clear();
  lcd.print("GPS Range Found");
  lcd.setCursor(0,1);
  lcd.print("GPS is Ready");
  delay(2000);
  lcd.clear();
  lcd.print("System Ready");
  temp=0;
}
void loop()
{
  serialEvent();
  if(temp)
  {
    get_gps();
    tracking();
  }
  value1=digitalRead(button1);
  if (value1 == LOW)
  {
    get_gps();
    tracking();
  }
  cold();
  value2=digitalRead(button2);
  if (value2 == LOW)
  {
    get_gps();
    tracking();
  }
}
void serialEvent()
{
  while(Serial.available())
  {
    if(Serial.find("Track"))
    {
      temp=1;
      break;
    }
    else
    temp=0;
  }
}
void gpsEvent()
{
  gpsString="";
  while(1)
  {
   while (gps.available()>0)            //checking serial data from GPS
   {
    char inChar = (char)gps.read();
     gpsString+= inChar;                    //store data from GPS into gpsString
     i++;
     if (i < 7)                      
     {
      if(gpsString[i-1] != test[i-1])         //checking for $GPGGA sentence
      {
        i=0;
        gpsString="";
      }
     }
    if(inChar=='\r')
    {
     if(i>65)
     {
       gps_status=1;
       break;
     }
     else
     {
       i=0;
     }
    }
  }
   if(gps_status)
    break;
  }
}
void gsm_init()
{
  lcd.clear();
  lcd.print("Finding Module..");
  boolean at_flag=1;
  while(at_flag)
  {
    Serial.println("AT");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      at_flag=0;
    }
    
    delay(1000);
  }
  lcd.clear();
  lcd.print("Module Connected..");
  delay(1000);
  lcd.clear();
  lcd.print("D0isabling ECHO");
  boolean echo_flag=1;
  while(echo_flag)
  {
    Serial.println("ATE0");
    while(Serial.available()>0)
    {
      if(Serial.find("OK"))
      echo_flag=0;
    }
    delay(1000);
  }
  lcd.clear();
  lcd.print("Echo OFF");
  delay(1000);
  lcd.clear();
  lcd.print("Finding Network..");
  boolean net_flag=1;
  while(net_flag)
  {
    Serial.println("AT+CPIN?");
    while(Serial.available()>0)
    {
      if(Serial.find("+CPIN: READY"))
      net_flag=0;
    }
    delay(1000);
  }
  lcd.clear();
  lcd.print("Network Found..");
  delay(1000);
  lcd.clear();
}
void get_gps()
{
   gps_status=0;
   int x=0;
   while(gps_status==0)
   {
    gpsEvent();
    int str_lenth=i;
    latitude="";
    longitude="";
    int comma=0;
    while(x<str_lenth)
    {
      if(gpsString[x]==',')
      comma++;
      if(comma==2)        //extract latitude from string
      latitude+=gpsString[x+1];     
      else if(comma==4)        //extract longitude from string
      longitude+=gpsString[x+1];
      x++;
    }
    int l1=latitude.length();
    latitude[l1-1]=' ';
    l1=longitude.length();
    longitude[l1-1]=' ';
    lcd.clear();
    lcd.print("LAT:");
    lcd.print(latitude);
    lcd.setCursor(0,1);
    lcd.print("LON:");
    lcd.print(longitude);
    i=0;x=0;
    str_lenth=0;
    delay(2000);
   }
}
void init_sms()
{
  Serial.println("AT+CMGF=1");
  delay(400);
  Serial.println("AT+CMGS=\"+9686490113\"");
  delay(400);
}
void send_data(String message)
{
  Serial.println(message);
  delay(200);
}
void send_sms()
{
  Serial.write(26);
}
void lcd_status()
{
  lcd.clear();
  lcd.print("Message Sent");
  delay(2000);
  lcd.clear();
  lcd.print("System Ready");
  return;
}
void tracking()
{
    init_sms();
    send_data("Location of Soilder:");
    Serial.print("LATITUDE :");
    send_data(latitude);
    Serial.print("LONGITUDE:");
    send_data(longitude);
    send_data("Please send Backup!..");
    send_sms();
    delay(2000);
    lcd_status();
}
void cold()
{
  temp1 = (5.0*analogRead(tempPin)*100.0)/1024;     // Conversion to Celcius
  Serial.print("Temperature:");          // Printing to serial monitor
  Serial.print(temp1);                     // Printing the values to serial monitor
  Serial.println("C");                   // Printing to serial monitor and jump to next line
  lcd.clear();
  lcd.print("  Temperature:  ");
  lcd.setCursor(6,1);
  lcd.print(temp1);
  lcd.setCursor(10,1);
  lcd.print("C");
                             // Delay of 2 seconds (1 second = 1000 milliseconds)

if (temp1 <= 25)                            // Creates boolean expression. if it evaluates to true,(if the surrounding temperatue is above 28*C)
 {
  digitalWrite(sense, LOW);               // Turns LED On                           // Delay
  delay(3000);
  digitalWrite(sense, HIGH);
 }
 else                                    // (if the surrounding temperature is below 28*C) 
 {
  digitalWrite(sense, HIGH);                            // Delay
 }
 delay(2000);
}
