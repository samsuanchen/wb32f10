// ws02.h // having wordset 02 for arithmetic operation
#ifndef WS02_H
#define WS02_H
void _07 () { V.dsPush(V.dsPop()+V.dsPop())        ; } const Word W07 PROGMEM={       LAST,"+"     ,_07};
void _08 () { V.dsPush(V.dsPop()*V.dsPop())        ; } const Word W08 PROGMEM={(Word*)&W07,"*"     ,_08};
int   X;
void _09 () { X=V.dsPop();V.dsPush(V.dsPop()-X)    ; } const Word W09 PROGMEM={(Word*)&W08,"-"     ,_09};
void _10 () { X=V.dsPop();V.dsPush(V.dsPop()/X)    ; } const Word W10 PROGMEM={(Word*)&W09,"/"     ,_10};
#define LAST (Word*)&W10
#endif WS02_H
