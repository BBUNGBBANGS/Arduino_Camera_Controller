
#include <ShiftRegister74HC595.h>

// parameters: <number of shift registers> (data pin, clock pin, latch pin)
//ShiftRegister74HC595<1> sr(4, 2, 3);

uint8_t latchPin = 3;
uint8_t dataPin = 4;
uint8_t clockPin = 2;

uint8_t Led_Status;
uint16_t X_Position,Y_Position = 999;
const uint8_t Segment_Num[] = 
{
    0b00111111, //0
    0b00000110, //1
    0b01011011, //2
    0b01001111, //3
    0b01100110, //4
    0b01101101, //5
    0b01111101, //6
    0b00000111, //7
    0b01111111, //8
    0b01101111  //9
};

void setup() 
{

    // put your setup code here, to run once:
    Serial.begin(115200);

    pinMode(latchPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
}

void loop() 
{
    Data_Output();
    delay(5);
}

static void Data_Output(void)
{
    uint8_t X_Out,Y_Out,SegOut,LED_Out;
    static uint8_t segment_index;

    X_Position++;
    Y_Position--;
    if(X_Position>999)
    {
        X_Position = 0;
    }
    if(Y_Position==0)
    {
        Y_Position = 999;
    }

    switch(segment_index)
    {
        case 0 :
            X_Out = Segment_Num[X_Position%10];
            Y_Out = Segment_Num[Y_Position%10];
            SegOut = 0b00011011;
        break;
        case 1 :
            X_Out = Segment_Num[(X_Position/10)%10];
            Y_Out = Segment_Num[(Y_Position/10)%10];
            SegOut = 0b00101101;
        break;
        case 2 :
            X_Out = Segment_Num[(X_Position/100)%10];
            Y_Out = Segment_Num[(Y_Position/100)%10];
            SegOut = 0b00110110; 
        break;
    }
    LED_Out = Led_Status;

    digitalWrite(latchPin,0);
    shiftOut(dataPin,clockPin,MSBFIRST,X_Out);
    shiftOut(dataPin,clockPin,MSBFIRST,SegOut);
    shiftOut(dataPin,clockPin,MSBFIRST,Y_Out);
    shiftOut(dataPin,clockPin,MSBFIRST,LED_Out);
    digitalWrite(latchPin,1);

    segment_index++;
    if (segment_index > 2)
    {
        segment_index = 0;
    }
}
