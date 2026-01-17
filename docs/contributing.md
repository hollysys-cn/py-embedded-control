# 贡献指南

感谢您对 PLCopen Python 运行时环境的关注！本文档说明如何参与项目开发。

## 目录

1. [行为准则](#行为准则)
2. [如何贡献](#如何贡献)
3. [开发环境设置](#开发环境设置)
4. [代码规范](#代码规范)
5. [提交规范](#提交规范)
6. [测试要求](#测试要求)
7. [文档要求](#文档要求)
8. [代码审查流程](#代码审查流程)

---

## 行为准则

### 我们的承诺

为了营造开放和友好的环境，我们承诺：

- **尊重所有贡献者**，无论经验水平如何
- **提供建设性反馈**，帮助他人成长
- **保持专业态度**，避免人身攻击
- **欢迎多样性**，尊重不同观点和经验

### 不可接受的行为

- 使用性别化语言或图像
- 发表挑衅性、侮辱性评论
- 公开或私下骚扰
- 未经许可发布他人私人信息
- 其他不道德或不专业行为

### 执行

违反行为准则的行为可向项目维护者报告，我们将采取适当措施。

---

## 如何贡献

### 报告 Bug

1. **搜索现有 Issue**，确认问题未被报告
2. **创建新 Issue**，包含：
   - 清晰的标题
   - 详细的问题描述
   - 复现步骤
   - 预期行为 vs 实际行为
   - 系统环境（OS、Python 版本、GCC 版本）
   - 相关日志或错误信息

### 建议新功能

1. **搜索现有 Issue**，避免重复
2. **创建 Feature Request**，说明：
   - 功能目的和用途
   - 预期行为
   - 可能的实现方案
   - 相关参考资料

### 提交代码

1. **Fork 仓库**
2. **创建功能分支** (`git checkout -b feature/my-feature`)
3. **编写代码**（遵循代码规范）
4. **添加测试**（覆盖新功能）
5. **更新文档**（如有必要）
6. **提交更改** (`git commit -m 'Add my feature'`)
7. **推送分支** (`git push origin feature/my-feature`)
8. **创建 Pull Request**

---

## 开发环境设置

### 1. 克隆仓库

```bash
git clone https://github.com/hollysys-cn/py-embedded-control.git
cd py-embedded-control
```

### 2. 安装依赖

```bash
# Debian/Ubuntu
sudo apt-get update
sudo apt-get install -y python3 python3-dev python3-pip gcc make

# 安装 Python 开发工具
pip3 install --user pytest pytest-cov pylint flake8 black mypy
```

### 3. 构建项目

```bash
# 构建 C 扩展
make build

# 安装 Python 包（开发模式）
make install

# 构建运行时
make runtime

# 运行测试
make test
```

### 4. Docker 开发

```bash
# 构建 Docker 镜像
docker build -t plcopen-runtime:dev .

# 运行测试
docker run --rm plcopen-runtime:dev make test
```

---

## 代码规范

### C 代码规范

遵循 Linux 内核风格，主要规则：

#### 缩进和格式

```c
// 使用 4 空格缩进（不使用 Tab）
void my_function(int param)
{
    if (param > 0) {
        do_something();
    } else {
        do_other_thing();
    }
}

// 函数定义的大括号单独一行
int my_function(void)
{
    return 0;
}
```

#### 命名规范

```c
// 函数：小写 + 下划线
void calculate_pid_output(void);

// 变量：小写 + 下划线
int current_value;
double setpoint_temperature;

// 常量：大写 + 下划线
#define MAX_BUFFER_SIZE 1024

// 结构体：小写 + 下划线 + _t 后缀
typedef struct pid_state_t {
    double kp;
    double ki;
    double kd;
} pid_state_t;
```

#### 注释规范

```c
/**
 * @brief 计算 PID 输出
 *
 * 使用标准 PID 算法计算控制输出。
 *
 * @param sp 设定值
 * @param pv 过程值
 * @return 控制输出
 */
double calculate_pid(double sp, double pv);

// 单行注释：简短说明
int count = 0;  // 计数器

// 多行注释：详细说明
/*
 * 这里实现了一阶惯性滤波器。
 * 传递函数: G(s) = 1 / (T*s + 1)
 * 离散化: 欧拉法
 */
```

#### 错误处理

```c
// 总是检查指针
void process_data(const char *data)
{
    if (data == NULL) {
        log_error("数据指针为空");
        return;
    }
    // 处理数据
}

// 总是检查 malloc 返回值
char *buffer = malloc(1024);
if (buffer == NULL) {
    log_error("内存分配失败");
    return -1;
}
```

#### 资源管理

```c
// 总是释放资源
void process_file(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        return;
    }

    // 处理文件

    fclose(fp);  // 确保关闭文件
}

// 使用 goto 统一清理（可接受）
int complex_function(void)
{
    char *buf1 = malloc(100);
    char *buf2 = malloc(200);

    if (buf1 == NULL || buf2 == NULL) {
        goto cleanup;
    }

    // 复杂处理

cleanup:
    free(buf1);
    free(buf2);
    return 0;
}
```

### Python 代码规范

遵循 PEP 8 和 PEP 484（类型提示）。

#### 格式化

```python
# 使用 black 自动格式化
black python/

# 行长度: 88 字符（black 默认）
# 缩进: 4 空格
```

#### 命名规范

```python
# 函数和变量：小写 + 下划线
def calculate_pid_output(setpoint: float, process_value: float) -> float:
    current_error = setpoint - process_value
    return current_error

# 类：大驼峰
class PIDController:
    def __init__(self):
        pass

# 常量：大写 + 下划线
MAX_ITERATIONS = 1000
DEFAULT_TIMEOUT = 30.0

# 私有变量：前缀下划线
class MyClass:
    def __init__(self):
        self._private_var = 0
```

#### 类型提示

```python
from typing import Optional, List, Dict

def process_data(
    values: List[float],
    config: Optional[Dict[str, Any]] = None
) -> float:
    """处理数据并返回结果"""
    if config is None:
        config = {}
    return sum(values)
```

#### 文档字符串

```python
def calculate_pid(sp: float, pv: float, dt: float) -> float:
    """计算 PID 控制输出

    使用标准 PID 算法计算控制输出。

    Args:
        sp: 设定值（Setpoint）
        pv: 过程值（Process Value）
        dt: 时间步长（秒）

    Returns:
        float: 控制输出

    Raises:
        ValueError: dt 为负数或零时

    Example:
        >>> output = calculate_pid(25.0, 23.5, 0.1)
        >>> print(output)
        15.0
    """
    if dt <= 0:
        raise ValueError("dt 必须为正数")
    # 实现
```

---

## 提交规范

### Commit Message 格式

```
<type>(<scope>): <subject>

<body>

<footer>
```

### Type 类型

- **feat**: 新功能
- **fix**: 修复 Bug
- **docs**: 文档更新
- **style**: 代码格式（不影响功能）
- **refactor**: 重构（不是新功能或 Bug 修复）
- **perf**: 性能优化
- **test**: 添加或修改测试
- **chore**: 构建过程或辅助工具更新

### 示例

```
feat(pid): 添加抗积分饱和功能

在 PID 控制器中实现 anti-windup 机制，防止积分饱和。

Closes #123
```

```
fix(scheduler): 修复周期时间抖动问题

使用 clock_nanosleep 替代 usleep，提高调度精度。

Fixes #456
```

---

## 测试要求

### C 代码测试

```bash
# 运行所有测试
make test

# 运行特定测试
make test-pid
```

### Python 代码测试

```bash
# 运行 pytest
pytest python/tests/

# 生成覆盖率报告
pytest --cov=python --cov-report=html python/tests/
```

### 测试覆盖率

- **新代码**: 测试覆盖率 >= 80%
- **Bug 修复**: 必须包含回归测试
- **新功能**: 必须包含单元测试和集成测试

---

## 文档要求

### 代码文档

- **C 代码**: Doxygen 风格注释
- **Python 代码**: Google 风格 docstring

### 用户文档

新功能或变更需要更新：

- **README.md**: 项目概述
- **docs/user_guide.md**: 用户手册
- **docs/api_reference.md**: API 参考
- **docs/faq.md**: 常见问题

### 示例代码

提供完整的示例代码，包括：

- 源代码文件
- 配置文件
- 文档说明
- 预期输出

---

## 代码审查流程

### Pull Request 检查清单

提交 PR 前，确认：

- [ ] 代码遵循规范（pylint, flake8, cppcheck）
- [ ] 测试全部通过
- [ ] 测试覆盖率达标
- [ ] 文档已更新
- [ ] Commit message 符合规范
- [ ] 无未解决的冲突
- [ ] CI/CD 通过

### 审查标准

代码审查关注：

1. **正确性**: 代码是否实现了预期功能
2. **可读性**: 代码是否清晰易懂
3. **可维护性**: 代码是否易于修改和扩展
4. **性能**: 是否有明显的性能问题
5. **安全性**: 是否存在安全隐患

### 审查流程

1. **提交 PR**: 创建 Pull Request
2. **自动检查**: CI/CD 运行测试和静态分析
3. **代码审查**: 维护者审查代码
4. **修改反馈**: 根据反馈修改代码
5. **合并**: 审查通过后合并到主分支

---

## 开发技巧

### 本地测试

```bash
# 快速测试（仅运行基本测试）
make test-quick

# 完整测试（包括集成测试）
make test-all

# 性能测试
make test-perf
```

### 调试技巧

```bash
# 使用 GDB 调试 C 代码
gdb --args bin/plcopen_runtime --config config/test.yaml

# 使用 pdb 调试 Python 代码
python3 -m pdb python/examples/my_script.py
```

### 性能分析

```bash
# C 代码性能分析
valgrind --tool=callgrind bin/plcopen_runtime --config config/test.yaml

# Python 代码性能分析
python3 -m cProfile -o profile.stats python/examples/my_script.py
```

---

## 许可证

所有贡献代码将采用 [MIT License](../LICENSE)。

提交代码即表示您同意以 MIT License 授权您的贡献。

---

## 获取帮助

- **文档**: [README.md](../README.md)
- **用户手册**: [docs/user_guide.md](user_guide.md)
- **API 参考**: [docs/api_reference.md](api_reference.md)
- **GitHub Issues**: https://github.com/hollysys-cn/py-embedded-control/issues
- **邮件列表**: dev@hollysys.cn

---

感谢您的贡献！
