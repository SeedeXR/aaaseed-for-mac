# **AAASeed_EXE repository ReadMe:**

# **What is AASeed ?**

**AAASeed** is the software its author **Mâa** (Emmanuel Berriet) has used for over 30 years to create real-time, interactive, and immersive experiences.  
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

This repository contains the **C++ source code used to build `AAASeed.exe`**.

As of **January 2026**, **AAASeed** is a **Windows application**, tested on **Windows 10 and 11**. It should also run on **Windows XP, 7, and 8**, although these platforms are no longer actively tested.  
The software was written with **portability in mind**, and is expected to be ported to **Linux and macOS** in the future.

**AAASeed** is more than just an executable. A significant part of its interface and runtime environments is implemented in **Lua** on top of the C++ core.  
This Lua-level code can be found in the **AAASeed repository of AAAFoundation**:  
👉 https://gitlab.com/aaa_foundation/aaaseed

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

---

# **How to build AAASeed.exe ?**
This is described here in a separate document [Build_AAASeed.md](Build_AAASeed.md).\
There is also a [CODE_STYLE.md](CODE_STYLE.md) document if you plan to write and commit C code.

 