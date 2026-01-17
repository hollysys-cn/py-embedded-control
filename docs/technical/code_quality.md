# 代码质量检查说明

## C/C++ 静态分析 (cppcheck)

### 安装 cppcheck

#### Windows
```powershell
# 使用 Chocolatey
choco install cppcheck

# 或从官网下载: http://cppcheck.sourceforge.net/
```

#### Linux
```bash
# Debian/Ubuntu
sudo apt-get install cppcheck

# CentOS/RHEL
sudo yum install cppcheck

# 从源码编译（获取最新版本）
git clone https://github.com/danmar/cppcheck.git
cd cppcheck
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

#### Docker
```bash
# 使用 Docker 镜像
docker run --rm -v $(pwd):/src neszt/cppcheck-docker --enable=all --std=c11 /src/src/
```

### 运行检查

#### 基本检查
```bash
cppcheck --enable=all --std=c11 src/
```

#### 详细检查（推荐）
```bash
cppcheck \
  --enable=all \
  --std=c11 \
  --suppress=missingIncludeSystem \
  --suppress=unusedFunction \
  --inline-suppr \
  --output-file=cppcheck_report.txt \
  src/
```

#### 仅检查错误和警告
```bash
cppcheck --enable=warning,error --std=c11 src/
```

### 检查项目

- **error**: 代码错误（如空指针解引用、内存泄漏）
- **warning**: 代码警告（如未使用变量、类型转换）
- **style**: 代码风格问题
- **performance**: 性能问题
- **portability**: 可移植性问题
- **information**: 一般信息

### 抑制规则

项目中已知的可接受问题：

```c
// 抑制未使用函数警告（公共 API）
// cppcheck-suppress unusedFunction
void public_api_function(void);

// 抑制空指针警告（已在调用前检查）
// cppcheck-suppress nullPointer
ptr->value = 10;
```

### 预期结果

本项目的 C 代码应通过以下检查：

- ✅ 无内存泄漏
- ✅ 无空指针解引用
- ✅ 无缓冲区溢出
- ✅ 无未初始化变量
- ✅ 无资源泄漏（文件句柄、互斥锁）

### 常见问题修复

#### 内存泄漏
```c
// 错误
char *str = malloc(100);
if (error) return;  // 泄漏

// 正确
char *str = malloc(100);
if (error) {
    free(str);
    return;
}
```

#### 空指针检查
```c
// 错误
void func(char *str) {
    printf("%s", str);  // 可能为 NULL
}

// 正确
void func(char *str) {
    if (str == NULL) return;
    printf("%s", str);
}
```

---

## Python 静态分析

### 安装工具

```bash
pip install pylint flake8 black mypy
```

### pylint 检查

```bash
# 基本检查
pylint python/

# 生成报告
pylint --output-format=text python/ > pylint_report.txt

# 使用配置文件
pylint --rcfile=.pylintrc python/
```

### flake8 检查

```bash
# 基本检查
flake8 python/

# 忽略特定错误
flake8 --ignore=E501,W503 python/

# 使用配置文件
flake8 --config=.flake8 python/
```

### black 格式化

```bash
# 检查格式
black --check python/

# 自动格式化
black python/
```

### mypy 类型检查

```bash
# 类型检查
mypy python/
```

### 预期结果

Python 代码应达到：

- ✅ pylint 评分 >= 9.0/10
- ✅ flake8 无错误
- ✅ black 格式一致
- ✅ mypy 类型正确

---

## 持续集成

### GitHub Actions 示例

```yaml
name: Code Quality

on: [push, pull_request]

jobs:
  cppcheck:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install cppcheck
        run: sudo apt-get install -y cppcheck
      - name: Run cppcheck
        run: cppcheck --enable=all --std=c11 --error-exitcode=1 src/

  pylint:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Setup Python
        uses: actions/setup-python@v2
        with:
          python-version: 3.13
      - name: Install dependencies
        run: pip install pylint
      - name: Run pylint
        run: pylint --fail-under=9.0 python/
```

---

## 代码审查检查清单

### C 代码

- [ ] 所有指针使用前检查 NULL
- [ ] 所有 malloc/calloc 后检查返回值
- [ ] 所有资源（内存、文件、互斥锁）正确释放
- [ ] 数组访问不越界
- [ ] 字符串操作使用安全函数（strncpy, snprintf）
- [ ] 避免使用未初始化变量
- [ ] 函数返回值正确处理
- [ ] 避免使用 goto（除非必要）
- [ ] 代码注释清晰（中英文）

### Python 代码

- [ ] 函数有 docstring
- [ ] 类型提示（type hints）
- [ ] 异常处理适当
- [ ] 避免全局变量（除非必要）
- [ ] 命名符合 PEP 8
- [ ] 行长度 <= 88 字符（black 默认）
- [ ] 导入语句排序正确
- [ ] 无未使用导入
- [ ] 代码注释清晰（中英文）

---

**最后更新:** 2026-01-01
