#!/bin/bash
# 文件编码和行尾验证脚本

echo "========================================="
echo "文件编码和行尾验证"
echo "========================================="
echo ""

# 检查 UTF-8 编码
echo "1. 检查 UTF-8 编码..."
echo "-------------------"
non_utf8_files=0
for file in $(find src python -type f \( -name "*.c" -o -name "*.h" -o -name "*.py" \)); do
    if ! file -i "$file" | grep -q "utf-8"; then
        echo "❌ 非 UTF-8: $file"
        file -i "$file"
        non_utf8_files=$((non_utf8_files + 1))
    fi
done

if [ $non_utf8_files -eq 0 ]; then
    echo "✅ 所有源文件都是 UTF-8 编码"
else
    echo "❌ 发现 $non_utf8_files 个非 UTF-8 文件"
fi
echo ""

# 检查 BOM
echo "2. 检查 UTF-8 BOM..."
echo "-------------------"
bom_files=0
for file in $(find src python docs -type f \( -name "*.c" -o -name "*.h" -o -name "*.py" -o -name "*.md" \)); do
    if head -c 3 "$file" | grep -q $'\xef\xbb\xbf'; then
        echo "❌ 包含 BOM: $file"
        bom_files=$((bom_files + 1))
    fi
done

if [ $bom_files -eq 0 ]; then
    echo "✅ 所有文件都无 BOM"
else
    echo "❌ 发现 $bom_files 个包含 BOM 的文件"
fi
echo ""

# 检查行尾（使用 git）
echo "3. 检查行尾 (LF)..."
echo "-------------------"
if git ls-files --eol | grep -v "i/lf" | grep -v "^i/-" > /dev/null; then
    echo "❌ 发现非 LF 行尾的文件："
    git ls-files --eol | grep -v "i/lf" | grep -v "^i/-"
else
    echo "✅ 所有已跟踪文件都使用 LF 行尾"
fi
echo ""

# 检查 .gitattributes
echo "4. 检查 .gitattributes 配置..."
echo "-------------------"
if [ -f .gitattributes ]; then
    echo "✅ .gitattributes 存在"
    echo "配置内容："
    grep -E "(text|eol)" .gitattributes | head -10
else
    echo "❌ .gitattributes 不存在"
fi
echo ""

# 总结
echo "========================================="
echo "验证总结"
echo "========================================="
if [ $non_utf8_files -eq 0 ] && [ $bom_files -eq 0 ]; then
    echo "✅ 编码验证通过"
    echo "   - UTF-8: 是"
    echo "   - 无 BOM: 是"
    echo "   - 行尾: LF"
    exit 0
else
    echo "❌ 编码验证失败"
    echo "   - 非 UTF-8 文件: $non_utf8_files"
    echo "   - BOM 文件: $bom_files"
    exit 1
fi
