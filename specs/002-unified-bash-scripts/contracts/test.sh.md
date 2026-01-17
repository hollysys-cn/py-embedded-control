# 脚本接口契约: test.sh

**版本**: 1.0.0 | **日期**: 2026年1月18日

## 概述

`test.sh` 负责运行项目的质量检查，包括静态分析（linting）和单元测试。

## 接口规范

### 命令签名

```bash
./test.sh [OPTIONS]
```

### 参数

| 参数 | 短选项 | 类型 | 必需 | 默认值 | 说明 |
|------|--------|------|------|--------|------|
| --lint | 无 | 标志 | 否 | false | 仅运行静态分析 |
| --unit | 无 | 标志 | 否 | false | 仅运行单元测试 |
| --help | -h | 标志 | 否 | false | 显示帮助信息并退出 |

**参数组合规则**:
- 无参数：运行所有测试（lint + unit）
- 仅 --lint：运行静态分析
- 仅 --unit：运行单元测试
- --lint --unit：等同于无参数

### 前置条件

1. **必需工具**:
   - `docker` 命令可用
   - `docker-compose` 命令可用

2. **必需状态**:
   - Docker 镜像 `plcopen-dev` 已构建（通过 `build.sh`）
   - 项目已编译（通过 `build.sh`）

3. **必需文件**:
   - `tests/` 目录存在
   - `tests/unit/` 目录包含测试用例

### 后置条件（成功时）

1. **测试执行**: 所有指定的测试已执行
2. **测试通过**: 所有测试用例通过
3. **退出状态**: 0

### 失败条件

| 失败场景 | 错误消息 | 退出码 |
|---------|---------|--------|
| 静态分析发现问题 | flake8 错误信息 | 1 |
| 单元测试失败 | pytest 失败信息 | 1 |
| Docker 镜像未构建 | Docker 错误信息 | 1 |
| 未知参数 | `[ERROR] Unknown option: <参数>` | 2 |

### 副作用

1. **文件系统**:
   - 可能创建 `.pytest_cache/` 目录
   - 可能创建 `htmlcov/` 目录（如果运行覆盖率）

2. **Docker 容器**: 在现有容器中执行测试命令

### 输出规范

#### 标准输出（所有测试）

```
[INFO] Running static analysis...
[INFO] Running flake8...
[INFO] Static analysis passed
[INFO] Running unit tests...
============================= test session starts ==============================
collected 25 items

tests/unit/test_pid.py .....                                             [ 20%]
tests/unit/test_ramp.py .....                                            [ 40%]
...
============================== 25 passed in 0.50s ==============================
[INFO] All tests passed
```

#### 标准输出（仅 lint）

```
[INFO] Running static analysis...
[INFO] Running flake8...
[INFO] Static analysis passed
```

#### 标准输出（仅 unit）

```
[INFO] Running unit tests...
============================= test session starts ==============================
...
============================== 25 passed in 0.50s ==============================
[INFO] All tests passed
```

#### 标准错误（失败 - lint）

```
[ERROR] Static analysis failed
python/plcopen/blocks.py:42:80: E501 line too long (85 > 79 characters)
python/plcopen/blocks.py:55:1: F401 'sys' imported but unused
```

#### 标准错误（失败 - unit）

```
[ERROR] Unit tests failed
============================= FAILURES ====================================
__________________________ test_pid_output ________________________________
...
```

### 性能特征

- **静态分析**: 2-5 秒
- **单元测试**: 5-10 秒
- **所有测试**: 7-15 秒

### 并发约束

- 支持多个 `test.sh` 实例并行运行（pytest 有文件锁）
- 建议在 CI 中顺序执行以避免资源竞争

### 幂等性

- **幂等**: 可以重复执行，不影响系统状态
- **无副作用**: 不修改源代码或配置

## 使用示例

### 示例 1: 运行所有测试

```bash
$ ./test.sh
[INFO] Running static analysis...
[INFO] Running flake8...
[INFO] Static analysis passed
[INFO] Running unit tests...
============================= test session starts ==============================
collected 25 items
tests/unit/test_pid.py .....                                             [ 20%]
tests/unit/test_ramp.py .....                                            [ 40%]
============================== 25 passed in 0.50s ==============================
[INFO] All tests passed
```

### 示例 2: 仅运行静态分析

```bash
$ ./test.sh --lint
[INFO] Running static analysis...
[INFO] Running flake8...
[INFO] Static analysis passed
```

### 示例 3: 仅运行单元测试

```bash
$ ./test.sh --unit
[INFO] Running unit tests...
============================= test session starts ==============================
collected 25 items
tests/unit/test_pid.py .....                                             [ 20%]
============================== 25 passed in 0.50s ==============================
[INFO] All tests passed
```

### 示例 4: 测试失败

```bash
$ ./test.sh
[INFO] Running static analysis...
[INFO] Running flake8...
[ERROR] Static analysis failed
python/plcopen/blocks.py:42:80: E501 line too long (85 > 79 characters)
```

### 示例 5: 显示帮助

```bash
$ ./test.sh --help
Usage: ./test.sh [OPTIONS]

运行项目的质量检查和测试

OPTIONS:
    --lint      仅运行静态分析（flake8）
    --unit      仅运行单元测试（pytest）
    --help      显示此帮助信息

EXAMPLES:
    ./test.sh           # 运行所有测试
    ./test.sh --lint    # 仅静态分析
    ./test.sh --unit    # 仅单元测试
```

## 集成测试场景

### 测试 1: 所有测试通过

**前置条件**: 已构建项目，代码质量良好
**执行**: `./test.sh`
**验证**:
- 退出码 = 0
- 输出包含 "Static analysis passed"
- 输出包含 "X passed"

### 测试 2: 静态分析失败

**前置条件**: 已构建项目，代码有 flake8 错误
**执行**: `./test.sh --lint`
**验证**:
- 退出码 = 1
- 输出包含 flake8 错误信息

### 测试 3: 单元测试失败

**前置条件**: 已构建项目，某个测试用例失败
**执行**: `./test.sh --unit`
**验证**:
- 退出码 = 1
- 输出包含 pytest 失败信息

### 测试 4: 未构建项目

**前置条件**: 未运行 build.sh
**执行**: `./test.sh`
**验证**:
- 退出码 = 1
- 错误提示镜像未构建或模块未编译

## 依赖关系

### 直接依赖

- Docker Engine
- Docker Compose
- Docker 镜像 `plcopen-dev`（由 build.sh 创建）

### 间接依赖

- pytest（容器内）
- flake8（容器内）
- shellcheck（可选，用于检查 Bash 脚本自身）

### 依赖其他脚本

- **必须先运行**: `build.sh`（构建项目和镜像）

## 质量标准

### 静态分析标准

- flake8 配置：遵循 `.flake8` 文件中的规则
- 最大行长度：79 字符（PEP 8）
- 无未使用的导入
- 无未定义的变量

### 单元测试标准

- 所有测试用例必须通过
- 测试覆盖率 ≥ 80%（目标）
- 测试执行时间 < 30 秒

## 变更历史

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0.0 | 2026-01-18 | 初始版本，整合多个测试脚本 |
