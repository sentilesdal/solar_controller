#ifndef Board_h
#define Board_h

String boardType;
byte digitalArraySize, analogArraySize;

char pinsMode[8];
int pinsValue[8];

void boardInit()
{
  for (byte i = 0; i <= 7; i++)
  {
    pinsMode[i] = 'o';
    pinsValue[i] = 0;
    pinMode(i, OUTPUT);
  }
  boardType = "mkr1000";
  digitalArraySize = 8;
  analogArraySize = 0;
}

#endif
