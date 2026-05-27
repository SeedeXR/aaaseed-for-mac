# **AAASeed repository ReadMe:**

Another repository 👉 https://gitlab.com/aaa_foundation/aaaseed_exe
contains the **Full C++ source code of the executable and detailed instructions to build `AAASeed.exe`**.

# **What is AASeed ?**

**AAASeed.exe** is the software its author **Mâa** (Emmanuel Berriet) has used for over 30 years to create real-time, interactive, and immersive experiences.  
**AAASeed** website is **[AAASeed.org](https://AAASeed.org)**.

**AAASeed** has been used in a wide variety of contexts: from **Video Jockeying** and **live election results on national TV**, to **live show backdrops** and **large-scale immersive collective games**... 
Its domains of application are intentionally broad and largely unbounded.

To support this flexibility, **AAASeed** relies on a **fast [Lua](https://lua.org/) scripting engine**, running on top of an **old-school C++ graphics and computation core**. The C++ core provides high-performance support for:

- 2D and 3D rendering  
- Fonts  
- OpenGL, OpenCL, OpenCV  
- Video playback and capture  
- Shaders (GLSL)
- MIDI, OSC  
- Multitouch  
- Multi-screen and multi-machine setups  
- ...


**AAASeed** is more than just an executable. Much of its interface and runtime environment is built in **Lua** on top of the C++ core.  
This **Lua**-level code and environment is the main focus of this repository and is what most users will interact with.

As of **January 2026**, **AAASeed** is a **Windows application**, tested on **Windows 10 and 11**. It should also run on **Windows XP, 7, and 8**, although these platforms are no longer actively tested.  
The software was written with **portability in mind**, and is expected to be ported to **Linux and macOS** in the future.

**AAASeed** is designed to be useful to many kinds of users, from curious hobbyists to hardcore developers.  
Feel free to explore, experiment, and contribute!

---

# **Why AAASeed exists ?**
**AAASeed** exists because when Mâa started, **no real-time software was available** for the kind of work he was doing.  
It then grew **organically from production to production**, and the introduction of [Lua](https://lua.org/) later **completely changed its perspective**.

Rather than focusing on a single domain or use case, **AAASeed** was built to adapt to constantly changing contexts: new hardware, new protocols, and new artistic or technical constraints.  
Today, it exists to provide a **long-lived, flexible, and expressive platform** for real-time interactive creation.

The combination of a **high-performance C++ core designed for dynamic graphics and incremental, live visual systems** and a **dynamic Lua scripting layer** allows rapid experimentation without sacrificing execution speed or low-level control.  
Things can be done incrementally, and even the interface and rendering behavior can be modified on the fly.

This architecture reflects decades of real-world use in live, time-critical, and often unpredictable environments.  
It is **extremely stable in production**, having been tested in continuous, intensive runs of up to **140 days**.

---

# **Who should use AAASeed ?**

**AAASeed** may be useful if you are:

- Creating **real-time interactive or immersive systems**
- Working in **live performance**, **installation art**, or **broadcast graphics**
- Looking for a **Lua-driven engine** with deep access to graphics, video, and I/O
- Interested in a **battle-tested codebase** evolved through long-term practical use
- Comfortable exploring a powerful but non-opinionated toolset

It is likely **not** a turnkey solution, but rather a **foundation** for developers, artists, and researchers who value flexibility and control over prescriptive workflows.

# **Manual and documentation**

In the `AAADoc` folder, you’ll find the documentation, including a [manual](AAADoc/AAASeed_Introduction.pdf). We are currently updating and cleaning the documentation to make it clearer and easier to use.

---

# A/ Install Notes
- If not done yet you need to install VScode: https://code.visualstudio.com/. Other text editor options are possible, but this is our default solution and best solution for us now (2025 March).
- We get our video codec from https://codecguide.com/download_kl.htm. Usually we Install the Mega Pack with all options at default (dialog ok ok ok ...).
- We also recommend Windows PowerToys: https://learn.microsoft.com/en-us/windows/powertoys/.
- XnView Classic or XnView MP are nice image browser at: https://www.xnview.com/en/.
- Blender at https://www.blender.org/ is your 3d free solution.
- Wings3d at https://www.wings3d.com/ need to be mentioned here it exports 3d files in .obj that **AAASeed** love.
- https://www.7-zip.org/ is our compression choice.


# B/ Use Metal version
- **AAASeed_NewMetal.exe** is the most optimized version (Tuned to the metal) and the default version to use.
- But it crashes at start with processors not having AVX2 (Advanced Vector Extension 2). It crashes with no message, no dialog, it just closes in 5 seconds.
- In this case use **AAASeed_Wood.exe** which doesn't require the AVX2 processor extension.


# C/ File tree and Start sequence 
**AAASeed** is not a traditional software: installed in ProgramFile, having its preference saved somewhere in the system... At some point **AAASeed** will run on other OS than Microsoft windows and we intend to keep the same kind of file tree structure on every OS.

## C.1/ **AAAFoundation** root folder
- **AAAFoundation** is the "root" directory, this name is not mandatory (Mâa use AAA on his Machine) but it is recommended and used by the installer/updater we provide.
```
AAAFoundation	// root folder
	AAADll			// dlls used by AAAFoundation
	AAASeed			// AAASeed environment
		AAAAPPs			// examples, tutorials, tests....	
		AAADoc			// documentation
		AAAKernel		// AAAKernel only for developer
		AAASeed_Metal.exe
		AAASeed_Wood.exe
		README.md		// this document
		ReleaseNotes.md
		version.txt		// version of the installer
	AAAUser		// folder holding your information
		Guest		// folder for AAASeed global preferences
		Guest_net	// folder for network configuration
		AAAWho.txt	// define folder used for prefs, net and APPs
	APPs_Guest	// your APP folder (can change Name)
	Media		// where you should store your medias
```

## C.2/ **AAASeed** folder
- The **AAASeed** folder, inside **AAAFoundation**, holds everything necessary to run **AAASeed** and its related lua AAAGaBuZoMeu environment. Again the **AAASeed** folder name is not mandatory. You can copy and/or rename this directory to keep older versions of the software.

- In fact, there is an exception to the "all in **AAAKernel** rule": the **AAADll** Folder under **AAAFoundation** which holds the dynamic libraries used by **AAASeed**. **DO NOT RENAME THIS FOLDER** or **AAASeed** will not be able to access its dlls.

- This **AAASeed** folder is what is touched when **AAASeed** is updated. Your own data should be somewhere else if you want to preserve it.

## C.3/ **Media** folder
- the **Media** folder, inside **AAAFoundation**, with subdirectories (e.g. users folder Alain, Priam, Tendayi, or project folder) is where users should put their own medias (images, videos, fonts...). We separate it from the applications folder because medias can be way bigger and are usually handled (and saved) very differently in the production process then the application data.
- This is your Media space, organize yourself as you want inside.

## C.4/ **APPs** folders
- this is where **AAASeed** Applications (or APP) are stored:   
	- **AAAAPPs**, in the **AAASeed** folder, with examples, test... Provided and maintained by the AAASeed crew. 
	- **APPs_Guest**, in the **AAAFoundation**, where **AAASeed** users (**Guest** by default) can store their own APPs.

## C.5/ **AAASeed** launch sequence and user notion
- The **start** folder is where **AAASeed** is launched from, for most of you it is where the executables reside, the **AAASeed** folder.
- Then, in this start folder, **AAASeed** finds the folder **AAAKernel** which become the kernel folder.
- At this point **AAASeed** will try to find which **user** run the software. **User** have their own global preferences, network preferences and APPs folder.  
- **AAASeed**, in the folder **AAAFoundation**, will try to open a file **AAAWho.txt** inside folder **AAAUser**. Path is ../AAAUser/AAAWho.txt from the starting directory.
```
AAAFoundation
	AAASeed
	AAAUser			// folder holding default user information
		Guest		// Folder holding pref for user Guest 
		Guest_net	// Folder holding network configuration for user Guest 
		AAAWho.txt	// default user configuration
	APPs_User	// your APP folder (can change Name)
	Media		// where you should store your medias
```

## C.6/ AAAWho.txt
- the file path is **AAAFoundation/AAAUser/AAAWho.txt**.
- **AAAWho.txt** contains 3 lines (2025 March):
	- name of the folder for the global preferences: **user folder**.
	- name of the folder for the network preferences: **net folder**.
	- name of the folder for the APPs folder: **app folder**.

- These names are relative to the AAAUser folder

- this is a Default AAAWho.txt file:
```
Guest
Guest_Net
../APPs_Guest
```
- To make your own just copy the directories Guest and Guest_net, also make your app folder then edit the AAAWho.txt. 
- this the AAAWho.txt used by Mâa:
```
Maa
Maa_Net
../APPs_Maa
```
- in this case you should have the corresponding folders:
	- Maa and Maa_net in **AAAUser**
	- APPs_Maa in **AAAFoundation**

## C.7/ APPs folder
- the **APPs** folder just help to the define a relative folder when saving filename, it making it more easy to move APPs around, move prefs from machine to machine, store previous one, etc...

## C.8/ WatchDog
- if the exe name is changed and include **watchdog** (e.g. AAASeed_WatchDog.exe), **AAASeed** start with no graphics windows (just the terminal) and execute a specific lua script in the user folder: **aaa_watchdog.lua**. Currently (2025 March) this script make sure an instance of **AAASeed_Metal.exe** is running. if none is running: it start one. If one is running: it expect to receive information from it at a certain interval. If not the running **AAASeed** is considered crashed, and so the "faulty" **AAASeed** executable is killed and then restarted. Hence the WatchDog concept. 

## C.9/ executable Arguments
- For now (2025 March) the arguments are ot used, but they are just stored in c_info (F10 in flatland, preferences/Executable/exe_arguments), so they can be accessed and stored.

## C.10/ Shift at start
- When **AAASeed** start if you hold the shift key **AAASeed** will be more verbose in the starting phase to help diagnose a problem at start. 



# D/ Git 

This repository holds the environment needed to run **AAASeed** except the dll.
The **AAASeed** installer uses these files, and there are regular updates, so, unless you are developing on **AAASeed**, *you should not need* to use this repository.

There is also the [release notes](./ReleaseNotes.md)

You will find documentation in the [./AAADoc](./AAADoc) directory.

- [System overview](./AAADoc/AAASeed_system_overview.md)
- [UI documentation](./AAADoc/AAASeed_design_system.md)
- [Design System](./AAADoc/AAASeed_UI_documentation.md)
- [Technical documentation](./AAADoc/AAASeed_technical_documentation.md)
- [Functional documentation](./AAADoc/AAASeed_functionnal_documentation.md)
- Old documentation in the [archives](./AAADoc/archive)

# TODOs
- Document watchdog (and the lua file at the root)
- App on command line to open an app (pass args to lua)