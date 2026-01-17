#!/usr/bin/env pwsh
# add_copyright.ps1 - 为所有源文件添加版权头

$COPYRIGHT_HEADER = @'
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

'@

function Add-CopyrightHeader {
    param(
        [string]$FilePath
    )

    Write-Host "处理文件: $FilePath"

    # 读取文件内容
    $content = Get-Content -Path $FilePath -Raw -Encoding UTF8

    # 检查是否已有版权头
    if ($content -match "Copyright \(c\) \d{4}") {
        Write-Host "  跳过: 文件已包含版权信息"
        return
    }

    # 添加版权头
    $newContent = $COPYRIGHT_HEADER + $content

    # 写回文件（保持 UTF-8 编码）
    $Utf8NoBomEncoding = New-Object System.Text.UTF8Encoding $False
    [System.IO.File]::WriteAllText($FilePath, $newContent, $Utf8NoBomEncoding)

    Write-Host "  完成: 已添加版权头"
}

# 主程序
Write-Host "开始为源文件添加版权头..."
Write-Host ""

# 处理 C 源文件
$cFiles = Get-ChildItem -Path "src" -Filter "*.c" -Recurse
foreach ($file in $cFiles) {
    Add-CopyrightHeader -FilePath $file.FullName
}

# 处理 C 头文件
$hFiles = Get-ChildItem -Path "src" -Filter "*.h" -Recurse
foreach ($file in $hFiles) {
    Add-CopyrightHeader -FilePath $file.FullName
}

# 处理 Python 文件
$pyFiles = Get-ChildItem -Path "python" -Filter "*.py" -Recurse -ErrorAction SilentlyContinue
foreach ($file in $pyFiles) {
    Write-Host "处理文件: $($file.FullName)"
    $content = Get-Content -Path $file.FullName -Raw -Encoding UTF8

    if ($content -match "Copyright \(c\) \d{4}") {
        Write-Host "  跳过: 文件已包含版权信息"
        continue
    }

    # Python 版权头
    $pyHeader = @'
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

'@

    $newContent = $pyHeader + $content
    $Utf8NoBomEncoding = New-Object System.Text.UTF8Encoding $False
    [System.IO.File]::WriteAllText($file.FullName, $newContent, $Utf8NoBomEncoding)
    Write-Host "  完成: 已添加版权头"
}

Write-Host ""
Write-Host "版权头添加完成！"
Write-Host "处理的文件数: $($cFiles.Count + $hFiles.Count + $pyFiles.Count)"
