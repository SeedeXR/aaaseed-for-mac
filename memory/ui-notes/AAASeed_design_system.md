
# Principles
	
## Strong concepts
		
*GABU_OBJ is a base class used to derive all the lua classes. It is an easy way to provide extended base functionality for the class. In particular, GABU_OBJ instances have a name or a symbolic name. GABU_OBJ is not represented below to simply the figures.*

### The GaBu building blocks

![GaBus](./images/GaBus.png)


GA (for Global Action) represents the global level accessible to all. It stores preferences and states. GA holds 3 BUSS which are simple objects just storing and handling a list of BUS, giving those lists a priority for UI and drawing.

BUS deals with a list of BU. BU (for Box User) is the basic building block for UI element (e.g. BU_TEXT, BUTTON, SLIDER …) or it can encapsula_te like a window a BUS containing another set of BU….

GaBu blocks could be considered as part of a UI library, as a lot can be build using them.

Something more is needed to encapsulate blocks of functionalities and manipulate these blocks (edit, reorder, regroup, load/save, cut/copy/paste, send/receive…). To do this a new concept was added: the Module Editable Unit or MEU in our lingo.

The MEU and some associated classes (e.g. BU_MEU, MU, MUS, GP, APP_GP….) and derived classes constitute what we call GaBuZoMeu the complete toolkit build with the lua language on top of AAASeed.

>AAASeed could be seen as on OS and GaBuZoMeu the SDK to develop Applications on top of it.

### The Meu building block

![Meu](./images/Meu.png)


A MEU encapsulates functionalities made of script and eventually C++ objects from AAAKernel.

It contains a specialized BU (a MU) which is used as a compact form to represent and manipulate a MEU in the interface. In particular: MU are used to define the execution order of the MEU. 

It also contains a BUS with all the interface elements needed to control it (this is called an inspector in some software).

Each MEU in general correspond to a real folder on the file system where its data is stored. Some specialized MEU (derived from MEU_DIR_BASE) contain other MEU.



### Hierarchy

Every single object can part of a hierarchy. GABU_OBJ provides a standard mechanism, up/down methods, to deal with this. The naming of GABU_OBJ is also very useful here: combined with the up/down mechanism it also accesses objects using names as it is done with paths in a file system.

In particular, BUS/BU/BUS/… chains are organized this way and MEU are organized in a hierarchy.

### What is a project / APP?

It can be anything, but to start things up and get an understanding of how AAASeed is built, let’s make a simple definition: a project is a series of MEUs that you configured using GaBuZoMeu. Each MEU handling its own UI. A GaBuZoMeu app provides a hierarchy where MEU can be seen as directories (and also have directories). Flatland is used to access C++ contained objects.


## User centric design

>If your mouse gets away too far from a UI, the UI texts and details will vanish and reappear when you get closer to it.*

User-centric design places the user's experience and needs at the forefront of product development, ensuring that systems are intuitive, efficient, and satisfying to use. One such approach to enhance user experience is by limiting mouse movements. By streamlining the interface and strategically placing frequently used controls within close proximity, users can navigate and execute commands more quickly, without the need to aim precisely, thus reducing physical strain and time spent searching. Furthermore, embracing concepts such as access by proximity optimizes the user's workflow, as tools and options become readily available when and where they're needed most. Lastly, the incorporation of double or triple controls adds layers of functionality and flexibility, allowing users to execute multiple actions without navigating away or diving deep into menus. This holistic approach ensures not only an efficient interaction but also a more enjoyable and seamless user experience.

## Scalability

AAASeed is adeptly designed for scalability. Its core architecture uses layered simple concepts, allowing projects to grow and adapt seamlessly from basic to complex forms. Whether a user is crafting a straightforward design or a comprehensive interactive experience, AAASeed accommodates with ease. Its modular framework, coupled with a variety of user interface elements, ensures flexibility for both beginners and professionals. The GaBuZoMeu environment, powered by a C++ core, guarantees consistent system responsiveness. Furthermore, its real-time data handling capabilities offer immediate user feedback, making it a reliable tool for projects of varying scales. 

## Everything is scriptable

AAASeed's design system operates on a straightforward tenet: everything is scriptable. This allows users to tailor elements and functions using scripts, offering a reasonable degree of customization. Whether it's a small tweak or a more extensive change, scripting in AAASeed accommodates it. By making the platform adaptable through scripts, users can achieve desired results without a steep learning curve. This scriptable approach ensures that AAASeed remains user-friendly and adaptable to various needs.

## Flexibility

AAASeed is built around the concept of flexibility. At its core, the software allows for easy adjustments and modifications to suit varying project needs. Users can choose from a wide range of tools and features, mix and match components, or even integrate external elements. This inherent adaptability ensures that both novice and expert users can mold their projects according to their specific requirements without unnecessary constraints. You can design exactly the UI you need.

## Multi user / multi touch

AAASeed supports both multi-user and multi-touch functionalities. This means the system can handle inputs from several users at the same time and can also detect multiple touchpoints simultaneously. This feature is particularly useful for collaborative projects or when using devices with touch screen capabilities. In essence, AAASeed's design accommodates varied input methods, making it more versatile for different user needs. Do not forget this ability when you design your UI.

## Analog world references

AAASeed uses several concepts from the analog world and tends to get some vocabulary and conventions from these domains, allowing specialists in these fields to quickly understand and use the concepts.

### Music synthesizer:

- The concept of bank/bind for image, video, font…
- In the old school interface (flatland) you can plug in or out traxs (track) to params. This like a patch cable in the analog synthesizer world where you used to plug the cables on an output, to an input. The reference is the trax: plug out a trax means that the information flows from the trax to the param, plug in a trax means that the param feeds a trax input. There is a complete trax documentation available.

### Traditional animation (2D, paper, cel animation)

- Like in traditional 2d animation, transparent layers (cel) can be superposed. The first drawn layers are in the back, the most recent layer is drawn on top. But in AAASeed, layers are 3D and can each have a different camera!

### Video Compositing

- An alpha of 0 means transparent and 1 means opaque by default. Intermediate levels can also be used: an alpha of 0,25 means that the result will be the mix of 75% of the image under and 25% of the image over. Each pixel (or fragment) can have his own alpha level (in this case the resulting alpha will be the product of the fragment alpha and the layer alpha. 
- Colors of a layer also by default multiply the pixel color.

> As regular OpenGL systems, the 3D coordinates are right hand up.**

## Incremental design

AAASeed's architecture embraces the principle of incremental design. This means users can start with basic structures and especially UIs, then gradually build upon them, adding more complexity over time. It allows for iterative refinements and adjustments without the need to overhaul the entire project. This design approach is efficient, reducing the barriers to initiate a project and offering the flexibility to expand and refine as ideas evolve. In short, AAASeed's incremental design approach provides a balance between simplicity for beginners and depth for more advanced users.

## Realtime

AAASeed places a strong emphasis on real-time functionality. Users can see changes and updates to their projects instantaneously, without delays, and without even restarting the software. This real-time response is not just about speed but also about facilitating a more intuitive and responsive user experience. It streamlines the feedback loop, allowing users to quickly iterate, adjust, and refine their projects based on immediate visual feedback. In essence, AAASeed's real-time capabilities make the design and modification processes smoother and more efficient for users.

# Design System

## Colors

In GaBuZoMeu: 

- Teal and dark blue are used for background / neutral colors.
- Light teal is a selected item.
- Red is used for important actions, for confirmations or alerts, or as a background to a BU/MEU when it is disabled.
- Green is used as background when a BU/MEU is active/enabled and to display the MU’s name. It is also used for the presets BU.
- Yellow is used for real time global information (FPS, time, …)
- Magenta is used to show that an item is selected

You can check all the colors in the BU.lua file: 

- BU:get_color_named(name) and
- BU:get_color_back_named(name)

In Flatland: 

- White is the parameters groups.
- Blue is the parameter’s name.
- Green for an “on” parameter.
- Red for an “off” parameter.
- Yellow is the real time, actual value and comments.

## Typography

There are no specific typography design guidelines. Two types of text displays are defined: lines and TTF (ftgl and freetype).

AAASeed is optimized so that the use of fonts doesn’t limit the framerate of the whole system. It integrates the SDF format[^2].

## Iconography

There are no iconography guidelines. 

The only icon built in the system are used in Flatland, and they are actually drawn, not icons.

They show the user which line is selected and if the selection has a sub part and if it is deployed or not.

For simple realtime, interactive, scalability reasons, the use of static icons is not recommended. You can design scalable icons, as BU and integrate them. The system will later propose a library for that.

## Spacing and layout

There is a global grid that is used to align the elements in GaBuZoMeu. Every BUS also has its own grid.

## Imagery

Images would be a 2D bitmap, they can be placed almost everywhere in the system, especially in BU. There are no specific guidelines as you can design pretty much everything you want. Again, do not forget scalability, 3D (cameras), layers, …

Images will be mostly present in the texture banks, so that they can be used everywhere in the system. You may take a look at the specific section that explains the textures in depth.

The same will apply for the videos.

## Components

AAASeed integrates basic components.

Global (encapsulating): 

- BU: Box User, anything with interaction with a user. Can be used as “windows” for example. 
- BUI: BU handling value(s) and so having a User Interface: BU with UI -> BUI. 
- BUS: Where BU are stored, gives them a coordinate system, BU can also have a BUS inside. 
- MEU: Central piece when using GPs (graph), regroups all the User interface and the code for a block of functionality. MUs are compact counterparts used to manipulate the rendering order. 
- MU: A Module BU, associated with a MEU, represents it in the rendering process. Derives from BU.

Specific: 

- BUTTON: A simple BUTTON, displays text according to a value. 
- SLIDER: A type of BUI for one float or int value, or base class for sliders with more values. 
- SELECTOR: SELECTOR is one type of BU for multiple choices with multiple selections. 
- BU_TEXT: BU to edit TEXT. 
- BU_MENU: A BUTTON associated Window encapsulating a SELECTOR. 
- BU_MONITOR: Encapsulates an image monitor, visualization of a texture with its associated UI.
- MULTI: SLIDER with more than one value. 
- SLIDER_XY: a slider for two (x, y) axis.
- BU_SHADING: for shader UI.
- BU_COLOR: for color UI.
 
## Patterns

Whole BUS can be copied, or even better, when you copy a MU, you will get a copy of the MEU (its UI). A MEU has its prototype, which can be used on multiple MEUs.

There are no other specific patterns.

## Documentation

You will get in depth information in the technical and functional descriptions later on this document.

## Governance

Governance is to be defined later on in the project, it will be at the core of the AAASeed foundation. It will include the update process for the design system and the documentation, the process for adding new components and updating existing ones, and the ability to assign roles and responsibilities for maintaining the design system.