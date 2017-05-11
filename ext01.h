// ext01.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void _13(){V.dsPush(16)                     ;} const Word W13 PROGMEM={       LAST,"led"   ,_13};
void _14(){pinMode(V.dsPop(),OUTPUT)        ;} const Word W14 PROGMEM={(Word*)&W13,"out"   ,_14};
void _15(){pinMode(V.dsPop(),INPUT)         ;} const Word W15 PROGMEM={(Word*)&W14,"inp"   ,_15};
void _16(){digitalWrite(V.dsPop(),HIGH)     ;} const Word W16 PROGMEM={(Word*)&W15,"hi"    ,_16};
void _17(){digitalWrite(V.dsPop(),LOW)      ;} const Word W17 PROGMEM={(Word*)&W16,"lo"    ,_17};
void _18(){digitalWrite(V.dsPop(),V.dsPop());} const Word W18 PROGMEM={(Word*)&W17,"p!"    ,_18};
void _19(){V.dsPush(digitalRead(V.dsPop())) ;} const Word W19 PROGMEM={(Word*)&W18,"p@"    ,_19};
void _20(){delay(V.dsPop())                 ;} const Word W20 PROGMEM={(Word*)&W19,"ms"    ,_20};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LAST (Word*)&W20