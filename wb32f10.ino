#include "wb32v10.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////
WB32V10 V = WB32V10(); // have V as an instence of wb32v10 forth Virtual machine
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LAST NULL
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void _00(){V.setBase( 2)                    ;} const Word W00 PROGMEM={       LAST,"bin"   ,_00};
void _01(){V.setBase( 8)                    ;} const Word W01 PROGMEM={(Word*)&W00,"oct"   ,_01};
void _02(){V.setBase(10)                    ;} const Word W02 PROGMEM={(Word*)&W01,"dec"   ,_02};
void _03(){V.setBase(16)                    ;} const Word W03 PROGMEM={(Word*)&W02,"hex"   ,_03};

void _04(){V.setBase(V.dsPop())             ;} const Word W04 PROGMEM={(Word*)&W03,"base!" ,_04};
void _05(){V.dsPush(V.getBase())            ;} const Word W05 PROGMEM={(Word*)&W04,"base@" ,_05};

void _06(){V.dsPush(V.dsPop()+V.dsPop())    ;} const Word W06 PROGMEM={(Word*)&W05,"+"     ,_06};
void _07(){V.dsPush(V.dsPop()*V.dsPop())    ;} const Word W07 PROGMEM={(Word*)&W06,"*"     ,_07};
int X;
void _08(){X=V.dsPop(),V.dsPush(V.dsPop()-X);} const Word W08 PROGMEM={(Word*)&W07,"-"     ,_08};
void _09(){X=V.dsPop(),V.dsPush(V.dsPop()/X);} const Word W09 PROGMEM={(Word*)&W08,"/"     ,_09};

void _10(){V.words()                        ;} const Word W10 PROGMEM={(Word*)&W09,"words" ,_10};
void _11(){V.see()                          ;} const Word W11 PROGMEM={(Word*)&W10,"see"   ,_11};
void _12(){V.dump()                         ;} const Word W12 PROGMEM={(Word*)&W11,"dump"  ,_12};
#define LAST (Word*)&W12
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ext01.h"
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
