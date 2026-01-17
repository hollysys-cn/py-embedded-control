# 空指针检查和错误处理增强计划

## 概述

本文档描述对 PLCopen Python 运行时环境的空指针检查和错误处理增强计划。

---

## 当前状态分析

### 已有的空指针检查

通过代码审查，以下函数已包含空指针检查：

1. **py_embed.c**
   - `py_embed_load_script()` - ✅ 检查 script_path 和 context

2. **logger.c**
   - 日志函数使用宏，可能需要增强

### 需要增强的函数

#### 高优先级（公共 API）

1. **config_loader.c**
   - `config_load()` - 需要检查 filename 参数
   - `parse_config_file()` - 需要检查文件指针

2. **py_embed.c**
   - `py_embed_call_init()` - 需要检查 context
   - `py_embed_call_step()` - 需要检查 context

3. **scheduler.c**
   - `scheduler_init()` - 需要检查配置参数
   - `scheduler_wait_next_cycle()` - 需要检查时间参数

4. **context.c**
   - `context_init()` - 需要检查配置参数
   - `context_get()` - 需要检查返回值使用

5. **logger.c**
   - `logger_init()` - 需要检查配置参数

#### 中优先级（功能块）

6. **fb_*.c** 功能块
   - 所有 `_init()` 函数 - 需要检查状态指针
   - 所有 `_compute()` 函数 - 需要检查输入输出指针

---

## 实施策略

### 1. 统一的错误处理宏

创建 `src/runtime/error_handling.h`:

```c
#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include "logger.h"

/**
 * 检查指针是否为空，如果为空则记录错误并返回
 */
#define CHECK_NULL_PTR(ptr, name, ret_val) \
    do { \
        if ((ptr) == NULL) { \
            LOG_ERROR_MSG("%s: %s 参数为空", __func__, (name)); \
            return (ret_val); \
        } \
    } while(0)

/**
 * 检查多个指针
 */
#define CHECK_NULL_PTR2(ptr1, name1, ptr2, name2, ret_val) \
    do { \
        CHECK_NULL_PTR(ptr1, name1, ret_val); \
        CHECK_NULL_PTR(ptr2, name2, ret_val); \
    } while(0)

/**
 * 检查文件指针
 */
#define CHECK_FILE_PTR(fp, filename, ret_val) \
    do { \
        if ((fp) == NULL) { \
            LOG_ERROR_MSG("%s: 无法打开文件 %s", __func__, (filename)); \
            return (ret_val); \
        } \
    } while(0)

/**
 * 检查内存分配
 */
#define CHECK_MALLOC(ptr, size, ret_val) \
    do { \
        if ((ptr) == NULL) { \
            LOG_ERROR_MSG("%s: 内存分配失败 (大小: %zu 字节)", __func__, (size_t)(size)); \
            return (ret_val); \
        } \
    } while(0)

#endif /* ERROR_HANDLING_H */
```

### 2. 增强函数空指针检查

#### config_loader.c

```c
int config_load(const char* filename, RuntimeConfig* config) {
    CHECK_NULL_PTR2(filename, "filename", config, "config", -1);

    // 现有实现...
}
```

#### py_embed.c

```c
int py_embed_call_init(PyEmbedContext* context) {
    CHECK_NULL_PTR(context, "context", -1);

    if (!context->initialized) {
        LOG_ERROR_MSG("Python 脚本未初始化");
        return -1;
    }

    // 现有实现...
}

int py_embed_call_step(PyEmbedContext* context) {
    CHECK_NULL_PTR(context, "context", -1);

    if (!context->initialized) {
        LOG_ERROR_MSG("Python 脚本未初始化");
        return -1;
    }

    // 现有实现...
}
```

#### scheduler.c

```c
int scheduler_init(const RuntimeConfig* config) {
    CHECK_NULL_PTR(config, "config", -1);

    // 现有实现...
}
```

#### 功能块示例 (fb_pid.c)

```c
void pid_init(pid_state_t* state, double kp, double ki, double kd) {
    CHECK_NULL_PTR(state, "state", /* void */);

    // 现有实现...
}

double pid_compute(pid_state_t* state, double sp, double pv, double dt) {
    CHECK_NULL_PTR(state, "state", 0.0);

    if (dt <= 0.0) {
        LOG_ERROR_MSG("pid_compute: 无效的时间步长 dt=%f", dt);
        return state->output;
    }

    // 现有实现...
}
```

---

## 实施检查清单

### Phase 1: 创建基础设施
- [ ] 创建 `src/runtime/error_handling.h`
- [ ] 在相关文件中包含头文件
- [ ] 更新 Makefile 依赖

### Phase 2: 运行时核心
- [ ] config_loader.c (3个函数)
- [ ] py_embed.c (4个函数)
- [ ] scheduler.c (3个函数)
- [ ] context.c (2个函数)
- [ ] logger.c (1个函数)

### Phase 3: 功能块
- [ ] fb_common.c
- [ ] fb_pid.c (2个函数)
- [ ] fb_first_order.c (2个函数)
- [ ] fb_ramp.c (2个函数)
- [ ] fb_limit.c (2个函数)

### Phase 4: Python 绑定
- [ ] py_pid.c (检查 PyObject 指针)
- [ ] py_first_order.c
- [ ] py_ramp.c
- [ ] py_limit.c

### Phase 5: 测试验证
- [ ] 单元测试：空指针输入测试
- [ ] 集成测试：错误路径测试
- [ ] 日志验证：错误消息正确输出

---

## 测试策略

### 单元测试示例

```c
// tests/test_null_pointer.c
#include "../src/runtime/py_embed.h"
#include <assert.h>

void test_py_embed_load_script_null_path() {
    PyEmbedContext ctx;
    int result = py_embed_load_script(NULL, &ctx);
    assert(result == -1);  // 应该返回错误
}

void test_py_embed_load_script_null_context() {
    int result = py_embed_load_script("test.py", NULL);
    assert(result == -1);  // 应该返回错误
}
```

### 集成测试

```bash
# tests/test_error_handling.sh
#!/bin/bash

# 测试无效配置文件
bin/plcopen_runtime --config /nonexistent/config.yaml 2>&1 | grep -q "无法打开文件"
if [ $? -eq 0 ]; then
    echo "✓ 无效配置文件检测正常"
else
    echo "✗ 无效配置文件检测失败"
    exit 1
fi

# 测试无效脚本路径
# ...类似测试
```

---

## 预期效果

### 代码质量提升

- ✅ 所有公共 API 函数都有空指针检查
- ✅ 一致的错误处理风格
- ✅ 清晰的错误日志消息
- ✅ 防止段错误（Segmentation Fault）

### 调试改进

- 🔍 错误日志包含函数名和参数名
- 🔍 可追踪错误发生的位置
- 🔍 便于问题诊断

### 生产环境稳定性

- 🛡️ 防御性编程
- 🛡️ 优雅降级
- 🛡️ 减少崩溃风险

---

## 工作量估算

| 阶段 | 工作量 | 说明 |
|------|--------|------|
| Phase 1 | 1 小时 | 创建头文件和宏 |
| Phase 2 | 2 小时 | 运行时核心（13个函数） |
| Phase 3 | 1.5 小时 | 功能块（8个函数） |
| Phase 4 | 1 小时 | Python 绑定 |
| Phase 5 | 1.5 小时 | 测试验证 |
| **总计** | **7 小时** | **约 1 个工作日** |

---

## 风险和限制

### 风险

1. **性能影响**: 额外的检查可能略微增加开销
   - 缓解：检查操作很快（纳秒级）
   - 缓解：可以在发布版本中使用条件编译优化

2. **代码膨胀**: 增加代码行数
   - 缓解：使用宏减少重复代码
   - 缓解：提高可维护性的收益大于代码增加的成本

### 限制

1. **不能防止所有错误**: 只检查空指针，不检查野指针
2. **需要配合测试**: 需要充分的测试覆盖

---

## 后续增强

### 短期
- [ ] 添加参数范围检查（如 dt > 0）
- [ ] 添加状态一致性检查

### 长期
- [ ] 使用 AddressSanitizer (ASan) 检测内存错误
- [ ] 使用 Valgrind 进行内存泄漏检测
- [ ] 考虑使用断言（assert）在调试版本中

---

**文档版本**: 1.0
**最后更新**: 2026-01-17
**状态**: 计划中
