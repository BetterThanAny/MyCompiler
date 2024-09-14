#pragma once
#include <memory>
#include <string>
#include <iostream>
#include <cassert>

static unsigned int tmp_symbol_num = 0; // 记录临时符号 %0, %1, %2, ...
enum class UnaryExpType { primaryT, unaryT };
enum class PrimaryExpType { numberT, expT };

// 所有 AST 的基类
class BaseAST
{
public:
  virtual ~BaseAST() = default;
     
  virtual void Dump() const = 0;
  virtual std::string DumpIR() const = 0;
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
    if(funcT_name == "int") std::cout << "i32" << " ";
    else std::cout << "Not allowed" << std::endl;
    return "";
  }
};

//Block ::= "{" Stmt "}"
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
    std::cout << "StmtAST {" << std::endl << "return ";
    exp->Dump();
    std::cout << "; }";
  }
  std::string DumpIR() const override
  {
    std::string ret_value = exp->DumpIR();
    std::cout << "ret " << ret_value << std::endl;
    // ret_value 是数字或者临时变量
  }
};

// Exp ::= UnaryExp;
class ExpAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> unary_exp;

  void Dump() const override
  {
    std::cout << "ExpAST {";
    unary_exp->Dump();
    std::cout << "}";
  }
  std::string DumpIR() const override
  {
    return unary_exp->DumpIR();
  }
};

// UnaryExp ::= PrimaryExp | UnaryOp UnaryExp 
class UnaryExpAST : public BaseAST
{ 
public:
  UnaryExpType type;// { primaryT, unaryT }
  std::string op;
  std::unique_ptr<BaseAST> exp;
  void Dump() const override
  {
    if (type == UnaryExpType::unaryT)
    {
      std::cout << op;;
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
        std::cout << tmp_symbol << " = sub 0, " << ret_value << std::endl;
        tmp_symbol_num++;
        return tmp_symbol;
      }
      else if (op == "!")
      {
        std::cout << tmp_symbol << " = eq " << ret_value <<", 0" << std::endl;
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
    else if(type == UnaryExpType::primaryT)
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
  PrimaryExpType type ;//{ numberT, expT }
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
  
