#include "MAX17043.h"
#include "Wire.h"
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

MAX17043 batteryMonitor;

//to detect that charging has started to trigger FULL status
boolean charge_started = 0 ;
float current = 0.0;
float current2 = 0.0;
float current3 = 0.0;
float capacity = 0;
//int capacity2 = 0;

void setup() {
  
  Wire.begin(); 
  batteryMonitor.reset();
  batteryMonitor.quickStart();
  delay(1000);  
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
}

void loop() {  
 
  float cellVoltage = batteryMonitor.getVCell();
  float stateOfCharge = batteryMonitor.getSoC();  
  
  //When no battery connected the TP4056 will spike to up to 4.25v 
  //this will messed up Max17043 lipo gauge percentage
  //I used this state to detect missing battery and reset the Max17043 chip.
  if(stateOfCharge>110 || cellVoltage>4.25) 
  {
    //reset the Max17043 chip when no battery connected 
    //to avoid wrong Battery gauge like 120% etc.
    batteryMonitor.reset();    
      
    if(charge_started)
    {
      //will not shown when no battery preciously inserted
      //like when charger powered up without battery
      lcd.setCursor(0, 0); // top left
      lcd.print("Battery Removed ");
    }
    else
    {
      lcd.setCursor(0, 0); // top left  
      lcd.print("++            --");      
    }   
    lcd.setCursor(0, 1); // bottom left
    lcd.print("Insert Battery  "); 
    capacity = 0;
    delay(300);    
  }
  else  
  {    
    lcd.setCursor(0, 0); // top left
    //lcd.print("Voltage : ");
    lcd.print(cellVoltage);
    lcd.print("v  | ");
    
    if(current > 0)
    {
      capacity = (capacity + (current3*1000/3600))  ;
    }
    //capacity2 = capacity;
    lcd.print(capacity,0);
    lcd.print("mAH  "   );
    lcd.setCursor(0, 1); // bottom left
    lcd.print(""); 
    if(stateOfCharge>=105)
    {
      //LCD will display FULL when gauge reaches 105%  but charging will continue as it reaches 4.2v 
      //but will take longer time. Battery can be use immediately after LCD Displays FULL,
      //this is because Max Lipo gauge can be innacurate.
      lcd.print("Battery Full     ");
    }
    else
    { //Value smoothing by using average
      for(int i = 0; i < 1000; i++) {
      current = (0.0049 * analogRead(A1)) - 2.5 ;
      current = current / 0.100; //20A Hall sensor
      current2 = current2 + current;    
      delay(1); }
 
      current2 = current2 / 1000; //Averaging devide by number of loops
      current3 = current2;  
      lcd.print(stateOfCharge); //shows Percent

      lcd.print("% | ");
      lcd.print(current3); //shows Current in amp
      //lcd.print(current3*1000); //shows current in miliamp
      lcd.print("A");
    }  
    charge_started = 1 ;
  }  
  //delay(500); 
}
