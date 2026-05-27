# How to build AAASeed.exe
### 1/ Install Visual Studio 2026
- We tested it with Microsoft Visual Studio 2026 Community Version: Insiders [11408.92]. We got it from https://visualstudio.microsoft.com/insiders but any version of Visual Studio 2026 or 2022 would do it.
- Visual Studio 2026 correspond to version v145 and  Visual Studio 2022 correspond to v143 version. Previous compilers should be able to build it also (v142, v141, v120), but wedid not use these for a while and removed the corresponding includes, librairies and configurations.
- Use only x64 configurations. We kept Win32 version of the projects just for history but removed all the corresponding libraries and dlls.

### 2/ Create a local git AAASeed_EXE Repository  
The online repository is here [AAASeed_EXE repository](https://gitlab.com/aaa_foundation/aaaseed_exe) : https://gitlab.com/aaa_foundation/aaaseed_exe

### 3/ Download and Install CUDA ToolKit
If you do not use Nvidia Flex (Fluid and spring/cloth simulation in AAASeed) or Cuda you can skip this step. But you will have to edit a file before Building.
- Go to [CUDA Toolkit Archive](https://developer.nvidia.com/cuda-toolkit-archive)  
- Download The lastest Release (13.1 is the last one we tested against).
- Install it.

### 4/ Get the right NVidia Flex include, librairies and dlls from Nvidia.
If you do not use Nvidia Flex (Fluid and spring/cloth simulation in AAASeed) you can skip this step.
- The Flex and Cuda library and Cuda dlls used by the flex library are not in the AAASeed_EXE repository, we are currently exploring the legal issues with NVidia. For the moment if you want to compile and run AAASeed without Flex you need to get yourself some files. Because of the license on these files we can not distribute it and you can not redistribute it too.  

- The Nvidia Flex repository is at https://github.com/NVIDIAGameWorks/FleX

- Download  https://github.com/NVIDIAGameWorks/FleX/blob/master/include/NvFlex.h  
to ```AAASeed_EXE\Include\NVidia\NvFlex.h```  
Which would be tricky the first time: this folder being empty in the repository, it does not exist yet. You have to create the ```NVidia``` folder inside the ```Include``` folder yourself.

- from  
https://github.com/NVIDIAGameWorks/FleX/tree/master/lib/win64  
Download  
```NvFlexReleaseCUDA_x64.lib```  
```NvFlexDebugCUDA_x64.lib```  
to ```AAASeed_EXE\lib_x64\NVidia```  
here too you have to create the ```NVidia``` folder inside the ```lib_x64``` folder yourself.

- from  
https://github.com/NVIDIAGameWorks/FleX/tree/master/bin/win64  
Download
```NvFlexDebugCUDA_x64.dll```  
```NvFlexReleaseCUDA_x64.dll```  
```NvFlexDebugCUDA_x64.dll.pdb``` (optional)  
```NvFlexReleaseCUDA_x64.dll.pdb``` (optional)  
```cudart64_92.dll```  
to  
```AAASeed_EXE\dll_x64```   
These dlls will be later needed in the AAASeed Dll directory for AAASeed.exe to run. We don't use a specific ```NVidia``` directory here. 

### 5/ Define environment variables
Use Windows Advanced system settings/Environment Variables User or system to do this.  
![EnvVars](doc/Images/ReadMe_Environment_Variables.png) 
#### -AAAOut
- it defines where the compiler and linker save intermediate and output files.
#### -AAASeed_DirStart
- it defines the folder where Visual Studio will start AAASeed (with all the lua stuff we mentioned earlier).
#### -AAASeed_Path_CUDA
- it defines the folder where Visual Studio get includes and libraries for CUDA (Nvidia low level language for their Graphic Cards). More later below under the Flex Chapter. It should be something like  
```C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v13.1```  
depending on the cuda version you installed.

### 6/ Launch Visual Studio and Open
```AAASeed_by_Maa.sln```

### 7/ Configure the use of the Nvidia Flex Library 
- It is now compiled with by default, so nothing to do if you installed the required files (see before).
- edit ```AAASeed_EXE\Src\flex\flex_sdk.h```
```
#define AAA_USE_FLEX() 1	// to use FLEX
or
#define AAA_USE_FLEX() 0	// to disable FLEX
```

### 8/ Pick a v145/x64 configuration
####  **Metal** version is the the default version for Users (Tuned to the Metal). It take 4/5 minutes to link (Global optimization).
- It crashes at start on processors not having AVX2 (Advanced Vector Extension 2). It crashes with no message, no dialog, it just closes in 5 seconds.
- In this case use **AAASeed_Wood.exe** which doesn't require the AVX2 processor extension.
####  **Debug** version is made to use Edit and Continue Features.
####  **Release** version is faster, use debugging but no editing on the Fly.
####  **Wood** version is like the **Metal** one but don't use AVX2 and so it functions on old older and cheaper PCs.
- AAASeed project should be able to use any C and C++ compiler options. By default we set it to  
```Preview - Features from the Latest C++ Working Draft (/std:c++latest)```  
and  
```Preview - Features from the Latest C Working Draft (std:clatest)```

### 9/ Build
Et Voila !
- All warnings were treated, compiling AAASeed generates nearly no messages when things go well.


### 10/ Optional use of VS Code
There is two file ```build.ps1``` (a helper) and ```.vscode/tasks.json``` to help you build AAASeed directly from VS Code.  
```.vscode/tasks.json``` get called when you do Ctrl + Shift + b, it will let you choose AAASeed configuration(s) to build, and launch the build(s).  

Add  
 ```
 {
	"key": "f7",
	"command": "workbench.action.tasks.runTask",
	"args": "AAASeed Build Last"
}
```
to VS Code  ```keybindings.json ``` and f7 will be a shorcut to recall the last selected build.
  