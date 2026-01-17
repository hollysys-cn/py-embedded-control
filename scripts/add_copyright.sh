#!/usr/bin/env bash
# add_copyright.sh - 为所有源文件添加版权头

set -e

# C/C++ 版权头
read -r -d '' C_COPYRIGHT_HEADER << 'EOF' || true
/*
 * Copyright (c) 2026 Hollysys Co., Ltd.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

EOF

# Python 版权头
read -r -d '' PYTHON_COPYRIGHT_HEADER << 'EOF' || true
# Copyright (c) 2026 Hollysys Co., Ltd.
#
# SPDX-License-Identifier: MIT
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

EOF

# 颜色定义
if [ -t 1 ]; then
    GREEN="\033[0;32m"
    YELLOW="\033[0;33m"
    RESET="\033[0m"
else
    GREEN=""
    YELLOW=""
    RESET=""
fi

# 统计变量
total_processed=0
total_skipped=0
total_added=0

# 添加版权头到 C/C++ 文件
add_c_copyright() {
    local file="$1"

    echo -e "${YELLOW}处理文件: $file${RESET}"

    # 检查文件是否已有版权头
    if grep -q "Copyright (c) [0-9]\{4\}" "$file"; then
        echo "  跳过: 文件已包含版权信息"
        ((total_skipped++))
        return
    fi

    # 创建临时文件
    local temp_file="${file}.tmp"

    # 写入版权头和原文件内容
    echo "$C_COPYRIGHT_HEADER" > "$temp_file"
    cat "$file" >> "$temp_file"

    # 替换原文件
    mv "$temp_file" "$file"

    echo -e "  ${GREEN}完成: 已添加版权头${RESET}"
    ((total_added++))
}

# 添加版权头到 Python 文件
add_python_copyright() {
    local file="$1"

    echo -e "${YELLOW}处理文件: $file${RESET}"

    # 检查文件是否已有版权头
    if grep -q "Copyright (c) [0-9]\{4\}" "$file"; then
        echo "  跳过: 文件已包含版权信息"
        ((total_skipped++))
        return
    fi

    # 创建临时文件
    local temp_file="${file}.tmp"

    # 保留 shebang 行（如果有）
    if head -n 1 "$file" | grep -q "^#!"; then
        # 提取 shebang 行
        head -n 1 "$file" > "$temp_file"
        echo "" >> "$temp_file"
        # 添加版权头
        echo "$PYTHON_COPYRIGHT_HEADER" >> "$temp_file"
        # 添加原文件剩余内容（跳过 shebang）
        tail -n +2 "$file" >> "$temp_file"
    else
        # 没有 shebang，直接添加版权头
        echo "$PYTHON_COPYRIGHT_HEADER" > "$temp_file"
        cat "$file" >> "$temp_file"
    fi

    # 替换原文件
    mv "$temp_file" "$file"

    echo -e "  ${GREEN}完成: 已添加版权头${RESET}"
    ((total_added++))
}

# 主程序
echo "开始为源文件添加版权头..."
echo ""

# 处理 C 源文件
echo "=== 处理 C 源文件 (.c) ==="
if [ -d "src" ]; then
    while IFS= read -r -d '' file; do
        add_c_copyright "$file"
        ((total_processed++))
    done < <(find src -type f -name "*.c" -print0)
fi
echo ""

# 处理 C 头文件
echo "=== 处理 C 头文件 (.h) ==="
if [ -d "src" ]; then
    while IFS= read -r -d '' file; do
        add_c_copyright "$file"
        ((total_processed++))
    done < <(find src -type f -name "*.h" -print0)
fi
echo ""

# 处理 Python 文件
echo "=== 处理 Python 文件 (.py) ==="
if [ -d "python" ]; then
    while IFS= read -r -d '' file; do
        add_python_copyright "$file"
        ((total_processed++))
    done < <(find python -type f -name "*.py" -print0)
fi
echo ""

# 输出统计信息
echo "========================================"
echo -e "${GREEN}版权头添加完成！${RESET}"
echo "----------------------------------------"
echo "处理的文件总数: $total_processed"
echo "已有版权跳过数: $total_skipped"
echo "新添加版权数量: $total_added"
echo "========================================"
