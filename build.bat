@echo off
REM Build wrapper - bypasses PowerShell execution policy
REM Usage: build.bat [options]
REM Examples:
REM   build.bat -All
REM   build.bat -Runtime
REM   build.bat -Python
REM   build.bat -Clean

PowerShell -ExecutionPolicy Bypass -File "%~dp0build.ps1" %*
