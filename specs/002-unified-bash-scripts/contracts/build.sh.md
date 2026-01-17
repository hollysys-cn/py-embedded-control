# 脚本接口契约: build.sh

**版本**: 1.0.0 | **日期**: 2026年1月18日

## 概述

`build.sh` 负责构建项目，包括 Docker 镜像构建和容器内的 C/Python 编译。

## 接口规范

### 命令签名

```bash
./build.sh [OPTIONS]
```

### 参数

| 参数 | 短选项 | 类型 | 必需 | 默认值 | 说明 |
|------|--------|------|------|--------|------|
| --clean | 无 | 标志 | 否 | false | 清理构建产物后再构建 |
| --help | -h | 标志 | 否 | false | 显示帮助信息并退出 |

### 前置条件

1. **必需工具**:
   - `docker` 命令可用
   - `docker-compose` 命令可用
   - 当前用户有权限执行 Docker 命令

2. **必需文件**:
   - `Dockerfile` 存在于项目根目录
   - `docker-compose.yml` 存在于项目根目录
   - `Makefile` 存在于项目根目录

3. **环境状态**:
   - Docker 守护进程正在运行
   - 项目根目录有写权限（用于创建 build/ 和 bin/）

### 后置条件（成功时）

1. **Docker 镜像**:
   - 镜像 `plcopen-dev` 已构建或更新
   - 镜像包含所有必需的开发工具和依赖

2. **编译产物**:
   - `bin/plcopen_runtime` 可执行文件存在
   - `build/lib.*.so` Python 扩展模块存在

3. **退出状态**: 0

### 失败条件

| 失败场景 | 错误消息 | 退出码 |
|---------|---------|--------|
| Docker 未安装或未运行 | 依赖 Docker 命令失败信息 | 1 |
| docker-compose.yml 不存在 | Docker Compose 错误信息 | 1 |
| 编译错误 | Make 或编译器错误信息 | 1 |
| 未知参数 | `[ERROR] Unknown option: <参数>` | 2 |

### 副作用

1. **文件系统更改**:
   - 创建/更新 `build/` 目录
   - 创建/更新 `bin/` 目录
   - 创建/更新 Docker 镜像层（在 Docker 存储中）

2. **--clean 参数额外副作用**:
   - 删除 `build/` 目录所有内容
   - 删除 `bin/` 目录所有内容
   - 在容器中执行 `make clean`

### 输出规范

#### 标准输出（成功）

```
[INFO] Building Docker image...
[INFO] Docker image built successfully
[INFO] Building project in container...
[INFO] Build completed successfully
```

#### 标准输出（--clean）

```
[INFO] Cleaning build artifacts...
[INFO] Build artifacts cleaned
[INFO] Building Docker image...
[INFO] Docker image built successfully
[INFO] Building project in container...
[INFO] Build completed successfully
```

#### 标准错误（失败）

```
[ERROR] <错误描述>
```

### 性能特征

- **首次构建**: 3-5 分钟（下载基础镜像和依赖）
- **增量构建**: 10-30 秒（仅编译更改的文件）
- **--clean 构建**: 30-60 秒（重新编译所有文件）

### 并发约束

- 不支持多个 `build.sh` 实例并行运行
- Docker 镜像构建会自动加锁（Docker 内部机制）

### 幂等性

- **非幂等**: 每次执行可能重新编译文件
- **建议**: 在 CI/CD 中始终使用 `--clean` 确保干净构建

## 使用示例

### 示例 1: 标准构建

```bash
$ ./build.sh
[INFO] Building Docker image...
[INFO] Docker image built successfully
[INFO] Building project in container...
[INFO] Build completed successfully
```

### 示例 2: 清理后构建

```bash
$ ./build.sh --clean
[INFO] Cleaning build artifacts...
[INFO] Build artifacts cleaned
[INFO] Building Docker image...
[INFO] Docker image built successfully
[INFO] Building project in container...
[INFO] Build completed successfully
```

### 示例 3: 显示帮助

```bash
$ ./build.sh --help
Usage: ./build.sh [OPTIONS]

构建 PLCopen Python Runtime 项目

OPTIONS:
    --clean     清理构建产物后再构建
    --help      显示此帮助信息

EXAMPLES:
    ./build.sh              # 标准构建
    ./build.sh --clean      # 清理后构建
```

### 示例 4: 错误处理

```bash
$ ./build.sh --unknown
[ERROR] Unknown option: --unknown
Use --help for usage information
```

## 集成测试场景

### 测试 1: 首次构建

**前置条件**: 从未构建过项目
**执行**: `./build.sh`
**验证**:
- 退出码 = 0
- `bin/plcopen_runtime` 文件存在
- `docker images | grep plcopen-dev` 有结果

### 测试 2: 增量构建

**前置条件**: 已构建过项目，修改一个 C 文件
**执行**: `./build.sh`
**验证**:
- 退出码 = 0
- 构建时间 < 60 秒
- 修改的文件被重新编译

### 测试 3: 清理构建

**前置条件**: 已构建过项目
**执行**: `./build.sh --clean`
**验证**:
- 退出码 = 0
- `build/` 目录被清空后重建
- 所有文件被重新编译

## 依赖关系

### 直接依赖

- Docker Engine
- Docker Compose
- Makefile（通过容器调用）

### 间接依赖

- GCC/Clang（容器内）
- Python 开发头文件（容器内）
- Make 工具（容器内）

## 变更历史

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0.0 | 2026-01-18 | 初始版本，从 PowerShell 迁移 |
