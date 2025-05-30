**项目名称**：clock_gettime与除数不变的无符号整数除法

**项目链接**：github.com/Fix-Point/projOPENVELA-InvariantDivisor (项目使用的测试程序、绘图脚本和说明还在编写中，后续会更新到本仓库)

**导师信息**：欧阳湘臻，18792877129，ouyangxiangzhen@xiaomi.com

**难度**：低

**分类**：其他算法类实现类

**题目要求**：

获取当前时间是许多用户应用程序的常见需求，尤其是在实时系统中，应用对时间获取的准确性和实时性有较高要求。
计算机通常使用硬件周期计数器对外部时钟信号的电平变化进行计数。外部时钟信号由晶振产生，晶振在施加电压后会产生固定周期的震荡信号。这些信号经过锁相环（Phase-Locked Loop, PLL）倍频后，作为时钟信号输出到硬件周期计数器。
因此，获取时间的流程主要包括以下两个步骤：
- 读取硬件周期计数器：从特定设备地址空间或寄存器读取硬件周期计数器
- 计算时间：根据频率计算自上一个时刻经过的时间差或者上电后的绝对时间

计算时间过程通常使用以下公式：
```math
 sec = \lfloor \frac{C}{freq} \rfloor
```
```math
 nsec = {\lfloor \frac{(C - sec \times freq) \times NSEC\_PER\_SEC}{freq} \rfloor}
```
其中，
$`C`$ - 硬件周期计数(cycle)

$`freq`$ - 外部时钟信号的频率(cycle/s)

$`sec`$ - 转换得到的秒数(s)

$`nsec`$ - 不足一秒的纳秒数(ns)

$`NSEC\_PER\_SEC`$ - 常量$`10^9`$纳秒/秒(ns/s)

注意，公式中使用了两次除法，且第二次除法依赖于第一次除法的结果。由于除法的复杂性，硬件实现的除法运算通常比加、减、乘和比特位运算消耗更多的CPU周期，甚至某些体系结构（如ARMv7）不支持硬件除法。在这些平台上，应尽量避免使用除法操作。

在现代CPU中，外部时钟信号的频率通常是固定的。因此根据硬件周期数计算时间的优化实现问题可以转化为：如何优化除数不变的无符号整数除法？

挑战一：请实现除数不变的无符号整数除法`invdiv`，支持32位周期计数/32位频率、64位周期计数/32位频率或64位周期计数/64位频率。
- 进阶挑战：您的`invdiv`实现对于所有可能的输入都是正确的吗？参考论文，请给出`invdiv`优化方法的正确性的形式化证明，确保对于所有可能的除数$`d`$和被除数$`n`$，其输出等价于整数除法，例如：
```math
\forall n \in [0, 2^{64}) d \in [1, 2^{64}), invdiv(n, d) = \lfloor \frac{n}{d} \rfloor
```

挑战二：在挑战一的基础上，请实现`int my_clock_gettime(int clock_id, struct timespec *time)`接口。该接口根据会获取当前时间并写入到`time`结构体中。
- 进阶挑战：您的`my_clock_gettime`实现，其性能和实时性能否超越Linux的C库实现？

提示1： 对于运行时除数不变的无符号整数除法，可以参考编译器优化常量除法的方法，将其转换为乘法和右移操作$`^{[1][2][3][4][5]}`$。即对于任意被除数$$n$$和除数$$d$$，可以找到一个乘数$$m$$和右移数$`s`$，使得$`\lfloor \frac {n \times m}{2^{s}} \rfloor = \lfloor \frac{n}{d} \rfloor`$。该式中除以$`2^{s}`$等价于逻辑右移$$s$$位，该操作在绝大多数体系结构中仅消耗1个CPU周期，而乘法通常消耗3个CPU周期，该方式优化过的除法开销最低能达到乘法数量级。

提示2： 本题并非禁用除法，在有硬件除法支持的平台上，如果硬件除法更快，则允许使用除法$`^{[6]}`$。

提示3： 在`clock_gettime`中允许采用近似算法$`^{[7][8]}`$，只要时间转换误差在可接受范围内即可。传统的除法优化是精确的，即转换成右移和乘法操作后，$`\lfloor \frac {n \times m}{2^{s}} \rfloor - \lfloor \frac{n}{d} \rfloor < 1`$。Linux的实现通过放宽该约束，允许采用非精确的近似算法，以纳秒级的转换误差为代价换取性能的提升。

**预期目标**：

目标1： 实现除数不变的无符号整数除法算法`invdiv`，确保计算结果与硬件除法一致，且性能和资源使用达到最优。
- 功能正确性：对给定的测试集，计算结果必须与硬件除法完全一致，无任何误差。
- 性能指标：在真实硬件上测量对给定的测试集的吞吐率（操作数/秒）。
- 实时性指标：以下两种方式任选一种
  - 测试方式：在真实硬件上测量对给定测试集的操作的尾延迟（90/99/99.9百分位延迟）。
  - 静态分析方式：通过静态分析工具（如LLVM-TA）得出invdiv的最坏情况执行时间，并对分析结果进行详细的说明。
- 内存指标：在真实硬件上测量执行过程中的最大RSS(Resident Set Size)。
- 结合体系结构优化：可利用特定体系结构的硬件指令进行优化，但请尽量不要使用浮点和向量寄存器，因为操作系统内核使用这些寄存器会带来额外的上下文保存和恢复开销。

目标2： 实现`int my_clock_gettime(int clock_id, struct timespec *time)`
- 时间误差：与操作系统C库clock_gettime实现的时间误差小于1微秒。
- 性能指标：在真实硬件上测量单线程循环100M次调用的吞吐率(操作数/秒)，以及以最大硬件线程数绑核循环100M次调用的吞吐率
- 实时性指标：以下两种方式任选一种
  - 测试方式：插桩测量循环1M次调用时的尾延迟（90/99/99.9百分位延迟）。
  - 静态分析方式：通过静态分析工具（如LLVM-TA）得出invdiv的最坏情况执行时间，并对分析结果进行详细的说明。
- 内存指标：在真实硬件上测量执行过程中的最大RSS。
- 可移植性：实现应具备跨平台可移植性。

注：此处的`90/99/99.9百分位延迟`指的是对执行延迟测量若干次，按延迟从小到大排列后，其中最大的`10%/1%/0.1%/0.01%/0.001%`的延迟。举个例子，测量1000次延迟，对其排序后第900个延迟就是`90百分位延迟`，通常也被称为`P90`。如果该值是`120纳秒`，那么意味着`90%`的请求都能在`120纳秒`内完成。

**License**：

Apache协议

**参考资料**：

[1] Granlund T, Montgomery P L. Division by invariant integers using multiplication[C]//Proceedings of the ACM SIGPLAN 1994 conference on Programming language design and implementation. 1994: 61-72. https://gmplib.org/~tege/divcnst-pldi94.pdf

[2] Möller N, Granlund T. Improved division by invariant integers[J]. IEEE Transactions on Computers, 2010, 60(2): 165-175. https://gmplib.org/~tege/division-paper.pdf

[3] Linux, __div64_const32, https://github.com/torvalds/linux/blob/v6.14/include/asm-generic/div64.h#L65

[4] Warren H S. Hacker's delight[M]. Pearson Education, 2013.

[5] Warren H S. 算法心得 高效算法的奥秘(原书第2版)[M]. 机械工业出版社. 2014.

[6] libdivide.h. libdivide. https://github.com/ridiculousfish/libdivide/blob/master/libdivide.h

[7] Linux. clocksource_cyc2ns. https://github.com/torvalds/linux/blob/v6.14/include/linux/clocksource.h#L212

[8] Linux. vdso_calc_ns. https://github.com/torvalds/linux/blob/v6.14/lib/vdso/gettimeofday.c#L39

[9] Leetcode. 两数相除. https://leetcode.cn/problems/divide-two-integers/description/

**编译说明**

本项目包含测试程序和绘图脚本，目录结构如下：
```bash
.
├── 3rdlib                # 第三方库
│   └── libdivide.h       # libdivide库
├── config.h              # 测试程序配置
├── data.c                # 测试数据集
├── helper_func.h         # 测试帮助函数
├── LICENSE               # Apache License
├── Makefile              # 编译和测试脚本
├── my_clock_gettime.c    # 修改本文件实现my_clock_gettime
├── my_invdiv.h           # 修改本文件实现invdiv
├── README.md             # README文件
├── script                # 绘图脚本
│   ├── avgdiv.py         # 平均性能条形图绘制
│   └── latencydiv.py     # 延迟分布图绘制
├── test_div.c            # invdiv测试程序
└── test_gettime.c        # clock_gettime测试程序
```

请修改`my_invdiv.h`和`my_clock_gettime.c`分别实现`invdiv`和`my_clock_gettime`，并通过以下命令进行编译和测试：
```bash
# Ubuntu执行以下命令安装依赖
sudo apt install make gcc python3 python3-pip
pip3 install matplotlib numpy pandas seaborn

# Fedora执行以下命令安装依赖
sudo dnf install make gcc python3 python3-pip
pip3 install matplotlib numpy pandas seaborn

# Mac OSX执行以下命令安装依赖
brew install make gcc python3 pipx
pipx install matplotlib numpy pandas seaborn --include-deps

# 获取代码
git clone https://github.com/Fix-Point/projOPENVELA-InvariantDivisor

make # 编译测试程序

make test # 绑定核心、提升优先级进行测试，需要sudo权限

make divfigure # 测试并生成性能图表avgdiv.pdf, div_box.pdf和div_violin.pdf
```