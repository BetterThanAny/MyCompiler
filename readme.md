# 9. 27 Win 修改

修改了RISCV.h 中的 Visit() 函数，目前还不支持3.1
AST.h支持3.1  
确定能通过本地测试Lv2.3

下午再改RISCV.h
再增加个自动测试脚本

现在暂时在做前端，最后一起做3的RISCV

./build/compiler -riscv ./debug/hello.c -o hello.riscv && \
