# Windows 环境快速开始指南

本指南面向 Windows 用户，使用 Docker Desktop 快速启动和测试 PLCopen Python 运行时环境。

## 前提条件

1. **Docker Desktop** - [下载安装](https://www.docker.com/products/docker-desktop)
2. **Git** - [下载安装](https://git-scm.com/downloads)
3. **VS Code**（可选，用于调试）- [下载安装](https://code.visualstudio.com/)

## 第一步：克隆项目

```powershell
# 打开 PowerShell
cd $HOME\github
git clone https://github.com/hollysys-cn/py-embedded-control.git
cd py-embedded-control
```

## 第二步：构建项目

```powershell
# 构建所有组件（运行时 + Python 扩展）
.\build.ps1 -All
```

**预期输出**：
```
[INFO] 构建 Docker 镜像...
[INFO] Docker 镜像构建成功
[INFO] 构建运行时程序...
Building runtime executable...
gcc -std=c11 ... (编译命令)
[INFO] 运行时构建成功
[INFO] 构建 Python 扩展...
[INFO] Python 扩展构建成功
[INFO] 所有操作完成
```

**构建时间**：首次构建约 2-5 分钟（需要下载 Docker 镜像）

**如果遇到编译警告**：
- `warning: 'strncpy' output may be truncated` - 已在最新版本修复，请更新代码
- 其他警告通常可以忽略

**如果遇到错误**：
- 确保 Docker Desktop 正在运行
- 检查网络连接（需要下载基础镜像）
- 查看错误信息并参考[故障排除](#故障排除)

## 第三步：运行示例

### 3.1 运行 PID 温度控制示例

```powershell
.\run.ps1
```

**第一次运行时的输出**：
```
[INFO] Starting development container...
[+] Running 1/1
 ✔ Container plcopen-dev  Started
[INFO] Starting runtime: config/pid_temperature.yaml
[INFO] Press Ctrl+C to stop
[INFO] Note: Python output will be shown in real-time

PID 温度控制初始化完成
  目标温度: 25.0°C
  初始温度: 20.0°C
  PID 参数: Kp=3.0, Ki=0.2, Kd=0.5
------------------------------------------------------------
周期   10 | 温度: 20.45°C | 误差:  4.554°C | 控制输出:  13.7%
周期   20 | 温度: 20.89°C | 误差:  4.112°C | 控制输出:  12.3%
周期   30 | 温度: 21.31°C | 误差:  3.689°C | 控制输出:  11.1%
...
```

**观察**：
- 首次运行会启动后台开发容器（plcopen-dev）
- Python 脚本的 `print()` 输出实时显示
- 温度值逐渐上升，接近目标温度 25°C
- PID 控制器自动调整输出功率
- 每 10 个周期（1秒）输出一次状态

**停止程序**：按 `Ctrl+C`（现在可以正常终止了！）

**重要说明**：
- 开发容器会在后台保持运行，下次运行 `.\run.ps1` 时会更快
- 如需停止容器：`docker-compose down`
- 查看容器状态：`docker ps`
- 日志文件位于容器内的 `logs/pid_temperature.log`
- **如果看不到 Python 输出**：请确保已重新构建项目（应用了输出刷新修复）：`.\build.ps1 -Runtime`

### 3.2 运行级联控制示例

```powershell
.\run.ps1 -Config config/cascade_control.yaml
```

## 第四步：调试功能（可选）

### 4.1 启动调试模式

```powershell
.\run.ps1 -Debug
```

**预期输出**：
```
[INFO] 使用调试配置: config/pid_temperature_debug.yaml
[INFO] 调试端口: 5678
[INFO] VS Code: 按 F5 选择 'Python: 附加到运行时'
[INFO] 启动运行时...
[INFO] 启动 debugpy 服务器: 0.0.0.0:5678
[INFO] debugpy 服务器已启动，等待调试器连接...
```

### 4.2 使用 VS Code 附加调试器

1. **在 VS Code 中打开项目**
   ```powershell
   code .
   ```

2. **打开 Python 脚本**
   - 文件：`python/examples/pid_temperature_control.py`

3. **设置断点**
   - 点击行号左侧，在 `step()` 函数第一行设置断点

4. **启动调试**
   - 按 `F5`
   - 选择配置：**"Python: 附加到运行时（本地）"**

5. **调试操作**
   - 程序会在断点处暂停
   - 查看变量值（左侧"变量"面板）
   - 使用调试工具栏：
     - **F5** - 继续
     - **F10** - 单步跳过
     - **F11** - 单步进入
   - 在调试控制台输入 Python 表达式：
     ```python
     >>> current_temp
     72.5
     >>> output
     45.8
     ```

## 第五步：代码质量检查（可选）

```powershell
# 运行代码质量检查
.\build.ps1 -Lint
```

**预期输出**：
```
[INFO] 运行代码质量检查...
==> flake8
（无输出表示通过）

==> black (check only)
All done! ✨ 🍰 ✨
XX files would be left unchanged.

==> pylint
（评分显示）

[INFO] 代码质量检查通过
```

## 第六步：进入开发环境（可选）

```powershell
.\run.ps1 -Shell
```

这会打开一个 Bash Shell，在 Docker 容器内工作：

```bash
# 查看文件结构
ls -la

# 手动编译
make runtime

# 运行测试
./tests/test_error_handling.sh

# 退出
exit
```

---

## 常用命令参考

### 构建命令

```powershell
# 构建所有
.\build.ps1 -All

# 仅构建运行时
.\build.ps1 -Runtime

# 仅构建 Python 扩展
.\build.ps1 -Python

# 清理构建产物
.\build.ps1 -Clean

# 代码质量检查
.\build.ps1 -Lint
```

### 运行命令

```powershell
# 运行默认配置
.\run.ps1

# 运行指定配置
.\run.ps1 -Config config/cascade_control.yaml

# 运行调试模式
.\run.ps1 -Debug

# 进入开发环境
.\run.ps1 -Shell
```

---

## 故障排除

### 问题 1：Docker Desktop 未运行

**错误信息**：
```
[ERROR] Docker 未安装或未运行
```

**解决方法**：
1. 启动 Docker Desktop
2. 等待 Docker 完全启动（任务栏图标显示绿色）
3. 重新运行命令

### 问题 2：Docker 镜像构建失败

**错误信息**：
```
ERROR: failed to solve: ...
```

**解决方法**：
1. 检查网络连接
2. 清理 Docker 缓存：
   ```powershell
   docker system prune -a
   ```
3. 重新构建：
   ```powershell
   .\build.ps1 -All
   ```

### 问题 3：端口被占用

**错误信息**：
```
Error: port 5678 is already in use
```

**解决方法**：
1. 停止占用端口的程序
2. 或修改配置文件中的端口号（`debug.port`）

### 问题 4：Ctrl+C 无法停止程序

**症状**：程序卡住，Ctrl+C 无响应

**解决方法**：
1. 打开新的 PowerShell 窗口
2. 执行：
   ```powershell
   docker-compose down
   ```
3. 重新运行 `.\run.ps1`

**注意**：最新版本已修复此问题，使用 `docker exec` 代替 `docker-compose run`

### 问题 5：看不到程序输出

**症状**：运行后只看到容器启动信息，没有 Python 输出

**解决方法**：
1. 确保已重新构建项目（需要应用 Python 缓冲修复）：
   ```powershell
   .\build.ps1 -Runtime
   ```
2. 检查容器日志：
   ```powershell
   docker logs plcopen-dev
   ```
3. 进入容器手动运行：
   ```powershell
   .\run.ps1 -Shell
   ./bin/plcopen_runtime --config config/pid_temperature.yaml
   ```

### 问题 6：调试器连接失败

**症状**：VS Code 显示"无法连接到调试器"

**解决方法**：
1. 确保运行时正在运行调试模式
2. 检查配置文件 `.vscode/launch.json` 中的端口与运行时一致
3. 检查防火墙设置
4. 尝试重启运行时和 VS Code

### 问题 5：PowerShell 脚本执行受限

**错误信息**：
```
.\build.ps1 : 无法加载文件 .\build.ps1，因为在此系统上禁止运行脚本
所在位置 行:1 字符: 1
+ .\build.ps1 -All
+ ~~~~~~~~~~~
    + CategoryInfo          : SecurityError: (:) []，PSSecurityException
    + FullyQualifiedErrorId : UnauthorizedAccess
```

**解决方法（选择其一）**：

#### 方法 1：使用 .bat 包装器（推荐，最简单）✅

项目提供了 `.bat` 文件自动绕过执行策略限制：

```cmd
# 使用 .bat 替代 .ps1
build.bat -All        # 替代 .\build.ps1 -All
run.bat               # 替代 .\run.ps1
start-debug.bat       # 替代 .\start-debug.ps1
```

**优点**：无需修改系统设置，开箱即用

#### 方法 2：临时绕过（单次执行）

```powershell
# 方式 A：直接绕过
PowerShell -ExecutionPolicy Bypass -File .\build.ps1 -All

# 方式 B：仅对当前会话设置
Set-ExecutionPolicy -ExecutionPolicy Bypass -Scope Process
.\build.ps1 -All
```

**优点**：不影响系统设置，执行后自动恢复

#### 方法 3：永久设置（当前用户）

```powershell
# 以管理员身份运行 PowerShell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser

# 然后正常运行
.\build.ps1 -All
```

**优点**：一次设置，永久生效
**说明**：`RemoteSigned` 策略允许运行本地脚本，从互联网下载的脚本需要数字签名

#### 执行策略说明

| 策略 | 说明 | 适用场景 |
|------|------|----------|
| `Restricted` | 默认策略，禁止所有脚本 | Windows 默认设置 |
| `RemoteSigned` | 本地脚本可运行，远程脚本需签名 | 开发环境（推荐）|
| `Bypass` | 无任何限制 | 临时执行 |
| `Unrestricted` | 所有脚本可运行，远程脚本会警告 | 不推荐 |

**推荐**：开发时使用**方法 1**（.bat 包装器）或**方法 3**（RemoteSigned）

---

## 下一步

完成快速开始后，可以：

1. **阅读用户指南**：`docs/user_guide.md`
2. **学习 API 参考**：`docs/api_reference.md`
3. **查看调试指南**：`docs/debugging.md`
4. **编写自己的控制脚本**：参考 `python/examples/`
5. **运行性能测试**：
   ```powershell
   .\run.ps1 -Shell
   python tests/benchmark/cycle_stability.py
   ```

---

## 获取帮助

- **文档**: `docs/` 目录
- **常见问题**: `docs/faq.md`
- **验证检查清单**: `VERIFICATION_CHECKLIST.md`
- **GitHub Issues**: [报告问题](https://github.com/hollysys-cn/py-embedded-control/issues)

---

**祝您使用愉快！** 🎉
