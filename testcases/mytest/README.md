# mytest —— 按 `docs/grammar.md` 手写的补充用例

官方 `basic-testcases` / `bigint-testcases` 的覆盖偏基础（没有关键字参数、没有链式比较、
没有负数取模、没有多变量增量赋值、没有作用域遮蔽），这里补一批**文档明确规定了、但官方数据没考**的点。

## 最重要的一条

**期望输出全部按 `docs/grammar.md` 推导，不是按 CPython 推导。**
下面是本目录里「真 Python 会给出不同答案」的地方，别拿 Python 去校对，会误判：

| 表达式 | 本文档 | CPython |
|---|---|---|
| `1 and 2` | `True`（doc 248：视作 C++ `&&`，结果是 bool） | `2` |
| `0 or "abc"` | `True` | `"abc"` |
| `1 != "1"` | `False`（doc 242：跨类型 `==`/`!=` 结果都是 `False`） | `True` |
| 函数内 `x = 2`（x 是全局） | 改的是全局（doc 295 / 671：赋值无法遮蔽全局） | 新建局部 |
| `-7 % 2` | `1`（floor 语义） | `1`（这里一致） |

## 运行

```bash
bash testcases/mytest/run.sh      # 只跑
bash testcases/mytest/run.sh -b   # 先 cmake --build 再跑
```

## 用例清单

| 文件 | 考什么 | 文档条款 |
|---|---|---|
| case0 | 关键字参数、位置+关键字混用、默认值补位 | 4.2（561 行原例） |
| case1 | 默认值在 `def` 执行时求值并快照，之后改 `e0` 不影响 | 4.1（522–535 行） |
| case2 | 赋值不能遮蔽全局（写穿到全局）；形参可以遮蔽全局 | 3.1.1 / 4.2 / 6（671 行） |
| case3 | `and`/`or` 返回 bool、短路、`not` 优先级低于比较 | 2.2.2（246–268 行） |
| case4 | `/` 恒为 float；`//`、`%` 的负数与异号行为 | 2.2.2（232–238 行） |
| case5 | 连续赋值 `a = b = c = 1`、解包交换、`return` 元组解包 | 3.1.2 / 3.1.3 |
| case6 | 多变量增量赋值的两阶段语义、`str *= int`、类型可改变 | 3.2.1 / 3.2.2 |
| case7 | `str*int` / `int*str`、`str*0`、字符串比较、空参数打印间距 | 2.2.2（224–230 行）、5（619 行） |
| case8 | 链式比较：短路 + 每个表达式只求值一次（靠打印次数验证） | 2.2.2（270–277 行） |
| case9 | f-string：`{{ }}`、嵌套 f-string、转义引号、表达式 | 2.3（106–136 行） |
| case10 | `while` + `break`/`continue` 组合、`while True` | 3.5 / 3.6 |
| case11 | 五个内置函数的转换与 float 6 位小数输出 | 5 |
| case12 | 递归：每层调用帧独立，局部变量互不污染 | 6 |

## 已知：f-string 里「纯标识符字面量块」会被 lexer 抢走

`lexer_tiebreak.in` 不是回归用例，是**问题复现文件**，不进 `run.sh` 的统计。

`print(f"a{x}b{x}c")` 完全合法（doc 2.3：字面内容可与 `{}` 混排，可含多个 `{}`），但现在的实现直接
报 `no viable alternative at input 'f"a'`。原因在 `resources/Python3Lexer.g4`：

- `NAME` 定义在第 167 行（token 号 23）
- `FORMAT_STRING_LITERAL` 定义在第 176 行（token 号 25）

ANTLR 的规则：**最长匹配优先，长度相同时按规则出现顺序取胜**。字面量块 `a` 既能被
`FORMAT_STRING_LITERAL` 匹配（1 字符），也能被 `NAME` 匹配（1 字符），平局 → 规则号小的 `NAME` 胜。
于是 `a` 变成标识符，`format_string` 规则收到意外的 `NAME`，整棵树崩掉。

这也解释了为什么官方 test14 全过：它的字面量块是 `"Formatted string with integer "` 这种**带空格的长串**，
长度上赢过 `NAME`，所以 FORMAT_STRING_LITERAL 正常胜出。只要块里出现空格、标点，或块不是合法标识符，就不会触发。

触发条件：某个字面量块**不含空格/标点**且**整体是一个合法标识符或关键字**。
例如 `f"a{x}"`、`f"{x}b"`、`f"abc{x}"`、`f"{x}and{y}"`。

这不是 `Evalvisitor` 能修的（错在词法阶段，parse tree 已经错了）。`src/main.cpp` 顶部也写了
「regenerating files in directory named generated is dangerous，需要重新生成请找助教」，
所以不要自己改 g4 重新生成。如果 OJ 有测试点踩到这种写法，属于词法器缺陷，应找助教确认。

`run.sh` 用 `case*.in` 通配，`lexer_tiebreak.in` 不参与统计，要看现象手动跑：

```bash
./build/code < testcases/mytest/lexer_tiebreak.in
```

## 失败时的判断顺序

1. **先看是不是文档条款本身有歧义**，比如 case4 的负数取模依赖 `int2048` 的 `/`、`%` 是否 floor 语义；
   若 `int2048` 是截断语义，这里挂是 `int2048` 的问题，不是 `Evalvisitor` 的问题。
2. 再确认不是自己把「CPython 行为」当成了期望值（见上面那张表）。
3. 最后才怀疑 visitor 实现。
