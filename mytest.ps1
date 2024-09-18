# 运行 Docker 容器并执行命令
docker run -it --rm -v "D:\MyStupidCompiler:/root/compiler" maxxing/compiler-dev bash -c "
  cd /root/compiler && \
  make && \
  ./build/compiler -koopa ./debug/hello.c -o hello.koopa && \
  ./build/compiler -riscv ./debug/hello.c -o hello.riscv && \
  exit && \
"
