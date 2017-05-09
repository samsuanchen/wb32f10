#include "wb32v10.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////
WB32V10 V = WB32V10(); // have V as an instence of wb32v10 forth Virtual machine
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int X;
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void _00(){V.setBase( 2)                    ;} const Word W00 PROGMEM={          0,"bin"   ,_00};
void _01(){V.setBase( 8)                    ;} const Word W01 PROGMEM={(Word*)&W00,"oct"   ,_01};
void _02(){V.setBase(10)                    ;} const Word W02 PROGMEM={(Word*)&W01,"dec"   ,_02};
void _03(){V.setBase(16)                    ;} const Word W03 PROGMEM={(Word*)&W02,"hex"   ,_03};
void _04(){V.setBase(V.dsPop())             ;} const Word W04 PROGMEM={(Word*)&W03,"base!" ,_04};
void _05(){V.dsPush(V.getBase())            ;} const Word W05 PROGMEM={(Word*)&W04,"base@" ,_05};
void _06(){V.dsPush(V.dsPop()+V.dsPop())    ;} const Word W06 PROGMEM={(Word*)&W05,"+"     ,_06};
void _07(){V.dsPush(V.dsPop()*V.dsPop())    ;} const Word W07 PROGMEM={(Word*)&W06,"*"     ,_07};
void _08(){X=V.dsPop(),V.dsPush(V.dsPop()-X);} const Word W08 PROGMEM={(Word*)&W07,"-"     ,_08};
void _09(){X=V.dsPop(),V.dsPush(V.dsPop()/X);} const Word W09 PROGMEM={(Word*)&W08,"/"     ,_09};
void _10(){V.words()                        ;} const Word W10 PROGMEM={(Word*)&W09,"words" ,_10};
void _11(){V.see()                          ;} const Word W11 PROGMEM={(Word*)&W10,"see"   ,_11};
void _12(){V.dump()                         ;} const Word W12 PROGMEM={(Word*)&W11,"dump"  ,_12};
void _13(){V.dsPush(16)                     ;} const Word W13 PROGMEM={(Word*)&W12,"led"   ,_13};
void _14(){pinMode(V.dsPop(),OUTPUT)        ;} const Word W14 PROGMEM={(Word*)&W13,"out"   ,_14};
void _15(){pinMode(V.dsPop(),INPUT)         ;} const Word W15 PROGMEM={(Word*)&W14,"inp"   ,_15};
void _16(){digitalWrite(V.dsPop(),HIGH)     ;} const Word W16 PROGMEM={(Word*)&W15,"hi"    ,_16};
void _17(){digitalWrite(V.dsPop(),LOW)      ;} const Word W17 PROGMEM={(Word*)&W16,"lo"    ,_17};
void _18(){digitalWrite(V.dsPop(),V.dsPop());} const Word W18 PROGMEM={(Word*)&W17,"p!"    ,_18};
void _19(){V.dsPush(digitalRead(V.dsPop())) ;} const Word W19 PROGMEM={(Word*)&W18,"p@"    ,_19};
void _20(){delay(V.dsPop())                 ;} const Word W20 PROGMEM={(Word*)&W19,"ms"    ,_20};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LAST (Word*)&W20
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void prompt() {
  static int i=0; // index of the input line
  V.showDataStack(); // show the depth, numbers, and the number coversion base of Vm data stack 
  Serial.printf("--------------------------------------------------\n");
  Serial.printf("inp %02d : ", i++); // ask to enter characters (until caridge return) as input line iL
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  V.dsClear();
  V.vocInit(LAST);
  pinMode(16,OUTPUT); // set led pin 16 as output
  Serial.begin(115200); delay(1000); 
  Serial.printf("==================================================\n");
  Serial.printf("Simple WifiBoy ESP 32 Forth System having 21 Words\n");
  Serial.printf("20170510 derek@wifiboy.org & samsuanchen@gmail.com\n");
  Serial.printf("==================================================\n");
  V.interpret("0x100 100 2 $20 -1 words");
  prompt();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  if( Serial.available() ) {
    char *line = V.readLine();
    V.interpret(line);
    prompt();
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
