# 容器调试快速开始

快速启动容器并使用 VS Code 进行远程调试。

## 🚀 三步开始

### 步骤 1: 启动容器

#### Windows (PowerShell)

```powershell
.\scripts\debug-container.ps1
```

#### Linux/macOS (Bash)

```bash
chmod +x scripts/debug-container.sh
./scripts/debug-container.sh
```

### 步骤 2: 在容器中启动调试服务器

```bash
# 进入容器
docker exec -it plcopen-dev bash

# 启动 Python 调试服务器
python3 -m debugpy --listen 0.0.0.0:5678 --wait-for-client python/examples/pid_temperature.py
```

### 步骤 3: 在 VS Code 中连接调试器

1. 打开 VS Code 调试面板（`Ctrl+Shift+D`）
2. 选择配置：**Python: 附加到 Docker 容器 (x86_64)**
3. 按 `F5` 启动调试
4. 在代码中设置断点并开始调试

## 💡 一键运行示例

### Windows

```powershell
# 运行 PID 温度控制示例并等待调试器
.\debug-container.ps1 -RunExample pid_temperature -WaitDebugger
```

### Linux/macOS

```bash
# 运行 PID 温度控制示例并等待调试器
./debug-container.sh -e pid_temperature -w
```

然后在 VS Code 中按 `F5` 连接调试器。

## 📚 完整文档

详细的调试配置和故障排除，请参考：[容器远程调试指南](container-debug.md)

## 🆘 常见问题

### Q: 容器启动失败怎么办？

A: 检查 Docker 是否运行：
```bash
docker info
```

### Q: 无法连接到调试端口？

A: 检查端口映射：
```bash
docker port plcopen-dev
```

应该显示：`5678/tcp -> 0.0.0.0:5678`

### Q: 断点没有被触发？

A: 确认路径映射配置正确，并重新加载窗口（`Ctrl+Shift+P` → `Reload Window`）

## 🔗 相关资源

- [完整调试指南](container-debug.md)
- [Python 调试配置](.vscode/launch.json)
- [Docker Compose 配置](../docker-compose.yml)
- [Dev Container 配置](.devcontainer/devcontainer.json)
