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
int32_t X_counter_Target;
int32_t Y_counter_Target;

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
}

void loop() 
{
    Data_Input();
    Data_Output();

    if (Motor_Init_Flag == 0)
    {
        Motor_Initial_Operation();
    }
    else
    {
        Motor_Control();
    }
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
}

static void Switch_Control(void)
{
    static uint32_t ctRight,ctM1,ctM2,ctM3,ctM4,ctM5;

    if (Switch_Right == SWITCH_PUSH)
    {
        ctRight++;
    }
    else
    {
        if (ctRight > 100)
        {
            Led_Right = (~Led_Right) & 0x01;
            ctRight = 0;
        }
        else
        {
            ctRight = 0;
        }
    }
    
    if (Switch_M1 == SWITCH_PUSH)
    {
       ctM1++;
    }
    else
    {
        if (ctM1 > 1000)
        {
            Motor_Memory_Status = MOTOR_SAVE_1;
            EEPROM.write(0, (X_counter >> 24) & 0xff);
            EEPROM.write(1, (X_counter >> 16) & 0xff);
            EEPROM.write(2, (X_counter >> 8) & 0xff);
            EEPROM.write(3, (X_counter >> 0) & 0xff);
            EEPROM.write(4, (Y_counter >> 24) & 0xff);
            EEPROM.write(5, (Y_counter >> 16) & 0xff);
            EEPROM.write(6, (Y_counter >> 8) & 0xff);
            EEPROM.write(7, (Y_counter >> 0) & 0xff);
            ctM1 = 0;
        }
        else if (ctM1 > 100)
        {
            Motor_Memory_Status = MOTOR_MEMORY_1;
            Led_M1 = 1;
            X_counter_Target = (EEPROM.read(0) << 24) | (EEPROM.read(1) << 16) | (EEPROM.read(2) << 8) | (EEPROM.read(3));
            Y_counter_Target = (EEPROM.read(4) << 24) | (EEPROM.read(5) << 16) | (EEPROM.read(6) << 8) | (EEPROM.read(7));
            ctM1 = 0;
        }        
        else
        {
            ctM1 = 0;
        }
    }
    
    if (Switch_M2 == SWITCH_PUSH)
    {
       ctM2++;
    }
    else
    {
        if (ctM2 > 1000)
        {
            Motor_Memory_Status = MOTOR_SAVE_2;
            EEPROM.write(8, (X_counter >> 24) & 0xff);
            EEPROM.write(9, (X_counter >> 16) & 0xff);
            EEPROM.write(10, (X_counter >> 8) & 0xff);
            EEPROM.write(11, (X_counter >> 0) & 0xff);
            EEPROM.write(12, (Y_counter >> 24) & 0xff);
            EEPROM.write(13, (Y_counter >> 16) & 0xff);
            EEPROM.write(14, (Y_counter >> 8) & 0xff);
            EEPROM.write(15, (Y_counter >> 0) & 0xff);
            ctM2 = 0;
        }
        else if (ctM2 > 100)
        {
            Motor_Memory_Status = MOTOR_MEMORY_2;
            Led_M2 = 1;
            X_counter_Target = (EEPROM.read(8) << 24) | (EEPROM.read(9) << 16) | (EEPROM.read(10) << 8) | (EEPROM.read(11));
            Y_counter_Target = (EEPROM.read(12) << 24) | (EEPROM.read(13) << 16) | (EEPROM.read(14) << 8) | (EEPROM.read(15));
            ctM2 = 0;
        }        
        else
        {
            ctM2 = 0;
        }
    }
    
    if (Switch_M3 == SWITCH_PUSH)
    {
       ctM3++;
    }
    else
    {
        if (ctM3 > 1000)
        {
            Motor_Memory_Status = MOTOR_SAVE_3;
            EEPROM.write(16, (X_counter >> 24) & 0xff);
            EEPROM.write(17, (X_counter >> 16) & 0xff);
            EEPROM.write(18, (X_counter >> 8) & 0xff);
            EEPROM.write(19, (X_counter >> 0) & 0xff);
            EEPROM.write(20, (Y_counter >> 24) & 0xff);
            EEPROM.write(21, (Y_counter >> 16) & 0xff);
            EEPROM.write(22, (Y_counter >> 8) & 0xff);
            EEPROM.write(23, (Y_counter >> 0) & 0xff);
            ctM3 = 0;
        }
        else if (ctM3 > 100)
        {
            Motor_Memory_Status = MOTOR_MEMORY_3;
            Led_M3 = 1;
            X_counter_Target = (EEPROM.read(16) << 24) | (EEPROM.read(17) << 16) | (EEPROM.read(18) << 8) | (EEPROM.read(19));
            Y_counter_Target = (EEPROM.read(20) << 24) | (EEPROM.read(21) << 16) | (EEPROM.read(22) << 8) | (EEPROM.read(23));
            ctM3 = 0;
        }        
        else
        {
            ctM3 = 0;
        }
    }
    
    if (Switch_M4 == SWITCH_PUSH)
    {
       ctM4++;
    }
    else
    {
        if (ctM4 > 1000)
        {
            Motor_Memory_Status = MOTOR_SAVE_4;
            EEPROM.write(24, (X_counter >> 24) & 0xff);
            EEPROM.write(25, (X_counter >> 16) & 0xff);
            EEPROM.write(26, (X_counter >> 8) & 0xff);
            EEPROM.write(27, (X_counter >> 0) & 0xff);
            EEPROM.write(28, (Y_counter >> 24) & 0xff);
            EEPROM.write(29, (Y_counter >> 16) & 0xff);
            EEPROM.write(30, (Y_counter >> 8) & 0xff);
            EEPROM.write(31, (Y_counter >> 0) & 0xff);
            ctM4 = 0;
        }
        else if (ctM4 > 100)
        {
            Motor_Memory_Status = MOTOR_MEMORY_4;
            Led_M4 = 1;
            X_counter_Target = (EEPROM.read(24) << 24) | (EEPROM.read(25) << 16) | (EEPROM.read(26) << 8) | (EEPROM.read(27));
            Y_counter_Target = (EEPROM.read(28) << 24) | (EEPROM.read(29) << 16) | (EEPROM.read(30) << 8) | (EEPROM.read(31));
            ctM4 = 0;
        }        
        else
        {
            ctM4 = 0;
        }
    }

    if (Switch_M5 == SWITCH_PUSH)
    {
       ctM5++;
    }
    else
    {
        if (ctM5 > 1000)
        {
            Motor_Memory_Status = MOTOR_SAVE_5;
            EEPROM.write(32, (X_counter >> 24) & 0xff);
            EEPROM.write(33, (X_counter >> 16) & 0xff);
            EEPROM.write(34, (X_counter >> 8) & 0xff);
            EEPROM.write(35, (X_counter >> 0) & 0xff);
            EEPROM.write(36, (Y_counter >> 24) & 0xff);
            EEPROM.write(37, (Y_counter >> 16) & 0xff);
            EEPROM.write(38, (Y_counter >> 8) & 0xff);
            EEPROM.write(39, (Y_counter >> 0) & 0xff);
            ctM5 = 0;
        }
        else if (ctM1 > 100)
        {
            Motor_Memory_Status = MOTOR_MEMORY_5;
            Led_M5 = 1;
            X_counter_Target = (EEPROM.read(32) << 24) | (EEPROM.read(33) << 16) | (EEPROM.read(34) << 8) | (EEPROM.read(35));
            Y_counter_Target = (EEPROM.read(36) << 24) | (EEPROM.read(37) << 16) | (EEPROM.read(38) << 8) | (EEPROM.read(39));
            ctM5 = 0;
        }        
        else
        {
            ctM5 = 0;
        }
    }
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