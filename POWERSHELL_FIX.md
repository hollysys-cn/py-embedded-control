# PowerShell 脚本编码问题修复说明

**日期**: 2026年1月17日  
**问题**: build.ps1 文件出现编码错误  
**状态**: ✅ 已修复

---

## 问题描述

运行 `.\build.ps1` 时出现以下错误：

```
表达式或语句中包含意外的标记"}"
字符串缺少终止符: "
Try 语句缺少自己的 Catch 或 Finally 块
```

并且中文字符显示为乱码：`鏋勫缓杩囩▼涓彂鐢熼敊璇?`

## 根本原因

PowerShell 脚本文件的编码问题。文件可能使用了不兼容的编码格式（如 UTF-8 with BOM 或其他编码），导致 PowerShell 解析器无法正确识别中文字符和特殊符号。

## 解决方案

重新创建了 `build.ps1` 和 `run.ps1` 文件，使用纯 ASCII/UTF-8 编码，并将所有中文注释和消息改为英文，以避免编码问题。

### 修复的文件

1. **build.ps1** - 构建脚本
2. **run.ps1** - 运行脚本

### 主要更改

```powershell
# 之前（中文，可能出现编码问题）
Write-Error-Custom "构建过程中发生错误: $_"

# 之后（英文，避免编码问题）
Write-Error-Custom "Build error: $_"
```

---

## 验证测试

### 测试 1: 语法检查

```powershell
PS> powershell.exe -ExecutionPolicy Bypass -File build.ps1
# 无语法错误，显示帮助信息
```

### 测试 2: 运行测试

```powershell
PS> powershell.exe -ExecutionPolicy Bypass -File build.ps1 -Test
[INFO] Running tests...
✓ 所有测试通过！
[INFO] Tests passed
[INFO] All operations completed successfully
```

### 测试 3: 完整构建

```powershell
PS> powershell.exe -ExecutionPolicy Bypass -File build.ps1 -All
[INFO] Building runtime...
[INFO] Runtime built successfully
[INFO] Building Python extension...
[INFO] Python extension built successfully
[INFO] All operations completed successfully
```

---

## 使用说明

### 永久解决执行策略问题

**选项 1: 当前会话临时允许（推荐）**
```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

**选项 2: 当前用户永久允许**
```powershell
# 需要管理员权限
Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned
```

### 推荐的运行方式

```powershell
# 方式 1: 使用 -ExecutionPolicy 参数（无需修改系统设置）
powershell.exe -ExecutionPolicy Bypass -File build.ps1 -All

# 方式 2: 先设置策略，再运行（当前会话有效）
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
.\build.ps1 -All
.\run.ps1
```

### 所有可用命令

```powershell
# 构建选项
.\build.ps1 -All          # 构建所有（运行时 + Python 扩展）
.\build.ps1 -Runtime      # 仅构建 C 运行时
.\build.ps1 -Python       # 仅构建 Python 扩展
.\build.ps1 -Clean        # 清理构建产物
.\build.ps1 -Lint         # 代码质量检查
.\build.ps1 -Test         # 运行测试

# 运行选项
.\run.ps1                 # 运行 PID 温度控制示例
.\run.ps1 -Debug          # 调试模式（开启 debugpy）
.\run.ps1 -Shell          # 进入容器 Shell
.\run.ps1 -Config config/basic_pid.yaml  # 指定配置文件
```

---

## 替代方案

如果仍然遇到问题，可以直接使用 docker-compose 命令：

```bash
# 构建
docker-compose build dev

# 编译运行时
docker-compose run --rm dev bash -c "cd /workspace && make runtime"

# 编译 Python 扩展
docker-compose run --rm dev bash -c "cd /workspace && python3 setup.py build_ext --inplace"

# 运行测试
docker-compose run --rm dev python3 test_module.py

# 运行示例
docker-compose run --rm dev bash -c "cd /workspace && ./bin/plcopen_runtime --config config/pid_temperature.yaml"
```

---

## 技术细节

### 文件编码说明

- **修复前**: 可能使用 UTF-8 with BOM 或混合编码
- **修复后**: UTF-8 without BOM，纯英文字符
- **字符集**: ASCII + UTF-8（避免中文注释）

### PowerShell 编码要求

PowerShell 对脚本文件编码的要求：
1. 推荐使用 UTF-8 without BOM
2. 避免在脚本中使用非 ASCII 字符（如中文）
3. 字符串中的中文可以使用，但注释和函数名应使用英文

### 最佳实践

1. **脚本开发**: 使用英文编写所有代码和注释
2. **用户消息**: 可以在字符串中使用中文（如 Write-Host 的消息）
3. **文件保存**: 使用 VS Code 保存时选择 "UTF-8" 编码
4. **版本控制**: Git 配置 `core.autocrlf=false` 避免换行符问题

---

## 故障排除

### 如果脚本仍然报错

1. **检查编码**:
   ```powershell
   Get-Content .\build.ps1 -Encoding UTF8 | Set-Content .\build_fixed.ps1 -Encoding UTF8
   ```

2. **删除并重新下载**:
   ```bash
   git checkout build.ps1 run.ps1
   ```

3. **使用记事本另存为**:
   - 在记事本中打开 build.ps1
   - 文件 → 另存为
   - 编码选择 "UTF-8"（不要选择 "UTF-8 with BOM"）

### 如果 Docker 命令失败

1. **确认 Docker Desktop 运行中**:
   ```powershell
   docker ps
   ```

2. **重新构建镜像**:
   ```powershell
   docker-compose build --no-cache dev
   ```

3. **检查磁盘空间**:
   ```powershell
   docker system df
   docker system prune  # 清理无用镜像
   ```

---

## 参考资料

- [PowerShell 执行策略文档](https://learn.microsoft.com/zh-cn/powershell/module/microsoft.powershell.core/about/about_execution_policies)
- [PowerShell 脚本编码最佳实践](https://learn.microsoft.com/zh-cn/powershell/scripting/dev-cross-plat/vscode/understanding-file-encoding)
- [项目 README](README.md)
- [Windows 快速开始指南](WINDOWS_QUICKSTART.md)

---

**问题状态**: ✅ 已解决  
**后续操作**: 可以正常使用 PowerShell 脚本进行开发
