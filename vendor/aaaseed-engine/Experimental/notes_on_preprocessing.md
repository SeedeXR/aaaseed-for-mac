

# Notes on Preprocessing

In this folder is an example of preprocessing configuration for params editing

## Objectives

 - define params in a separate file, to avoid verbosity of the actual macro defined.
   (  params are here for easy acces to some object properties from the ui. )
   (   bdd(s) are a common example of an AAASeed object where params need to be defined. )
   - at compilation a preprocessing script should parse this definition file and produce an output in
   some targeted .h and .cpp file(s)
   - build should update only target files when the preprocessing is run
   - preprocessing should run only when needed ( when param file is edited ) 

## Project configuration

There are 3 ways to launch a custom script or tool during the build with visual studio.

The first one is the build event, but it can't trigger recompilation of source files.

The second and third are custom build step and custom build tool which are specified in similar ways.

### Set up of custom build step

#### Reference explanations from MSVisual Documentation

 - vcxproj general file structure :
  https://learn.microsoft.com/en-us/cpp/build/reference/vcxproj-file-structure?source=recommendations&view=msvc-170
 - vcxproj custom build step specification :
  https://learn.microsoft.com/en-us/cpp/build/how-to-add-a-custom-build-step-to-msbuild-projects?view=msvc-170

#### Define CustomBuildStep

 - see in vcxproj :
    <CustomBuildStep>
      <Command>"$(ProjectDir)Utils\lua5.1.exe" $(ProjectDir)Src\preprocess.lua"</Command>
      <Outputs>$(ProjectDir)Src\obj_ui\bdd\bdd_ben.h;$(ProjectDir)Src\obj_ui\bdd\bdd_ben.cpp</Outputs>
      <Inputs>$(ProjectDir)Src\obj_ui\bdd\bdd_ben.param</Inputs>
    </CustomBuildStep>

#### Set up before build
 - see in vcxproj :
  <PropertyGroup>
    <CustomBuildBeforeTargets>ClCompile</CustomBuildBeforeTargets>
  </PropertyGroup>


## Lua example with bdd_ben.param

### The code

The lua code has a few minimum features to implement :

 - return errors formated as visual expects it, to retain the build to go on when there was an error.

 - produce an output in a target file, only when something valid is parsed in a param input file

For now the second feature only produce a commentary output to validate that the build is updated only on the target file when the param file is edited.

This can be tested by inserting or removing "token" from param file


