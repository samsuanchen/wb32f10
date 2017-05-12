// ws04.h // having wordset 04 for digital output
#ifndef WS04_H
#define WS04_H
void _14(){V.dsPush(16)                     ;} const Word W14 PROGMEM={       LAST,"led",_14};
void _15(){pinMode(V.dsPop(),OUTPUT)        ;} const Word W15 PROGMEM={(Word*)&W14,"out",_15};
void _16(){digitalWrite(V.dsPop(),HIGH)     ;} const Word W16 PROGMEM={(Word*)&W15,"hi" ,_16};
void _17(){digitalWrite(V.dsPop(),LOW)      ;} const Word W17 PROGMEM={(Word*)&W16,"lo" ,_17};
void _18(){digitalWrite(V.dsPop(),V.dsPop());} const Word W18 PROGMEM={(Word*)&W17,"p!" ,_18};
void _19(){delay(V.dsPop())                 ;} const Word W19 PROGMEM={(Word*)&W18,"ms" ,_19};
#define LAST (Word*)&W19
#endif WS04_H
