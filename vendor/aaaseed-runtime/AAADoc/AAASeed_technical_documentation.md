#AAASeed, technical specifications

## Software architecture

AAASeed is portable, currently developed and operated on Windows in the form of an executable file (.exe) accompanied by several dynamic libraries (dll).

Developed since 1996 entirely in C and C++, the software is operating system independent. For this purpose, AAASeed relies on several components:

An extended version of the GLUT (a publicly sourced library) handles basic interface tasks with the operating system (OS), such as opening graphical windows, basic event management, mouse, keyboard, and menu handling.

The OpenGL graphics library centralizes all display functions. However, it's partially isolated from AAASeed through an intermediate software layer, the GOL (see source files gol.h and .cpp).

Standard C and C++ libraries and the models from the Standard Template Library (STL) are used as described in the ANSI standards.

Specialized modules for specific tasks (e.g., sound management) are accessed through generic interfaces (integral to AAASeed) but are specialized for target OSes using compilation options (preprocessor).

All source files consistently use the ourtypes.h file, which defines variable types based on the target OS.

### Conceptual logic: the analogic synthesizer metaphor 

AAASeed's conceptual logic is based on the metaphor of analog synthesizers from the era before digital technology. These devices consist of independent physical blocks, each encapsulating a specific function: keyboard, oscillator, filter, ring modulator, reverb, etc. The musician connects these blocks using standard cables to create more complex processes.

AAASeed's blocks cover a broader application range, managing 2D and 3D geometric primitives, mathematical primitives, sound, video decompression, a wide range of input sensors, communication protocols, and more. AAASeed can be described as a "virtual Lego," a world editor, or a development environment. Using interconnected AAASeed blocks allows for virtually any software application. The application's scope is limited only by the features of existing blocks and the host machines' power.

### Key components of AAASeed

- **Block** (AAASeed Block): Represented mainly in the source files obj_ui (.h and .cpp), describing the classes c_obj and c_obj_ui. Almost all classes used in AAASeed inherit from c_obj_ui, which handles object reading/writing, object parameter access, and relations to other objects.

- **Connections** (AAASeed Connections): Defined in the source files connex (.h and .cpp), detailing the class c_connex, which encapsulates a connection between two parameters of two objects.

- **Base Class** (c_bdd): Due to its initial graphical nature, AAASeed's structure is organized around objects inheriting from the class c_bdd, encapsulating graphic primitives.

- **Upper Class** (c_layer): Objects of c_bdd are contained within layers defined as c_layer objects, which prepare the optimal rendering conditions.

- **Group** (c_layers): This class is a group of 26 layers (objects of c_layer from A to Z) and manages their successive display.

- **Module Class** (c_module): A group of groups (objects of c_layers from 0 to xxx), overseeing their successive display.

- **Modules Class** (c_modules): This class combines the list of c_module.

- **Global Class** (c_app): This application class (c_app in the source files app.h and .cpp) manages a c__modules object and controls the objects and attributes associated with all modules.

- **Rendering Loop**: The GLUT library acts as an event loop and calls AAASeed (callback functions).

### AAASeed block

The concept of a block (and in the longer term, a cell) is primarily embodied in the source files named obj_ui (both .h and .cpp). These files describe the c_obj and c_obj_ui classes. Almost all the classes used within AAASeed are derived from the c_obj_ui class. This class manages the foundational aspects of object reading/writing (through the load/save functions), access to object parameters (via the param functions), and relationships to other objects (through the child, parent, and branch functions).

Moreover, this class describes the interface for two essential virtual functions (in the C++ sense) vital to the operation of AAASeed:

The update() function refreshes the object, thereby setting the stage for a potential optimal call to the draw() function.

The draw() function, broadly speaking, "renders." If the subject is a geometric object (like a cube), this function manages the call to primitives that display the object based on the current attributes (lighting, color, texture projection, materials, etc.). In the case of an audio object, this function ensures that the associated sound is heard.

Each derived class (in terms of inheritance) from c_obj_ui outlines its accessible parameters (or in technical terms, describes its external interface). This is the means by which interactions with it are possible and is achieved through a list of structures of the ST_PARAM type (source files: param.h and param.cpp). The param_init_pt() function, provided by every child class of c_obj_ui, updates this description relevant to the whole class by adding details specific to that object instance. This concept will be further refined and expanded.

### AAASeed connections

The associated source files, connex (both .h and .cpp), define the c_connex class, which encapsulates a connection between two parameters (defined by the ST_PARAM structure) of two objects from the c_obj_ui class. The concept of a standard cable, as alluded to in the synthesizer metaphor, is captured in the source files named trax (both .h and .cpp), which describe the c_trax class.

The c_trax class not only facilitates the connection of parameters (including those of different types) and the propagation of their changes, but it also allows for connections to periodic or random signals (such as white noise, Gaussian noise, or Perlin noise). Furthermore, it accommodates connections to external sensors (like analog/digital converters and motion sensors), sending or receiving parameters through various communication links, recording, replaying, and filtering the temporal variations of these parameters, and applying various mathematical operations (such as addition, multiplication, division, inversion, averaging, etc.).

The c_trax class is also a child class of c_obj_ui, which means that all its parameters are accessible and connectable.

A class named traxs, defined in the source files traxs (both .h and .cpp), encompasses multiple c_trax objects within a singular c_traxs object, allowing for group management of these c_trax objects.

### AAASeed Base class

Owing to its initially graphical nature, the structure of AAASeed is organized around child objects of the c_bdd class (as described in the source files bdd.h and bdd.cpp). Each encapsulates a graphical primitive, among others:

- c_bdd_algo_maa: A fractal algorithm invented by the author.
- c_bdd_boid: A point cloud with behavior (representing flocks of birds or schools of fish).
- c_bdd_boxes: A series of rectangular boxes.
- c_bbd_circle: A circle.
- c_bdd_cone: A cone.
- c_bdd_face: Line, plane, and volume primitives organized in a grid.
- c_bdd_gene: Geometric primitives like Cubes, Tetrahedrons, Dodecahedrons, etc.
- c_bdd_grid: A flat grid.
- c_bdd_mocap: Animation and motion capture trajectories.
- c_bdd_null: A marker.
- c_bdd_movie: Displays a video window in the foreground.
- c_bdd_part: A comprehensive particle system.
- c_bdd_proj_cone: Visualization of a projection cone.
- c_bdd_quak: A level from the video game Quake.
- c_bdd_snd_wave: Sound file playback.
- c_bdd_sound: Visualization of a fast Fourier transform (FFT).
- c_bdd_sphere: A sphere.
- c_bdd_torus: A torus.
- c_bdd_tri: Polygonal object derived from .obj (Alias/Wavefront) or .geo files.
- c_bdd_tube_path: Tube extruded along a three-dimensional line.
- c_bddtex2d: Typographical text (TrueType Font).

Some of these objects descend from intermediate classes:

- c_bdd_multiple (as described in bdd.h and bdd.cpp): This redefines the display modalities of each point. For instance, a sphere can be drawn using cubes attached to each of its points, oriented according to its normals. This class also manages iterative loop calls to higher-level structures (c_layer and c_layers, as described later).
- c_bdd_uv (found in bdd_uv.h and bdd_uv.cpp): This encompasses functionalities of objects resembling a deformed two-dimensional grid (e.g., c_bdd_grid, c_bdd_sphere, c_bdd_torus, etc.).
 
### c_layer class

The c_bdd objects are contained within layers defined as the c_layer object (source files: layer.h and .cpp). This prepares the conditions for optimal rendering based on current attributes. The c_layer object may contain instances of the following:

- c_def_node: Deforms the geometry of objects point by point.
- c_render: 3D rendering attributes.
- c_multiple: Multiple rendering attribute related to c_bdd_multiple objects.
- c_map: Texture mapping attribute.
- c_texturing: Multi-texturing.
- c_color: Color attribute.
- c_lights_switch: Light switches.
- c_tex_anim: Texture animation.
- c_feedback: Image feedback process (sometimes called autoMap).
- c_model: Size and position information.
- c_tex_video: Plays video sequences in textures.
- c_transfo: Transformations such as rotation, scale, translation.
- c_fog: Fog.
- c_material: Materials (diffuse, specular, etc.).
- c_shading: Shader usage.

The c_layer object directly manages the switching and utilization of the above objects, as well as other options. For all these classes, there's a pointer to the current object (variables denoted as \*_cur) that each object can use or modify.

### c_layers class

The c_layers class (source files: layers.h and .cpp) is a group of 26 layers (c_layer objects from A to Z) and manages their sequential display as well as possibly other options and objects:

- c_lua_wrap: Ability to call a Lua script.
- c_obj_value: A set of neutral parameters available as variables.
- c_seedcam: Camera objects defining the viewpoint.
- c_traxs: So-called local tracks connectable within the object and its sub-objects.
- c_fbo: A framebuffer object to define textures to draw on.
- c_transfo: Transformation applied to all internal layers.

###c_module class

The c_module class (source files: module.h and .cpp) is a group of groups (c_layers objects from 0 to xxx) and manages their sequential display and possibly other options and objects:

- c_lua_wrap: Ability to call a Lua script.
- c_obj_value: A set of neutral parameters available as variables.
- c_seedcam: A camera object defining the viewpoint.
- c_traxs: So-called local tracks connectable within the object and its sub-objects.
- c_materials: Ability to redefine the material bank.
- c_lights: Ability to redefine light sources.

### c_modules class

This class compiles a list of c_module and has:

- c_lua_wrap: Ability to call a Lua script.
- c_obj_value: A set of neutral parameters available as variables.
- c_traxs: So-called local tracks connectable within the object and its sub-objects, known as Global Traxs.
- c_seedcam: A camera object defining the viewpoint.

### c_app class

This application class (c_app in source files: app.h and .cpp) manages a c_modules object and controls objects and attributes associated with all the modules.

### The rendering loop

The Glut library functions as an event loop and invokes AAASeed (callback functions). One of these callback functions triggers the render() function (found in the seeddraw.cpp file) which manages most of the tasks. Additionally, at this level, there are bank structures (for images, materials, etc.) that the classes mentioned above can select simply by an index number, or even by a bank number.

The rendering process can be summarized as follows:

- Update global objects:
  - Time
  - Interface
  - Sensor
  - Communication
  - Attributes
  - Banks
  - ... (refer to the source file for more details)
- Update the c_app object:
  - C_APP -> C_MODULES
- Potential script invocation
- Update global tracks (C_TRAXS)
- Execute draw() for all active c_module:
  - C_MODULE
  - Potential script invocation
  - Update the Module tracks (C_TRAXS)
  - Execute draw() for all active c_layers:
    - C_LAYERS
    - Potential script invocation
    - Update local tracks (C_TRAXS)
    - Potential update of the camera and transformations
    - Execute update() and then draw() for all active c_layer:
      - C_LAYER
      - Update all necessary objects for rendering
      - Potential execution of update() and then draw() for the active c_bdd object
      - Potential invocation of C_BDD object
      - In case of stereoscopy, execute draw() again for all active c_module
- Potential drawing of the interface
- Possible wait for frame return
- Potential saving of the rendered image

In technical jargon, this can be described as a static scene rendering tree. Although its structure is predefined in its organization for now, the mechanisms of banks, scripts (Lua), tracks (traxs), switches, current objects, layer and group invocations at the tree's lower level (c_bdd_multiple) grant the software immense flexibility and richness.

## System requirements

There are no specific requirements to run AAASeed. The only strong requirement is that your OS is Windows, and at least Windows XP. You will need at least 5GB of free space on your storage device.

There are different versions of the basic executable, that you can use, to provide you with the same features whether you have a GPU on your machine or not. Though some of the apps won’t work perfectly without a GPU, you will be able to run and experiment with AAASeed on any machine. 

Linux and Mac ports would be a great move and could be made by the community with support from the foundation.

There are library dependencies, which you need to setup on your system, as well as some basic configuration in the environment variables. Please find a short setup listing below: 

- Download the latest installer from AAASeed.org
- The directory containing the dlls should be present in the PATH
- Install the latest version of VS Code from Microsoft
- Install the visual studio 2019 redistributable, from Microsoft
- Install a version of FFDShow
- Install the K-Lite codec pack: K-Lite_Codec_Pack_1766_Mega
- Install the Direct Show filters: LAVFilters-0.65-Installer
- Install a Kinect SDK if you plan on using a Kinect

There are no known compatibility issues with other software.

You can keep AAASeed updated using the installer's integrated updater, or come to the public repository, which will always contain the latest version of the executable and the files needed to run it.

## Languages and frameworks

### Kernel

With almost 1640 files written in C++, half a million lines of code, more than 500 namespaces and 830 classes, AAASeed is a solid Windows desktop application that can run as a server or with a user interface.

The foundation classes are essential to the underlying functionality of the system. They include encapsulations for various objects, implementations of data structures, and high-level interfaces for images, videos, shaders, etc.

Some examples:

- Images and Videos: VIDEO, IMG, IMG_SEQ_PLAYER.
- Data Management: QUEUE, STACK.
- Graphics: SHADING, FBO, BIND_TEX.
- Math and Transformations: QUATERNION, TRANSFO_THREE.
- Communication: OSC_MESS, SMTP.

The kernel also has classes to handle the GaBuZoMeu interface, they are focused on the user interface, visualization, and specific interactions within the program. They include various types of user interface elements, debugging tools, and visual representations.

Some examples:

- User Interface Elements: BU, BUI, BUTTON, SLIDER, SELECTOR, MULTI, SLIDER_XY.
- Visualizations: BU_MONITOR, BU_VIZ, BU_EYE, BU_FPS.
- Debugging and Monitoring: BU_DOC, BU_MEMORY, BU_WATCH.
- Specialized Interfaces: NOVATION (controller interface), PIXAR (pixel text drawing), PLAYLIST (soundtracks).

### GaBuZoMeu

GaBuZoMeu, the user environment, is mostly written in Lua. It connects to the GaBuZoMeu kernel classes to make them available using simple Lua scripts. You will be able to use AAASeed out of the box, and program it using Lua for most of the common uses as there is a strong binding between the C code and Lua.

###Extensive list of frameworks used in AAASeed


|Framework|Official Website|
| :------ | :------ |
|Assimp|http://www.assimp.org/|
|Bass|http://www.un4seen.com/|
|Bassasio|http://www.un4seen.com/bassasio.html|
|Bullet|http://www.pybullet.org/|
|Direct show|https://www.microsoft.com/|
|Directsound|https://www.microsoft.com/en-us/download/details.aspx?id=35|
|dlib|http://dlib.net/|
|Fbx|https://www.autodesk.com/|
|ffmpeg|https://ffmpeg.org/|
|Flycapture|https://www.ptgrey.com/|
|Freeimage|http://freeimage.sourceforge.net/|
|Freetype|https://www.freetype.org/|
|FTGL|https://sourceforge.net/projects/ftgl/|
|Ftd2xx|https://www.ftdichip.com/|
|GLEW|https://glew.sourceforge.net/|
|Glm|https://glm.g-truc.net/|
|Hdi|https://www.handheldgroup.com/|
|Kinect v1/v32/azure Sdk|https://www.microsoft.com/|
|Libfftw|http://www.fftw.org/|
|Libre office|https://www.libreoffice.org/|
|Lua|https://www.lua.org/|
|LuaJit|https://luajit.org/|
|Nvidia flex|https://www.nvidia.com/|
|OpenAL|https://www.openal.org/|
|OpenCL|https://www.khronos.org/opencl/|
|OpenCV|https://www.opencv.org/|
|OpenExr|https://www.openexr.com/|
|OpenGL|https://www.opengl.org/|
|OpenSL ES|https://www.khronos.org/opensles/|
|OpenVDB|https://www.openvdb.org/|
|OpenVR|http://steamvr.com/|
|Poco|https://pocoproject.org/|
|RapidJson|http://rapidjson.org/|
|Realsense|https://www.intel.com/|
|Sixsense|https://6sense.com/|
|Spout|https://spout.zeal.co/|
|SteamVR|http://steamvr.com/|
|Tbb|https://www.oneapi.io/|
|Unity|https://unity.com/|
|Unreal|https://www.unrealengine.com/|
|Vive|https://www.vive.com/us/|
|Windows Mixed Reality|https://www.microsoft.com/en-us/mixed-reality/windows-mixed-reality|


## Main concepts

### Everything is obj_ui with params

The base entities in AAASeed are Objects (called obj or obj_ui, class c_obj_ui in C++). Objects represent a concept and encapsulate a process. Each of these objects expose several named parameters (called param, c_param in C++), a set of params define a representation, a state, an interface to the process itself.

You could say that the param set is like a remote control of the object. Params can control the process or give information on the state of the encapsulated process. In fact, obj_ui means an object with a user interface (the params are the ui). Some jargon will say that a param is a named value, or name/value pair.

For example, for a sphere obj_ui the params set could be a single floating point param named “radius”.

### obj_ui update and draw

Each obj_ui contain at least 2 functions (actions that the object can perform), These names come from the graphic origin of AAASeed.

-update() means from the point of view of the obj_ui: using the current state of your param set (and eventually some context more on this later), update your state, to eventually execute as fast as you can the other function draw(). This could have been called prepare().

-draw() mean draw yourself. This function could be eventually empty and do nothing or be called once or plenty of time.

What the object does should be documented by object. AAASeed is agnostic about what happens inside the object as long as it don't violate a set of rules (e.g. never crash the program).

For example: a sphere obj_ui, update() would use the radius (and eventually other params) to prepare internally the coordinates of all the points of a sphere of the given radius, and draw() would really draw the sphere on the screen (more on this later).

Another example: an obj_ui representing a real “physical button” would have a single boolean param (ON/OFF), and the function update() will change the state of this param according to the physical state of the button. The draw() function will do nothing.

### Params are the ui 

These params appear in the old school interface named Flatland (Neil Stephenson Snow Crash), where they are displayed and can eventually be changed, edited using the mouse and keyboard, they are the “visible part of the iceberg”. All the params in an obj_ui are regrouped internally in a sequence of param we call the params (C++: an array of c_param inside a c_params object)


![TDFLATLAND](./images/td_flatlandsphere.png)

*View of an obj_ui sphere as of 2023 February using his params in Flatland.On successive lines spaces replace similar character from the previous line: e.g. y is center_y.*



A param is designated (Dev: referenced) by a name: a string of characters composed of alphanumeric characters, spaces and underscores. Lowercase and uppercase are equivalent.

For example: the param “name_symbo”, can be referenced using “Name_Symbo” or even “NamE_symBO”.

### Params are used to load and save obj_ui

Serializing (loading, saving, sending, receiving …) is by default handled through the object params. Some obj_ui require some additional work to serialize their state... They are saved in a human readable form, so humans could read it. Eventually, if needed, these text representations can be edited or generated.

### Trax plugs to param

very early in its life AAASeed got a mechanism to “plug” param. Param are like a socket where you can plug a cable where the information can flow in or out. In fact, in the AAASeed lingo you plug to a particular obj_ui named a trax (C++: class c_trax) and the flow of information is defined relative to the trax. If you plug in a param it means the value of the param will flow in the trax, if you plug out the trax will output and impose the param value. Of course, a trax being also an obj_ui it has params than can also be plugged in and out to other traxs.

In the old time and for years this was how AAASeed was dynamic (Dev: a data flow structure without a graphic cable interface). e.g., The data fetched by a dedicated web obj_ui was getting election data in realtime, feeding through traxs a dynamic map of the country. The journalist was piloting through physical interface, mouse and keyboard a user interface build with params and traxs to navigate the result and present it to the audience (put the dates here)

### Params are the base access for code
Somewhere between 2006 and 2007, Franz Hildgen or Mâa (they both think it is the other one) included the Lua scripting Language into the AAASeed core. This transformed a solid but rigid architecture into a very flexible one (metal meet jelly). Lua can manipulate obj_ui and access them at minimum through the params using a small set of functions. We should soon also be able to write directly a small lua expression for each param.

One of the main advantages on top of Lua solidity, flexibility is its speed. This slowly transformed the production process with AAASeed. Today there is even a whole interface written in Lua, that can be changed and extended on the fly.

>For Developers: Python will be added later as a supported language too and will use the same bindings than lua.

### details on c_obj_ui
A factory object is associated with each class of c_obj_ui.
At creation c_obj_ui get an id (for now, 2023 December, using 20 bits which mean 1024x1024 objects maximum, so 1 Million). Getting a c_obj_ui by this is a very fast operation.
Each c_obj_ui have a root and eventually branches to which it is the root. Roots are "Up" and branches "Down".
c_obj_ui have several names
- eventually a name_ui which is defined only by the C part of AAASeed and used for message and display purpose only
- a name_human build dynamicly in conjunction with its class factory and is human readable
- a name_symbo (Symbolic Name) usually assigned by the user (param name_symbo in certain c_obj_ui)


## Interfacing with the C core (AAA level) - glue

### Access to the objects

Everything needed for development is described in the 
[./lua_aaaseed_interface.lua](./lua_aaaseed_interface.lua) file.

### Drawing

Everything you can access is described in this file: 
[./lua_aaaseed_draw.lua](./lua_aaaseed_draw.lua)

## GABU Obj

Full tree: 

![TDGABUOBJT](./images/td_gabuobjtree.png)

Focus on BU: 

![TDGABUFBU](./images/td_gabuobjbutree.png)

Focus on GABU: 

![TDGABUFBU2](./images/td_gabuobjbutree2.png)


## UI scripting (GaBuZoMeu level)

![TDGABUZOTREE](./images/td_gabuzotree.png)

## LUA preprocessor (TBD)

AAASeed simple base architecture relies on a generic class obj_ui exposing params. The definition of these params is using information spread in several places:

The C types and names of member variables located in the include file ( e.g. shadok.h )

REAL _center_ui[3];

The declaration of each param is currently using a whole set of C macros (from param_declare.h) specifying all the user interfaces (flatland, lua and other later) needed to expose the param. This is located in the C++ file ( e.g. shadok.cpp) in a named or unnamed namespace.

```
static  c_param_def param[PARAM_NB] =

{

	…
	PARAM_DEF_GROUP_CLOSED( Draw, DRAW_PARAM_NB )

		PARAM_DEF_POINT_XYZ( center )

};
```
A macro or an explicit definition to have the index of each individual param in the array when we need direct fast access from C functions, mainly param_init_pt() and prepare_ui().

`PARAM_DEF_MAKE_INDEX(center)`

A param_init_pt() function (param_init_pt_static() for recently updated obj_ui) which has to reproduce the order of the c_param_def array definition and serves the purpose of associating the param and the member variable (could be a global or a member from another object but we will not deal with this case now)

``` 
void	c_shadok::param_init_pt_static()
{
	INT32	h = 0;
	…
	++h;
		param_set_pt_v3( h, _center_ui );
	…
	err_param_init_pt( h );
}

```


We run a check using the h variable and when the count is wrong: we generate an error message at run time, we must count manually to find the discrepancy, recompile, fail again this is hell.

We needed a simpler way to do this, and decided to centralize all of this in one separate file (shadok.param).

If this file exist a preprocessor (written in lua) will interpret this file, emit warning and error message if needed and stop the compilation.

Generate C++ code and insert it in the .h and .cpp file using the following markers

```
//aaa begin param, generated code, should not be edited till end marker

…

//aaa end param
```


## Exhaustive classes list

### Kernel core

- GABU_OBJ: Base class for all the Lua objects.
- BLOB: Encapsulates one blob.
- COLOR_REF: Encapsulates a C color object.
- VIDEO: High-level interface to open and control a VIDEO.
- IMG: Encapsulates an image defined by a filename and a bind.
- SHADING: Encapsulates the C shading object.
- BDD_CLEAR_SCREEN: Encapsulates the C bdd_clear_screen object, doing frame buffer erase.
- FBO: Encapsulates the C FBO object: frame buffer object.
- TBUF: Time buffer.
- SCOPE: Oscilloscope.
- QUEUE: Implements a FIFO.
- BIND_TEX: Centralizes code to deal with a bank of texture used by TEXS.
- AAACAM: Encapsulates accesses to the C camera object.
- STACK: Implements a LIFO.
- OPENCL: Encapsulates the access to the C bdd_opencl_xxx objects.
- BOID: Helper to access the C bdd_boid object. More info here.
- ZO: When Apps are defined.
- IMG_READER: Used by tree installation to handle asynchronous read of images.
- CALAGE: No description provided.
- BLOBS: Deals with BLOBS, including reading from the C bdd.
- IMG_READER_POOL_3D: The 3D version of an IMG_READER_POOL.
- IMG_READER_POOL: Base class for the 2d and 3d versions, handles several IMG_READERS.
- IMG_SEQ_PLAYER: Uses an IMG_SEQ, and this class handles playing it too.
- IMG_SEQ: Deals with a sequence of images.
- EVENT: No description provided.
- CHANGER: CHANGERS deal with them.
- LOG: Used for file logs.
- FX: No description provided.
- MARK_LINE: No description provided.
- CELT: No description provided.
- FXS: No description provided.
- IMGS: More a namespace than a class, entry points to get IMGs.
- DATAGRID: Encapsulates a C datagrid object.
- LINE_STRIP: No description provided.
- MARKS_LINE: No description provided.
- GGREA: No description provided.
- LUA_WRAP: Wraps a Lua script using a C lua_wrap obj, update() executes it.
- LINE_STRIP_TABLE: No description provided.
- CREATURE: No description provided.
- FBO_AND_OUT: Used by old app to encapsulate the C FBO object and drawing it.
- LANG: Deals with different languages.
- FACE_INFO: No description provided.
- CHANGERS: Designed to create pools of CHANGER and TCHANGER, and update them.
- TEXS: Bank of texture should be accessed through here.
- PUPPET_PART: No description provided.
- TRANSFO_THREE: Encapsulates a C 2d transformation object defined using 3 points.
- POINT_LIST: No description provided.
- SCOPE_3D: Oscilloscope 3D.
- POINT_3D: No description provided.
- PCS: Once created and set.
- VIDEOS: Encapsulates a group of VIDEO.
- PUPPET_BODY_DEF: No description provided.
- PIXEL_SIZE: Helper for pixel size interfaces.
- PC: Abstraction for a Personal Computer and its graphics output with cameras.
- SEQS: No description provided.
- TCHANGER: CHANGERS run them.
- PUPPET_PART_DEF: No description provided.
- RACES: No description provided.
- SPACE: No description provided.
- PHASOR_A: PHASOR having just an Attack signal.
- PUPPET_BODY: No description provided.
- PLOT: No description provided.
- QUATERNION: Quaternion math is here.
- OSC_MESS: Receives and sends Open Sound Control messages.
- SMTP: No description provided.
- TANK: No description provided.
- SEGMENT_LIST: No description provided.
- PHASOR: Base class for an object that generates a signal or phase after a wait.
- RACE: No description provided.

### GaBuZoMeu

- BU: Box User, anything with interaction with a user. Can be used as “windows” for example. Derives from GABU_OBJ.
- BUI: BU handling value(s) and so having a User Interface: BU with UI -> BUI. Derives from BU.
- BALUE: Abstraction handling value used by BUI. Derives from GABU_OBJ.
- BUTTON: A simple BUTTON, displays text according to a value. Derives from BUI.
- SLIDER: A type of BUI for one float or int value, or base class for sliders with more values. Derives from BUI.
- SELECTOR: SELECTOR is one type of BU for multiple choices with multiple selections. Derives from BUI.
- BUS: Where BU are stored, gives them a coordinate system, BU can also have a BUS inside. Global bus_cur is used to access the current bus. Derives from GABU_OBJ.
- BU_TEXT: BU to edit TEXT. Derives from BU.
- BU_MENU: A BUTTON associated Window encapsulating a SELECTOR. Derives from BUI.
- BU_MONITOR: Encapsulates an image monitor, visualization of a texture with its associated UI. Derives from BU.
- MEU: Central piece when using GPs (graph), regroups all the User interface and the code for a block of functionality. MUs are compact counterparts used to manipulate the rendering order. Derives from GABU_OBJ.
- MU: A Module BU, associated with a MEU, represents it in the rendering process. Derives from BU.
- MULTI: SLIDER with more than one value. Derives from SLIDER.