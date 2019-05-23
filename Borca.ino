#include "HX711.h"
#include <string.h>
#include <stdlib.h>
#define OFFSET 87900
#define lrd A0
#define RS 1
#define EN 2
#define CONTROL 0
#define DATA 1
// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

HX711 scale;

void setup() {
  Serial.begin(9600);
  DDRB = 0x07;
  DDRD = 0xF1;
  pinMode(lrd, INPUT);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  lcdStart();
  setLcdCursor(0, 0);
  lcdString("El puntaje se");
  setLcdCursor(1, 0);
  lcdString("da por peso");
  delay(5000);
  clearLcd();
  
  


  //scale.tare();
  
}

void loop() {

    int value = analogRead(lrd);
    long reading = scale.read();
    reading += OFFSET;
    Serial.print("lrd reading: ");
    Serial.println(value);
    Serial.print("HX711 reading: ");
    Serial.println(reading);
    if(reading >= 1000 && value >= 800){
      if((value >= 850 && value <= 905) && (reading >= 1090000 && reading <= 200000)){
          clearLcd();
          int puntos = reading/100000;
          setLcdCursor(0, 0);
          lcdString("vidrio");
          setLcdCursor(1, 0);
          lcdString("Puntos: ");
          lcdNumber(puntos);
          delay(100);
          
          
      }else if((value >= 910 && value <= 925)&&(reading >= 10000 && reading <= 30000)){
          clearLcd();
          int puntos2 = reading/10000;
          setLcdCursor(0, 0);
          lcdString("plastico");
          setLcdCursor(1, 0);
          lcdString("Puntos: ");
          lcdNumber(puntos2);
          delay(100);
      }
    }
    delay(500);
  
  
}


// LCD Functions
void lcdStart()
{
  _delay_ms(16);  //wait for LCD startup: 15ms

  writeCommand( CONTROL, 0x20 );  // function set: 4-bit bus
  _delay_us(45);    //execution time is 40us
  // this instruction will be handled as 8-bit bus. However the function assumes 4-bit bus
  // this will be ok, because the lower bits will be ignored by the lcd when it is busy

  writeCommand( CONTROL, 0x28 );  // function set: 4-bit bus, 2 lines, 5x8 chars
  _delay_us(45);    //execution time is 40us

  writeCommand( CONTROL, 0x0E );  // display on/off control: display on, cursor on, no blink
  _delay_us(45);    //execution time is 40us

  writeCommand( CONTROL, 0x06 );  // entry mode set: increment, no display shift
  _delay_us(45);    //execution time is 40us

  writeCommand( CONTROL, 0x01 );  // clear display
  _delay_ms(2);   //execution time is 1.64ms

  return;
}

//-------------------------------------------------------------------------

// write an lcd data/command, 4-bit version
// PB1: RS, PB2: Enable. PD3-PD0: D7-D4
void writeCommand( char RS_type, char CMD )
{
  // set RS line
  if ( RS_type ) PORTB |= 1 << RS;
  else PORTB &= ~(1 << RS);
  _delay_us(1); // min delay is 140ns (t_AS)

  // set enable
  PORTB |= 1 << EN;

  // place high 4-bit data/command on bus
  PORTD = (PORTD & 0x0F) | (CMD & 0xF0);
  _delay_us(1); // min delay is 195ns (t_DSW)

  // note: min Enable pulse is 450ns (t_PWH). At this point this delay is already met

  // clear enable
  PORTB &= ~(1 << EN);
  _delay_us(1); // delay for next data is 300ns

  // set enable
  PORTB |= 1 << EN;

  // place low 4-bit data/command on bus
  PORTD = (PORTD & 0x0F) | (CMD << 4);
  _delay_us(1); // min delay is 195ns (t_DSW)

  // note: min Enable pulse is 450ns (t_PWH). At this point this delay is already met

  // clear enable
  PORTB &= ~(1 << EN);
  _delay_us(1); // delay is 10ns (t_H)

  return;
}

//-------------------------------------------------------------------------

void lcdString(char text[])
{
  int i, length;

  length = strlen(text);

  for (i = 0; i < length; i++)
  {
    writeCommand( DATA, text[i] );
    _delay_us(45);    //execution time is 40us
  }

  return;
}

//-------------------------------------------------------------------------

void lcdChar(char c)
{
  writeCommand( DATA, c );
  _delay_us(45);    //execution time is 40us

  return;
}

//-------------------------------------------------------------------------

void lcdNumber(int num)
{
  char n[8];

  itoa( num, n, 10);
  lcdString( n );

  return;
}

//-------------------------------------------------------------------------

void lcdFloat(float num)
{
  int i, d;
  float n;

  i = (int) num;  //get integer part
  if ( num < 0 ) n = i - num;   //get fraction
  else n = num - i;
  d = (int)( n * 100 ); //convert fraction to integer: 2 digits

  lcdNumber( i );
  lcdChar( '.' );
  lcdNumber( d );

  return;
}

//-------------------------------------------------------------------------

void setLcdCursor(char line, char col)
{
  char position = 0x00;

  // make sure position is inside limits
  if ( line < 0 ) line = 0; // 2-line lcd
  if ( line > 1 ) line = 1;
  if ( col < 0 ) col = 0;   // 16-char lcd
  if ( col > 15 ) col = 15;

  if ( line == 0 ) position = 0x80 + col;
  if ( line == 1 ) position = 0xC0 + col;

  writeCommand( CONTROL, position );    // set DDRAM address to position
  _delay_us(45);    //execution time is 40us

  return;
}

//-------------------------------------------------------------------------

void clearLcd()
{
  writeCommand( CONTROL, 0x01 );  // clear display
  _delay_ms(2);   //execution time is 1.64ms

  return;
}
