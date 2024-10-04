#pragma once
#include <iostream>
#include <string>
#include <cassert>
#include <map>
#include "koopa.h"

struct register_t
{
  int reg_name;
  int reg_num;
};
std::string reg_names[16] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6",
                             "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "x0"};
koopa_raw_value_t registers[16];
int reg_stats[16] = {0};
koopa_raw_value_t present_value = 0;
std::map<const koopa_raw_value_t, Reg> value_map;
int stack_size = 0, stack_top = 0;
std::map<uintptr_t, int> stack_frame;


void Visit(const koopa_raw_program_t &program);
void Visit(const koopa_raw_slice_t &slice);
void Visit(const koopa_raw_function_t &func);
void Visit(const koopa_raw_basic_block_t &bb);
void Visit(const koopa_raw_value_t &value);
void Visit(const koopa_raw_return_t &ret);
void Visit(const koopa_raw_integer_t &integer);
void Visit(const koopa_raw_binary_t &binary);
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
  std::cout << "\t.text" << std::endl;
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
  std::cout << "\t.globl " << func->name + 1 << std::endl;
  if (fun->bbs.len == 0) return;
  std::cout << func->name + 1 << ":\n";

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
  int stack_s = cal_stack_s(bb->insts);
  if (stack_s > 0){
    std::cout << "\taddi sp, sp, -" << stack_s * 4 << std::endl;
    
  } 
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
typedef struct koopa_raw_type_kind {
  koopa_raw_type_tag_t tag;
  union {
    struct {
      const struct koopa_raw_type_kind *base;
      size_t len;
    } array;
    struct {
      const struct koopa_raw_type_kind *base;
    } pointer;
    struct {
      koopa_raw_slice_t params;
      const struct koopa_raw_type_kind *ret;
    } function;
  } data;
} koopa_raw_type_kind_t;

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
  case KOOPA_RBO_SUB: // -
    std::cout << "\tsub   " << std::endl;
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