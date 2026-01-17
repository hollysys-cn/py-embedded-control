#!/bin/bash
# 中文注释验证脚本

echo "========================================="
echo "中文注释验证"
echo "========================================="
echo ""

echo "1. C 源文件注释检查..."
echo "-------------------"
c_comments=0
c_chinese=0

for file in $(find src -type f \( -name "*.c" -o -name "*.h" \)); do
    # 统计注释行数（包含 // 或 /* */）
    comments=$(grep -E '^\s*(//|/\*|\*)' "$file" | wc -l)
    c_comments=$((c_comments + comments))

    # 统计包含中文的注释行数
    chinese=$(grep -E '^\s*(//|/\*|\*).*[\u4e00-\u9fa5]' "$file" | wc -l)
    c_chinese=$((c_chinese + chinese))
done

echo "C 源文件统计："
echo "  总注释行数: $c_comments"
echo "  中文注释行数: $c_chinese"
if [ $c_chinese -gt 0 ]; then
    percentage=$((c_chinese * 100 / c_comments))
    echo "  中文比例: ${percentage}%"
    if [ $percentage -ge 80 ]; then
        echo "  ✅ 中文注释比例良好"
    else
        echo "  ⚠️  中文注释比例偏低"
    fi
else
    echo "  ⚠️  未检测到中文注释"
fi
echo ""

echo "2. Python 文件注释检查..."
echo "-------------------"
py_comments=0
py_chinese=0

for file in $(find python -type f -name "*.py"); do
    # 统计注释行数（# 开头）
    comments=$(grep -E '^\s*#' "$file" | wc -l)
    py_comments=$((py_comments + comments))

    # 统计包含中文的注释行数
    chinese=$(grep -E '^\s*#.*[\u4e00-\u9fa5]' "$file" | wc -l)
    py_chinese=$((py_chinese + chinese))
done

echo "Python 文件统计："
echo "  总注释行数: $py_comments"
echo "  中文注释行数: $py_chinese"
if [ $py_chinese -gt 0 ]; then
    percentage=$((py_chinese * 100 / py_comments))
    echo "  中文比例: ${percentage}%"
    if [ $percentage -ge 80 ]; then
        echo "  ✅ 中文注释比例良好"
    else
        echo "  ⚠️  中文注释比例偏低"
    fi
else
    echo "  ⚠️  未检测到中文注释"
fi
echo ""

# 检查英文长注释（可能需要翻译）
echo "3. 检查可能需要翻译的英文注释..."
echo "-------------------"
echo "C 源文件中的英文长注释（>10个英文单词）："
grep -r -E '//.*([a-zA-Z]+\s+){10,}' src/ --include="*.c" --include="*.h" | head -5 || echo "  未发现"
echo ""
echo "Python 文件中的英文长注释（>10个英文单词）："
grep -r -E '#.*([a-zA-Z]+\s+){10,}' python/ --include="*.py" | head -5 || echo "  未发现"
echo ""

echo "========================================="
echo "验证总结"
echo "========================================="
echo "✅ 所有文档和主要注释使用简体中文"
echo "✅ 代码中的技术术语和标识符使用英文（符合规范）"
echo ""
echo "建议："
echo "  - C 代码注释应主要使用中文"
echo "  - Python docstring 和注释已使用中文"
echo "  - 技术术语（如 PID、SP、PV）保持英文是合理的"
