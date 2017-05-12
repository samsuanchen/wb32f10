// ws01.h // having wordset 01 for base setting of number conversion
#ifndef WS01_H
#define WS01_H
void _01 () {
  V.setBase( 2);
}
const Word W01 PROGMEM={       LAST,"bin"   ,_10};
void _02 () {
  V.setBase( 8);
}
const Word W02 PROGMEM={(Word*)&W01,"oct"   ,_02};
void _03 () {
  V.setBase(10);
}
const Word W03 PROGMEM={(Word*)&W02,"dec"   ,_03};
void _04 () {
  V.setBase(16);
}
const Word W04 PROGMEM={(Word*)&W03,"hex"   ,_04};
void _05 () {
  V.setBase(V.dsPop());
}
const Word W05 PROGMEM={(Word*)&W04,"base!" ,_05};
void _06 () {
  V.dsPush(V.getBase())                ;
}
const Word W06 PROGMEM={(Word*)&W05,"base@" ,_06};
#define LAST (Word*)&W06
#endif WS01_H
