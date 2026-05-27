# AAASeed, functional specifications

## Workflow to create on AAASeed

There are several kind of users for AAASeed, but they are all, for the moment, presented with the same UI: GaBuZoMeu. It is possible to develop other UIs or a customer dedicated UIs using GaBuZoMeu while hiding it, but this a subject for advanced developers and it is not covered here.

Basic users will not directly create in AAASeed, but create using AAASeed and an already built environments adapted to the task at hand: controlling the visual on stage, handling the change of a visualization of an election on Tv… They are engaged in a creative process where AAASeed (or the APP built with it) is a just one of the tools and they use the interface to set, adjust and control the process.

### Existing process

Using the GaBuZoMeu interface, users have Access to a static hierarchy of MEUs.

They can start and stop each MEU and control the order of execution of the MEUs. Users edit the MEU parameters using the predefined MEU interface. This includes choosing resources or files used. A lot can be done this way but they can do even more. Undo and redo are implemented. Some extended copy and paste features are implemented. There is also a preset system included in each MEU.

It is important to insist on the fact that the MEU hierarchy is not a graph where you plug input and output on blocks generating a rendering graph. You can of course plug an external controller or data to a BU, and some specialized UI and MEU exist to visualize the flow of data and plug them in or out. But the main concept in a hierarchy level is that you can choose what MEU are active and the order in which they execute. The order is determined simply by rules using the 2nd position of the MU in the interface. This can be disconcerting at start but is way more expressive and efficient than plugging / unplugging in a graph. This is in fact an asset of the software.

They can edit params for the C objects which are not exposed in the MEU through a mechanism trigerring the MEU to give access to these params using Flatland.

They can edit text resources linked to the MEU (Meu lua script, Shaders , opencl script…) using an external text editor directly in the directory (VSCode or Notepad++ recommended but it can be any). MEUs provide several ways to open files directly in the editor, lua error in particular opens the editor with the file at the line where the error happens. Saving the file can trigger an automatic update of the file in AAASeed. You can for example see the output of a drawing scripted function you edit on the AAASeed Window while you edit it with the text editor.

If you are more knowledgeable you can even edit and refine the UI of a MEU on the fly by altering the corresponding script and using the redefined button (Def) which is present in any MEU. MEU from the same type share their script, so when you edit a script defining the Ui and the behavior of a MEU, you do that for all the MEUs of the same type living in the hierarchy.

If you want a MEU to be created, deleted or moved to other places in the hierarchy, or if you want to  create new classes of MEU, you cannot do it directly using GaBuZoMeu in the current version of AAASeed. You can accomplish these tasks, but you have to exit AAASeed, go to file system and copy, delete, move and edit folders and text files. That will change the static hierarchy of the MEUs. And then you have to relaunch AAASeed. It is not an immediate process, it is prone to errors and requires documentation for a number of step by step instructions or an advanced knowledge of the AAASeed and GaBuZoMeu Architectures.

One deeper level is available if need, the extension of the C++ core. Adding a c\_obj\_ui can be done easily and quickly but requires more of a developer expertise. Sometime a process is tried and adjusted using GaBuZoMeu and scripts, then implemented in C++ to gain performance or minimize the power required from a machine.

### New process (alpha in September 2024)

It will be the same process, with a major change: the support for a dynamic hierarchy system. All the above manipulations done through the file system will be done directly in the interface: so MEUs will be selected, cut, pasted, erased, duplicated, moved, renamed, saved, loaded, inserted... It will be possible to select some MEUs and encapsulate them directly in a new MEUs.

It will also mean that the users will be able start from an empty page and add when they need it, MEU from existing classes or copy them from other APPs or even create new MEU Classes on the fly.

This future workflow doesn’t change the metaphor, it makes it easier to use. It will require the addition of a new concept of local resources, and a little more complex strategy on the naming of resources and objects to be able to navigate up and down the hierarchy, instead of the global space which is in place now.

## Library: the MEUs
> We are working on a MEU library


Here below, you will find most of the useful MEUs to build your APP in AAASeed.



|MEU|Description|
| :---- | :---- |
|Boid|an advanced boids system (simulate flock of bird, fish…)|
|Cam|encapsulate just a single 3d camera|
|DepthPick|In conjunction with a MEU dedicated to depth Camera, help to create the transformation to align a 3d cloud of point with the floor or a wall.|
|App|experience to encapsulate APP in a MEU to do a "desktop"|
|Deform||
|Dir|encapsulate a list of MEUs and provide the UI the manipulate them, this can be seen as a directory or a folder.|
|GridSel|like a MEU dir but with a different UI to activate MEU|
|Displace|Render a grid using a displacement map and some custom extension (OpenCl for Now)|
|Fbo|Frame Buffer Object. This define a Frame buffer, the textures where rendering happen, it control their size, format and eventually their erasing.|
|Kinect|Control a Kinect Camera (v1, v2, Azure)|
|Kinect1|Older depreciated version of MEU Kinect|
|KinectMove|Older depreciated version of KinectMoveAuto|
|KinectMoveAuto|transform the Kinect Inputs in a 2d Image seen from a camera|
|Lidar|Control one Lidar (RealSense l515 for now 2023 Sept)|
|Mire|MEU build around c\_bdd\_img\_gradient to generate image(CPU)/texture(GPU) with patterns: dot, gradient, greylevel, colorbar…|
|MuBegin|Couple of MEU to control MEU in between, used to do repeat/loop. Used mainly for multipass rendering (Gbuffer), shadows…|
|MuEnd|Couple of MEU to control MEU in between, used to do repeat/loop. Used mainly for multipass rendering (Gbuffer), shadows…|
|Out|made for image output (screen, projetor, led...). Choice of screen, uv edit, editable adjustable grid|
|PIP|Picture In Picture, display a texture in a rectrangle, integrated automatic fade, inscrustation|
|Video|read videofile or live input|
|Ref|Reference, point to another MEU, used to reuse the same MEU instance in different places|
|Spiral|Spiral uv mapping on a grid using a fragment shader|
|TankFish|regroup several boid\_system with config and update files, rendering. Used mainly in Aquarium projet|
|TexCpy|Low level Textrure Copy|
|ImgSend|Send Image on the network, very efficient for 1 channel capture image (compression x50)|
|TRAX|collect input data, visualize and process to connect to BU|
|WebGl|quick hack to integrate a bunch of shaders|
|2054|Artistic effects|
|BddPointDraw|direct Gol drawing base on a bdd\_point|
|BlobDetect|build blobs from an image, filter. eventually send using Osc|
|Blur|Blur using OpenCV|
|Bullet|interface with the Bullet physic library|
|CaptureRect|Stitch several images to constitute a global capture image, edit caches and simulate people|
|Clear|clear the frame buffer|
|Clous|Artistic effects|
|ClPool|Particule system done with Opencl, made for DPool project in Jeddah|
|Derviche|Done with Hugo Verlinde to generate sequence with his "derviche"|
|Digital Projection|control for digital projection projector (tested on laser 18K)|
|DistField|Compute a distance Field using OpenCV|
|NdcJumpFlood|Custom implementation of the Jump flood algorithm to generate distance field and related image effects|
|FaceTrak|Use dlib for face tracking (landmarks), was done for the Dior project|
|FaceUV|Deal with Uv Mapping on a face, was done for the Dior project|
|Fbx|Read and display Fbx file. Deal with animation, dones, skinning....|
|FbxMatte|Read and display Fbx file. Used for static objects|
|FieldGene|useful to simulate a field|
|Flex|A pretty complete implementation of NVidia Flex: fluid, rigid and soft bodies, cloth|
|FP|Bois system without an interface|
|Hexa|Draw a grid using and hexagonal tiling for the texture|
|NdcReactDif|Reaction difussion in a texture|
|OpenCV|use Open for blur, brightness contrast, erode/dilate|
|OpenCvBase|OpenCV|
|Spout|Low level sharing of texture between several software|
|TK|Artistic effect from a time signal (audio) inspired by CXhrristian Amoretti|
|TriMix|Mix two textures using a third|

## Focus on the real time interaction / capture

### Camera capture

Presently (since Kinect), we typically use depth cameras but previously we also used stereo cameras to extract the same information (a 3D cloud of points).

We have also worked with various digital cameras with or without IR filters but the information we got out of those was more primitive and it was sometimes difficult to separate what is projected from the real public/visitors. But some parts of the process still apply.

The different streams on a Kinect Azure: 
![FDKINECTS](./images/fd_kinect_streams.png)


In the previous figure, the top right image is a depth image (the color represents the distance from the pixel to the camera).

From these real time streams and eventually some more calibration information on the camera, we can construct a 3D point cloud (or surfaces) in a known metric and project it in any direction using perspective or orthogonal mode.

To do this efficiently, we use the graphic architecture of the computer (GPU) and programmable shaders (bits of code that are executed directly on the GPU). In particular, we use a geometry shader pass to filter information we want to keep, filtering valid information or cropping the space for example.

### View of the points cloud

4 views, different angles / camera position, of the same point cloud: 
![FDPC1](./images/fd_pc1.png)
![FDPC2](./images/fd_pc2.png)
![FDPC3](./images/fd_pc3.png)
![FDPC4](./images/fd_pc4.png)



### Re-projection

This pass achieves several goals at the same time:

- get a view of the visitors from a desired direction (generally from the top or facing a wall) that is different from the camera position.
- transform the 3D data into a 2D image where we control the metric.
- pixels can have a size corresponding to the real world.
- color (or grayscale) represents the height from the floor or the distance from an interactive wall.
- remove the floor, the walls and any fixed elements we don’t want to confuse with the visitors.
- select what we consider valid data.
- more on demand

### 3D calibration

This is the hard part of the previous stage ”transform the 3D data into a 2D image where we control the metric”. It requires some Black Magic and typically can’t be performed by an untrained technician. This is one of the main hurdles in the deployment of immersive technology.

- The cameras give us absolute information but only relative to the camera position, yet we must link the information from the different cameras to the same metric and eventually to the real space. For the moment, we have some tools to estimate a transformation, so the reprojection will align with the floor or a wall and we can continue to adjust manually from there. 
- Usually, we use things in the real space (already present or added by us with sometimes precise measurements, e.g. chairs, boxes, wood planks…) as markers, rulers… Sometimes, we even do that remotely, using remote desktop software.
- The approach at UPSaclay uses VR goggles and controllers to allow the calibrating user to move camera positions and align projection planes (of multiple cameras and projectors) directly in 3D.

### Send

Using a custom compression protocol, we send the images to the computer that takes the interaction data as input to produce the final output/render. We do that using UDP (user datagram protocol) packets to introduce as little latency as possible.

### Receive

The different packets are reassembled to reconstitute the sent image. We must make sure that lost packets cannot crash the system or produce temporary aberrant images.

### Reassembly

Using editable 2D deformation grids we recompose the different input images to form a master image (or several master images if we use different points of view, e.g., top and front).

![FDKR1](./images/fd_kr1.png)
![FDKR2](./images/fd_kr2.png)
![FDKR3](./images/fd_kr3.png)

These images come from the superposition of 6 Kinect Azure cameras. The visitors are interacting with a 9-meter LED wall. The grayscale indicates the distance from the LED wall (white => 20 cm, the darkest gray => 300 cm).

### 2D Assembly

In the rare case where the 3D calibration is done perfectly, we just must superpose the 2D image according to the metric of the projection. But very often we realize at this stage that there are some problems. There are two ways to achieve a better superposition:

- Use this visualization in conjunction with the 3D calibration to better adjust it in a tedious iterative process. Sometimes we can’t do a full 3D calibration, but the errors/projection mismatches are minor. Other times, somebody might have moved a camera (can happen during cleaning or replacement for example) while the installation is running.
- Using an editable 2D deformation grid, we can simply adjust the superposition of the projections. This is often fast and dirty but requires some practice (Black Magic here too).

## Filtering

At this stage, we sometimes apply traditional image processing techniques (blur, erode/dilate, threshold…) to reduce noise, dismiss certain parts of the image or improve quality (more on demand). We use OpenCV or custom fragment shaders.

## Distance field

This could be seen as part of the filtering, but it is quite specific. We currently use a custom implementation of the JumpFlood algorithm.

>(<https://en.wikipedia.org/wiki/Jump_flooding_algorithm>) 

which is fast and avoids lengthy GPU to CPU transfers. The data generated in this step is typically used later in the interactivity pipeline (generally as a force field). It allows interaction with distant objects.

Distance field: 
![FDDF](./images/fd_df1.png)

### Edge detection

We can use the areas, sizes and point numbers of each contour as additional tools to decide if we want to process the contours in the next stage.

Figure contour detection and blobs: 
![FDBLOB](./images/fd_blob.jpeg)


The cyan points are the result of the contour detection, the green rectangle with numbers indicates the blobs (see below), the rest is the result of more processing done on each blob.

### Blob tracking

The results of the edge detection can be used as the input (new frame) of a blob tracking pass. The idea is to minimize a cost function that assigns each blob an ID, considering potential previous IDs. From here, each blob has a unique ID which means that we can follow individuals (this is a highly optimized C++ process).

Each blob can carry its own polyline, describing the contour in vector form.

It works fine until some blobs start merging or separating - in this case, tracking is not enough and should be augmented/improved by a smarter process. In these cases, we sometimes use additional scripts on top of the blob tracking to better deal with the allocation/deallocation of blob IDs in these cases.

Dealing at the same time with different views (top and front in the case of a wall) could improve the process a lot, too but so far, we don’t know of any existing implementation of this technique.

### Polyline analysis

For the moment, we use it to detect how much the arms are raised. It is imperfect but useful in some specific contexts (currently done using specific scripts written in Lua, more details on demand).

### Temporal filtering

A very reactive capture can in certain interactivity scenarios provoke vibrations, skipping or instabilities. Sometimes we apply temporal filtering/smoothing to the numeric output values (e.g., position of the blob) to smooth the experience but this can introduce a perceivable lag. There is a sweet spot between reactivity and smoothness to be found (each time). Perhaps different filtering methods could also reduce the need for these tradeoffs.

Tim is proposing to use a simple physics model.
> *I like to use a simple physics model using target positions that basically exert forces to pull our output towards its new position.*
>
> *For example, for the Eye of Mars: First, I process the images to get regions of motion and I calculate their centers and sizes. Then, I calculate a new target position (where the eye should look) as a function of the latest motion centers and sizes. This new target position might move a little erratically because of glitches/inconsistencies in the image motion processing. So, instead of setting the Eye's gaze position directly to the new target, the target will act as a force that pulls the Eye towards it. Single-frame glitches are then smoothed out by the Eye's inertia and the final movement looks overall quite natural.*

## Focus on the Textures

Textures are important in AAASeed. It is by using them that you can apply a texture on a surface, this texture being either of: an image, a video, a monitor, …

You can click on BANK in GaBuZoMeu to display them. They are organized in BANKS and BINDS.

![FDBK1](./images/fd_banks1.png)

Clicking on BANK will open up this screen presenting the textures, you can click on them to choose one of them.

![FDBK2](./images/fd_banks2.png)

You can change the bank by clicking down on the teal table, each bank will present a full bind of textures. Some may be empty, you can use the spot !

The superbanks are collections of banks if you need more space.

Changing banks: 
![FDBK3](./images/fd_banks3.png)


You can use these textures for example in a MEU as displayed (the TEX button pops up a mini texture selection)

Texture selection: 
![FDBK4](./images/fd_banks4.png)

