
volatile int modes = 0; // 1 - Normal mode || 2 - Temprature Threshold programming mode || 3 - Humidity Threshold programming mode
volatile int tempThreshold = 0;
volatile int mode = 1;
volatile int humidThreshold = 0;
volatile bool UPstate = 0,DOWNstate = 0;
volatile long int tempLastupdate,humidLastupdate;
// Include Wire Library for I2C
#include <Wire.h>
 #include<avr/interrupt.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Reset pin not used but needed for library
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);
 
ISR(INT0_vect)
{
    
        mode++;
        delay(10);
        if(mode>3)
        {
          mode=1;
        }
        delay(10);
        

}
ISR(PCINT2_vect)
{
   if(bit_is_set(PIND,PD5) && !UPstate)
    { 
        if(mode == 2)
        {
          tempThreshold++;
          tempLastupdate = millis();
        }
        if(mode == 3)
        {
          humidThreshold++;
          humidLastupdate = millis();
        }
        UPstate = HIGH;
    }
    else if (!bit_is_set(PIND,PD5) && UPstate)
    {
      UPstate = LOW;
    }
    Serial.print("Humidity :");
  Serial.println(humidThreshold);
   Serial.print("Temprature :");
  Serial.println(tempThreshold);
}

void pcint_enable()
{
    
    PCICR |= (1<<PCIE2); // Pin change intrrepts enables to PORTD pins   
    PCMSK2 |= (1<<PCINT21)| (1<<PCINT22); // pin change intrrept enable for PD4 pin 
    sei(); 
  
}

void external_intrrept_init()
{
    EIMSK |= (1<<INT0);
    EICRA |= (1<<ISC00)|(1<<ISC01);  //The falling edge of INT0 generates an interrupt request.
    sei();
}

 
void setup() {
  // Start Wire library for I2C
  Wire.begin();
Serial.begin(9600);   
Serial.print("Humidity :");
  Serial.println(humidThreshold);
  DDRB |= (1<<PB5); //pinMode(13,OUTPUT);
 DDRD |= (1<<PD0); //pinMode(0,OUTPUT);
  external_intrrept_init();
  PORTD |= (1<<PD2)|(1<<PD5)|(1<<PD6);
  pcint_enable();
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

}
 
void displayTempHumid(){
  // Delay to allow sensor to stabalize
  delay(70);
  // Clear the display
  display.clearDisplay();
  //Set the color - always use white despite actual display color
  display.setTextColor(WHITE);
  //Set the font size
  display.setTextSize(1);
  //Set the cursor coordinates
  display.setCursor(0,0);
  display.print("DroneBot Workshop");
  display.setCursor(0,10); 
  display.print("Humidity:    "); 
  display.print(9);
  display.print(" %");
  display.setCursor(0,20);
  display.print("Temperature: "); 
  display.print(18);
  display.print(" C");
}


void setTemprature(){


 // Clear the display
  display.clearDisplay();
  //Set the color - always use white despite actual display color
  display.setTextColor(WHITE);
  //Set the font size
  display.setTextSize(1);
  //Set the cursor coordinates
  display.setCursor(0,0);
  display.print("Temprature Thereshold");
  display.setCursor(1,10); 
  display.print(tempThreshold); 
      
}
void setHumidity(){


 // Clear the display
  display.clearDisplay();
  //Set the color - always use white despite actual display color
  display.setTextColor(WHITE);
  //Set the font size
  display.setTextSize(1.1);
  //Set the cursor coordinates
  display.setCursor(0,0);
  display.print("Humidity Thereshold");
  display.setCursor(1,10); 
  display.print(humidThreshold); 
      
}
void loop() {
  
  display.display();
  if(mode == 3)
  {
     humidLastupdate = millis();    
    setHumidity();
    if(millis()-humidLastupdate>10000)mode=1;
  }
  if(mode == 2)
  {
    tempLastupdate = millis();humidLastupdate = millis();  
    setTemprature();
    if(millis()-tempLastupdate>10000)mode=1;
  }
  if(mode == 1)
  {
  displayTempHumid();
  }
  Serial.print("Humidity :");
  Serial.println(humidThreshold);
  delay(100);
}
