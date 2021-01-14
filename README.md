# graduationDesign

graduation  design

## 主要结构


| 文件 | 描述 |
| :-- | :-- |
|auto-debug-from-files/auto-debug-from-path | 利用下面的单个文件自动编译的shell文件，针对不同情况，完成对多个文件以及指定目录下两种情况的支持性 |
| auto-debug | 利用shell下面两个操作步骤进行合并，完成对单个文件的自动编译 |
| LogFilter | 对下面生成的日志文件进行过滤，筛取有用信息 |
| script.exp | 利用expect的交互特性，自动完成对源文件(c)编译，生成，(gdb)智能断点添加，变量环境单步输出，并生成初步的日志文件 |


> 用了较多的现有的unix工具，为了不再增加对jdk环境的依赖性，LogFilter日志过滤器是用c++编写，直接生成的可执行代码。

## 依赖目录：[updating]

```
|--shell
|--tcl-expect
|--gcc/g++/gdb
|--unix2dos

```
