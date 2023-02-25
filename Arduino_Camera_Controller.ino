
#include <ShiftRegister74HC595.h>

// parameters: <number of shift registers> (data pin, clock pin, latch pin)
//ShiftRegister74HC595<1> sr(4, 2, 3);

const uint8_t OutLatchPin = 3;
const uint8_t OutDataPin = 4;
const uint8_t OutClockPin = 2;
const uint8_t InShiftPin = 10;
const uint8_t InClockPin = 12;
const uint8_t InClockEnPin = 9;
const uint8_t InDataPin = 11;

int8_t Led_Status;
uint16_t X_Position,Y_Position = 999;
uint8_t Switch_Status1,Switch_Status2;
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

    pinMode(OutLatchPin, OUTPUT);
    pinMode(OutDataPin, OUTPUT);
    pinMode(OutClockPin, OUTPUT);
    pinMode(InShiftPin, OUTPUT);
    pinMode(InClockPin, OUTPUT);
    pinMode(InClockEnPin, OUTPUT);
    pinMode(InDataPin, INPUT);

    digitalWrite(InClockEnPin, LOW); 
    digitalWrite(InClockPin, HIGH); 
    digitalWrite(InShiftPin, HIGH);
}

void loop() 
{
    Data_Input();
    delay(5);
    Data_Output();
    delay(5);
    Serial.print(Switch_Status1);
    Serial.print(", ");
    Serial.println(Switch_Status2);
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

    digitalWrite(OutLatchPin,LOW);
    shiftOut(OutDataPin, OutClockPin, MSBFIRST, X_Out);
    shiftOut(OutDataPin, OutClockPin, MSBFIRST, SegOut);
    shiftOut(OutDataPin, OutClockPin, MSBFIRST, Y_Out);
    shiftOut(OutDataPin, OutClockPin, MSBFIRST, LED_Out);
    digitalWrite(OutLatchPin,HIGH);

    segment_index++;
    if (segment_index > 2)
    {
        segment_index = 0;
    }
}

static void Data_Input(void)
{
    uint8_t bitVal,bytesVal;
    static uint8_t index;

    digitalWrite(InClockPin, HIGH); 
    digitalWrite(InShiftPin, LOW); 
    delayMicroseconds(5);
    digitalWrite(InShiftPin, HIGH); 
  

    for(int i = 0; i < 8; i++)
    {
        Switch_Status1 = shiftIn(InDataPin, InClockPin, MSBFIRST);
    }

    for(int i = 0; i < 8; i++)
    {
        Switch_Status2 = shiftIn(InDataPin, InClockPin, MSBFIRST);
    }

}