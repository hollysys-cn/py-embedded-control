# 🚀 快速链接

> 项目完整信息请查看 [README.md](README.md) | 详细文档导航 → [docs/README.md](docs/README.md)

## ⚡ 立即开始

| 我想... | 点击这里 |
|---------|---------|
| **运行示例程序** | [Windows](docs/guides/WINDOWS_QUICKSTART.md) \| [Linux/macOS](docs/guides/LOCAL_SETUP.md) |
| **启动远程调试** | [远程调试快速开始](docs/guides/REMOTE_DEBUG_QUICKSTART.md) |
| **理解项目结构** | [项目文件说明](PROJECT_FILES.md) |
| **查看API文档** | [API 参考手册](docs/api_reference.md) |
| **运行测试** | `make test` 或查看 [用户手册](docs/user_guide.md) |

## 📚 按角色导航

| 角色 | 推荐文档 |
|------|---------|
| 👨‍💼 **产品经理** | [README.md](README.md) → [完成报告](docs/reports/PROJECT_COMPLETION_REPORT.md) |
| 👨‍💻 **开发者** | [本地搭建](docs/guides/LOCAL_SETUP.md) → [API文档](docs/api_reference.md) → [调试指南](docs/guides/REMOTE_DEBUG_QUICKSTART.md) |
| 🧪 **测试人员** | [验证清单](docs/reports/VERIFICATION_CHECKLIST.md) → [质量报告](docs/reports/quality_report_20260117.md) |
| 🎓 **学习者** | [README.md](README.md) → [示例代码](python/examples/) → [用户手册](docs/user_guide.md) |

## 🔥 一键命令

### Windows
```powershell
# 一键启动调试（最简单！）
.\start-debug.ps1

# 构建项目
.\build.ps1 -All

# 运行示例
.\run.ps1
```

### Linux/macOS
```bash
# 一键启动调试
./start-debug.sh

# 构建项目
make all

# 运行单元测试
make test
```

## 📦 Docker 环境

```bash
# 启动开发容器
docker-compose up -d dev

# 进入容器 Shell
docker exec -it plcopen-dev bash

# 查看容器日志
docker-compose logs -f dev
```

## 🛠️ 常见任务

- **修改功能块代码**: `src/function_blocks/fb_*.c`
- **修改Python API**: `python/plcopen/blocks.py`
- **添加示例**: `python/examples/`
- **运行特定测试**: `python -m pytest tests/unit/test_*.py -v`
- **修改配置**: `config/pid_temperature.yaml`

## ❓ 遇到问题？

1. 查看 [FAQ](docs/faq.md)
2. 查看 [调试文档](docs/guides/DEBUGGING.md)
3. 检查 [错误处理摘要](docs/error_handling_summary.md)
4. 提交 Issue 到 GitHub

---

**项目状态**: ✅ 1.0.0 已完成（6个阶段，76个任务全部完成）
**最后更新**: 2026-01-17
**维护者**: Hollysys CN 团队
