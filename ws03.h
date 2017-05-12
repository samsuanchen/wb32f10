// ws03.h // having wordset 03 for tools
#ifndef WS03_H
#define WS03_H
void _11 () { V.words(V.parseToken())              ; } const Word W11 PROGMEM={       LAST,"words" ,_11};
void _12 () { V.see(V.vocSearch(V.parseToken()))   ; } const Word W12 PROGMEM={(Word*)&W11,"see"   ,_12};
void _13 () { X=V.dsPop();V.dump((int*)V.dsPop(),X); } const Word W13 PROGMEM={(Word*)&W12,"dump"  ,_13};
#define LAST (Word*)&W13
#endif WS03_H
