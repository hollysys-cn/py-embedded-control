# 脚本接口契约: run.sh

**版本**: 1.0.0 | **日期**: 2026年1月18日

## 概述

`run.sh` 负责在 Docker 容器中运行 PLCopen Python Runtime 应用程序，或提供交互式 Shell。

## 接口规范

### 命令签名

```bash
./run.sh [CONFIG_FILE] [OPTIONS]
```

### 参数

| 参数 | 短选项 | 类型 | 必需 | 默认值 | 说明 |
|------|--------|------|------|--------|------|
| CONFIG_FILE | 无 | 位置参数 | 否 | config/pid_temperature.yaml | 应用配置文件路径 |
| --shell | 无 | 标志 | 否 | false | 进入容器 Shell 而非运行应用 |
| --help | -h | 标志 | 否 | false | 显示帮助信息并退出 |

### 前置条件

1. **必需工具**:
   - `docker` 命令可用
   - `docker-compose` 命令可用

2. **必需状态**:
   - Docker 镜像 `plcopen-dev` 已构建（通过 `build.sh`）
   - Docker 守护进程正在运行

3. **文件要求**（非 --shell 模式）:
   - 指定的配置文件必须存在且可读

### 后置条件（成功时）

#### 运行模式（无 --shell）

1. **应用启动**: Python Runtime 应用在容器中运行
2. **日志输出**: 应用日志输出到终端
3. **退出状态**: 0（应用正常退出）或 130（用户中断）

#### Shell 模式（--shell）

1. **Shell 启动**: 进入容器的交互式 Bash Shell
2. **工作目录**: `/workspace`（容器内项目根目录）
3. **退出状态**: Shell 退出码

### 失败条件

| 失败场景 | 错误消息 | 退出码 |
|---------|---------|--------|
| 配置文件不存在 | `[ERROR] Configuration file not found: <文件路径>` | 1 |
| Docker 镜像未构建 | Docker 错误信息 | 1 |
| 应用运行错误 | Python 应用错误信息 | 1 |
| 未知参数 | `[ERROR] Unknown option: <参数>` | 2 |

### 副作用

1. **Docker 容器**:
   - 启动容器 `plcopen-dev`（如未运行）
   - 容器在应用退出后继续运行（用于重复执行）

2. **文件系统**: 无副作用（只读操作）

### 输出规范

#### 标准输出（运行模式）

```
[INFO] Starting application with config: config/pid_temperature.yaml
[INFO] Container is ready
<应用日志输出>
```

#### 标准输出（Shell 模式）

```
[INFO] Entering container shell...
[INFO] Type 'exit' to leave the shell
root@container:/workspace#
```

#### 标准错误（失败）

```
[ERROR] <错误描述>
```

### 性能特征

- **容器启动**: 1-3 秒（首次启动）
- **应用启动**: < 1 秒
- **Shell 响应**: 即时

### 并发约束

- 支持多个 `run.sh` 实例（Docker 容器可以运行多个进程）
- Shell 模式下，每个 Shell 会话独立

### 幂等性

- **幂等**: 可以重复执行，不影响系统状态
- **容器重用**: 如容器已运行，直接在其中执行命令

## 使用示例

### 示例 1: 使用默认配置运行

```bash
$ ./run.sh
[INFO] Starting application with config: config/pid_temperature.yaml
[INFO] Container is ready
[2026-01-18 10:30:00] INFO: PLCopen Runtime starting...
[2026-01-18 10:30:00] INFO: Loading configuration...
[2026-01-18 10:30:01] INFO: PID controller initialized
...
```

### 示例 2: 使用自定义配置运行

```bash
$ ./run.sh config/custom.yaml
[INFO] Starting application with config: config/custom.yaml
[INFO] Container is ready
[2026-01-18 10:30:00] INFO: PLCopen Runtime starting...
...
```

### 示例 3: 进入容器 Shell

```bash
$ ./run.sh --shell
[INFO] Entering container shell...
[INFO] Type 'exit' to leave the shell
root@plcopen-dev:/workspace# ls
bin  build  config  docs  python  src  tests
root@plcopen-dev:/workspace# exit
exit
```

### 示例 4: 配置文件不存在

```bash
$ ./run.sh config/missing.yaml
[ERROR] Configuration file not found: config/missing.yaml
```

### 示例 5: 显示帮助

```bash
$ ./run.sh --help
Usage: ./run.sh [CONFIG_FILE] [OPTIONS]

在 Docker 容器中运行 PLCopen Python Runtime 应用

ARGUMENTS:
    CONFIG_FILE     配置文件路径（默认：config/pid_temperature.yaml）

OPTIONS:
    --shell         进入容器 Shell 而非运行应用
    --help          显示此帮助信息

EXAMPLES:
    ./run.sh                        # 使用默认配置
    ./run.sh config/custom.yaml     # 使用自定义配置
    ./run.sh --shell                # 进入容器 Shell
```

## 集成测试场景

### 测试 1: 标准运行

**前置条件**: 已构建项目，config/pid_temperature.yaml 存在
**执行**: `./run.sh`
**验证**:
- 退出码 = 0 或 130（用户中断）
- 输出包含应用日志
- 应用正常启动和运行

### 测试 2: 自定义配置

**前置条件**: 已构建项目，config/test.yaml 存在
**执行**: `./run.sh config/test.yaml`
**验证**:
- 退出码 = 0
- 应用使用 test.yaml 配置

### 测试 3: Shell 模式

**前置条件**: 已构建项目
**执行**: `./run.sh --shell`（在 Shell 中执行 `pwd` 和 `exit`）
**验证**:
- 进入容器 Shell
- 工作目录 = /workspace
- 可以执行命令

### 测试 4: 配置文件不存在

**前置条件**: config/nonexistent.yaml 不存在
**执行**: `./run.sh config/nonexistent.yaml`
**验证**:
- 退出码 = 1
- 错误消息提示文件不存在

## 依赖关系

### 直接依赖

- Docker Engine
- Docker Compose
- Docker 镜像 `plcopen-dev`（由 build.sh 创建）

### 间接依赖

- Python 3.11+（容器内）
- PLCopen Python 库（容器内）
- YAML 配置文件（应用依赖）

### 依赖其他脚本

- **必须先运行**: `build.sh`（构建 Docker 镜像）

## 与外部系统的交互

### Docker

- 通过 `docker-compose up -d` 启动容器
- 通过 `docker exec` 在容器中执行命令
- 容器端口映射（由 docker-compose.yml 定义）

### 文件系统

- 读取配置文件（只读）
- 应用可能写入日志文件（通过 Volume 挂载）

## 变更历史

| 版本 | 日期 | 变更说明 |
|------|------|---------|
| 1.0.0 | 2026-01-18 | 初始版本，从 PowerShell 迁移 |
