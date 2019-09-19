# implement ule and uge
 - [x] add ult and ugt
 - [x] update BinOpPrecedence std::map<char,int> -> std::map<int, int > in parser.h
 - [x] add enum in mc.cpp
 - [x] add checking whether Curtok is '=' before calling ParsePrimary() in ParseBinOpRHS() in parser.h
   - [x] if CurTok is '=', call getNextToken() and ParsePrimary()