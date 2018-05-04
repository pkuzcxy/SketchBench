运行过程请查看 `run.sh`

---

整个工程分为 config、dataset、hash、sketch、task 五大结构，结构之间相互独立：

* config 为 sketch 配置文件，如 hash num、数组大小等。

* dataset 用于读取数据集、输出统计信息等。

* sketch 使用 config 初始化，支持插入、查询。

* task 执行任务，每次测试一种 sketch 在一种 dataset 上的表现。

* hash 支持设置种子、计算哈希的功能。

实现细节：

* “相互独立”意味着实现某个结构的时候不要引入其他结构的信息。例如实现 sketch 时，不要 `#include "BOBHash.h`，而是利用模版 `template<class Hash>` 来实现。

* 为了确保每个结构实现了基础功能，在定义一个新的 class 时要继承自抽象基类。如 `class CountMinSketch: public BaseSketch`。上述五个结构的基类均已实现，用 BaseXX 开头。

* 为了相互独立，基本上每实现一个新的 class 除了引入基类的头文件不需要引入任何头文件（task 需要输出、文件操作的除外）。

* 在所有文件中尽量不要 #define 或设置全局变量等，以免污染命名空间或是造成不必要的相互依赖。

