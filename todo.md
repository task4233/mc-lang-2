# implement ule and uge
 - [ ] add ult and ugt
 - [ ] update BinOpPrecedence std::map<char,int> -> std::map<int, int > in parser.h
 - [ ] add enum in mc.cpp
 - [ ] add checking whether Curtok is '=' before calling ParsePrimary() in ParseBinOpRHS()
   - [ ] if CurTok is '=', call getNextToken() and ParsePrimary()