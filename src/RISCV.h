#pragma once
#include <iostream>
#include <string>
#include <cassert>
#include "koopa.h"


void Visit(const koopa_raw_program_t &program){

  //访问所有全局变量
  Visit(program.values);
  //访问所有函数
  Visit(program.funcs);
}

// 访问 raw slice
void Visit(const koopa_raw_slice_t &slice){
  for (size_t i = 0; i < slice.len; ++i){
    auto ptr = slice.buffer[i];
    // 根据 slice 的 kind 决定将 ptr 视作何种元素
    switch (slice.kind){
      case KOOPA_RSIK_FUNCTION:
      // 访问函数
        Visit(reinterpret_cast<koopa_raw_function_t>(ptr));
        break;
      case KOOPA_RSIK_BASIC_BLOCK:
      // 访问基本块
        Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
        break;
      case KOOPA_RSIK_VALUE:
      // 访问值
        Visit(reinterpret_cast<koopa_raw_value_t>(ptr));
        break;
      default:
      // 暂时不会遇到其他内容，不做处理
        assert(false);

    }
  }
}