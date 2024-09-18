#pragma once
#include <iostream>
#include <string>
#include <cassert>
#include "koopa.h"

void Visit(const koopa_raw_program_t &program);
void Visit(const koopa_raw_slice_t &slice);
void Visit(const koopa_raw_function_t &func);
void Visit(const koopa_raw_basic_block_t &bb);
void Visit(const koopa_raw_value_t &value);
void Visit(const koopa_raw_return_t &ret);
void Visit(const koopa_raw_integer_t &integer);

/*
typedef struct {
  /// Global values (global allocations only).
  koopa_raw_slice_t values;
  /// Function definitions.
  koopa_raw_slice_t funcs;
} koopa_raw_program_t;
*/
// 访问 raw program
void Visit(const koopa_raw_program_t &program)
{
  // 执行一些其他的必要操作
  std::cout << "   .text" << std::endl;
  // 访问所有全局变量
  Visit(program.values);
  // 访问所有函数
  Visit(program.funcs);
}

/*
typedef struct {
  /// Buffer of slice items.
  const void **buffer;
  /// Length of slice.
  uint32_t len;
  /// Kind of slice items.
  koopa_raw_slice_item_kind_t kind;
} koopa_raw_slice_t;
*/
// 访问 raw slice
void Visit(const koopa_raw_slice_t &slice)
{
  for (size_t i = 0; i < slice.len; ++i)
  {
    auto ptr = slice.buffer[i];
    // 根据 slice 的 kind 决定将 ptr 视作何种元素
    switch (slice.kind)
    {
    case KOOPA_RSIK_FUNCTION:
      // 访问函数
      Visit(reinterpret_cast<koopa_raw_function_t>(ptr));
      // reinterpret_cast<type-id>(expression) 强制类型转换
      break;
    case KOOPA_RSIK_BASIC_BLOCK:
      // 访问基本块
      Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
      break;
    case KOOPA_RSIK_VALUE:
      // 访问指令
      Visit(reinterpret_cast<koopa_raw_value_t>(ptr));
      break;
    default:
      // 我们暂时不会遇到其他内容, 于是不对其做任何处理
      assert(false);
    }
  }
}

/*
typedef struct {
  /// Type of function.
  koopa_raw_type_t ty;
  /// Name of function.
  const char *name;
  /// Parameters.
  koopa_raw_slice_t params;
  /// Basic blocks, empty if is a function declaration.
  koopa_raw_slice_t bbs;
} koopa_raw_function_data_t;
*/
// 访问函数，函数里面是基本块
void Visit(const koopa_raw_function_t &func)
{
  // 执行一些其他的必要操作
  std::cout << "   .globl " << func->name + 1 << std::endl;
  std::cout << func->name + 1 << ":" << std::endl;

  Visit(func->bbs); // 访问基本块
}

/*
typedef struct {
  /// Name of basic block, null if no name.
  const char *name;
  /// Parameters.
  koopa_raw_slice_t params;
  /// Values that this basic block is used by.
  koopa_raw_slice_t used_by;
  /// Instructions in this basic block.
  koopa_raw_slice_t insts;
} koopa_raw_basic_block_data_t;
*/
// 访问基本块
void Visit(const koopa_raw_basic_block_t &bb)
{
  Visit(bb->insts); // 访问指令
}

/*
struct koopa_raw_value_data {
  /// Type of value.
  koopa_raw_type_t ty;
  /// Name of value, null if no name.
  const char *name;
  /// Values that this value is used by.
  koopa_raw_slice_t used_by;
  /// Kind of value.
  koopa_raw_value_kind_t kind;
};
*/
// 访问指令
void Visit(const koopa_raw_value_t &value)
{
  const auto &kind = value->kind;
  switch (kind.tag)
  {
  case KOOPA_RVT_RETURN:
    Visit(kind.data.ret);
    break;
  case KOOPA_RVT_INTEGER:
    Visit(kind.data.integer);
    break;
  case KOOPA_RVT_BINARY:
    Visit(kind.data.binary);
    break;
  default:
    assert(false);
  }
}

void Visit(const koopa_raw_return_t &ret)
{
  koopa_raw_value_t ret_value = ret.value;
  Visit(ret_value);
  std::cout << "  ret" << std::endl;
}
/*
typedef struct {
  /// Operator.
  koopa_raw_binary_op_t op;
  /// Left-hand side value.
  koopa_raw_value_t lhs;
  /// Right-hand side value.
  koopa_raw_value_t rhs;
} koopa_raw_binary_t;
*/
void Visit(const koopa_raw_binary_t &binary)
{
  koopa_raw_value_t lhs = binary.lhs;
  koopa_raw_value_t rhs = binary.rhs;
  Visit(lhs);
  Visit(rhs);
  switch (binary.op)
  {
  case KOOPA_RBO_EQ: // ==
    std::cout << " = eq " << std::endl;
    break;
  default:
    assert(false);
  }
}

void Visit(const koopa_raw_integer_t &integer)
{
  int32_t int_val = integer.value;
  std::cout << "  li a0, " << int_val << std::endl;
}