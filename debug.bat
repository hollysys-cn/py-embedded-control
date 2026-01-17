@echo off
REM Debug mode wrapper - bypasses PowerShell execution policy
REM Usage: debug.bat

PowerShell -ExecutionPolicy Bypass -File "%~dp0run.ps1" -Debug
