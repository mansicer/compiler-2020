# 编译原理第一次实验报告

## 文件结构

- Code：包含所有代码文件。其中`lexical.l`，`syntax.y`，`main.c`为原有文件，进行了相关实现；`config.h`包含实验中的部分类型和宏定义；`treenode.h`和`treenode.c`为实验需要定义的多叉树数据结构；`symtable.h`和`symtable.c`是实验所用到的符号表实现；`semantic.h`和`semantic.c`包含了语义分析的相关函数；`Makefile`为得到二进制文件的原有代码，未进行修改。
- Test：测试用例目录。
- parser：使用make命令得到的可执行文件。
- README：原有文件，未修改。
- report.pdf：本报告文件。



## 实验内容

### 函数与变量类型

我们借用讲义中提到的`Type`和`FieldList`的定义方法，以这两个基本结构组成函数和变量的基本类型。其中结构`Type`即为一个变量可能的类型，包括基本类型（`int`，`float`），数组类型和结构类型三种，数组类型和结构类型中又会嵌套其它的基本结构；而结构`FieldList`描述了一个包含多个类型及名字的域，通过链表的方法组成结构。可以看出一个变量的类型即可用`Type`结构来描述；而一个函数的类型包含返回类型和参数类型两部分，前者可以用`Type`结构来描述，后者可以用`FieldList`结构来描述，二者共同构成一个函数的类型。



### 符号表

我们的选做2.3没有对变量作用域的要求，因此我们考虑采用高效的哈希表作为符号表的结构。在语义分析的过程中，我们的符号表只需要包含符号的名字，类别以及是否被定义过（由于我们的选做内容中没有包含仅声明的语法句式，实际上表示是否被定义过的域是可以去掉的）。符号表的哈希函数选用了讲义中提供的函数，保证了符号表的查找性能。我们为符号表实现了包括添加和查找符号的相关函数。



### 语义分析

由于我们已经在实验1中建立起了一棵语法树，这样的树结构能够帮助我们直接进行语义分析。而具体到语义分析的过程中，由于每个不同的语法单元需要进行的语义分析都是各不相同的，根据语法制导翻译（SDT）的理论，我们的某些语法产生式仅需对其中的语法单元定义和传递综合属性即可完成语法分析，如对`Exp`的语义分析，我们只需根据其产生式各个单元的类型，即可确定当前`Exp`的类型；而某些语法产生式则需要对其中的语法单元定义和传递综合属性与继承属性，才能满足语义分析的要求，如`Def : Specifier DecList SEMI`产生式中，我们需要先根据语法单元`Specifier`的语义分析结果（一个具体类型），并将其作为`VarDec`的继承属性，参与到`DecList`的语义分析中，使得`DecList`中每发现一个变量名，都可以以正确的类型加入到符号表中，最终我们需要获取到`DecList`中定义的域的结果，作为`Def`单元的综合属性，返回给上级语法单元，供其进行进一步的语义分析。

因此，我们需要按照合理的语义分析顺序，根据语法树建立起的语法单元关系，自顶而下的对各个单元进行语义分析。我们可以以语法单元的顶层为根节点，建立起一棵语义分析树，対树中的每个节点都分配继承属性和综合属性的空间。但考虑到在语法树中并非所有单元都需要这些属性，这样的方法会有一些空间上的浪费。我们退而发现，实际上我们仅需定义不同语法单元的语义分析函数，将继承属性作为函数的参数一并传入，而将综合属性作为函数的返回值，即可实现我们的语法制导翻译需求，而对于许多不需要综合属性或是继承属性的单元，这样的方法也可以降低空间上的浪费。基于这样的方法，我们在不同的语义分析函数中，根据实验的报错需要，利用条件控制语句加入相应的报错信息。每当我们在一个语法单元的语义分析发现报错时，该**语法单元及其子结点的语义分析所产生的结果将会被编译器忽略**（如在该语法单元的子结点中定义的变量等等），这样的处理能够使得我们继续向下进行语义分析，但当然也会**因为前面语义分析的不完全而产生连锁的报错情况**，这样的情况其实是在我们的预期之内的。



### 类型等价与选做内容

由于我们的类型结构`Type`与`FieldList`是嵌套定义的，我们自然也可以递归地判断两个类型是否相同。由于选做内容2.3需要我们将结构等价作为结构类型的等价机制，我们需要考虑到一些情况，如下面的两个结构

```c
struct A { int a1; struct { int a2; float a3; } s1; };
struct B { int b1; struct { int b2; float b3; } s2; };
```

可以看做是结构等价的。也就是说我们在判断`FieldList`的等价时，需要对其中每个类型的等价进行比较，这样的等价判别方法也是可以递归进行的。