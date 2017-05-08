#define PRINTF(format, ...) Serial.printf(format, ## __VA_ARGS__) 

//#include "wb32f10.h"
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv wb32f10.h vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
typedef void (*FuncP) (); // function code pointer type
/////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct Word {     // forth word type
  struct Word      *prev; // point to previous forth word
  char             *name; // name of forth word
  FuncP             code; // point to function code
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct Voc {      // forth vocaburary type
  Word             *last; // point to the last defined forth word
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
boolean EOL       (char c      ); // check if c is end of line
boolean baskSpace (char c      ); // check if c is back space
boolean whiteSpace(char c      ); // check if c is white space
boolean tibEmpty  (            ); // check if tib is empty
boolean dsHasItems(int  n      ); // check if data stack has n items
boolean dsHasSpace(int  n      ); // check if data stack has space for n items
boolean dsFull    (            ); // check if data stack full
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void    tibOpen   (char*t,int n); // take n-byte space at t as tib and wait for input
void    tibClose  (            ); // add '\0' at tibEnd
void    tibPop    (            ); // pop last input character from tib
void    tibPush   (char c      ); // collect  input character into tib
void    dsClear   (            ); // reset data stack
void    dsPush    (int  n      ); // push a number onto data stack
int     dsPop     (            ); // pop a number from data stack
int     dsDepth   (            ); // depth of data stack
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void    showCharCodes(char*str ); // show str length and each charCode in str
void    showDataStack(         ); // show data stack info
char    toDigit (int x         ); // oververt integer x into a single digit
char   *toDigits(uint x,int base); // convert integer x into digits in given base
void    parseOpen    (char*src ); // reset for parsing
char   *parseToken   (         ); // use white spaces as delimiters to parse a token
char   *hexPrefix    (char*str ); // 0x, 0X, or $ are all acceptable as hexadecimal number prefix
void    words        (         ); // words <substr> ( -- ) show all word names having specific substring
void    dump         (         ); // dump ( adr n -- ) // dump n cells at adr
Word   *vocSearch(Voc*voc,char*name); // search name in dict
void    see          (         ); // show the forth word of given name
void    setBase      (int base ); // set number convertion base
int     getBase      (         ); // get number convertion base
/////////////////////////////////////////// forth primative /////////////////////////////////////////////
void _bin  (); // 01. bin               (     --     ) set number convertion base B= 2
void _oct  (); // 02. oct               (     --     ) set number convertion base B= 8
void _dec  (); // 03. dec               (     --     ) set number convertion base B=10
void _hex  (); // 04. hex               (     --     ) set number convertion base B=16
void _add  (); // 05. +                 ( a b -- a+b ) a add b
void _mul  (); // 06. *                 ( a b -- a*b ) a multiply b
void _sub  (); // 07. -                 ( a b -- a-b ) a subtract b
void _div  (); // 08. /                 ( a b -- a/b ) a divide b
void _words(); // 09. words             (     --     ) show all forth word names
               //     words <substring> (     --     ) show all forth word names with given substring
void _see  (); // 10. see   <name>      (     --     ) see the word of given name
void _dump (); // 11. dump              ( a n --     ) show n cells at address a
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ wb32f10.h ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//#include "wb32f10.c"
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv wb32f10.c vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
boolean        EOL(char c){ return c=='\n'||c=='\r'                 ; } // check if c is end of line
boolean  baskSpace(char c){ return c=='\b'                          ; } // check if c is back space
boolean whiteSpace(char c){ return c==' '||c=='\t'||c=='\n'||c=='\r'; } // check if c is white space
/////////////////////////////////////////////////////////////////////////////////////////////////////////
char*tibBegin ; // terminal input buffer begin
char*tibEnd   ; // terminal input buffer end
char*tibLimit ; // terminal input buffer limit
char*tibRemain; // terminal input buffer remain for parsing
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void tibOpen  (char*t,int s){ tibBegin=tibEnd=t,tibLimit=t+s-1; } // init tib and wait for input
void tibClose ()            { *tibEnd='\0'                    ; } // add '\0' at end of input
void tibPop   ()            { --tibEnd                        ; } // pop last input character
void tibPush  (char c)      { *(tibEnd++)=c                   ; } // collect input character
/////////////////////////////////////////////////////////////////////////////////////////////////////////
boolean tibEmpty      (){ return tibEnd==tibBegin; } // check if buffer is empty
boolean tibFull       (){ return tibEnd==tibLimit; } // check if buffer is full
boolean parseAvailable(){ return *tibRemain      ; } // check if tib remain string to parse
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DS_DEPTH 32
int DS[DS_DEPTH]; // data stack
#define DS_LIMIT (DS+DS_DEPTH-1)        // data stack limit
int*DP=DS-1;      // data stack pointer
/////////////////////////////////////////////////////////////////////////////////////////////////////////
boolean dsHasItems (int n) { return DP>=DS+n-1       ; } // check if data stack has n items
boolean dsHasSpace (int n) { return DP+n<=DS+DS_DEPTH; } // check if data stack has space for n items
boolean dsFull     ()      { return DP>=DS+DS_DEPTH  ; } // check if data stack full
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int T;            // top of data stack
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void dsClear()      { DP=DS-1       ; } // reset data stack
void dsPush (int n) { T=n, *(++DP)=T; } // push a number onto data stack
int  dsPop  ()      { return *DP--  ; } // pop a number from data stack
int  dsDepth()      { return DP-DS+1; } // depth of data stack
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define  TMP_SIZE 256
char tmp[TMP_SIZE]; // used in parseToken() and toDigits()
#define  TMP_LIMIT (tmp+TMP_SIZE)
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void parseOpen(char*src){ tibRemain=src, tibEnd=src+strlen(src); } // reset for parsing
/////////////////////////////////////////////////////////////////////////////////////////////////////////
char*parseToken() { // use white spaces as delimiters to parse a token
  char c=*tibRemain++, *p, *q, *start, *limit; 
  while (c &&  whiteSpace(c)) c=*tibRemain++; start=tibRemain-1; // ignore leading white spaces
  while (c && !whiteSpace(c)) c=*tibRemain++; limit=tibRemain-1; // colect non     white spaces
  if(c){
    p=start, start=q=tmp; while(*p && p<limit)*(q++)=*(p++); *q=0; // make a zero ended string at tmp
  } else tibRemain=limit;
  return start;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
char toDigit(int x) { return x+=x<10?0x30:0x61-10; } // convert integer x into single digit
/////////////////////////////////////////////////////////////////////////////////////////////////////////
char*toDigits(uint x, int b){ // convert integer x into digits in given base b
  char*p=TMP_LIMIT; *(--p)='\0';
  if(x==0){ *(--p)='0'; return p; }
  boolean neg; if( neg=(b==10&&(int)x<0) ) x=-x;
  while(x){
    *(--p)=toDigit(x%b),x/=b; // convert current digit to character
  }
  if(neg) *(--p)='-';
  return p;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int B=10; // base for number conversion (at least 2, at most 36)
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void showDataStack(){ // show data stack info
  PRINTF("< dsDepth %d [ ",dsDepth()); // show depth
  if(dsDepth()>0){
    if(dsDepth()>5)PRINTF(".. "); // showing at most top 5 items
    for ( int *i=max(DP-4,DS); i<=DP; i++ ) PRINTF("%s ",toDigits(*i,B)); // show data
  }
  PRINTF("] base%d >\n",B); // show base 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
char* hexPrefix(char *s) { // 0xff 0XFF $ff $FF $Ff are all acceptable as hexadecimal numbers
  char c;
  if((c=*s++) != '0' && c != '$') return 0;
  if(c=='0' && (c=*s++) != 'x' && c != 'X') return 0;
  return s; // remain string
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
Voc *forth;
/////////////////////////////////////////////////////////////////////////////////////////////////////////
Word *vocSearch(Voc *voc, char *name) { // search name in dict
    Word *w=voc->last; while ( w && strcmp(w->name,name) ) w=w->prev; return w;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void setBase(int base ){ B=base; } // set number convertion base
int  getBase(         ){return B;} // get number convertion base
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv forth vocabulary vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
#define LAST NULL
//////////////////////////////////////////////// group 1 ////////////////////////////////////////////////
// extern setBase
//.......................................................................................................
void _bin(){setBase( 2)                  ;}const Word W_bin PROGMEM={LAST         ,"bin",_bin};
void _oct(){setBase( 8)                  ;}const Word W_oct PROGMEM={(Word*)&W_bin,"oct",_oct};
void _dec(){setBase(10)                  ;}const Word W_dec PROGMEM={(Word*)&W_oct,"dec",_dec};
void _hex(){setBase(16)                  ;}const Word W_hex PROGMEM={(Word*)&W_dec,"hex",_hex};
//.......................................................................................................
#define LAST (Word*)&W_hex
//////////////////////////////////////////////// group 2 ////////////////////////////////////////////////
// extern dsPush, dsPop
//.......................................................................................................
int  $0;
//.......................................................................................................
void _add(){dsPush(dsPop()+dsPop())      ;}const Word W_add PROGMEM={LAST         ,"+"  ,_add};
void _mul(){dsPush(dsPop()*dsPop())      ;}const Word W_mul PROGMEM={(Word*)&W_add,"*"  ,_mul};
void _sub(){$0=dsPop(),dsPush(dsPop()-$0);}const Word W_sub PROGMEM={(Word*)&W_mul,"-"  ,_sub};
void _div(){$0=dsPop(),dsPush(dsPop()/$0);}const Word W_div PROGMEM={(Word*)&W_sub,"/"  ,_div};
//.......................................................................................................
#define LAST (Word*)&W_div
//////////////////////////////////////////////// group 3 ////////////////////////////////////////////////
// extern dsPush, dsPop
//.......................................................................................................
#define CONSOLE_WIDTH 80
//.......................................................................................................
void words() { // show all word names having specific substring
  PRINTF("\n"); int m=0, n; Word *w=forth->last; char*sub=parseToken();
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
void see() { // show the forth word of given name
  Word *w = vocSearch(forth,parseToken());
  if(!w){ PRINTF(" ? undefinded "); return; }
  PRINTF("\n----------------------");
  PRINTF("\n%x prev %08x"            ,&w->prev,w->prev        );
  PRINTF("\n%x name %08x %s"         ,&w->name,w->name,w->name);
  PRINTF("\n%x code %08x"            ,&w->code,w->code        );
  PRINTF("\nforth primative word %s ", w->name                );
}
//.......................................................................................................
void dump() { // dump ( adr n -- ) // dump n cells at adr
// extern dsPop, PRINTF
    int n=dsPop(), *adr=(int*)dsPop(), *lmt=adr+n; char*badr,*blmt;
    for( ; adr<lmt; adr+=4) { 
      PRINTF("\n%8.8x : ", (int) adr);
      for(int i=0; i< 4; i++){
        if( adr+i>=lmt )PRINTF("         ");
        else            PRINTF("%8.8x ", *( adr+i));
        PRINTF(": ");
      }
      badr=(char*)adr, blmt=(char*)lmt;
      for(int i=0; i<16; i++){
        if(badr+i>=blmt)PRINTF("   ");
        else            PRINTF("%2.2x ", *(badr+i));
        PRINTF(": ");
      }
      for(int i=0; i<16; i++){
        if(badr+i>=blmt)PRINTF(" ");
        else {
          char c=*(badr+i); n=(int)c;
          if( n==0 ) c='.';
          else if( n<0x20 || (n>0x7e&&n<0xa4) || n>0xc6 ) c='_';
          else if(n>=0xa4&&n<=0xc6) { // head-byte of commmon big5
            n=(int)*(badr+i+1);
            if( n<0x40 || (n>0x7e&&n<0xa1) || n>0xfe) c='_'; // next is not tail-byte of commmon big5 
            else PRINTF("%c",c), c=(char)n, i++; // show head-byte of commmon big5 and get tail-byte
          }
          PRINTF("%c",c);
        }
      }
    }
}
//.......................................................................................................
void _words(){words();}const Word W_words PROGMEM={LAST           ,"words",_words};
void _see  (){see  ();}const Word W_see   PROGMEM={(Word*)&W_words,"see"  ,_see  };
void _dump (){dump ();}const Word W_dump  PROGMEM={(Word*)&W_see  ,"dump" ,_dump };
//.......................................................................................................
#define LAST (Word*)&W_dump
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ wb32f10.c ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv the main program vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
#define  TIB_SIZE 1024
char tib[TIB_SIZE]; // terminal input buffer
int iLine=0,nErr=0; // input line index and error count
//.......................................................................................................
void prompt(){
// extern showDataStack, PRINTF
  showDataStack();
  PRINTF("--------------------------------------------------\n");
  PRINTF("inp %02d : ",iLine++); // wait for an input string until caridge return
}
//.......................................................................................................
void eval(char *token){
// extern Word, forth, getBase, vocSearch, hexPrefix, PRINTF, dsPush
  char *remain, *p=token, c; Word* w=vocSearch(forth,token);
  if(w){ w->code(); return; }
  int b=getBase(); if(remain=hexPrefix(token)) p=remain, b=16; 
  int n=strtol(p, &remain, b); // conver string at p to integer n on base b (*remain is an illigal digit)
  if(c=*remain) PRINTF("\nerr %02d %s ? illigal '%c' at %d as base%d digit",++nErr,token,c,remain-token,b);
  else dsPush(n);
}
//.......................................................................................................
void interpret(char*src){
// extern parseOpen, parseAvailable, parseToken, eval, PRINTF
  char*token;
  parseOpen(src); while(parseAvailable() && (token=parseToken())) eval(token);
  PRINTF("\n");
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
// extern prompt, dsClear, forth, Voc, forth, LAST, tib, TIB_SIZE, tibOpen, PRINTF
  Serial.begin(115200), dsClear(), forth=(Voc*)malloc(sizeof(Voc)), forth->last=LAST;
  PRINTF("==================================================\n");
  PRINTF("Simple WifiBoy esp32 System of Only 11 Forth Words\n");
  PRINTF("20170508 derek@wifiboy.org & samsuanchen@gmail.com\n");
  PRINTF("==================================================\n");
  tibOpen(tib,TIB_SIZE),prompt(); // wait for input
}
//.......................................................................................................
void loop() {
// extern baskSpace, EOL, tibOpen, tibClose, tibEmpty, tibFull, tibPush, tibPop, PRINTF
  if (Serial.available()) { char c = Serial.read();     // get input char
    if (baskSpace(c)) {                               // if backspace ------------------------
      if ( !tibEmpty() ) tibPop(),PRINTF("\b \b");          // erase last input char
    } else if ( EOL(c) || tibFull() ) {               // if end of line or buffer full -------
      if ( tibFull() ) PRINTF("\n??? terminal input buffer full ???\n");
      tibClose(), interpret(tib);                           // interpret input string
      tibOpen(tib,TIB_SIZE),prompt();                       // wait for next input
    } else {                                          // else --------------------------------
      tibPush(c), PRINTF("%c",c);                           // collect and echo input char
    }
  }
}
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ the main program ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^