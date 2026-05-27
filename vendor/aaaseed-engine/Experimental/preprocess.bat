
set pre=---AAAPre:
set arg1=%1
setlocal enabledelayedexpansion
echo %pre% script called with arg "%arg1%"
set dir_obj=%arg1%Src
echo %pre% dir_obj is "%dir_obj%"
set src_dir=%~dp0Src
echo %pre% Begin loop for param files in dir: %src_dir%
echo %pre% src_dir is %src_dir%
rem echo %pre% M:\AAAMaa\AAADev\AAASeed\Src
rem for /R %src_dir% %%f in (*.param) do (
for /R %src_dir% %%f in (*.param) do (
    set fname=%%~dpnf
    echo %pre% Processing file: %%f !fname!
    rem set path_relative=%%~nxf
    set fname_rel=!fname:%src_dir%=!
    echo %pre% relative fname is !fname_rel!
    set fname_param=%src_dir%!fname_rel!.param
    echo %pre% rebuild param fname is !fname_param!
    set fname_obj=%dir_obj%!fname_rel!.obj
    echo %pre% rebuild obj fname is !fname_obj!

    rem echo %pre% Relative file name: %%~pnxf %%~nxf
    if not exist "!fname_param!" (
        echo %pre% No param file skipping: "%%~nf.param"
        rem skip files without a corresponding .param file
    ) else (
        for %%g in ("%%~dpnf.h" "%%~dpnf.cpp") do (
            echo %pre% testing: %%g timestamp are %%~tg lss %%~tf
            rem %%~tg %%~tf
            if %%~tg lss %%~tf (
                echo %pre% is older then param: %%g
                rem "%%~dpnf.param" "%%~dpnf.h" "%%~dpnf.cpp"
                rem "C:\path\to\lua.exe" preprocess.lua "%%~dpnf.param" "%%~dpnf.h" "%%~dpnf.cpp"
                rem GOTO :break
            ) else (
                echo %pre% is more recent then param: %%g
            )
        )
    )
)
echo %pre% End loop: