@echo off
setlocal enabledelayedexpansion
set /a count=1
for /f "skip=2 tokens=2,*" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine" /s /v "InstalledDirectory"') do (
    set UnrealEnginePath=%%b
    set UnrealEditorPath=!UnrealEnginePath!\Engine\Binaries\Win64\UnrealEditor.exe
    if exist "!UnrealEditorPath!" (
        echo !count!: !UnrealEnginePath!
        set UnrealEditorPath[!count!]=!UnrealEditorPath!
        set /a count+=1
    )
)

if %count% equ 2 (
    if exist "Client.uproject" (
        call "!UnrealEditorPath[1]!" "%~dp0Client.uproject" -game -windowed -ResX=1024 -ResY=576
    ) else (
        echo Client.uproject not found
    )
    exit
)

set /p choice="Enter the number of the engine you want to run: "
if defined UnrealEditorPath[%choice%] (
    if exist "Client.uproject" (
        call "!UnrealEditorPath[%choice%]!" "%~dp0Client.uproject" -game -windowed -ResX=1024 -ResY=576
    ) else (
        echo Client.uproject not found
    )
) else (
    echo Invalid choice
)

