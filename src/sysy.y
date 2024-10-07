%code requires {
  #include <memory>
  #include <string>
  #include "AST.hpp"
  #include <cstring>
  #include <vector>
  #include <map>
}

%{

#include <iostream>
#include <memory>
#include <string>
#include "AST.hpp"
#include <cstring>
#include <vector>
#include <map>

// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;

%}

// 定义 parser 函数和错误处理函数的附加参数
// 我们需要返回一个字符串作为 AST, 所以我们把附加参数定义成字符串的智能指针
// 解析完成后, 我们要手动修改这个参数, 把它设置成解析得到的字符串
%parse-param { std::unique_ptr<BaseAST> &ast }

// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是字符串指针, 有的是整数
// 之前我们在 lexer 中用到的 str_val 和 int_val 就是在这里被定义的
// 至于为什么要用字符串指针而不直接用 string 或者 unique_ptr<string>?
// 请自行 STFW 在 union 里写一个带析构函数的类会出现什么情况
%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
  MulVecType *mul_val;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT RETURN CONST
%token <str_val> IDENT UNARYOP MULOP ADDOP RELOP EQOP LANDOP LOROP
%token <int_val> INT_CONST

// 非终结符的类型定义
%type <ast_val> FuncDef FuncType Block BlockItem Stmt Exp UnaryExp PrimaryExp
%type <ast_val> AddExp MulExp RelExp EqExp LAndExp LOrExp
%type <int_val> Number

%%

// 开始符, CompUnit ::= FuncDef, 大括号后声明了解析完成后 parser 要做的事情
// 之前我们定义了 FuncDef 会返回一个 str_val, 也就是字符串指针
// 而 parser 一旦解析完 CompUnit, 就说明所有的 token 都被解析了, 即解析结束了
// 此时我们应该把 FuncDef 返回的结果收集起来, 作为 AST 传给调用 parser 的函数
// $1 指代规则里第一个符号的返回值, 也就是 FuncDef 的返回值
CompUnit
  : FuncDef {
    auto comp_unit = make_unique<CompUnitAST>();
    comp_unit->func_def = unique_ptr<BaseAST>($1);
    ast = move(comp_unit);
  }
  ;

Decl
  : ConstDecl{
    auto decl_ast = new DeclAST();
    decl_ast->const_decl = unique_ptr<BaseAST>($1);
    $$ = decl_ast;
  }
  | VarDecl{
    auto decl_ast = new DeclAST();
    decl_ast->var_decl = unique_ptr<BaseAST>($1);
    $$ = decl_ast;
  }
  ;

ConstDecl
  : CONST BType ConstDefList ';' {
    auto const_decl_ast = new ConstDeclAST();
    const_decl_ast->btype = unique_ptr<BaseAST>($2);
    const_decl_ast->const_def_list = unique_ptr<BaseAST>($3);
    $$ = const_decl_ast;
  }
  ;

ConstDefList
  : ConstDef {
    auto const_def_list_ast = new ConstDefListAST();
    const_def_list_ast->const_def = unique_ptr<BaseAST>($1);
    $$ = const_def_list_ast;
  }
  | ConstDefList ',' ConstDef {
    auto const_def_list_ast = new ConstDefListAST();
    const_def_list_ast->const_def_list = unique_ptr<BaseAST>($1);
    const_def_list_ast->const_def = unique_ptr<BaseAST>($2);
    $$ = const_def_list_ast;
  }
  ;

BType
  : INT {
    auto btype_ast = new BTypeAST();
    btype_ast->btype_name = "int";
    $$ = btype_ast;
  }
  ;

ConstDef
  : IDENT '=' ConstInitVal {
    auto const_def_ast = new ConstDefAST();
    const_def_ast->ident = *unique_ptr<string>($1);
    const_def_ast->const_init_val = unique_ptr<BaseAST>($3);
    $$ = const_def_ast;
  }
  ;

ConstInitVal
  : ConstExp {
    auto const_init_val_ast = new ConstInitValAST();
    const_init_val_ast->const_exp = unique_ptr<BaseAST>($1);
    $$ = const_init_val_ast;
  }
  ;

VarDecl
  : BType VarDefList ';' {
    auto var_decl_ast = new VarDeclAST();
    var_decl_ast->btype = unique_ptr<BaseAST>($1);
    var_decl_ast->var_def_list = unique_ptr<BaseAST>($2);
    $$ = var_decl_ast;
  }
  ;

VarDefList
  : VarDef {
    auto var_def_list_ast = new VarDefListAST();
    var_def_list_ast->var_def = unique_ptr<BaseAST>($1);
    $$ = var_def_list_ast;
  }
  | VarDefList ',' VarDef {
    auto var_def_list_ast = new VarDefListAST();
    var_def_list_ast->var_def_list = unique_ptr<BaseAST>($1);
    var_def_list_ast->var_def = unique_ptr<BaseAST>($2);
    $$ = var_def_list_ast;
  }
  ;

VarDef
  : IDENT {
    auto var_def_ast = new VarDefAST();
    var_def_ast->ident = *unique_ptr<string>($1);
    $$ = var_def_ast;
  }
  | IDENT '=' InitVal {
    auto var_def_ast = new VarDefAST();
    var_def_ast->ident = *unique_ptr<string>($1);
    var_def_ast->init_val = unique_ptr<BaseAST>($3);
    $$ = var_def_ast;
  }
  ;

InitVal
  : Exp {
    auto init_val_ast = new InitValAST();
    init_val_ast->exp = unique_ptr<BaseAST>($1);
    $$ = init_val_ast;
  }
  ;
// FuncDef ::= FuncType IDENT '(' ')' Block;
// 我们这里可以直接写 '(' 和 ')', 因为之前在 lexer 里已经处理了单个字符的情况
// 解析完成后, 把这些符号的结果收集起来, 然后拼成一个新的字符串, 作为结果返回
// $$ 表示非终结符的返回值, 我们可以通过给这个符号赋值的方法来返回结果
// 你可能会问, FuncType, IDENT 之类的结果已经是字符串指针了
// 为什么还要用 unique_ptr 接住它们, 然后再解引用, 把它们拼成另一个字符串指针呢
// 因为所有的字符串指针都是我们 new 出来的, new 出来的内存一定要 delete
// 否则会发生内存泄漏, 而 unique_ptr 这种智能指针可以自动帮我们 delete
// 虽然此处你看不出用 unique_ptr 和手动 delete 的区别, 但当我们定义了 AST 之后
// 这种写法会省下很多内存管理的负担
FuncDef
  : FuncType IDENT '(' ')' Block {
    auto funcD_ast = new FuncDefAST();
    funcD_ast->func_type = unique_ptr<BaseAST>($1);
    funcD_ast->ident = *unique_ptr<string>($2);
    funcD_ast->block = unique_ptr<BaseAST>($5);
    $$ = funcD_ast;
  }
  ;

// 同上, 不再解释
FuncType
  : INT {
    auto funcT_ast = new FuncTypeAST();
    funcT_ast -> funcT_name = "int";
    $$ = funcT_ast;
  }
  ;
// 实际上语法解释器不支持用大括号来表示重复出现，所以我们需要用递归来表示
Block
  : '{' BlockItemList '}' {
    auto block_ast = new BlockAST();
    block_ast->stmt = unique_ptr<BaseAST>($2);
    $$ = block_ast;
  }
  ;

BlockItemList
  : BlockItem {
    
  }
  | BlockItemList BlockItem {
    
  }
  ;

BlockItem
  : Decl {
    auto block_item_ast = new BlockItemAST();
    block_item_ast->decl = unique_ptr<BaseAST>($1);
    $$ = block_item_ast;
  }
  | Stmt {
    auto block_item_ast = new BlockItemAST();
    block_item_ast->stmt = unique_ptr<BaseAST>($1);
    $$ = block_item_ast;
  }
  ;

Stmt
  : LVal '=' Exp ';' {
    auto stmt_ast = new StmtAST();
    stmt_ast->lval = unique_ptr<BaseAST>($1);
    stmt_ast->exp = unique_ptr<BaseAST>($3);
    $$ = stmt_ast;
  } 
  | RETURN Exp ';' {
    auto stmt_ast = new StmtAST();
    stmt_ast->exp = unique_ptr<BaseAST>($2);
    $$ = stmt_ast;
  }
  ;

LVal
  : IDENT {
    auto lval_ast = new LValAST();
    lval_ast->ident = *unique_ptr<string>($1);
    $$ = lval_ast;
  }
  ;

ConstExp
  : Exp {
    auto const_exp_ast = new ConstExpAST();
    const_exp_ast->exp = unique_ptr<BaseAST>($1);
    $$ = const_exp_ast;
  }
  ;

Exp
  : LOrExp {
    auto exp_ast = new ExpAST();
    exp_ast->lor_exp = unique_ptr<BaseAST>($1);
    $$ = exp_ast;
  }
  ; 

LOrExp
  : LAndExp {
    auto lor_exp_ast = new LOrExpAST();
    lor_exp_ast->land_exp = unique_ptr<BaseAST>($1);
    $$ = lor_exp_ast;
  }
  | LOrExp LOROP LAndExp {
    auto lor_exp_ast = new LOrExpAST();
    lor_exp_ast->lor_exp = unique_ptr<BaseAST>($1);
    lor_exp_ast->op = *unique_ptr<string>($2);
    lor_exp_ast->land_exp = unique_ptr<BaseAST>($3);
    $$ = lor_exp_ast;
  }
  ;

LAndExp
  : EqExp {
    auto land_exp_ast = new LAndExpAST();
    land_exp_ast->eq_exp = unique_ptr<BaseAST>($1);
    $$ = land_exp_ast;
  }
  | LAndExp LANDOP EqExp {
    auto land_exp_ast = new LAndExpAST();
    land_exp_ast->land_exp = unique_ptr<BaseAST>($1);
    land_exp_ast->op = *unique_ptr<string>($2);
    land_exp_ast->eq_exp = unique_ptr<BaseAST>($3);
    $$ = land_exp_ast;
  }
  ;

EqExp 
  : RelExp {
    auto eq_exp_ast = new EqExpAST();
    eq_exp_ast->rel_exp = unique_ptr<BaseAST>($1);
    $$ = eq_exp_ast;
  }
  | EqExp EQOP RelExp {
    auto eq_exp_ast = new EqExpAST();
    eq_exp_ast->eq_exp = unique_ptr<BaseAST>($1);
    eq_exp_ast->op = *unique_ptr<string>($2);
    eq_exp_ast->rel_exp = unique_ptr<BaseAST>($3);
    $$ = eq_exp_ast;
  }
  ;

RelExp
  : AddExp {
    auto rel_exp_ast = new RelExpAST();
    rel_exp_ast->add_exp = unique_ptr<BaseAST>($1);
    $$ = rel_exp_ast;
  }
  | RelExp RELOP AddExp {
    auto rel_exp_ast = new RelExpAST();
    rel_exp_ast->rel_exp = unique_ptr<BaseAST>($1);
    rel_exp_ast->op = *unique_ptr<string>($2);
    rel_exp_ast->add_exp = unique_ptr<BaseAST>($3);
    $$ = rel_exp_ast;
  }
  ;

AddExp
  : MulExp {
    auto add_exp_ast = new AddExpAST();
    add_exp_ast->mul_exp = unique_ptr<BaseAST>($1);
    $$ = add_exp_ast;
  }
  | AddExp ADDOP MulExp {
    auto add_exp_ast = new AddExpAST();
    add_exp_ast->add_exp = unique_ptr<BaseAST>($1);
    add_exp_ast->op = *unique_ptr<string>($2);
    add_exp_ast->mul_exp = unique_ptr<BaseAST>($3);
    $$ = add_exp_ast;
  }
  ;

MulExp
  : UnaryExp {
    auto mul_exp_ast = new MulExpAST();
    mul_exp_ast->unary_exp = unique_ptr<BaseAST>($1);
    $$ = mul_exp_ast;
  }
  | MulExp MULOP UnaryExp {
    auto mul_exp_ast = new MulExpAST();
    mul_exp_ast->mul_exp = unique_ptr<BaseAST>($1);
    mul_exp_ast->op = *unique_ptr<string>($2);
    mul_exp_ast->unary_exp = unique_ptr<BaseAST>($3);
    $$ = mul_exp_ast;
  }
  ;
  
UnaryExp
  : PrimaryExp {
    auto unary_exp_ast = new UnaryExpAST();
    unary_exp_ast->type = UnaryExpType::primaryT;
    unary_exp_ast->exp = unique_ptr<BaseAST>($1);
    $$ = unary_exp_ast;
  }
  | UNARYOP UnaryExp {
    auto unary_exp_ast = new UnaryExpAST();
    unary_exp_ast->type = UnaryExpType::unaryT;
    unary_exp_ast->op = *unique_ptr<string>($1);
    unary_exp_ast->exp = unique_ptr<BaseAST>($2);
    $$ = unary_exp_ast;
  }
  | ADDOP UnaryExp {
    auto unary_exp_ast = new UnaryExpAST();
    unary_exp_ast->type = UnaryExpType::unaryT;
    unary_exp_ast->op = *unique_ptr<string>($1);
    unary_exp_ast->exp = unique_ptr<BaseAST>($2);
    $$ = unary_exp_ast;
  }
  ;

PrimaryExp
  : '(' Exp ')' {
    auto primary_exp_ast = new PrimaryExpAST();
    primary_exp_ast->type = PrimaryExpType::expT;
    primary_exp_ast->exp = unique_ptr<BaseAST>($2);
    $$ = primary_exp_ast;
  }
  | LVal {
    auto primary_exp_ast = new PrimaryExpAST();
    primary_exp_ast->type = PrimaryExpType::lvalT;
    primary_exp_ast->lval = unique_ptr<BaseAST>($1);
    $$ = primary_exp_ast;
  }
  | Number {
    auto primary_exp_ast = new PrimaryExpAST();
    primary_exp_ast->type = PrimaryExpType::numberT;
    primary_exp_ast->number = ($1);
    $$ = primary_exp_ast;
  }
  ;

Number
  : INT_CONST {
    $$ = ($1);
  }
  ;


%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s) {
    extern int yylineno;    // defined and maintained in lex
    extern char *yytext;    // defined and maintained in lex
    int len=strlen(yytext);
    int i;
    char buf[512]={0};
    for (i=0;i<len;++i){
        sprintf(buf,"%s%d ",buf,yytext[i]);
    }
    fprintf(stderr, "ERROR: %s at symbol '%s' on line %d\n", s, buf, yylineno);
}
