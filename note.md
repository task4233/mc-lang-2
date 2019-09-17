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
 