
```c
int getint()
int getchar() //返回ASCII码值 EOF返回-1

```

一般在 powershell 运行临时 docker 容器并删除
```` bash
docker run -it --rm maxxing/compiler-dev bash
````
执行 `exit` 退出
最开始直接复制 sysy. l 和 sysy. y 和 main. cpp
然后在. ignore 中添加

将 Windows 系统中的 `D:\Mycompiler` 文件夹映射到容器中的 `/root/compiler` 文件夹
```sh
docker run -it --rm -v /d/MyStupidCompiler:/root/compiler maxxing/compiler-dev bash
```

## Lv1.2
添加了段注释的处理
```bash
docker run -it --rm -v /d/MyStupidCompiler:/root/compiler maxxing/compiler-dev bash

cd compiler
make
build/compiler -koopa ./debug/hello.c -o hello.koopa
```
## Lv1.3
根据 EBNF 设计 AST
```EBNF
CompUnit ::= FuncDef;
FuncDef  ::= FuncType IDENT "(" ")" Block; 
FuncType ::= "int"; 

Block ::= "{" Stmt "}"; 
Stmt ::= "return" Number ";"; 
Number ::= INT_CONST;
```

## Lv1.4
```C++
int main()
{
// 忽略我的存在
/*这个也需要忽略*/
return 0;
}
```
```koopa
fun @main(): i32 {
  %entry:
    ret 0
}
```
`Program` 应该用 `CompUnitAST` 定义。
`Function` 用 `FuncDef` 定义，包括`{}`和外面的都应该属于它。

## Lv 2.1
引用`libkoopa`：