# mc-lang-2
## 2-1
 - update files
 - done 2-1

## 2-2
 - hoge, fuga: identifier
 - hoge(fuga): function
 - `Lexer lexer` has been declared in mc.c
 - To make unique_ptr of new instance, use `llvm::make_unique< Hoge >(fuga)`
 - unique_ptr is smart pointer, which has a ownership to access and release the memory.

## 2-3
 - use getIdentifier to get identifier

## 2-4
 - VariableExprAST::variableName is static
 - static std::map<std::string, llvm::Value* > NamedValues
 - namedValue()
   - first: variable name(std::string)
   - second: value(llvm::Value)

## 2-5
 - fixed bugs in 2-2
   - it's caused `break;` before `emplace_back`

## 2-6
 - write the code in ./src/main.cppto call the function made by mc-lang

## implement-udiv
 - add '/' to codegen.h & mc.cpp

## implement ult and ugt
 - add Builder.CreateICmpUGT(L, R, "ugttmp");
 - add Builder.CreateICmpULT(L, R, "ulttmp");
 - update BinopPrecedence
 
## implement cast
 - add Builder.CreateIntCast<L, Type::getInt64Ty(Context), true, "cast_i1_to_i64") to cast

## implement ule and uge
 - update the difinition of BinopPrecedence
   - std::map< char, int > ->
### checklist
 - [x] add ult and ugt
 - [x] update BinOpPrecedence std::map<char,int> -> std::map<int, int > in parser.h
 - [x] add enum in mc.cpp
 - [x] add checking whether Curtok is '=' before calling ParsePrimary() in ParseBinOpRHS() in parser.h
   - [x] if CurTok is '=', call getNextToken() and ParsePrimary()
 - [x] add IR order in codegen.h
 - [x] check it's implemented correctly