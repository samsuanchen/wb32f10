// ws05.h // having wordset 05 for digital input
#ifndef WS05_H
#define WS05_H
void _20(){pinMode(V.dsPop(),INPUT)         ;} const Word W20 PROGMEM={       LAST,"inp",_20};
void _21(){V.dsPush(digitalRead(V.dsPop())) ;} const Word W21 PROGMEM={(Word*)&W20,"p@" ,_21};
#define LAST (Word*)&W21
#endif WS05_H
