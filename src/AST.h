#pragma once
#include <memory>
#include <string>
#include <iostream>

// 所有 AST 的基类
class BaseAST
{
public:
  virtual ~BaseAST() = default;

  virtual void Dump() const = 0;
  virtual void DumpIR() const = 0;
};

// CompUnit 是 BaseAST
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
  void DumpIR() const override
  {
    func_def->DumpIR();
  }
};

// FuncDef 也是 BaseAST
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
  void DumpIR() const override
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
  }
};

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
  void DumpIR() const override
  {
    std::cout << "i32" << " ";
  }
};

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
  void DumpIR() const override
  {
    std::cout << "%entry:" << std::endl; // %e 会变蓝，不行的话分开输出
    std::cout << "ret ";
    stmt->DumpIR();
  }
};

class StmtAST : public BaseAST
{
public:
  std::unique_ptr<int> number;

  void Dump() const override
  {
    std::cout << "StmtAST {";
    std::cout << *number;
    std::cout << "}";
  }
  void DumpIR() const override
  {
    std::cout << *number << std::endl;
  }
};
