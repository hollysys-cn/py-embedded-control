# 文档索引

本目录包含 PLCopen Python 运行时环境项目的所有文档。

## 📚 文档导航

### 🚀 快速开始

**首次使用？从这里开始：**

1. **[README.md](../README.md)** - 项目概览和快速开始
2. **[快速开始指南](guides/WINDOWS_QUICKSTART.md)** - Windows 用户快速上手
3. **[本地安装指南](guides/LOCAL_SETUP.md)** - Linux/macOS 原生安装

### 📖 用户指南

**深入了解系统功能：**

- **[用户手册](user_guide.md)** - 完整的功能和使用说明
- **[API 参考](api_reference.md)** - 所有功能块 API 文档
- **[常见问题](faq.md)** - 常见问题解答

### 🔧 开发指南

**开发者和贡献者必读：**

- **[远程调试指南](guides/REMOTE_DEBUG_QUICKSTART.md)** - VS Code 远程调试完整教程 ⭐
- **[容器调试配置](guides/CONTAINER_DEBUG_SETUP.md)** - Docker 开发环境配置
- **[容器调试详解](guides/container-debug.md)** - 容器调试深入指南
- **[完整调试指南](guides/DEBUGGING.md)** - 所有调试技术汇总
- **[调试界面截图](guides/debugging_screenshots.md)** - 可视化调试界面
- **[贡献指南](contributing.md)** - 如何参与项目开发

### 📝 示例和教程

**学习最佳实践：**

- **[PID 温度控制示例](examples/pid_temperature.md)** - 完整的 PID 控制示例 ⭐
- **[容器调试示例](guides/container-debug-examples.md)** - Docker 调试实战
- **[容器调试快速开始](guides/container-debug-quickstart.md)** - 容器调试速成

### 📊 技术报告

**项目实施和质量报告：**

- **[项目完成报告](reports/PROJECT_COMPLETION_REPORT.md)** - 整体项目总结 ⭐
- **[Phase 5 完成报告](reports/PHASE5_COMPLETION_REPORT.md)** - 远程调试实施总结
- **[Phase 5 验证清单](reports/PHASE5_VERIFICATION_CHECKLIST.md)** - 质量验证记录
- **[远程调试实施报告](reports/REMOTE_DEBUG_IMPLEMENTATION_REPORT.md)** - 详细技术实现
- **[构建验证报告](reports/BUILD_VERIFICATION.md)** - 构建系统验证
- **[编译验证报告](reports/COMPILATION_VERIFICATION_REPORT.md)** - 编译过程验证
- **[容器调试总结](reports/CONTAINER_DEBUG_SUMMARY.md)** - Docker 调试实施
- **[实施总结](reports/IMPLEMENTATION_SUMMARY.md)** - 功能实施概览
- **[设置完成报告](reports/SETUP_COMPLETE.md)** - 环境设置验证
- **[验证清单](reports/VERIFICATION_CHECKLIST.md)** - 完整验证清单
- **[PowerShell 修复](reports/POWERSHELL_FIX.md)** - Windows 脚本优化
- **[设置完成报告](reports/SETUP_COMPLETE.md)** - 环境设置验证

### 🔍 技术深入

**高级主题和技术细节：**

- **[代码质量报告](technical/quality_report_20260117.md)** - 代码质量分析
- **[质量总结](technical/quality_summary.md)** - 质量指标汇总
- **[代码质量检查](technical/code_quality.md)** - 质量检查细节
- **[错误处理总结](technical/error_handling_summary.md)** - 错误处理机制
- **[Phase 6 总结](technical/phase6_summary.md)** - 打磨阶段总结
- **[空指针检查计划](technical/null_pointer_check_plan.md)** - 安全性增强
- **[项目进度追踪](technical/project_progress.md)** - 详细进度记录
- **[构建验证详情](technical/build-verification.md)** - 构建系统详解

### 📋 项目管理

**项目进度和规划：**

- **[项目完成报告](reports/PROJECT_COMPLETION_REPORT.md)** - 整体项目总结 ⭐
- **[许可证信息](licenses.md)** - 依赖许可证说明

---

## 🗂️ 文档结构

```
docs/
├── README.md                        # 本文件 - 文档导航索引 ⭐
├── api_reference.md                 # API 完整参考手册
├── user_guide.md                    # 用户使用手册
├── contributing.md                  # 贡献指南
├── faq.md                           # 常见问题解答
├── licenses.md                      # 许可证信息
│
├── guides/                          # 📖 用户指南和教程
│   ├── REMOTE_DEBUG_QUICKSTART.md  # 远程调试快速开始 ⭐
│   ├── WINDOWS_QUICKSTART.md        # Windows 快速开始
│   ├── LOCAL_SETUP.md               # Linux/macOS 安装指南
│   ├── DEBUGGING.md                 # 完整调试指南
│   ├── CONTAINER_DEBUG_SETUP.md     # 容器调试配置
│   ├── container-debug.md           # 容器调试详解
│   ├── container-debug-quickstart.md # 容器调试快速开始
│   ├── container-debug-examples.md  # 容器调试示例
│   └── debugging_screenshots.md     # 调试界面截图
│
├── reports/                         # 📊 技术报告和验证文档
│   ├── PROJECT_COMPLETION_REPORT.md # 项目完成报告 ⭐
│   ├── PHASE5_COMPLETION_REPORT.md  # Phase 5 完成报告
│   ├── PHASE5_VERIFICATION_CHECKLIST.md
│   ├── REMOTE_DEBUG_IMPLEMENTATION_REPORT.md
│   ├── BUILD_VERIFICATION.md
│   ├── COMPILATION_VERIFICATION_REPORT.md
│   ├── CONTAINER_DEBUG_SUMMARY.md
│   ├── IMPLEMENTATION_SUMMARY.md
│   ├── SETUP_COMPLETE.md
│   ├── VERIFICATION_CHECKLIST.md
│   └── POWERSHELL_FIX.md
│
├── technical/                       # 🔬 技术深入文档
│   ├── quality_report_20260117.md   # 代码质量分析
│   ├── quality_summary.md           # 质量指标汇总
│   ├── code_quality.md              # 代码质量检查
│   ├── error_handling_summary.md    # 错误处理机制
│   ├── null_pointer_check_plan.md   # 空指针安全计划
│   ├── phase6_summary.md            # Phase 6 打磨总结
│   ├── project_progress.md          # 项目进度追踪
│   └── build-verification.md        # 构建验证详情
│
├── examples/                        # 💡 示例文档
│   └── pid_temperature.md           # PID 温度控制示例
│
└── api/                             # 🔌 API 详细文档目录
    └── （API 模块详细文档）
```

---

## 🎯 按角色查找文档

### 👨‍💼 产品经理 / 决策者
- [README.md](../README.md) - 项目概览
- [项目完成报告](reports/PROJECT_COMPLETION_REPORT.md) - 整体成果 ⭐
- [项目进度追踪](technical/project_progress.md) - 进度记录

### 👨‍💻 开发者
- [远程调试指南](guides/REMOTE_DEBUG_QUICKSTART.md) - 必读 ⭐
- [API 参考](api_reference.md) - API 文档
- [贡献指南](contributing.md) - 开发规范
- [代码质量报告](technical/quality_report_20260117.md) - 质量标准
- [容器调试配置](guides/CONTAINER_DEBUG_SETUP.md) - 开发环境

### 🧪 测试工程师
- [调试指南](guides/DEBUGGING.md) - 测试环境设置
- [容器调试示例](guides/container-debug-examples.md) - 调试实战
- [构建验证报告](reports/BUILD_VERIFICATION.md) - 验证方法
- [验证清单](reports/VERIFICATION_CHECKLIST.md) - 测试清单

### 📚 最终用户
- [用户手册](user_guide.md) - 完整使用指南
- [常见问题](faq.md) - 问题解答
- [PID 温度控制示例](examples/pid_temperature.md) - 实例教程

### 🎓 学习者
- [快速开始指南](guides/WINDOWS_QUICKSTART.md) - 入门必读
- [PID 温度控制示例](examples/pid_temperature.md) - 学习案例
- [容器调试快速开始](container-debug-quickstart.md) - 调试入门

---

## 📌 重要提示

- **⭐ 标记**的文档为推荐优先阅读
- 所有文档使用简体中文编写
- 文档持续更新，最后更新日期见各文档页脚
- 遇到问题？先查看 [常见问题](faq.md)

---

## 🔗 外部资源

- **项目仓库**: [GitHub](https://github.com/hollysys-cn/py-embedded-control)
- **问题追踪**: [Issues](https://github.com/hollysys-cn/py-embedded-control/issues)
- **PLCopen 标准**: [IEC 61131-3](https://www.plcopen.org/)

---

**最后更新**: 2026-01-17
**维护者**: PLCopen Python Runtime Team
