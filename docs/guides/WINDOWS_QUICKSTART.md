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
[INFO] 运行时构建成功
[INFO] 构建 Python 扩展...
[INFO] Python 扩展构建成功
[INFO] 所有操作完成
```

**如果遇到错误**：
- 确保 Docker Desktop 正在运行
- 检查网络连接（需要下载基础镜像）
- 查看错误信息并参考[故障排除](#故障排除)

## 第三步：运行示例

### 3.1 运行 PID 温度控制示例

```powershell
.\run.ps1
```

**预期输出**：
```
[INFO] 启动运行时: config/pid_temperature.yaml
[INFO] 按 Ctrl+C 停止运行
[INFO] 配置加载成功
[INFO] Python 脚本加载成功
[INFO] init() 函数执行成功
[INFO] 运行时启动：周期=100 ms

周期 1: 当前温度=25.0°C, 目标=75.0°C, 输出=100.0%
周期 2: 当前温度=28.5°C, 目标=75.0°C, 输出=95.2%
周期 3: 当前温度=32.1°C, 目标=75.0°C, 输出=88.7%
...
```

**观察**：
- 温度值逐渐上升，接近目标温度 75°C
- PID 控制器自动调整输出功率
- 周期时间稳定在 100ms 左右

**停止程序**：按 `Ctrl+C`

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

### 问题 4：调试器连接失败

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
```

**解决方法**：
```powershell
# 以管理员身份运行 PowerShell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

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
