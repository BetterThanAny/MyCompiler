#pragma once
#include <memory>
#include <string>
#include <iostream>
#include <cassert>

static unsigned int tmp_symbol_num = 0; // 记录临时符号 %0, %1, %2, ...
enum class UnaryExpType
{
  primaryT,
  unaryT
};
enum class PrimaryExpType
{
  numberT,
  expT
};
enum class AddExpType
{
  mulT,
  addT
};
enum class MulExpType
{
  unaryT,
  mulT
};
enum class LOrExpType
{
  landT,
  lorT
};
enum class LAndExpType
{
  eqT,
  landT
};
enum class EqExpType
{
  relT,
  eqT
};

class BaseAST;
class CompUnitAST;
class FuncDefAST;
class FuncTypeAST;
class BlockAST;
class StmtAST;
class ExpAST;
class LOrExpAST;
class LAndExpAST;
class EqExpAST;
class RelExpAST;
class AddExpAST;
class MulExpAST;
class UnaryExpAST;
class PrimaryExpAST;

// 所有 AST 的基类
class BaseAST
{
public:
  virtual ~BaseAST() = default;

  virtual void Dump() const = 0;
  virtual std::string DumpIR() const = 0;// 输出koopa IR
};

// CompUnit ::= FuncDef
class CompUnitAST : public BaseAST
{
public:
  // 用智能指针管理对象
  std::unique_ptr<BaseAST> func_def;

  void Dump() const override
  {
    std::cout << "CompUnitAST {";
    func_def->Dump();
    std::cout << "}";
  }
  std::string DumpIR() const override
  {
    return func_def->DumpIR();
  }
};

// FuncDef ::= FuncType IDENT "(" ")" Block
class FuncDefAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  void Dump() const override
  {
    std::cout << "FuncDefAST {";
    func_type->Dump();
    std::cout << ", " << ident << ", ";
    block->Dump();
    std::cout << "}";
  }

  std::string DumpIR() const override
  {
    if (ident != "main")
    {
      std::cerr << "Error: only main function is supported" << std::endl;
      exit(1);
    }
    std::cout << "fun @" << ident << "(): ";
    func_type->DumpIR();
    std::cout << "{" << std::endl;
    block->DumpIR();
    std::cout << "}" << std::endl;
    return "";
  }
};

// FuncType ::= "int"
class FuncTypeAST : public BaseAST
{
public:
  std::string funcT_name;

  void Dump() const override
  {
    std::cout << "FuncTypeAST {";
    std::cout << funcT_name;
    std::cout << "}";
  }
  std::string DumpIR() const override
  {
    if (funcT_name == "int")
      std::cout << "i32" << " ";
    else
      std::cout << "Not allowed" << std::endl;
    return "";
  }
};

// Block ::= "{" Stmt "}"
class BlockAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> stmt;

  void Dump() const override
  {
    std::cout << "BlockAST {";
    stmt->Dump();
    std::cout << "}";
  }
  std::string DumpIR() const override
  {
    std::cout << "%entry:" << std::endl; // %e 会变蓝，不行的话分开输出
    return stmt->DumpIR();
  }
};

// Stmt ::= "return" Exp ";"
class StmtAST : public BaseAST
{
public:
  int ret_number;
  std::unique_ptr<BaseAST> exp;
  void Dump() const override
  {
    std::cout << "StmtAST {" << std::endl
              << "return ";
    exp->Dump();
    std::cout << "; }";
  }
  std::string DumpIR() const override
  {
    std::string ret_value = exp->DumpIR();
    std::cout << "  ret " << ret_value << std::endl;
    // ret_value 是数字或者临时变量
    return "";
  }
};

// Exp ::= LOrExp;
class ExpAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> lor_exp;

  void Dump() const override
  {
    std::cout << "ExpAST {";
    lor_exp->Dump();
    std::cout << "}";
  }
  std::string DumpIR() const override
  {
    return lor_exp->DumpIR();
  }
};

// LOrExp ::= LAndExp | LOrExp "||" LAndExp
class LOrExpAST : public BaseAST
{
public:
  LOrExpType type; // { landT, lorT }
  std::unique_ptr<BaseAST> lor_exp;
  std::unique_ptr<BaseAST> land_exp;
  void Dump() const override
  {
    std::cout << "LOrExpAST {";
    if (type == LOrExpType::lorT)
    {
      lor_exp->Dump();
      std::cout << "||";
    }
    else
    {

      land_exp->Dump();
    }
    std::cout << "}";
  }
  std::string DumpIR() const override
  {
    if (type == LOrExpType::landT)
    {
      return land_exp->DumpIR();
    }
    std::string ret_value = lor_exp->DumpIR();
    std::string ret_value2 = land_exp->DumpIR();
    // 当前的临时变量 tmp_symbol
    std::string tmp_symbol = "%" + std::to_string(tmp_symbol_num);
    std::cout << "  " << tmp_symbol << " = or " << ret_value << ", " << ret_value2 << std::endl;
    tmp_symbol_num++;
    return tmp_symbol;
  }
};

// LAndExp ::= EqExp | LAndExp "&&" EqExp
class LAndExpAST : public BaseAST
{
public:
  LAndExpType type; // { eqT, landT }
  std::unique_ptr<BaseAST> land_exp;
  std::unique_ptr<BaseAST> eq_exp;
  void Dump() const override
  {
    std::cout << "LAndExpAST {";
    if (type == LAndExpType::landT)
    {
      land_exp->Dump();
      std::cout << "&&";
    }
    eq_exp->Dump();

    std::cout << "}";
  }
  std::string DumpIR() const override
  {
    if (type == LAndExpType::eqT)
    {
      return eq_exp->DumpIR();
    }
    std::string ret_value = land_exp->DumpIR();
    std::string ret_value2 = eq_exp->DumpIR();
    // 当前的临时变量 tmp_symbol
    std::string tmp_symbol = "%" + std::to_string(tmp_symbol_num);
    std::cout << "  " << tmp_symbol << " = and " << ret_value << ", " << ret_value2 << std::endl;
    tmp_symbol_num++;
    return tmp_symbol;
  }
};

// EqExp ::= RelExp | EqExp "==" RelExp | EqExp "!=" RelExp
class EqExpAST : public BaseAST
{
public:
  EqExpType type; // { relT, eqT }
  std::unique_ptr<BaseAST> eq_exp;
  std::unique_ptr<BaseAST> rel_exp;
  std::string op;
  void Dump() const override
  {
    std::cout << "EqExpAST {";
    if (type == EqExpType::eqT)
    {
      eq_exp->Dump();
      std::cout << op;
    }
    else
    {
      rel_exp->Dump();
    }
    std::cout << "}";
  }
  std::string DumpIR() const override
  {
    if (type == EqExpType::relT)
    {
      return rel_exp->DumpIR();
    }
    std::string ret_value = eq_exp->DumpIR();
    std::string ret_value2 = rel_exp->DumpIR();
    // 当前的临时变量 tmp_symbol
    std::string tmp_symbol = "%" + std::to_string(tmp_symbol_num);
    if (op == "==")
    {
      std::cout << "  " << tmp_symbol << " = eq " << ret_value << ", " << ret_value2 << std::endl;
      tmp_symbol_num++;
      return tmp_symbol;
    }
    else if (op == "!=")
    {
      std::cout << "  " << tmp_symbol << " = ne " << ret_value << ", " << ret_value2 << std::endl;
      tmp_symbol_num++;
      return tmp_symbol;
    }
    else
    {
      assert(false);
    }
  }
};

// RelExp ::= AddExp | RelExp "<" AddExp | RelExp ">" AddExp | RelExp "<=" AddExp | RelExp ">=" AddExp
class RelExpAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> rel_exp;
  std::unique_ptr<BaseAST> add_exp;
  std::string op;
  void Dump() const override
  {
    std::cout << "RelExpAST {";
    rel_exp->Dump();
    std::cout << op;
    add_exp->Dump();
    std::cout << "}";
  }
  std::string DumpIR() const override
  {
    std::string ret_value = rel_exp->DumpIR();
    std::string ret_value2 = add_exp->DumpIR();
    // 当前的临时变量 tmp_symbol
    std::string tmp_symbol = "%" + std::to_string(tmp_symbol_num);
    if (op == "<")
    {
      std::cout << "  " << tmp_symbol << " = slt " << ret_value << ", " << ret_value2 << std::endl;
      tmp_symbol_num++;
      return tmp_symbol;
    }
    else if (op == ">")
    {
      std::cout << "  " << tmp_symbol << " = sgt " << ret_value << ", " << ret_value2 << std::endl;
      tmp_symbol_num++;
      return tmp_symbol;
    }
    else if (op == "<=")
    {
      std::cout << "  " << tmp_symbol << " = sle " << ret_value << ", " << ret_value2 << std::endl;
      tmp_symbol_num++;
      return tmp_symbol;
    }
    else if (op == ">=")
    {
      std::cout << "  " << tmp_symbol << " = sge " << ret_value << ", " << ret_value2 << std::endl;
      tmp_symbol_num++;
      return tmp_symbol;
    }
    else
    {
      assert(false);
    }
  }
};

// AddExp ::= MulExp | AddExp "+" MulExp | AddExp "-" MulExp
class AddExpAST : public BaseAST
{
public:
  AddExpType type; // { mulT, addT }
  std::unique_ptr<BaseAST> add_exp;
  std::unique_ptr<BaseAST> mul_exp;
  std::string op;
  void Dump() const override
  {
    std::cout << "AddExpAST {";
    if (type == AddExpType::mulT)
    {
      add_exp->Dump();
      std::cout << op;
    }
    else
    {
      mul_exp->Dump();
    }
    std::cout << "}";
  }
  std::string DumpIR() const override
  {
    if (type == AddExpType::mulT)
    {
      return add_exp->DumpIR();
    }
    std::string ret_value = add_exp->DumpIR();
    std::string ret_value2 = mul_exp->DumpIR();
    // 当前的临时变量 tmp_symbol
    std::string tmp_symbol = "%" + std::to_string(tmp_symbol_num);
    if (op == "+")
    {
      std::cout << "  " << tmp_symbol << " = add " << ret_value << ", " << ret_value2 << std::endl;
      tmp_symbol_num++;
      return tmp_symbol;
    }
    else if (op == "-")
    {
      std::cout << "  " << tmp_symbol << " = sub " << ret_value << ", " << ret_value2 << std::endl;
      tmp_symbol_num++;
      return tmp_symbol;
    }
    else
    {
      assert(false);
    }
  }
};

// MulExp ::= UnaryExp | MulExp "*" UnaryExp | MulExp "/" UnaryExp | MulExp "%" UnaryExp
class MulExpAST : public BaseAST
{
public:
  MulExpType type; // { unaryT, mulT }
  std::unique_ptr<BaseAST> mul_exp;
  std::unique_ptr<BaseAST> unary_exp;
  std::string op;
  void Dump() const override
  {
    std::cout << "MulExpAST {";
    if (type == MulExpType::mulT)
    {
      mul_exp->Dump();
      std::cout << op;
    }
    else
    {
      unary_exp->Dump();
    }
    std::cout << "}";
  }
  std::string DumpIR() const override
  {
    if (type == MulExpType::unaryT)
    {
      return unary_exp->DumpIR();
    }
    std::string ret_value = mul_exp->DumpIR();
    std::string ret_value2 = unary_exp->DumpIR();
    // 当前的临时变量 tmp_symbol
    std::string tmp_symbol = "%" + std::to_string(tmp_symbol_num);
    if (op == "*")
    {
      std::cout << "  " << tmp_symbol << " = mul " << ret_value << ", " << ret_value2 << std::endl;
      tmp_symbol_num++;
      return tmp_symbol;
    }
    else if (op == "/")
    {
      std::cout << "  " << tmp_symbol << " = div " << ret_value << ", " << ret_value2 << std::endl;
      tmp_symbol_num++;
      return tmp_symbol;
    }
    else if (op == "%")
    {
      std::cout << "  " << tmp_symbol << " = mod " << ret_value << ", " << ret_value2 << std::endl;
      tmp_symbol_num++;
      return tmp_symbol;
    }
    else
    {
      assert(false);
    }
  }
};

// UnaryExp ::= PrimaryExp | UnaryOp UnaryExp
class UnaryExpAST : public BaseAST
{
public:
  UnaryExpType type; // { primaryT, unaryT }
  std::string op;
  std::unique_ptr<BaseAST> exp;
  void Dump() const override
  {
    if (type == UnaryExpType::unaryT)
    {
      std::cout << op;
      ;
    }
    exp->Dump();
  }
  std::string DumpIR() const override
  {
    if (type == UnaryExpType::unaryT)
    {
      std::string ret_value = exp->DumpIR();
      // 当前的临时变量 tmp_symbol
      std::string tmp_symbol = "%" + std::to_string(tmp_symbol_num);
      if (op == "-")
      {
        std::cout << "  " << tmp_symbol << " = sub 0, " << ret_value << std::endl;
        tmp_symbol_num++;
        return tmp_symbol;
      }
      else if (op == "!")
      {
        std::cout << "  " << tmp_symbol << " = eq " << ret_value << ", 0" << std::endl;
        tmp_symbol_num++;
        return tmp_symbol;
      }
      else if (op == "+")
      {
        return ret_value;
      }
      else
      {
        assert(false);
      }
    }
    else if (type == UnaryExpType::primaryT)
    {
      return exp->DumpIR();
    }
    else
    {
      assert(false);
    }
  }
};

class PrimaryExpAST : public BaseAST
{
public:
  PrimaryExpType type; //{ numberT, expT }
  std::unique_ptr<BaseAST> exp;
  int number;
  void Dump() const override
  {
    if (type == PrimaryExpType::expT)
    {
      exp->Dump();
    }
    else if (type == PrimaryExpType::numberT)
    {
      std::cout << number;
    }
    else
    {
      assert(false);
    }
  }
  std::string DumpIR() const override
  {
    std::string ret_value = "";
    if (type == PrimaryExpType::expT)
    {
      ret_value = exp->DumpIR();
    }
    else if (type == PrimaryExpType::numberT)
    {
      ret_value = std::to_string(number);
    }
    return ret_value;
  }
};
