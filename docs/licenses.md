# 依赖许可证说明

本项目依赖以下开源软件，各软件的许可证如下：

## 运行时依赖

### Python 3.8+
- **许可证**: Python Software Foundation License (PSF License)
- **用途**: 脚本解释器和 C Extension API
- **兼容性**: 与 MIT 许可证兼容
- **链接**: https://docs.python.org/3/license.html

## 开发依赖

### pytest
- **许可证**: MIT License
- **用途**: Python 单元测试框架
- **链接**: https://docs.pytest.org/en/stable/

### debugpy（计划中）
- **许可证**: MIT License
- **用途**: Python 远程调试协议实现
- **链接**: https://github.com/microsoft/debugpy

### cppcheck
- **许可证**: GPL v3
- **用途**: C/C++ 静态代码分析工具（仅开发时使用）
- **兼容性**: 仅作为开发工具使用，不链接到最终产品
- **链接**: https://cppcheck.sourceforge.io/

### Docker
- **许可证**: Apache License 2.0
- **用途**: 容器化构建和测试环境
- **兼容性**: 仅作为构建工具使用
- **链接**: https://www.docker.com/legal/docker-software-end-user-license-agreement/

## 系统库依赖

### GNU C Library (glibc)
- **许可证**: LGPL v2.1+
- **用途**: Linux 系统 C 标准库
- **兼容性**: 动态链接，符合 LGPL 要求
- **链接**: https://www.gnu.org/software/libc/

### pthread
- **许可证**: LGPL v2.1+ (glibc 的一部分)
- **用途**: POSIX 线程库
- **兼容性**: 动态链接，符合 LGPL 要求

## 许可证兼容性

本项目使用 **MIT License**，与上述所有依赖的许可证兼容：

- ✅ Python PSF License: 与 MIT 兼容
- ✅ MIT License 组件: 完全兼容
- ✅ LGPL 库（动态链接）: 符合 LGPL 要求
- ✅ GPL 开发工具: 不影响最终产品许可证

## 归属声明

使用本软件时，建议在产品文档中包含以下归属声明：

```
本软件使用了以下开源组件：
- Python (Python Software Foundation License)
- 其他开源库详见 docs/licenses.md
```

## 更新日志

- 2026-01-17: 初始版本，列出核心依赖
