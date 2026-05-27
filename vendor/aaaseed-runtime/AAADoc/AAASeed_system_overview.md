# AAASeed, system overview
_
The AAASeed software provides users with powerful 3D rendering capabilities and an easy-to-use environment. It is purpose-built to empower creators in developing interactive, immersive, and large-scale experiences that push the boundaries of digital art and data visualization.

## Main concepts
The architecture of AAASeed tries to use simple concepts and to deal with complexity by layering these to form more elaborate concepts. AAASeed can be seen as a construction game where the user builds complex objects with plenty of simple pieces (thus the Lego metaphor).

AAASeed is designed to build applications (APPs). The results of a project build on AAASeed is an application or a collection of applications that will run together and produce what the user wants it to do.

>AAASeed is a kind of *Operating System*, that lets the user launch applications and connect to everything he would need on the machine he’s using, as well as other machines or external resources (Kinect, microphone, storage, APIs, …)

When a user starts AAASeed, the first thing he will see is the GaBuZoMeu environment. This is the space where the result of the app is displayed. GaBuZoMeu can be switched to Edition mode, this will be the main working environment, where the user will specify everything about his project, define the monitors, outputs, inputs, MEUs, edit specific code, choose the right parameters, …

The GaBuZoMeu environment is mainly configurable using the built-in Edition mode, and Lua code. It relies on the backend of AAASeed, the kernel and core system which is written in C++ for Windows.

On top of the AAASeed Kernel written in C++, the GaBuZoMeu environment is written using scripting (lua only for the moment - 2023 September). It can be used very basically as the control interface on a predefined process, but it is very flexible and can be modified on the fly. In fact, its design evolved over time to give more advanced users the possibility to incrementally design and build an APP.

Here are all the concepts used in AAASeed, they will be described later the tech and functional documents: 

- The **environments**: Flatland & GaBuZoMeu
- **MEU Objects**: A series of functional modules usable in GaBuZoMeu, they are the basic building blocks that enable users to create, modify, and interact with multimedia elements.
- **BU Interface and Interaction Elements**: Objects like SLIDER, BUTTON, MONITOR, and SELECTOR enable intuitive user interaction, making AAASeed accessible to diverse users, ranging from professional developers to artists.
- **Multimedia Support**: Elements like BLOBS, Kinect, Camera, Shaders, Lights, and Image support the integration and manipulation of various multimedia types, including video, audio, and images.
- **Modular Approach**: Concepts like Bdd, Layer, Layers, Layerss, Module, Modules, App enable a flexible and modular approach to design, allowing for the creation of customized and reusable components..
- **3D Objects and Visualization**: concepts like color, shading, multiple, and texture support 3D modeling, rendering, and visualization, fostering creativity in design and presentation. OpenGL rendering.
- **Scripting and Customization**: With Lua support (edit and change on the fly) and Source code access, developers have the freedom to write and edit C++ blocks or scripts, allowing for tailored applications and advanced control over the system's behavior.
- **Hardware and System Interaction**: Elements like CPU, OS, executable, Mouse, Keyboard, and Cursor, file reader, dialog ... while insulating from underlying hardware and system processes, facilitate interactions with it, enabling efficient and optimized performance.
- **Networking and Communication**: Features like Midi, OSC, Net, Net_link and DMX support networking and communication between different devices and platforms, allowing for collaborative and connected experiences.
- **Additional Utilities**: AAASeed offers a range of auxiliary utilities, including timing, stereoscopic support, power management, file handling, picking, and much more.
- **Multi Touch**: The backend dealing with the UI elements is natively multitouch.
- **Multi Users, Multi Machines**: Some high end projects are designed by a team editing and working on the realtime process at the same time. Some installations also require several machines, maybe several dozen machines, all synced, having partial or total status of master and slave on different tasks, all needing to be globally scripted.
- **Mono task:** AAASeed is globally mono task, as the rendering, drawing and UI are done in a deterministic order. Parallelism can be used locally when performance is key.
- **Synchronous rendering**: The rendering, using GOL (the Graphics Open Library) encapsulation of OpenGL, is done every frame in a very deterministic way allowing a succession of rendering from C objs and scripts, with the possibility to hybrid the two.
- **Portability**: From the beginning, the code and the concepts have been developed with portability in mind. AAASeed now relies on a modern event pump architecture, for the same reason. If the port itself (Linux & OSX) is outside the scope of ArtCast4D, it will be an important task for the AAASeed community.
- **Realtime**: AAASeed is written to create realtime graphics (a minimum of 25 frames by second). It includes a lot of optimization and specific code using an update then draw strategy. Its architecture was created to harness the power of modern GPUs.
- **Stability**: Stability and regularity is crucial to AAASeed applications, as they shall not crash or fail. The memory allocation strategy is specifically designed to ensure that. Heavy duty applications have been used 24/7 for up to 85 days without a single restart.
- **Simplicity and exposed complexity**: Complexity can not be hidden, it always comes to surface. This is why AAASeed presents complex processes as a combination of more simple and understandable elements, to let the user choose his strategy instead of hiding things under the UI.
- **Build generic**: This can be seen as a strength or weakness. It doesn’t assume certain usages or domains biases. This means that you can always design what you want. Sometimes the cost in blocks and options is more important, but the application field is limitless.

## Areas covered

AAASeed is packaged to provide tools on 3 areas.

### VJing
*VJing, or Video Jockeying, is a real-time visual performance art where imagery is created, manipulated, and mixed in synchrony with music or sound. VJing apps and projects are instrumental in enhancing the audio-visual experience at concerts, clubs, festivals, and other live events. These apps often offer a wide range of features, such as integration with various media sources, real-time video manipulation, special effects, and the ability to work with different hardware controllers. They enable artists to blend visuals, videos, and real-time graphics creatively, allowing for improvisation and artistic expression that aligns with the music's rhythm and mood. VJing projects may vary in complexity and scale, ranging from simple visual accompaniments to intricate multimedia installations that captivate audiences and elevate the overall sensory experience. The ongoing innovation in VJing apps and projects continues to push the boundaries of live visual entertainment, making it a vibrant and essential aspect of modern multimedia art.*

AAASeed is a key tool for VJing, providing VJs the following features, out of the box: 

- An integrated building environment to create apps and projects
- An integrated development environment to customize the projects
- Real time performance control
- Hardware integration
- Collaboration with other live components and software
- Pre-built apps, libraries, and tools
- Modularity and flexibility using the built-in GaBuZoMeu environment.

### Digital artistic installations / Mapping / Museums

*Digital artistic installations and especially in museums represent a convergence of art, technology, and space, transforming ordinary environments into interactive and immersive experiences. Utilizing projectors, sensors, and various multimedia elements, artists can create stunning visuals that respond to human interaction or environmental changes. These installations can breathe life into historical artifacts, add depth to exhibitions, and engage visitors in a unique way. Mapping allows for the precise projection of visuals onto irregular surfaces, creating illusions and enhancing the physical structure of objects. Museums around the world have begun to embrace these techniques, redefining the way audiences interact with art and history.*

AAASeed is providing all the tools to create: 

- Comprehensive tools for creation and mapping
- Integration of various media types
- Interactive and immersive experience
- Collaboration and flexibility
- Ease of use and customization
- Support for large scale installations (public capture in particular)
- Scripting
- Extended remote control

### Open Data & Data Visualization and interaction 

*Open Data is about making data publicly available without restrictions, whereas Data Visualization is the representation of this information in a visual format. Interaction refers to the ability to engage with these visualizations, allowing users to manipulate, explore, and derive insights from the data. The combination of Open Data with visualization and interaction is powerful, as it not only democratizes access to complex information but also makes it more understandable and actionable. Governments, organizations, and researchers can use this synergy to communicate information more effectively, stimulate innovation, enhance decision-making, and foster community engagement.*

AAASeed provides everything needed to display and interact with data: 

- Dynamic visualization tools
- Real time interaction with data
- Accessibility and usability
- Integration of multimedia elements
- Customizable modules
- Collaboration and app sharing
- Scalability and performance
- Sovereignty and Open Source

## What software AAASeed is related to?

- Touch designer
  - TouchDesigner is a visual development platform that equips artists, designers, and researchers with the tools to create stunning real-time projects and rich user experiences. Whether for live installations, film production, or interactive media, TouchDesigner offers an intuitive node-based interface that allows users to build complex, interactive visuals.
- Vvvv
  - Vvvv is a visual programming environment that enables artists, designers, and developers to create a wide range of multimedia projects. It's particularly known for its applications in real-time video synthesis and has become popular within the audiovisual and interactive art communities. Its node-based approach simplifies complex programming tasks.
- Ventuz
  - Ventuz is a real-time 3D graphics content creation, authoring, and playout control software, used for creating interactive and immersive presentations, events, and broadcast graphics. It combines strong data visualization features with robust interactivity options, making it ideal for professional use in various multimedia fields.
- Max MSP
  - Max MSP is a visual programming language for music and multimedia. Often used by composers, performers, and artists to create complex audio, video, and interactive works, Max MSP provides an intuitive way to build interactive algorithms without having to write code, allowing for greater creativity and experimentation.
- pure data
  - Pure Data (Pd) is an open-source visual programming language designed for creating interactive computer music and multimedia works. Like Max MSP, it allows users to create sounds, graphics, and interactivity in a visual logic environment, providing a great platform for artists and musicians.
- Chataigne
  - Chataigne is an open-source modular software designed to create versatile and synchronized show control. It allows the integration of various devices, software, and protocols to enable synchronized control over complex multimedia setups, making it suitable for live shows and exhibitions.
- Processing
  - Processing is an open-source graphical library and integrated development environment (IDE) built for the electronic arts, new media art, and visual design communities. It provides a flexible software sketchbook for learning how to code within the context of visual arts.
- Smode
  - Smode is a real-time 2D/3D creation, compositing, and video-mapping software. It's used extensively in live shows, fixed installations, and for broadcast applications, offering a wide array of features to manipulate visuals in real time.
- UnReal
  - Unreal Engine is a complete suite of tools for game developers, designers, and artists, known for its cutting-edge graphics capabilities. It's widely used in the creation of video games, simulations, architectural visualizations, and real-time 3D movies.
- Unity
  - Unity is a powerful cross-platform game engine that allows developers to create interactive 3D content with ease. Known for its user-friendly interface, it's used not only in game development but also in virtual reality, augmented reality, simulations, and other interactive media applications.
- Roblox
  - Roblox is an online platform that allows users to design, create, and play games created by other users. Featuring a unique user-generated multiplayer experience, Roblox offers a diverse and community-driven ecosystem where creativity and collaboration thrive.
- Notch
  - Notch is a real-time content creation tool that empowers artists to create interactive and video content in a 3D space. Widely used in live events, installations, virtual productions, and more, Notch offers a node-based workflow that integrates seamlessly with other media servers and production tools.

