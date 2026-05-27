# AAASeed Installation Guide

## 1/4 - Creating Folders

Before installing AAASeed, set up your environment by creating the following hieracgy of folders:
```
AAA or AAAFoundation
  Media       : Store videos, images, sounds...
  Install     : Store installers, drivers, and required software for AAASeed
  AAADll          : Store DLLs associated with AAASeed
```
---

## 2/4 - Download Required Files

Access one of the following links to download the necessary files:

- [Remote Access](https://78.193.202.92:7001/index.cgi)
- [Local Access (faster)](https://192.168.1.42:5001/webman/index.cgi)

1. **Trust the site** if prompted.
2. *look it*
3. Navigate to **FileStation**:
4. Download and store the following files in `AAA/Install/`:
- `AAAGaBUZo.7z` → AAASeed with environments
- `Main_x64_2024.7z` → Associated DLLs
- `VC_redist_2015_2022_... .exe` → Required installer for AAASeed dependencies
- [K-Lite Mega Codec Pack](https://www.codecguide.com/download_kl.htm) → Handles video decompression
- [7-Zip (64-bit)](https://www.7-zip.org/) → Manages compression and decompression of 7z files
- [Visual Studio Code](https://code.visualstudio.com/) → Recommended text editor

---

## 3/4 - Installing Files and Configuring the System

1. **Install 7-Zip**  
Run `7z ... -x64.exe` and complete the installation.

2. **Extract AAASeed**  
- Extract `AAAGaBUZo.7z` into `AAAFoundation/`
- Launch `AAASeed_Metal.exe`
  - **An error may appear** because some system components are missing.
  - Pin `AAASeed_Metal.exe` to the **taskbar** for quick access.

3. **Install System Dependencies**  
- Run `VC_redist_2015_2022_... .exe`
- Extract `Main_x64_2024.7z` into `AAA/Dll/`
- **Add the DLL path to the system environment variables:**
  1. Copy the path of `AAA/Dll/`
  2. Open:
     ```
     This PC > Properties > Advanced System Settings > Environment Variables
     ```
  3. Under **User Variables**, click **Edit** > **New** and paste the path.
  4. Adjust the order if necessary and confirm.

4. **Test AAASeed Launch**
- Relaunch `AAASeed_Metal.exe`
- The application should now work.
- **Double Escape (Esc)** to save and exit.

---

## 4/4 - Final Installation and Configuration

1. **Install Additional Software:**
- **K-Lite Codec Pack** → Install with **default options**.
- **Visual Studio Code** → Install for Lua and configuration file editing.

2. **Set Up Display Configuration**
- Adjust **resolution** and **screen position**.
- Ensure that **scaling is set to 100%** on all screens.

---

### AAASeed is now installed and ready to use! 🚀

