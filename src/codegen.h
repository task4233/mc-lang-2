//===----------------------------------------------------------------------===//
// Code Generation
// このファイルでは、parser.hによって出来たASTからLLVM IRを生成しています。
// といっても、難しいことをしているわけではなく、IRBuilder(https://llvm.org/doxygen/IRBuilder_8h_source.html)
// のインターフェースを利用して、parser.hで定義した「ソースコードの意味」をIRに落としています。
// 各ファイルの中で一番LLVMの機能を多用しているファイルです。
//===----------------------------------------------------------------------===//

// https://llvm.org/doxygen/LLVMContext_8h_source.html
static LLVMContext Context;
// https://llvm.org/doxygen/classllvm_1_1IRBuilder.html
// LLVM IRを生成するためのインターフェース
static IRBuilder<> Builder(Context);
// https://llvm.org/doxygen/classllvm_1_1Module.html
// このModuleはC++ Moduleとは何の関係もなく、LLVM IRを格納するトップレベルオブジェクトです。
static std::unique_ptr<Module> myModule;
// 変数名とllvm::Valueのマップを保持する
static std::map<std::string, Value *> NamedValues;

// https://llvm.org/doxygen/classllvm_1_1Value.html
// llvm::Valueという、LLVM IRのオブジェクトでありFunctionやModuleなどを構成するクラスを使います
Value *NumberAST::codegen() {
  // 64bit整数型のValueを返す
  return ConstantInt::get(Context, APInt(64, Val, true));
}

Value *LogErrorV(const char *str) {
  LogError(str);
  return nullptr;
}

// TODO 2.4: 引数のcodegenを実装してみよう
Value *VariableExprAST::codegen() {
  // NamedValuesの中にVariableExprAST::NameとマッチするValueがあるかチェックし、
  // あったらそのValueを返す。
  // namedValue
  // static std::map<std::string, Value *> NamedValues;
  if (NamedValues.count(VariableExprAST::variableName)) {
    return NamedValues[VariableExprAST::variableName];
  }
  return nullptr;
}

// TODO 2.5: 関数呼び出しのcodegenを実装してみよう
Value *CallExprAST::codegen() {
  // 1. myModule->getFunctionを用いてcalleeがdefineされているかを
  // チェックし、されていればそのポインタを得る。
  
  // std::unique_ptr <Module> myModule
  // ref:
  // https://llvm.org/doxygen/classllvm_1_1Module.html
  // Function * Module::getFunction(StringRef Name) const
  // look up the specified function in the module symbol table
  auto calleeFuncPtr = myModule->getFunction(callee);
  if (calleeFuncPtr == nullptr) {
    return LogErrorV("NullPtr in CallExprAST codegen\n");
  }
  // 2. llvm::Function::arg_sizeと実際に渡されたargsのサイズを比べ、
  // サイズが間違っていたらエラーを出力。
  
  if (calleeFuncPtr->arg_size() != args.size()) {
    std::string res = "Wrong args size: expected:" + std::to_string(calleeFuncPtr->arg_size()) + ", out:" + std::to_string(args.size());
    return LogErrorV(res.c_str());
  }

  std::vector<Value *> argsV;
  // 3. argsをそれぞれcodegenしllvm::Valueにし、argsVにpush_backする。
  for (auto&& arg: args) {
    Value* argV = arg->codegen();
    if (argV == nullptr) {
      return LogErrorV("codegen is failed(CallExprAST).");
    }
    argsV.emplace_back(arg->codegen());
  }
  // 4. IRBuilderのCreateCallを呼び出し、Valueをreturnする。
  return Builder.CreateCall(calleeFuncPtr, argsV, "calltmp");
}

Value *BinaryAST::codegen() {

  if (Op == '=') {
    // if target has no child, it will be returned nullptr after dynamic_cast
     
    // LHSE = LHS Expression
    VariableExprAST* LHSE = dynamic_cast< VariableExprAST* >(LHS.get());
    if (LHSE == nullptr) {
      return LogErrorV("destination of '=' must be a variable.");
    }
    Value* val = RHS->codegen();
    if (val == nullptr) {
      return nullptr;
    }
    
    // static std::map<std::string, Value *> NamedValues;
    Value* variable = nullptr;
    if (NamedValues.count(LHSE->getName())) {

      variable = NamedValues[LHSE->getName()];
    }
    if (variable == nullptr) {
      return LogErrorV((LHSE->getName() + "does not exist.").c_str());
    }

    // store val <- variable
    Builder.CreateStore(val, variable);
    return val;
  }

  
  // 二項演算子の両方の引数をllvm::Valueにする。
  Value *L = LHS->codegen();
  Value *R = RHS->codegen();
  if (!L || !R)
    return nullptr;
  
  switch (Op) {
  case '+':
    // LLVM IR Builerを使い、この二項演算のIRを作る
    return Builder.CreateAdd(L, R, "addtmp");
    // TODO 1.7: '-'と'*'に対してIRを作ってみよう
    // 上の行とhttps://llvm.org/doxygen/classllvm_1_1IRBuilder.htmlを参考のこと
  case '-':
    return Builder.CreateSub(L, R, "subtmp");
  case '*':
    return Builder.CreateMul(L, R, "multmp");
  case '/':
    // ref
    // https://llvm.org/doxygen/Value_8h_source.html#l00245
    return Builder.CreateUDiv(L, R, "divtmp");
  case ge:
    return Builder.CreateICmpUGE(L, R, "ugetmp");
    /*
    return Builder.CreateIntCast(
				 Builder.CreateICmpUGE(L, R, "ugetmp"),
				 Type::getInt64Ty(Context),
				 true,
				 "cast_i1_to_i64");
    */
  case le:
    return Builder.CreateICmpULE(L, R, "uletmp");
    /*
    return Builder.CreateIntCast(
				 Builder.CreateICmpULE(L, R, "uletmp"),
				 Type::getInt64Ty(Context),
				 true,
				 "cast_i1_to_i64");
    */
  case '>':
    return Builder.CreateICmpUGT(L, R, "ugttmp");
    /*
    return Builder.CreateIntCast(
				 Builder.CreateICmpUGT(L, R, "ugttmp"),
				 Type::getInt64Ty(Context),
				 true,
				 "cast_i1_to_i64");
    */
  case '<':
    return Builder.CreateICmpULT(L, R, "ugttmp");
    /*
    return Builder.CreateIntCast(
				 Builder.CreateICmpULT(L, R, "ugttmp"),
				 Type::getInt64Ty(Context),
				 true,
				 "cast_i1_to_i64");
    */
  default:
    return LogErrorV("invalid binary operator");
  }
}

Function *PrototypeAST::codegen() {
  // MC言語では変数の型も関数の返り値もintの為、関数の返り値をInt64にする。
  std::vector<Type *> prototype(args.size(), Type::getInt64Ty(Context));
  FunctionType *FT =
    FunctionType::get(Type::getInt64Ty(Context), prototype, false);
  // https://llvm.org/doxygen/classllvm_1_1Function.html
  // llvm::Functionは関数のIRを表現するクラス
  Function *F =
    Function::Create(FT, Function::ExternalLinkage, Name, myModule.get());

  // 引数の名前を付ける
  unsigned i = 0;
  for (auto &Arg : F->args())
    Arg.setName(args[i++]);

  return F;
}

Function *FunctionAST::codegen() {
  // この関数が既にModuleに登録されているか確認
  Function *function = myModule->getFunction(proto->getFunctionName());
  // 関数名が見つからなかったら、新しくこの関数のIRクラスを作る。
  if (!function)
    function = proto->codegen();
  if (!function)
    return nullptr;

  // エントリーポイントを作る
  BasicBlock *BB = BasicBlock::Create(Context, "entry", function);
  Builder.SetInsertPoint(BB);

  // Record the function arguments in the NamedValues map.
  NamedValues.clear();
  for (auto &Arg : function->args())
    NamedValues[Arg.getName()] = &Arg;

  // 関数のbody(ExprASTから継承されたNumberASTかBinaryAST)をcodegenする
  if (Value *RetVal = body->codegen()) {
    // returnのIRを作る
    Builder.CreateRet(RetVal);

    // https://llvm.org/doxygen/Verifier_8h.html
    // 関数の検証
    verifyFunction(*function);

    return function;
  }

  // もし関数のbodyがnullptrなら、この関数をModuleから消す。
  function->eraseFromParent();
  return nullptr;
}

//===----------------------------------------------------------------------===//
// MC コンパイラエントリーポイント
// mc.cppでMainLoop()が呼ばれます。MainLoopは各top level expressionに対して
// HandleTopLevelExpressionを呼び、その中でASTを作り再帰的にcodegenをしています。
//===----------------------------------------------------------------------===//

static std::string streamstr;
static llvm::raw_string_ostream stream(streamstr);
static void HandleDefinition() {
  if (auto FnAST = ParseDefinition()) {
    if (auto *FnIR = FnAST->codegen()) {
      FnIR->print(stream);
    }
  } else {
    getNextToken();
  }
}

// その名の通りtop level expressionをcodegenします。例えば、「2+1;3+3;」というファイルが
// 入力だった場合、この関数は最初の「2+1」をcodegenして返ります。(そしてMainLoopからまだ呼び出されます)
static void HandleTopLevelExpression() {
  // ここでテキストファイルを全てASTにします。
  if (auto FnAST = ParseTopLevelExpr()) {
    // できたASTをcodegenします。
    if (auto *FnIR = FnAST->codegen()) {
      streamstr = "";
      FnIR->print(stream);
    }
  } else {
    // エラー
    getNextToken();
  }
}

static void MainLoop() {
  myModule = llvm::make_unique<Module>("my cool jit", Context);
  while (true) {
    switch (CurTok) {
    case tok_eof:
      // ここで最終的なLLVM IRをプリントしています。
      fprintf(stderr, "%s", stream.str().c_str());
      return;
    case tok_def:
      HandleDefinition();
      break;
    case ';': // ';'で始まった場合、無視します
      getNextToken();
      break;
    default:
      HandleTopLevelExpression();
      break;
    }
  }
}
