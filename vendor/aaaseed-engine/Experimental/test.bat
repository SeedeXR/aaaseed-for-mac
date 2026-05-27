ECHO Bordel:
FOR /R "M:\AAAMaa\AAADev\AAASeed\Src" %%f IN (*.h, *.cpp) DO (
    ECHO Processing file: "%%f"
    IF NOT EXIST "%%~nf.param" (
        ECHO No param file skipping: "%%~nf.param"
        REM skip files without a corresponding .param file
        GOTO :continue
    )
    FOR %%g IN ("%%~dpnf.h" "%%~dpnf.cpp") DO (
        ECHO testing: "%%g"
        IF %%~tg LSS %%~t"%%~dpnf.param" (
            ECHO found: "%%g"
            "C:\path\to\lua.exe" preprocess.lua "%%~dpnf.param" "%%~dpnf.h" "%%~dpnf.cpp"
            GOTO :break
        )
    )
)
ECHO End loop: give me a message :)
