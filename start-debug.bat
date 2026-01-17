@echo off
REM Debug wrapper - bypasses PowerShell execution policy
REM Usage: start-debug.bat
REM
REM This script automatically:
REM   1. Starts Docker container
REM   2. Starts debugpy server
REM   3. Opens VS Code
REM   4. Waits for debugger connection

PowerShell -ExecutionPolicy Bypass -File "%~dp0start-debug.ps1" %*
