@echo off
REM Run wrapper - bypasses PowerShell execution policy
REM Usage: run.bat [options]
REM Examples:
REM   run.bat
REM   run.bat -Config config/cascade_control.yaml
REM   run.bat -Debug
REM   run.bat -Shell

PowerShell -ExecutionPolicy Bypass -File "%~dp0run.ps1" %*
