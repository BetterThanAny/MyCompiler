# 运行 Docker 容器并执行命令
docker run -it --rm -v "D:\MyStupidCompiler:/root/compiler" maxxing/compiler-dev bash -c "
   autotest -koopa -s lv3 /root/compiler && \
  exit && \
"
