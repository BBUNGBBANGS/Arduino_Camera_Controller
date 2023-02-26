

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

const uint8_t OutLatchPin = 3;
const uint8_t OutDataPin = 4;
const uint8_t OutClockPin = 2;
const uint8_t InShiftPin = 10;
const uint8_t InClockPin = 12;
const uint8_t InClockEnPin = 9;
const uint8_t InDataPin = 11;

uint8_t Led_Right,Led_M1,Led_M2,Led_M3,Led_M4,Led_M5,Output_Trigger;
uint16_t X_Position,Y_Position = 999;
uint8_t Switch_Y_D,Switch_Y_U,Switch_X_D,Switch_X_U,Switch_Start,Switch_Finish;
uint8_t Switch_M1,Switch_M2,Switch_M3,Switch_M4,Switch_M5,Switch_Right;
uint8_t Motor_Memory_Status;
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
}

void loop() 
{
    Data_Input();
    delay(5);
    Data_Output();
    delay(5);
    Motor_Control();
    Serial.print(Switch_Y_D);
    Serial.print(", ");
    Serial.println(Switch_Y_U);
}

static void Motor_Control(void)
{
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