#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>

#include "AST.h"
#include "koopa.h"
#include "RISCV.h"

using namespace std;

// 声明 lexer 的输入, 以及 parser 函数
// 为什么不引用 sysy.tab.hpp 呢? 因为首先里面没有 yyin 的定义
// 其次, 因为这个文件不是我们自己写的, 而是被 Bison 生成出来的
// 你的代码编辑器/IDE 很可能找不到这个文件, 然后会给你报错 (虽然编译不会出错)
// 看起来会很烦人, 于是干脆采用这种看起来 dirty 但实际很有效的手段
extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

int main(int argc, const char *argv[])
{
  // 不和 C 的 stdio 混用，提高IO效率
  // ios::sync_with_stdio(false);
  // 解除std::cin和std::cout之间的绑定，
  // 从而避免每次读取输入前自动刷新输出缓冲区，以提高性能
  // cin.tie(nullptr);

  // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
  // compiler 模式 输入文件 -o 输出文件
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
  yyin = fopen(input, "r");
  assert(yyin);

  // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);
  if (string(mode) == "-test")
  {
    // freopen(output, "w", stdout);
    // 输出 AST
    ast->Dump();
    cout << endl;
    return 0;
  }
  else if (string(mode) == "-koopa")
  {
    freopen(output, "w", stdout);
    // 输出 koopa IR
    ast->DumpIR();
    cout << endl;
    return 0;
  }
  else if (string(mode) == "-riscv")
  {
    // freopen("RISCV.txt", "w", stdout);
    //  创建一个stringstream对象，用于存储输出
    stringstream ss;
    // 保存cout的当前缓冲区指针到coutBuf，以便恢复
    streambuf *coutBuf = cout.rdbuf();
    // 将cout的缓冲区指向ss的缓冲区，这样cout输出的内容就会存到ss中
    cout.rdbuf(ss.rdbuf());
    // 输出IR
    ast->DumpIR();
    // 从ss中读取字符串，存到IRstr中
    string IRstr = ss.str();
    // 获取c风格的字符串表示，存储在ir中
    const char *ir = IRstr.data();
    // 恢复cout的缓冲区指针
    cout.rdbuf(coutBuf);
    freopen(output, "w", stdout);
    parse_string(ir);
    cout << endl;
    return 0;
  }
  else
    cout << "Unknown mode: " << endl;
  cout << endl;
  return 0;
}
