#ifndef _WB32V10_H_
#define _WB32V10_H_
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv wb32v10.c vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
#include "Arduino.h"

#define PRINTF		Serial.printf
#define AVAILABLE	Serial.available
#define READ		Serial.read
#define WRITE		Serial.write

#define TIB_SIZE 	1024
#define TMP_SIZE	256
#define DS_DEPTH	32
#define DS_LIMIT	(DS+DS_DEPTH-1) // data stack limit
#define LAST NULL

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
class WB32V10;  
class WB32V10 {
  public:
    WB32V10 () {}
    virtual ~WB32V10 () {}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
    void	tibOpen (); // begin to collect input characters
	void	tibClose (); // add '\0' as end of input string
	void	tibPop (); // pop last input character
	void	tibPush (char c); // collect input character
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	boolean EOL (char c); // check if c is end of line
	boolean backSpace (char c); // check if c is back space
	boolean whiteSpace (char c); // check if c is white space
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	boolean tibEmpty (); // check if buffer is empty
	boolean tibFull (); // check if buffer is full
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	void	waitInput();
	char*	readLine (); // read an input line
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	void	parseBegin (char* str); // begin to parse
	char	parseAvailable (); // non '\0' means available to parse
	char*	parseToken (); // parse a token
	char*	hexPrefix (char *s);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	void	interpret(char* line);
	void	eval(char *token);
	void	showDataStack();
    void	dsClear(); // reset data stack
    void	dsPush(int n); // push a number onto data stack
    int		dsPop(); // pop a number from data stack
    int		dsDepth(); // depth of data stack
    boolean dsHasItems (int n);
    boolean dsHasSpace (int n); // check if data stack has space for n items
    boolean dsFull();
    char	toDigit(int x); // convert integer x into single digit
    char*	toDigits(uint x, int b);
    void	setBase(int base); // set number convertion base
    int		getBase(); // get number convertion base
    Voc*    getVoc();
    Word*	vocSearch(char *name);
    void	words(char *sub); // show all word names having specific substring
    void 	see(Word *w); // show the forth word w
    void	dump(int *a, int n); // dump ( adr n -- ) // dump n cells at adr
    void	vocInit (Word* last); // initialize the vocabulary of forth words
  private:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
    char	tib[TIB_SIZE];
    char	tmp[TMP_SIZE]; // used in parseToken() and toDigits()
    int		DS[DS_DEPTH]; // data stack
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	char*	tibBegin ; // terminal input buffer begin
	char*	tibEnd   ; // terminal input buffer end
	char*	tibLimit ; // terminal input buffer limit
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
    char*	parseRemain; // terminal input buffer remain for parsing
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
    int*	DP;  // data stack pointer
    int		T; // top of data stack
    int		B=10; // base for number conversion (at least 2, at most 36)
    Voc*	voc;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
};
#endif _WB32V10_H