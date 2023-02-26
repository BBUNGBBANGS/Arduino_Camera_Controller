#include <stdint.h>
#include <EEPROM.h>

#define SWITCH_PUSH     (0)
#define SWITCH_NONE     (1)

#define SWITCH_REQ_ON   (1)
#define SWITCH_REQ_OFF  (0)

#define MOTOR_INIT      (0)
#define MOTOR_MEMORY_1  (1)
#define MOTOR_MEMORY_2  (2)
#define MOTOR_MEMORY_3  (3)
#define MOTOR_MEMORY_4  (4)
#define MOTOR_MEMORY_5  (5)
#define MOTOR_SAVE_1    (6)
#define MOTOR_SAVE_2    (7)
#define MOTOR_SAVE_3    (8)
#define MOTOR_SAVE_4    (9)
#define MOTOR_SAVE_5    (10)

#define MOTOR_X_TOTAL   (31300)
#define MOTOR_Y_TOTAL   (31000)

const uint8_t OutLatchPin = 3;
const uint8_t OutDataPin = 4;
const uint8_t OutClockPin = 2;
const uint8_t InShiftPin = 10;
const uint8_t InClockPin = 12;
const uint8_t InClockEnPin = 9;
const uint8_t InDataPin = 11;
const uint8_t Motor_X_Pulse_Pin = A2;
const uint8_t Motor_X_Dir_Pin  = A1;
const uint8_t Motor_X_Ena_Pin  = A0;
const uint8_t Motor_Y_Pulse_Pin  = A5;
const uint8_t Motor_Y_Dir_Pin  = A4;
const uint8_t Motor_Y_Ena_Pin  = A3;
const uint8_t Motor_X_U_Limit_Pin = 5;
const uint8_t Motor_X_D_Limit_Pin = 6;
const uint8_t Motor_Y_U_Limit_Pin = 7;
const uint8_t Motor_Y_D_Limit_Pin = 8;

int32_t X_counter; //total 이동거리 : 31300
int32_t Y_counter; //total 이동거리 : 31000

uint8_t Led_Right,Led_M1,Led_M2,Led_M3,Led_M4,Led_M5,Output_Trigger;
uint16_t X_Position,Y_Position;
uint8_t Switch_Y_D,Switch_Y_U,Switch_X_D,Switch_X_U,Switch_Start,Switch_Finish;
uint8_t Switch_M1,Switch_M2,Switch_M3,Switch_M4,Switch_M5,Switch_Right;
uint8_t Motor_Memory_Status;
uint8_t Switch_X_D_Limit,Switch_X_U_Limit,Switch_Y_U_Limit,Switch_Y_D_Limit;
uint32_t Time_EEP;

uint8_t Motor_Init_Flag;
uint8_t Motor_X_Init,Motor_Y_Init;
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
    pinMode(Motor_X_Pulse_Pin, OUTPUT);
    pinMode(Motor_X_Dir_Pin, OUTPUT);
    pinMode(Motor_X_Ena_Pin, OUTPUT);
    pinMode(Motor_Y_Pulse_Pin, OUTPUT);
    pinMode(Motor_Y_Dir_Pin, OUTPUT);
    pinMode(Motor_Y_Ena_Pin, OUTPUT);
    pinMode(Motor_X_U_Limit_Pin, INPUT);
    pinMode(Motor_X_D_Limit_Pin, INPUT);
    pinMode(Motor_Y_U_Limit_Pin, INPUT);
    pinMode(Motor_Y_D_Limit_Pin, INPUT);

    digitalWrite(InClockEnPin, LOW); 

    EEPROM_Read();
}

void loop() 
{
    uint32_t Time_Loc = millis();
    Data_Input();
    Data_Output();

    if (Motor_Init_Flag == 0)
    {
        Motor_Initial_Operation();
    }
    else
    {
        Motor_Control();
        if((Time_Loc - Time_EEP) > 10000)
        {
            Time_EEP = Time_Loc;
            EEPROM_Write();
        }
    }

    Serial.print(Motor_X_Init);
    Serial.print(", ");
    Serial.print(Motor_Y_Init);
    Serial.print(", ");
    Serial.print(X_counter);
    Serial.print(", ");
    Serial.println(Y_counter);
    
}

static void EEPROM_Read(void)
{
    Motor_Memory_Status = EEPROM.read(0);
    X_counter = (EEPROM.read(1) << 24) | (EEPROM.read(2) << 16) | (EEPROM.read(3) << 8) | (EEPROM.read(4));
    Y_counter = (EEPROM.read(5) << 24) | (EEPROM.read(6) << 16) | (EEPROM.read(7) << 8) | (EEPROM.read(8));
}

static void EEPROM_Write(void)
{
    EEPROM.write(0, Motor_Memory_Status);
    EEPROM.write(1, (X_counter >> 24) & 0xff);
    EEPROM.write(2, (X_counter >> 16) & 0xff);
    EEPROM.write(3, (X_counter >> 8) & 0xff);
    EEPROM.write(4, (X_counter >> 0) & 0xff);
    EEPROM.write(5, (Y_counter >> 24) & 0xff);
    EEPROM.write(6, (Y_counter >> 16) & 0xff);
    EEPROM.write(7, (Y_counter >> 8) & 0xff);
    EEPROM.write(8, (Y_counter >> 0) & 0xff);
}

static void Motor_Initial_Operation(void)
{
    if ((Switch_X_U_Limit == SWITCH_NONE) && (Motor_X_Init == 0))
    {
        digitalWrite(Motor_X_Dir_Pin, LOW);
        digitalWrite(Motor_X_Ena_Pin, HIGH);
        digitalWrite(Motor_X_Pulse_Pin, HIGH);
        delayMicroseconds(50);
        digitalWrite(Motor_X_Pulse_Pin, LOW);
        delayMicroseconds(50);        
    }
    else
    {
        Motor_X_Init = 1;
        X_counter = 0;
    }

    if ((Switch_Y_U_Limit == SWITCH_NONE) && (Motor_Y_Init == 0))
    {
        digitalWrite(Motor_Y_Dir_Pin, LOW);
        digitalWrite(Motor_Y_Ena_Pin, HIGH);
        digitalWrite(Motor_Y_Pulse_Pin, HIGH);
        delayMicroseconds(50);
        digitalWrite(Motor_Y_Pulse_Pin, LOW);
        delayMicroseconds(50);
    }
    else
    {
        Motor_Y_Init = 1;
        Y_counter = 0;
    }

    if ((Motor_X_Init == 1) && (Motor_Y_Init == 1))
    {
        Motor_Init_Flag = 1;
    }
}

static void Motor_Control(void)
{
    if ((Switch_X_U == SWITCH_NONE) && (Switch_X_D == SWITCH_PUSH))
    {
        digitalWrite(Motor_X_Dir_Pin, LOW);
        digitalWrite(Motor_X_Ena_Pin, HIGH);
        digitalWrite(Motor_X_Pulse_Pin, HIGH);
        delayMicroseconds(50);
        digitalWrite(Motor_X_Pulse_Pin, LOW);
        delayMicroseconds(50);
        X_counter--;        
    }
    else if ((Switch_X_U == SWITCH_PUSH) && (Switch_X_D == SWITCH_NONE))
    {
        digitalWrite(Motor_X_Dir_Pin, HIGH);
        digitalWrite(Motor_X_Ena_Pin, HIGH);
        digitalWrite(Motor_X_Pulse_Pin, HIGH);
        delayMicroseconds(50);
        digitalWrite(Motor_X_Pulse_Pin, LOW);
        delayMicroseconds(50);
        X_counter++;
    }
    else
    {
        digitalWrite(Motor_X_Pulse_Pin, LOW);
        digitalWrite(Motor_X_Ena_Pin, LOW);
    }

    if ((Switch_Y_U == SWITCH_PUSH) && (Switch_Y_D == SWITCH_NONE))
    {
        digitalWrite(Motor_Y_Dir_Pin, LOW);
        digitalWrite(Motor_Y_Ena_Pin, HIGH);
        digitalWrite(Motor_Y_Pulse_Pin, HIGH);
        delayMicroseconds(50);
        digitalWrite(Motor_Y_Pulse_Pin, LOW);
        delayMicroseconds(50);
        Y_counter--;
    }
    else if ((Switch_Y_U == SWITCH_NONE) && (Switch_Y_D == SWITCH_PUSH))
    {
        digitalWrite(Motor_Y_Dir_Pin, HIGH);
        digitalWrite(Motor_Y_Ena_Pin, HIGH);
        digitalWrite(Motor_Y_Pulse_Pin, HIGH);
        delayMicroseconds(50);
        digitalWrite(Motor_Y_Pulse_Pin, LOW);
        delayMicroseconds(50);
        Y_counter++;
    }
    else
    {
        digitalWrite(Motor_Y_Ena_Pin, LOW);
        digitalWrite(Motor_Y_Pulse_Pin, LOW);
    }

    if(X_counter > 0)
    {
        X_Position = ((float)X_counter * 100.0f / (float)MOTOR_X_TOTAL);
    }
    else
    {
        X_Position = 0;
    }

    if(Y_counter > 0)
    {
        Y_Position = ((float)Y_counter * 100.0f / (float)MOTOR_Y_TOTAL);
    }
    else
    {
        Y_Position = 0;
    }


    Led_M1 = 1;
    Led_M2 = 1;
    Led_M3 = 1;
    Led_M4 = 1;
    Led_M5 = 1;
}

static void Switch_Control(void)
{
    static uint8_t stM1Old,stM2Old,stM3Old,stM4Old,stM5Old,stRightOld;

    if ((stRightOld == SWITCH_NONE) && (Switch_Right == SWITCH_PUSH))
    {
        Led_Right = (~Led_Right) & 0x01;
    }
    stRightOld = Switch_Right;
    if ((stM1Old == SWITCH_NONE) && (Switch_M1 == SWITCH_PUSH))
    {
        Motor_Memory_Status = MOTOR_MEMORY_1;
    }
    stM1Old = Switch_M1;
    if ((stM2Old == SWITCH_NONE) && (Switch_M2 == SWITCH_PUSH))
    {
        Motor_Memory_Status = MOTOR_MEMORY_2;
    }
    stM2Old = Switch_M2;
    if ((stM3Old == SWITCH_NONE) && (Switch_M3 == SWITCH_PUSH))
    {
        Motor_Memory_Status = MOTOR_MEMORY_3;
    }
    stM3Old = Switch_M3;
    if ((stM4Old == SWITCH_NONE) && (Switch_M4 == SWITCH_PUSH))
    {
        Motor_Memory_Status = MOTOR_MEMORY_4;
    }
    stM4Old = Switch_M4;
    if ((stM5Old == SWITCH_NONE) && (Switch_M5 == SWITCH_PUSH))
    {
        Motor_Memory_Status = MOTOR_MEMORY_5;
    }
    stM5Old = Switch_M5;    

}

static void Data_Output(void)
{
    uint8_t X_Out,Y_Out,SegOut,LED_Out;
    static uint8_t segment_index;

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
    LED_Out = (Led_Right & 0x01) | ((Led_M1 & 0x01) << 1) | ((Led_M2 & 0x01) << 2) | ((Led_M3 & 0x01) << 3) | 
              ((Led_M4 & 0x01) << 4) | ((Led_M5 & 0x01) << 5) | ((Output_Trigger & 0x01) << 6);

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
    uint8_t Switch_Status1,Switch_Status2;
    Data_Read_Begin();
    Switch_Status1 = Data_Read();
    Switch_Status2 = Data_Read();

    Switch_X_U_Limit = digitalRead(Motor_X_U_Limit_Pin);
    Switch_X_D_Limit = digitalRead(Motor_X_D_Limit_Pin);
    Switch_Y_U_Limit = digitalRead(Motor_Y_U_Limit_Pin);
    Switch_Y_D_Limit = digitalRead(Motor_Y_D_Limit_Pin);

    Switch_M5 = (Switch_Status1 >> 0) & 0x01;
    Switch_M4 = (Switch_Status1 >> 1) & 0x01;
    Switch_M3 = (Switch_Status1 >> 2) & 0x01;
    Switch_M2 = (Switch_Status1 >> 3) & 0x01;
    Switch_M1 = (Switch_Status1 >> 4) & 0x01;
    Switch_Right = (Switch_Status1 >> 5) & 0x01;

    Switch_Y_D = (Switch_Status2 >> 0) & 0x01;
    Switch_Y_U = (Switch_Status2 >> 1) & 0x01;
    Switch_X_D = (Switch_Status2 >> 2) & 0x01;
    Switch_X_U = (Switch_Status2 >> 3) & 0x01;
    Switch_Start = (Switch_Status2 >> 4) & 0x01;
    Switch_Finish = (Switch_Status2 >> 5) & 0x01;
}
static uint8_t Data_Read_Begin(void)
{
    digitalWrite(InShiftPin, LOW); 
    delayMicroseconds(5);
    digitalWrite(InShiftPin, HIGH); 
}

static uint8_t Data_Read(void)
{
    uint8_t result = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        uint8_t value = digitalRead(InDataPin);
        result |= (value << ((8 - 1) - i));

        digitalWrite(InClockPin,HIGH);
        delayMicroseconds(5);
        digitalWrite(InClockPin,LOW);
    }
    return result;
}