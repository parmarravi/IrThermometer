/* 
 * using button to move line up and down  on spi oled display


*/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90614.h>
// If using software SPI (the default case):
#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 6
int i =0;
int counter =0;
const int buttonPin = 5;    // the number of the pushbutton pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long start, finished, elapsed;
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup()  
{                
  Serial.begin(9600);
  mlx.begin(); 
  pinMode( buttonPin,INPUT);
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay(); 
  firstScreen();
  delay(2000);
  displayMenu();
  
}


void loop()
{
  if(elapsed >=180000)
  {
    digitalWrite(13,HIGH);
    extern volatile unsigned long timer0_millis;
    unsigned long new_value = 0;
    uint8_t oldSREG = SREG;
    cli();
    timer0_millis = new_value;
    SREG = oldSREG;
    Serial.println("shutting off");
    delay(200);
    Serial.end();
    display.ssd1306_command(SSD1306_DISPLAYOFF); 
    display.display();
  }  
  else
  {
    start=millis();
    delay(200); // for debounce
    Serial.println("Started...");
    delay(200); // for debounce
    displayResult();
    if(buttonPress()== 1)
   {  
   //button press
   }
    }
  
}
void firstScreen()
{
  display.drawRoundRect(15,18,100, 40,10, WHITE);
  display.setTextColor(WHITE,BLACK);
  display.setTextSize(1);
  display.setCursor(30,26);
  display.println("Thermometer");
  display.setCursor(35,45);
  display.println("Ambi+Obj");
  display.display();
}
void displayMenu()
{
  display.clearDisplay();
  display.drawRect(0, 0, 128, 64, WHITE);
  display.setTextColor(WHITE,BLACK);
  display.setTextSize(1);
  display.setCursor(30,7);
  display.println("Thermometer");
  display.drawLine(0, 20, 128, 20, WHITE);
  display.display();
  
  display.setTextSize(1);
  display.setCursor(30,30);
  display.println("Ambient Mode");
  display.setTextSize(1);
  display.setCursor(30,45);
  display.println("Object Mode");
  display.display();
  

}


boolean buttonPress() 
{   
  int reading=digitalRead(buttonPin);
  
  if (reading != lastButtonState) 
  {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
   // if the button state has changed:   
    if (reading != buttonState)
    {
      buttonState = reading;
      if (buttonState == LOW) 
      {
        SelectOption(counter);
       counter++;
    }   
   }
  }
 lastButtonState = reading;
}

void SelectOption(int sel)
{
  if(sel%2 == 1)
  {
  display.setCursor(10,30);
  display.println(">");
  display.display();
  delay(2000);
   display.clearDisplay();
   displayMenu();
   display.display();
   delay(50);
   ambientMode();
}
else
{
  display.setCursor(10,45);
  display.println(">");
  display.display();
  delay(2000);
  display.clearDisplay();
  displayMenu();
   display.display();
   delay(50);
    objectMode();
}
}
void ambientMode()
{
  display.clearDisplay();
  display.drawRect(0, 0, 128, 64, WHITE);
  display.setTextColor(WHITE,BLACK);
  display.setTextSize(1.5);
  display.setCursor(10,30);
  display.print(mlx.readAmbientTempC());
  display.print("C");
  display.display();
  
  display.drawLine(60,10, 60, 50, WHITE);
  display.display();
  
  display.setCursor(80,30);
  display.print(mlx.readAmbientTempF());
  display.print("F");
  display.display();
  delay(5000);
  display.clearDisplay();
  displayMenu();
  
}
void objectMode()
{
  display.clearDisplay();
  display.drawRect(0, 0, 128, 64, WHITE);
  display.setTextColor(WHITE,BLACK);
  display.setTextSize(1.5);
  display.setCursor(10,30);
  display.print(mlx.readObjectTempC());
  display.print("C");
  display.display();
 
  display.drawLine(60,10, 60, 50, WHITE);
  display.display();
  
  display.setCursor(80,30);
  display.print(mlx.readObjectTempF());
  display.print("F");
  display.display();
  delay(5000);
  display.clearDisplay();
  displayMenu();
}
void displayResult()
{
 
  float h,m,s,ms;
  unsigned long over;
  elapsed=start;
  h=int(elapsed/3600000);
  over=elapsed%3600000;
  m=int(over/60000);
  over=over%60000;
  s=int(over/1000);
  ms=over%1000;
  Serial.print("Raw elapsed time: ");
  Serial.println(elapsed);
  Serial.print("Elapsed time: ");
  Serial.print(h,0);
  Serial.print("h ");
  Serial.print(m,0);
  Serial.print("m ");
  Serial.print(s,0);
  Serial.print("s ");
  Serial.print(ms,0);
  Serial.println("ms");
  Serial.println();
  return elapsed;
}



