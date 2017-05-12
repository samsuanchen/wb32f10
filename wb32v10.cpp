#include "wb32v10.h"
void	WB32V10::tibOpen  () { tibBegin=tibEnd=tib,tibLimit=tib+TIB_SIZE-1; } // init tib and wait for input
void	WB32V10::tibClose () { *tibEnd='\0'; } // add '\0' at end of input
void	WB32V10::tibPop   () { --tibEnd; } // pop last input character
void	WB32V10::tibPush  (char c) { *(tibEnd++)=c; } // collect input character
/////////////////////////////////////////////////////////////////////////////////////////////////////////
boolean WB32V10::EOL        (char c) { return c=='\n'||c=='\r'; } // check if c is end of line
boolean WB32V10::backSpace  (char c) { return c=='\b'; } // check if c is back space
boolean WB32V10::whiteSpace (char c) { return c==' '||c=='\t'||c=='\n'||c=='\r'; } // check if c is white space
boolean WB32V10::tibEmpty   (){ return tibEnd==tibBegin; } // check if buffer is empty
boolean WB32V10::tibFull    (){ return tibEnd==tibLimit; } // check if buffer is full
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void	WB32V10::waitInput() { // read input characters until end of line
  while ( !AVAILABLE() ); // wait until input available
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
char*	WB32V10::readLine() { // read input characters until end of line
  tibOpen();
  while ( AVAILABLE() ) {
    char c=READ(); // get input char
    if ( backSpace(c) ) { // if backspace ------------------------
      if ( !tibEmpty() ) tibPop(), PRINTF("\b \b");    // erase last input char
    } else if ( EOL(c) || tibFull() ) {
      tibPush('\0');
      return tib;
    } else {
      tibPush(c); // collect character c
      WRITE(c);
      waitInput(); // wait until input available
    }
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void	WB32V10::parseBegin(char *str){ parseRemain = tibBegin = str; tibEnd = tibBegin+strlen(str); }
char	WB32V10::parseAvailable(){ return *parseRemain; } // non '\0' means available to parse
char*	WB32V10::parseToken() { // use white spaces as delimiters to parse a token
  char c = *parseRemain;
  while ( c && whiteSpace(c) ) c = *++parseRemain; // ignore leading white spaces
  char *start = parseRemain; // set start at non white space
  if ( c ) {
    while ( c && !whiteSpace(c) ) c = *++parseRemain; // colect  non white spaces
    char *limit = parseRemain; // set limit at white space
    if ( c ) { // if not end of string
      int n=limit-start;
      if ( n>TMP_SIZE-1 ) { //
      	PRINTF("\n error WB32V10::parseToken length %d > %d\n",n,TMP_SIZE-1);
      	return start;
      }
      strncpy(tmp,start,n);
      tmp[n] = '\0'; // add null as the end of string
      start = tmp;
    }
  }
  return start;
}

//.......................................................................................................
void	WB32V10::interpret(char *line){
  parseBegin(line);
  while ( parseAvailable() ) {
    char *token = parseToken();
    eval(token);
  }
  PRINTF("\n");
}
//.......................................................................................................
void	WB32V10::eval(char *token){
// extern Word, forth, getBase, vocSearch, hexPrefix, Serial.printf, dsPush
  char *remain, *p=token, c;
  Word* w=vocSearch(token);
  if(w){ w->code(); return; }
  int b=getBase(); if(remain=hexPrefix(token)) p=remain, b=16; 
  int n=strtol(p, &remain, b); // conver string at p to integer n on base b (*remain is an illigal digit)
  if(c=*remain) PRINTF("\nerr %02d %s ? illigal '%c' at %d as base%d digit",++n,token,c,remain-token,b);
  else dsPush(n);
}
//.......................................................................................................
void	WB32V10::showDataStack(){ // show data stack info
  PRINTF("< dsDepth %d [ ",dsDepth()); // show depth
  if(dsDepth()>0){
    if(dsDepth()>5)PRINTF(".. "); // showing at most top 5 items
    for ( int *i=max(DP-4,DS); i<=DP; i++ ) PRINTF("%s ",toDigits(*i,B)); // show data
  }
  PRINTF("] base%d >\n",B); // show base 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
boolean WB32V10::dsHasItems (int n) { return DP >= DS+n-1       ; } // check if data stack has n items
boolean WB32V10::dsHasSpace (int n) { return DP+n <= DS+DS_DEPTH; } // check if data stack has space for n items
boolean WB32V10::dsFull     ()      { return DP >= DS+DS_DEPTH  ; } // check if data stack full
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void	WB32V10::dsClear  () { DP=DS-1 ; } // reset data stack
void	WB32V10::dsPush   (int n) { T=n, *(++DP)=T; } // push a number onto data stack
int		WB32V10::dsPop    () { return *DP-- ; } // pop a number from data stack
int		WB32V10::dsDepth  () { return DP-DS+1; } // depth of data stack
char	WB32V10::toDigit  (int x) { return x += x<10 ? 0x30 : 0x61-10; } // convert integer x into single digit
char*	WB32V10::toDigits (uint x, int b) { // convert integer x into digits in given base b
  char*p=tmp+TMP_SIZE; *(--p)='\0';
  if(x==0){ *(--p)='0'; return p; }
  boolean neg; if( neg=(b==10&&(int)x<0) ) x=-x;
  while(x){
    *(--p)=toDigit(x%b),x/=b; // convert current digit to character
  }
  if(neg) *(--p)='-';
  return p;
}
void	WB32V10::setBase (int base) { B=base; }; // set number convertion base
int		WB32V10::getBase () {return B;}; // get number convertion base
char*	WB32V10::hexPrefix(char *s) { // 0xff 0XFF $ff $FF $Ff are all acceptable as hexadecimal numbers
  char c;
  if((c=*s++) != '0' && c != '$') return 0;
  if(c=='0' && (c=*s++) != 'x' && c != 'X') return 0;
  return s; // remain string
}
Word*	WB32V10::vocSearch (char *name) { // search name in dict
  Word *w=getVoc()->last; while ( w && strcmp(w->name,name) ) w=w->prev; return w;
}
Voc*	WB32V10::getVoc() {
  return voc;
}
#define CONSOLE_WIDTH 80
//.......................................................................................................
void WB32V10::words(char*sub) { // show all word names having specific substring
  PRINTF("\n");
  int m=0, n;
  Word *w=getVoc()->last;
  while (w) { 
    if(!*sub || strstr(w->name,sub) ){
      n=strlen(w->name);
      if(m+n>CONSOLE_WIDTH)PRINTF("\n"), m=0;
      if(m)PRINTF(" "), m++;
      PRINTF(w->name), m+=n;
    }
    w=w->prev;
  }
}
//.......................................................................................................
void WB32V10::see(Word *w) { // show the forth word
  if(!w){ PRINTF(" ? undefinded "); return; }
  PRINTF("\n----------------------");
  PRINTF("\n%x prev %08x"            ,&w->prev,w->prev        );
  PRINTF("\n%x name %08x %s"         ,&w->name,w->name,w->name);
  PRINTF("\n%x code %08x"            ,&w->code,w->code        );
  PRINTF("\nforth primative word %s ", w->name                );
}
//.......................................................................................................
void WB32V10::dump(int *a,int n) { // dump n cells at adr
// extern dsPop, PRINTF
    int *lmt=a+n;
    char *ba;
    char *blmt;
    for( ; a<lmt; a+=4) { 
      PRINTF("\n%8.8x : ", (int) a);
      for(int i=0; i< 4; i++){
        if( a+i>=lmt )PRINTF("         ");
        else          PRINTF("%8.8x ", *( a+i));
      }
      PRINTF(": ");
      ba=(char*)a, blmt=(char*)lmt;
      for(int i=0; i<16; i++){
        if(ba+i>=blmt)PRINTF("   ");
        else          PRINTF("%2.2x ", *(ba+i));
      }
      PRINTF(": ");
      for(int i=0; i<16; i++){
        if(ba+i>=blmt)PRINTF(" ");
        else {
          char c=*(ba+i); n=(int)c;
          if( n==0 ) c='.';
          else if( n<0x20 || (n>0x7e&&n<0xa4) || n>0xc6 ) c='_';
          else if(n>=0xa4&&n<=0xc6) { // head-byte of commmon big5
            n=(int)*(ba+i+1);
            if( n<0x40 || (n>0x7e&&n<0xa1) || n>0xfe) c='_'; // next is not tail-byte of commmon big5 
            else PRINTF("%c",c), c=(char)n, i++; // show head-byte of commmon big5 and get tail-byte
          }
          PRINTF("%c",c);
        }
      }
    }
}
//.......................................................................................................
void WB32V10::vocInit (Word *last) {
  voc = (Voc*) malloc(sizeof(Voc));
  voc->last = last;
}