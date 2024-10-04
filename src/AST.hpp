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
    std::cout << "%entry:" << std::endl; // %e 会变蓝，\% 会变红，什么鬼？
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
    std::cout << "\tret " << ret_value << std::endl;
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
  std::string op;
  std::unique_ptr<BaseAST> lor_exp;
  std::unique_ptr<BaseAST> land_exp;
  void Dump() const override
  {
    std::cout << "LOrExpAST {";
    if (op == "")
      land_exp->Dump();
    else
    {
      lor_exp->Dump();
      std::cout << op;
      land_exp->Dump();
    }
    std::cout << "}";
  }
  std::string DumpIR() const override
  {
    std::string result_var = "";
    if (op == "")
      return land_exp->DumpIR();
    assert(op == "||");
    std::string lorexp = lor_exp->DumpIR();
    std::string landexp = land_exp->DumpIR();
    // TODO: Handle And operation (A && B is considered (A!=0) && (B!=0) here).
    std::cout << "\t%" << tmp_symbol_num++ << " = eq " << lorexp << ", 0\n";
    std::cout << "\t%" << tmp_symbol_num++ << " = eq " << landexp << ", 0\n";
    std::cout << "\t%" << tmp_symbol_num << " = or %" << (tmp_symbol_num - 2) << ", %"
              << (tmp_symbol_num - 1) << "\n";
    return "%" + std::to_string(tmp_symbol_num++);
  }
};

// LAndExp ::= EqExp | LAndExp "&&" EqExp
class LAndExpAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> land_exp;
  std::unique_ptr<BaseAST> eq_exp;
  std::string op;
  void Dump() const override
  {
    std::cout << "LAndExpAST {";
    if (op == "")
    {
      // LAndExp := EqExp
      eq_exp->Dump();
    }
    else
    {
      // LAndExp := LAndExp LANDOP EqExp
      land_exp->Dump();
      std::cout << op;
      eq_exp->Dump();
    }
    std::cout << "}";
  }
  std::string DumpIR() const override
  {
    
   if (op == ""){ 
            // LAndExp := EqExp
            return eq_exp->DumpIR();
        }

        // LAndExp := LAndExp LANDOP EqExp
        assert(op == "&&");
        std::string landexp = land_exp->DumpIR();
        std::string eqexp = eq_exp->DumpIR();
        // TODO: Handle And operation (A && B is considered (A!=0) && (B!=0) here).
        std::cout << "\t%" << tmp_symbol_num++ << " = ne " << landexp << ", 0\n";
        std::cout << "\t%" << tmp_symbol_num++ << " = ne " << eqexp << ", 0\n";
        std::cout << "\t%" << tmp_symbol_num << " = and %" << (tmp_symbol_num - 2) << ", %" 
                    << (tmp_symbol_num - 1) << "\n";
        return "%" + std::to_string(tmp_symbol_num++);
  }
};

// EqExp ::= RelExp | EqExp "==" RelExp | EqExp "!=" RelExp
class EqExpAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> eq_exp;
  std::unique_ptr<BaseAST> rel_exp;
  std::string op;
  void Dump() const override
  {
    std::cout << "EqExpAST {";
    if (op == "")
    {
      // EqExp := RelExp
      rel_exp->Dump();
    }
    else
    {
      // EqExp := EqExp EQOP RelExp
      eq_exp->Dump();
      std::cout << op;
      rel_exp->Dump();
    }
    std::cout << "}";
  }
  std::string DumpIR() const override
  {
    if (op == "")
    {
      // EqExp := RelExp
      return rel_exp->DumpIR();
    }
    else
    {
      // EqExp := EqExp EQOP RelExp
      std::string eqexp = eq_exp->DumpIR();
      std::string relexp = rel_exp->DumpIR();
      if (op == "==")
      {
        std::cout << "\t%" << tmp_symbol_num << " = eq " << eqexp << ", "
                  << relexp << "\n";
      }
      else if (op == "!=")
      {
        std::cout << "\t%" << tmp_symbol_num << " = ne " << eqexp << ", "
                  << relexp << "\n";
      }
      else
        assert(false);
      return "%" + std::to_string(tmp_symbol_num++);
    }
    return "";
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
    if (op == "")
    {
      // RelExp := AddExp
      add_exp->Dump();
    }
    else
    {
      // RelExp := RelExp RELOP AddExp
      rel_exp->Dump();
      std::cout << op;
      add_exp->Dump();
    }
    std::cout << "}";
  }
  std::string DumpIR() const override
  {
    if (op == ""){ 
            // RelExp := AddExp
            return add_exp->DumpIR();
        }
        else{
            // RelExp := RelExp RELOP AddExp
            std::string relexp = rel_exp->DumpIR();
            std::string addexp = add_exp->DumpIR();
            if (op == "<") {
                std::cout << "\t%" << tmp_symbol_num << " = lt " << relexp << ", " 
                          << addexp << "\n";
            }
            else if (op == ">"){
                std::cout << "\t%" << tmp_symbol_num << " = gt " << relexp << ", "
                          << addexp << "\n";
            }
            else if (op == "<="){
                std::cout << "\t%" << tmp_symbol_num << " = le " << relexp << ", "
                          << addexp << "\n";
            }            
            else if (op == ">="){
                std::cout << "\t%" << tmp_symbol_num << " = ge " << relexp << ", "
                          << addexp << "\n";
            }            
            else assert(false);
            return "%" + std::to_string(tmp_symbol_num++);
        }
        return "";
  }
};

// AddExp ::= MulExp | AddExp "+" MulExp | AddExp "-" MulExp
class AddExpAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> add_exp;
  std::unique_ptr<BaseAST> mul_exp;
  std::string op;
  void Dump() const override
  {
    std::cout << "AddExpAST {";
     if (op == ""){ 
            // AddExp := MulExp
            mul_exp->Dump();
        }
        else{
            // AddExp := AddExp AddOp MulExp
            add_exp->Dump();
            std::cout << op;
            mul_exp->Dump();
        }
    std::cout << "}";
  }
  std::string DumpIR() const override
  {
     if (op == ""){ 
            // AddExp := MulExp
            return mul_exp->DumpIR();
        }
        else{
            // AddExp := AddExp AddOp MulExp
            std::string addexp = add_exp->DumpIR();
            std::string mulexp = mul_exp->DumpIR();
            if (op == "+") {
                std::cout << "\t%" << tmp_symbol_num << " = add " << addexp << ", " <<
                    mulexp << "\n";
            }
            else if (op == "-"){
                std::cout << "\t%" << tmp_symbol_num << " = sub " << addexp << ", " <<
                    mulexp << "\n";
            }
            else assert(false);
            return "%" + std::to_string(tmp_symbol_num++);
        }
        return "";
  }
};

// MulExp ::= UnaryExp | MulExp "*" UnaryExp | MulExp "/" UnaryExp | MulExp "%" UnaryExp
class MulExpAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> mul_exp;
  std::unique_ptr<BaseAST> unary_exp;
  std::string op;
  void Dump() const override
  {
    std::cout << "MulExpAST {";
     if (op == ""){ 
            // MulExp := UnaryExp
            unary_exp->Dump();
        }
        else{
            // MulExp := MulExp MulOp UnaryExp
            mul_exp->Dump();
            std::cout << op;
            unary_exp->Dump();
        }
    std::cout << "}";
  }
  std::string DumpIR() const override
  {
     if (op == ""){ 
            // MulExp := UnaryExp
            return unary_exp->DumpIR();
        }
        else{
            // MulExp := MulExp MulOp UnaryExp
            std::string mulexp = mul_exp->DumpIR();
            std::string unaryexp = unary_exp->DumpIR();
            if (op == "*") {
                std::cout << "\t%" << tmp_symbol_num << " = mul " << mulexp << ", " <<
                    unaryexp << "\n";
            }
            else if (op == "/"){
                std::cout << "\t%" << tmp_symbol_num << " = div " << mulexp << ", " <<
                    unaryexp << "\n";
            }
            else if (op == "%"){
                std::cout << "\t%" << tmp_symbol_num << " = mod " << mulexp << ", " <<
                    unaryexp << "\n";
            }
            else assert(false);
            return "%" + std::to_string(tmp_symbol_num++);
        }
        return "";
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
