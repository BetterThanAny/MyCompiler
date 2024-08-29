# 编译Lab

## 测试命令

一般在 powershell 运行临时 docker 容器并删除

`docker run -it --rm maxxing/compiler-dev bash`

执行 `exit` 退出
最开始直接复制 sysy. l 和 sysy. y 和 main. cpp
然后在. ignore 中添加

将 Windows 系统中的文件夹映射到容器中的 `/root/compiler` 文件夹

`docker run -it --rm -v /d/MyStupidCompiler:/root/compiler maxxing/compiler-dev bash`
`cd compiler`
然后 `make`
最后`build/compiler -koopa ./debug/hello.c -o hello.koopa`

autotest
`docker run -it --rm -v /d/MyStupidCompiler:/root/compiler maxxing/compiler-dev`  

`autotest -koopa -s lv1 /root/compiler`

## Lv1.2

添加了段注释的处理

## Lv1.3

根据 EBNF 设计 AST

``` EBNF

CompUnit ::= FuncDef;
FuncDef  ::= FuncType IDENT "(" ")" Block; 
FuncType ::= "int"; 

Block ::= "{" Stmt "}"; 
Stmt ::= "return" Number ";"; 
Number ::= INT_CONST;
```

## Lv1.4

``` C++
int main()
{
// 忽略我的存在
/*这个也需要忽略*/
return 0;
}


``` koopa
fun @main(): i32 {
  %entry:
    ret 0
}
```

## Lv1.5

本地测试通过

## Lv 2

版本1 本地测试通过，之后可以改一下Visit函数
