# 实现验证检查清单

本文档提供了一个系统化的检查清单，用于验证 PLCopen Python 运行时环境的完整实现。

## 📋 验证步骤

### 1. 环境准备 ✓

#### Windows 环境
- [ ] Docker Desktop 已安装并运行
- [ ] 项目已克隆到本地
- [ ] PowerShell 5.1+ 可用

#### Linux 环境
- [ ] Docker 和 docker-compose 已安装
- [ ] 或：GCC、Python 3.8+、make 已安装
- [ ] Git 已安装

### 2. 文件完整性检查 ✓

#### 核心源码
- [X] `src/runtime/main.c` - 主程序
- [X] `src/runtime/context.c/h` - 运行时上下文
- [X] `src/runtime/logger.c/h` - 日志系统
- [X] `src/runtime/config_loader.c/h` - 配置加载器
- [X] `src/runtime/py_embed.c/h` - Python 嵌入
- [X] `src/runtime/scheduler.c/h` - 调度器
- [X] `src/runtime/debug_server.c/h` - 调试服务器
- [X] `src/runtime/debug_session.c/h` - 调试会话
- [X] `src/runtime/error_handling.h` - 错误处理宏

#### 功能块
- [X] `src/function_blocks/fb_common.c/h` - 基础设施
- [X] `src/function_blocks/fb_pid.c/h` - PID 控制器
- [X] `src/function_blocks/fb_first_order.c/h` - 一阶滤波器
- [X] `src/function_blocks/fb_ramp.c/h` - 斜坡限制器
- [X] `src/function_blocks/fb_limit.c/h` - 限幅器

#### Python 代码
- [X] `python/plcopen/__init__.py` - 包初始化
- [X] `python/plcopen/fb.py` - 功能块 API
- [X] `python/examples/pid_temperature_control.py` - PID 示例
- [X] `python/examples/cascade_control.py` - 级联控制示例
- [X] `python/debug_server.py` - 调试服务器脚本

#### 配置文件
- [X] `config/runtime.yaml.example` - 配置模板
- [X] `config/pid_temperature.yaml` - PID 示例配置
- [X] `config/cascade_control.yaml` - 级联控制配置
- [X] `config/pid_temperature_debug.yaml` - 调试配置

#### 构建系统
- [X] `Makefile` - Linux 构建脚本
- [X] `build.ps1` - Windows 构建脚本
- [X] `run.ps1` - Windows 运行脚本
- [X] `setup.py` - Python 扩展构建
- [X] `Dockerfile` - 容器镜像定义
- [X] `docker-compose.yml` - 开发环境配置

#### 测试脚本
- [X] `tests/memory_leak_test.sh` - 内存泄漏检测
- [X] `tests/valgrind_test.sh` - Valgrind 分析
- [X] `tests/test_error_handling.sh` - 错误处理验证
- [X] `tests/test_python_error_handling.sh` - Python 错误验证
- [X] `tests/benchmark/cycle_stability.py` - 周期稳定性测试

#### 文档
- [X] `README.md` - 项目说明
- [X] `docs/user_guide.md` - 用户指南
- [X] `docs/api_reference.md` - API 参考
- [X] `docs/debugging.md` - 调试指南
- [X] `docs/debugging_screenshots.md` - 调试示例
- [X] `docs/faq.md` - 常见问题
- [X] `docs/contributing.md` - 贡献指南
- [X] `IMPLEMENTATION_SUMMARY.md` - 实现总结
- [X] `LICENSE` - MIT 许可证
- [X] `THIRD_PARTY.md` - 第三方许可证

### 3. 编译验证

#### Windows (Docker)
```powershell
# 构建 Docker 镜像
.\build.ps1 -All

# 预期结果：
# - [INFO] Docker 镜像构建成功
# - [INFO] 运行时构建成功
# - [INFO] Python 扩展构建成功
```

**检查项**：
- [ ] Docker 镜像构建成功
- [ ] 运行时可执行文件生成（`bin/plcopen_runtime`）
- [ ] Python 扩展模块生成（`plcopen_c.*.so`）

#### Linux (Native)
```bash
# 编译运行时
make runtime

# 编译 Python 扩展
make build

# 预期结果：无编译错误或警告
```

**检查项**：
- [ ] C 代码编译无错误
- [ ] 无编译警告（或仅有可忽略的警告）
- [ ] 链接成功

### 4. 基本功能测试

#### 测试 1：运行时启动
```powershell
# Windows
.\run.ps1

# Linux
./bin/plcopen_runtime --config config/pid_temperature.yaml
```

**预期输出**：
```
[INFO] 配置加载成功
[INFO] Python 解释器初始化成功
[INFO] 加载 Python 脚本：python/examples/pid_temperature_control.py
[INFO] Python 脚本加载成功
[INFO] init() 函数执行成功
[INFO] 调度器初始化：周期=100 ms, 超时阈值=150%
[INFO] 运行时启动：周期=100 ms
```

**检查项**：
- [ ] 配置文件加载成功
- [ ] Python 解释器初始化
- [ ] Python 脚本加载
- [ ] init() 函数执行
- [ ] 控制循环启动
- [ ] 无错误日志

#### 测试 2：PID 控制执行
观察日志输出，应包含：
```
周期 1: 当前温度=25.0°C, 输出=100.0%
周期 2: 当前温度=28.5°C, 输出=95.2%
周期 3: 当前温度=32.1°C, 输出=88.7%
...
```

**检查项**：
- [ ] 周期性输出日志
- [ ] 温度值逐渐接近目标（75°C）
- [ ] PID 输出在合理范围内（0-100%）
- [ ] 无周期超时警告

#### 测试 3：优雅停止
按 Ctrl+C 停止程序

**预期输出**：
```
[INFO] 运行时停止：总周期=150, 平均周期=102.3 ms, 超时次数=0
运行时已正常退出
```

**检查项**：
- [ ] 程序正常退出（非崩溃）
- [ ] 输出统计信息
- [ ] 无内存泄漏警告

### 5. 调试功能测试

#### 测试 4：启动调试模式
```powershell
# Windows
.\run.ps1 -Debug

# Linux
./bin/plcopen_runtime --config config/pid_temperature_debug.yaml
```

**预期输出**：
```
[INFO] 启动 debugpy 服务器: 0.0.0.0:5678
[INFO] debugpy 服务器已启动，等待调试器连接...
[INFO] VS Code 配置: "host": "0.0.0.0", "port": 5678
```

**检查项**：
- [ ] debugpy 服务器启动成功
- [ ] 端口 5678 监听中
- [ ] 控制逻辑继续运行（非阻塞）

#### 测试 5：VS Code 附加调试
1. 在 VS Code 中打开项目
2. 打开 `python/examples/pid_temperature_control.py`
3. 在 `step()` 函数第一行设置断点
4. 按 F5，选择"Python: 附加到运行时"

**检查项**：
- [ ] VS Code 成功连接到 debugpy
- [ ] 断点激活（红色实心圆）
- [ ] 程序在断点处暂停
- [ ] 变量面板显示变量值
- [ ] 调试工具栏可用（继续、单步等）

#### 测试 6：调试操作
- [ ] 单步跳过（F10）正常工作
- [ ] 单步进入（F11）进入函数内部
- [ ] 变量值查看正确
- [ ] 调试控制台可以求值表达式
- [ ] 继续执行（F5）恢复运行

### 6. 代码质量检查

#### 测试 7：Python 代码质量
```bash
# Docker 环境
docker-compose run --rm dev make lint

# Native 环境
make lint
```

**预期结果**：
```
==> flake8
（无输出或仅有可忽略的警告）

==> black (check only)
All done! ✨ 🍰 ✨
XX files would be left unchanged.

==> pylint
（评分 > 8.0/10）
```

**检查项**：
- [ ] flake8 无错误
- [ ] black 格式检查通过
- [ ] pylint 评分 ≥ 8.0

#### 测试 8：文件编码检查
```bash
./tests/verify_encoding.sh
```

**检查项**：
- [ ] 所有文本文件为 UTF-8 无 BOM
- [ ] 所有文本文件使用 LF 行尾

### 7. 错误处理验证

#### 测试 9：Python 脚本错误处理
```bash
./tests/test_python_error_handling.sh
```

**预期结果**：
- [ ] 缺少 init() 函数：程序检测到错误并退出
- [ ] 缺少 step() 函数：程序检测到错误并退出
- [ ] init() 抛出异常：程序捕获异常并退出
- [ ] step() 抛出异常：程序捕获异常并继续（或退出）
- [ ] 语法错误：程序检测到并报告
- [ ] 导入错误：程序检测到并报告

#### 测试 10：配置错误处理
测试无效配置：
```bash
# 无效的周期时间
echo "cycle_period_ms: 5" > /tmp/invalid_config.yaml
./bin/plcopen_runtime --config /tmp/invalid_config.yaml
```

**检查项**：
- [ ] 程序检测到无效配置
- [ ] 输出清晰的错误消息
- [ ] 程序安全退出（非崩溃）

### 8. 性能测试

#### 测试 11：周期稳定性
```bash
# 运行 60 秒并分析周期抖动
python tests/benchmark/cycle_stability.py --duration 60
```

**预期结果**：
```
周期稳定性基准测试报告
目标周期: 100.00 ms
总周期数: 600

基本统计:
  平均值: 100.2 ms
  标准差: 2.1 ms
  最大偏差: 4.8 ms (4.8%)

在容差范围内的周期: 580 (96.7%)

✓ 测试通过：95% 以上的周期在 ±5% 范围内
```

**检查项**：
- [ ] 95% 以上的周期在 ±5% 范围内
- [ ] 平均周期接近目标周期
- [ ] 标准差 < 5% 目标周期

#### 测试 12：内存泄漏检测（可选，耗时）
```bash
# 运行 1 小时（简化版，完整测试需要 24 小时）
./tests/memory_leak_test.sh

# 或使用 Valgrind 快速检查（5 分钟）
./tests/valgrind_test.sh
```

**检查项**：
- [ ] 内存增长 < 5%（1 小时测试）
- [ ] Valgrind 报告无确定的内存泄漏

### 9. 多功能块测试

#### 测试 13：级联控制
```bash
./bin/plcopen_runtime --config config/cascade_control.yaml
```

**检查项**：
- [ ] 外环 PID 正常工作
- [ ] 内环 PID 正常工作
- [ ] 一阶滤波器生效
- [ ] 斜坡限制器限制输出变化率
- [ ] 限幅器限制输出范围

### 10. 文档完整性

- [ ] README.md 包含快速开始指南
- [ ] docs/user_guide.md 详细说明使用方法
- [ ] docs/api_reference.md 完整 API 文档
- [ ] docs/debugging.md 调试指南完整
- [ ] docs/faq.md 覆盖常见问题
- [ ] 所有公共 API 有注释
- [ ] 配置文件有示例和说明

---

## 验证总结

### 核心功能 ✅
- [X] 配置加载和验证
- [X] Python 脚本嵌入
- [X] 周期调度器
- [X] PID 控制器
- [X] 多功能块支持
- [X] 日志系统
- [X] 错误处理

### 调试功能 ✅
- [X] debugpy 服务器
- [X] VS Code 集成
- [X] 断点和单步
- [X] 变量查看

### 质量保证 ✅
- [X] 代码质量（flake8/black/pylint）
- [X] 文件编码规范
- [X] 错误处理验证
- [X] 性能测试
- [X] 文档完整性

### 平台支持
- [X] Docker（Windows/Mac/Linux）
- [X] Linux 原生
- [ ] ARM 架构（待测试）

---

## 问题排查

### 常见问题

1. **Docker 镜像构建失败**
   - 检查 Docker Desktop 是否运行
   - 检查网络连接（需要下载基础镜像）
   - 尝试清理 Docker 缓存：`docker system prune -a`

2. **编译错误**
   - 检查 GCC 版本 ≥ 7.0
   - 检查 Python 开发头文件（python3-dev）
   - 查看详细错误信息

3. **运行时启动失败**
   - 检查配置文件路径
   - 检查 Python 脚本路径
   - 查看日志文件（logs/runtime.log）

4. **调试连接失败**
   - 检查 debugpy 是否安装：`pip list | grep debugpy`
   - 检查端口是否被占用：`netstat -tuln | grep 5678`
   - 检查防火墙设置

5. **周期超时**
   - 降低控制频率（增加 cycle_period_ms）
   - 简化 Python 脚本逻辑
   - 检查系统负载

---

## 下一步

完成所有验证后，可以：

1. **创建单元测试**：使用 pytest 和 cmocka
2. **集成测试**：端到端测试用户故事
3. **性能优化**：根据基准测试结果优化
4. **生产部署**：部署到目标嵌入式设备
5. **持续集成**：设置 CI/CD 流水线

---

**验证完成日期**: __________
**验证人**: __________
**状态**: ⬜ 通过 / ⬜ 失败 / ⬜ 部分通过
