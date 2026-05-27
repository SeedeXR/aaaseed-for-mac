# Compiling AAASeed
## Install Visual Studio community 2022 Preview
2024 August: it should function also with Visual Studio community 2022 or Visual Studio community 2019.  

## Define some environement variables
- goto to Settings/System/About
- press Button Advanced system settings: the System Properties dialog open
- press Environement Variables: the Environement Variables dialog open
- define variable AAAGaBuZo for the folder where the exe will be launch
- define variable AAAOut for the folder of the intermediate files
- close dialogs

## Launch Visual Studio
- Open solution file AAASeed_Maa.sln
- choose one of Debug/Metal/Release/Wood_v143 configuration in x64 node  
other are still here in case but unused  
-Debug can do edit and continue  
-Metal is the most optimised one  
-Wood also but for machine with no AVX2 Instruction set  
-Release is faster than debug  
- Compile and Link
