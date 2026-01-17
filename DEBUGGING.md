# 🐛 容器远程调试 - 快速参考

> **一键启动容器并使用 VS Code 进行远程调试**

## 🚀 立即开始

### Windows 用户

```powershell
# 启动容器并运行示例
.\debug-container.ps1 -RunExample pid_temperature -WaitDebugger
```

### Linux/macOS 用户

```bash
# 启动容器并运行示例
./debug-container.sh -e pid_temperature -w
```

**然后在 VS Code 中**：
1. 按 `F5`
2. 选择 "Python: 附加到 Docker 容器 (x86_64)"
3. 开始调试！🎉

---

## 📖 完整文档

| 文档 | 描述 | 链接 |
|------|------|------|
| **快速开始** | 三步开始调试 | [container-debug-quickstart.md](docs/container-debug-quickstart.md) |
| **完整指南** | 详细的调试说明和故障排除 | [container-debug.md](docs/container-debug.md) |
| **调试示例** | 7个实用调试场景 | [container-debug-examples.md](docs/container-debug-examples.md) |
| **配置验证** | 检查配置是否正确 | [CONTAINER_DEBUG_SETUP.md](docs/CONTAINER_DEBUG_SETUP.md) |
| **完成总结** | 所有更新的详细说明 | [CONTAINER_DEBUG_SUMMARY.md](CONTAINER_DEBUG_SUMMARY.md) |

---

## 🎯 支持的调试场景

### Python 调试
- ✅ 本地 Python 调试
- ✅ Docker 容器 Python 调试 (x86_64/ARM64)
- ✅ 远程设备 Python 调试
- ✅ 直接启动并调试示例

### C/C++ 调试
- ✅ 本地 gdb 调试
- ✅ 容器中 gdbserver 远程调试
- ✅ 混合调试 (Python + C++)

### 调试工具
- ✅ Valgrind 内存检查
- ✅ strace 系统调用追踪
- ✅ cProfile 性能分析

---

## 🛠️ 常用命令

### 容器管理
```bash
# 启动容器
docker-compose up -d dev

# 停止容器
docker-compose down

# 查看日志
docker-compose logs -f dev

# 进入容器
docker exec -it plcopen-dev bash
```

### 调试启动
```bash
# Python 调试
docker exec -it plcopen-dev python3 -m debugpy --listen 0.0.0.0:5678 --wait-for-client python/examples/pid_temperature.py

# C 调试
docker exec -it plcopen-dev gdbserver :9000 bin/plcopen_runtime --config config/pid_temperature.yaml
```

### VS Code 任务
按 `Ctrl+Shift+P` → "Tasks: Run Task" 选择：
- **Docker: 启动开发容器**
- **容器: 构建项目**
- **容器: 启动 Python 调试服务器**
- **容器: 运行内存检查 (Valgrind)**

---

## 📊 端口映射

| 端口 | 服务 | 用途 |
|------|------|------|
| 5678 | dev | Python debugpy (x86_64) |
| 9000 | dev | C gdbserver (x86_64) |
| 5679 | arm-test | Python debugpy (ARM64) |
| 9001 | arm-test | C gdbserver (ARM64) |

---

## 💡 调试技巧

### Python
```python
# 条件断点
temperature > 80.0

# 日志断点
温度: {temperature}, 输出: {output}

# 调试控制台
pid.kp = 5.0  # 修改参数
print(pid.integral)  # 查看状态
```

### C/C++
```gdb
# 查看变量
print *pid

# 查看内存
x/20xw pid

# 监视变量
watch pid->integral

# 条件断点
break pid_update if error > 10.0
```

---

## 🆘 遇到问题？

1. **查看** [故障排除指南](docs/container-debug.md#故障排除)
2. **运行** `.\verify-debug-setup.ps1` 验证配置
3. **查看** [常见问题](docs/faq.md)
4. **提交** [Issue](https://github.com/hollysys-cn/py-embedded-control/issues)

---

## 🎓 推荐工作流

### 日常开发
1. 使用 **Dev Container**: `F1` → "Remote-Containers: Reopen in Container"
2. 直接在容器中编辑和调试

### Python 调试
1. 启动容器：`.\debug-container.ps1`
2. 启动调试：在 VS Code 按 `F5`
3. 选择配置并调试

### C/C++ 调试
1. 启动 gdbserver
2. 在 VS Code 中附加调试器
3. 设置断点并调试

### 性能调试
1. 使用 Valgrind 检查内存
2. 使用 cProfile 分析性能
3. 使用 strace 追踪系统调用

---

## 🎉 你已准备好开始调试！

选择一个开始方式：
- 🚀 [快速开始](docs/container-debug-quickstart.md) - 3 步开始
- 📚 [完整指南](docs/container-debug.md) - 详细说明
- 💻 [调试示例](docs/container-debug-examples.md) - 实践案例

**祝调试愉快！** 🐛✨
