# Release Notes
# --- AAASeed 0.88.1.22 ---
## Main changes
- Menu stay alive when under the mouse.
- The Direct Show capture have been rewritten and cleaned (it use now a renderer filter). YUV,NV12,I420 and MPJG now supported and use AVX2 (4K ~= 4 ms). OBS Virtual Camera now ok. Connection and deconnection of ds devices now detected and provoke a new enumeration of devices. Color space for capture are now correct and can now be switched on the fly. MEU Video deal with capture now, but some details still need ironing on MEU_VIDEO BU_MENU updates.
- Display grabber refined (no more strange cropping). MEU Video inegration done too.
- All capture path end with RGBA* except if other format is forced.
- c_trax and so MEU Trax had an output bug skipping Gain/Bias/Threshold and filter.
- OpenCV updated to 4.13.0.
- Terminal messages should not mix each other anymore.
- A lot of cleaning done with AI inspection. Less lua Garbage Collector pressure. QUEUE redone in particular.
- OSC documentation redone. PureData OSC AAASeed sender patches extended and refined.
- MIDI improved and modified: Control start now at 0 instead of 1, c_midi refined, MIDI windows extended, BCF2000 revives, OSC/MIDI/BCF2000 doc added and PureData patch extended.
## For the coder
- unit test by class implemented (look at STACK and QUEUE as examples).
- aaa.obj.get_instances_by_class() added
# --- AAASeed 0.69.2.0 ---
## Thing to do
- Your font 0 should point to Roboto_SemiConsensed-Medium that you cab find in AAAKernel/Fonts/Google/Roboto/Static. MEU Typo is am easy way to change this (double click on font bind selector and open a font).
## Main changes
- The exe went through a lot of change and compile now with Visual studio Insiders 2026.

- Text stuff in C and Lua have being improved and rewritten for quality and speed.
- AAASeed and GaBuZoMeu seems now to deal correctly with window screen scaling.
- Add "Audio Split to OSC.pd" providing real time Sound analysis to AAASeed.
- All use of attachement have replaced by attachment (in lua, param, C ...).
- AAASeed_introduction Version 1 completed: 167 Slides of documentation.
- Low level C function checking and creating directories was unable to perform in certain cases: it was rewritten.
- Text drawing went througth deep changes: AAASeed look better and is faster. On the C side c_bdd_text refined: shadows added, FTGL integrated and improved, character 0 redesigned for Roman line font.
- Osc with address "/aaa/midi/ctrl" now processed directly by the Executable with an accept flag in the Osc group of the net object. MEU OscReceiveLab now a proto, so midi can be controlled using OSC packets. in AAADoc there is pure data patch to try.
## UI
- StarMenu for BU_TEXTURE and Monitor extended. The bank selector is now 20 by 5, adding the M texture name group.
- ALt Click now function correctly.
- BU_MONITOR: fix are better, use key + - to change alpha mode, deal well will now bu alpha, UI info display bugs fixed. Alpha mode and More option fixed. Click Zone act on mouse button release now. StarMenu extended. Text chgange color when bind is not nominal.
- BU_COLOR for gamma can now show gamma.
- BU_TEXTURE shortcut show separation and have now have UVMap.
- BU_BANK let move image around using Ctrl Arrrow. Start in a state where Monitors show info. Bank Offset replace Super banks.
- Correct key + - bug.
- MULTI renamed SLIDER_MULTI, UI redone to support Move/Resize with Alt, insert/delete improved.
- MEU ratio_uv and pixel size now support undo.
- PRESET have a size effects when one is saved or removed.
- BU_COLOR gamma have now a max of 8.
- Paste ans shift Paste refined.
- BU_SHOW display warning and blinking frame when needed.
- SLIDER_XY show value not canonical value anymore.
- BU_DOC AND BU_CREATE_MEU have their oen search string inited.
- StarMenu refined in some cases (TRue/False for boolean values).
- Selector with mutiple selection drawn differently from regular selector.
- GA, BU and GP windows cleaned and extended.
- Finder visualization hacked to shoot tutorial (take in account viewport now).
- UI grid now use the UI camera to draw.
- BU_DOC have the bu info updated all the time.
- Copy/Paste works with BU_TEXT now.
- GP Grid button now a Grid Alpha slider.
- in MEU_DIR, MU are forced back when they are outside the MEU they belong to (this happens when MEU_DIR is displayed and touched). This protect from a rare or potential bug. UI have been clarified.
- BU_COLOR and BU_TEXTURE deal now better with alpha_bu.
- More and more change of transparency to refine UI.
- MEU TRAX connections to BU are showed with triangles on top (left output of TRAX, right input of trax). Also done for Midi controller connexion. Links are drawn between BU and MEU TRAX connected and active.
- The links between UIx and MU, between MU of the same typem shown when we click on MEU and MU have been greatly improved. When a MU is not rendered the MU containing it and rendered is used instaed. It avoid all this ghost links going to the center.
- Moving camera only happens with middle button pressed first (was happening with just Wheel before).
- UIF revert to start value when not on any zone/command. Remove neutral zone around start click when it have no sense.
- double Alt and n regrouped.
- MU and MU slider highlight (contact and current) now draw correctly even in non active or hidden case.
- New code to change curremtBU with arrow keys.
- Blending mode BU now flash discretly when default mode Add is not selected.
- MEU Camera UI improved.
- pixel_format in C renamed for clarity and coherency.
- SLIDER and SLIDER_TWO now can use two lines.
- MU have a colored frame.
- drawing of icon refined.
## MEU
- MEU ImgBank added.
- MEU Boid have more multiple stuff and can draw "birds" in lua.
- MEU ReactDif now restart with time.
- MEU PIP refined and have Color Incrustation. Names have being change (for texture mask in particular) and this could trigger upward compatibility problem. UI refined.
- MEU Out improved (functionnality and UI). Can be now more easily used to Edit UV Map (don't use Deform and give the c_bdd_adjustable a Name_sympo so a c_mapping can access it). A Draw button was added (if was necessary because we need to draw the layer so the bdd is at least updated). Grid Reset have confirmation. Shader faster and better.
- MEU Trax in threshold mode added Push mode, deal better with an output being BU trig. Now support 9 outputs and ready to extend for more. Trax information circulation is shown with animated links. 
- MEU Fbo, UI refined, if name correspond to a texture bind it is used.
- MEU Monitor refined.
- MEU ShaMosaic refined (nearly ok).
- MEU TK have a better UI, and got extended.
- MEU exShaderGrid refined.
- MEU video step now loop.
- MEU TutoLuaLoadImages refined a lot, and have now draw method.
- MEU AAASlides getting ready.
- MEU CartoMAton added.
- MEU ImgAnalMulti added, which detect coverage and position on multiple rectangles in a single image.
- MEU Video:  
Can now be created and instantiated from existing. The instance name define which bind the MEU will be using.  
Capture situation is clearer but still unfinished.  
Step now loop
- MEU Kinect and KinMoveAuto refined. For Azure: handle RGB resolution, handle color control settings (auto and manual) and display rgb camera transform and intrisics. This is now used in shaders to map the rgb image correclty. Shaders have being refined and improved. Added two params to process (or not) depth and rgb fluxs.

- MEU TutoLuaNetRequests refined, getting weather data and drawing it.
- MEU AIChatgptLouis added: deal with prompt and preprompt on an Askem test server.
- MEU FBO looks better, for depth as texture, have on cpu ok for depth now, deal with texture name as instance name at creation or renaming. Multiple fbo hacked backed. use BU_alpha on erase interface.
- MEU Typo extended and better.
## Documentatio
- introduction is more complete.
## Menu
- GaBuZoMeu Menu Utils extended with dump of texture named.
## In Flatland
- shading object improved.
## For the coder
- BU store the current GP render_index when drawn.
- introduce aaa.bdd.hook_draw_multiple( id ) and expand C++ part with TYPE_LUA.
- Update to Visual studio Version 17.14.0 Preview 3.0.
- Update to current stable version of FTgl and FreeType2.
- GOL bend_equation_advanced cleaned.
- add GOL and gol.get_color() (doen because of c_bdd_text).
- Add GLSL AAA_transform_model_to_projection(). bias/gain functions and smoothstep_safe() now in glsl includes.
- file and dir fns protected from crash due to non ASC character in names. Getting modification time now can be done using std::filesystem.
- BU_MENU reated by set_menu are noe used for preset anymore.
- BU_pos centralize and improved fx stuff with verbose option.
- remove bug when drawing axes.
- MEU bar BUS have now their own group.
- aaa.file.get_folder() return nil when canceled now.
- adjust deal_with_touch methods.
- now deal with rotation arounbt top left corner in BU creation (fifth field in rect).
- in code viz is now visu or wiz when appropriate.
- in some method the existence of a value argument was tested as trueinstead of not nil, to false value did not pass the test.
- BUS:add_selector_target_lua() had two argument named "name".
- BU_TEXT have now __b_value_load_save set to true, so are loaded/saved by default.
- BUS:save_bu_pos() had a bug wheb saving only value producing "" as string to save.\
- c_bdd_image_analyse protected from crash when no image.
- c_movie_player release through c_flux_store was causing potential crash. Now crash is avoided but bug still around (Mâa 2025 May).
- BUTTON:is_inside_xy_local() replace hack in  BU:is_inside_xy_local().
- BU draw text logic changed (still need more unification).
- Capture Kinect Azure extended to deal with color control settings (auto and manual) and camera info.
- aaa.screen.get_lbrt/get_sxy() now return data for screen 1 when the id correspond to a screen with null sx or sy.
- In exe and file uv_use_bdd_ref renamed uv_src_bdd_use.
- lua aaa.net_requests is now aaa.net.requests and have been extended.
- BU_SHOW deal with instant value.
- aaa.img/img3d extended: set_bind_filename(), erase() and swap() added.
- MULTI and MULTI_CURVE are now SLIDER_MULTI and SLIDER_MULTI_CURVE, UI redone and improved.
- GABU_OBJ: show() cleaned, GABU_OBJ:show_ui_error() added.
- mu_ui stuff removed (MEU bu_title color handled with new code), MU fast draw improved.
- add dump code for texture named.
- exe more protected when loading param using also a bigger load buffer.
- exe cancel some operation when img bind _index is out of bound.
- c_bdd_boid draw_normal() what buggy in non parallel mode.
- c_bdd_uv subclass now draw along u or v depending on nb_u/v. Order of point is now along_u first always. Some bdd deal with turn (sphere,torus,grid).
- add aaa.draw_line_2d_stipple()
- GABU_OBJ have a unique id: get it with get_gabu_obj_id().
- Update at end of September libs: OpenVR, dlib(v20.0), Glew(2.2.0), NVTX, fbx sdk(2020.3.7), freeImage(3.18.0), PixFC-sse, Bass(2.4.5.17), BassAsio (1.4.2.0), FTTW(dll 3.3.5, Src 3.3.10), Ptgrey now optionnal and removed bu default.
- added aaa.font lua glue.
- Font stuff in C and lua went throught a lot of improvement.
- lenght is now length everywhere.
- the lua fns help definitions for C registered fn now fit the .doc strategy used in lua compatible with BU_DOC.

# --- installer.1.3.1--- "NewImages Festival Workshops"
## UIs
- **BU_MONITOR** added **key + and -**: change alpha mode.
- **Shift Ctrl Drag MU** don't provoque errors anymore.
- in **MEU Bar Def and Both** keep the current Tab
- **Alt Move/Resize** extended with **Alt \*** and **Alt /**
- graphic adjustements.
## Net Requests
- This is the long wanted possibility to do asynchronous request on the net. This is done with a C++ part integrated in the executable, lua glue under aaa.net_requests. It require a new dl	l LibCurl.dll, include in this installer.
- Change will happens. But the base is here and look promising. First so current documentation is here too [AAASeed_Net_requests](./AAADoc/AAASeed_Net_requests.pdf)
## Documentation
- MEU and MU document expanded.
- The basic training in AAADoc/Training extended, updated, getting close to a starting manual. You can see the current state of this here [Introduction 3 Hours](./AAADoc/Training/Introduction_3_hours.pdf). Install section should be ignored.
- MEU TutoBU refined and more complete, co evolved with training.
- Add a documentation on semi permanent/permanent installations. Headers only for now, to be completed.
## APPlication
- APP_Garden_Base is now in AAAAPPs/Tuto.
- The mirror APP for the Issy / Hownslow network installation.
## For the coder
- MEU free refined and debugged.
- MU:get_meu() replace MU:get_meu_direct().
- GABU_OBJ is now in class table and so appear in BU_DOC.
- more crash protection and error messages on aaa.file/dir fnsm checking string arguments mainly.
---
---

# --- installer.1.3.0 !!! Major file system changes !!!
- This is a major release, changes where done to the filesystem: previous version have to be manually transformed to load again. The backward compatibility is broken here on multiple stuff.
- You need new dlls, they should be in this installer, they are also on Mâa's NAS (Main_x64_2024.7z) and on the SVN for the dev who have access.
- please read the [README](./README.md) to have some details in particular on the file tree.
## Folder AAAKernel replace GaBuData
- some stuff moved inside.
## APP folders moved
- the folder where the **AAASeed** executable live is celled the **start** folder. This folder content is reset at every install/update. So don't place anything there you want to keep. The **AAASeed** crew maintant its content.
- there is now a folder **AAAAPPs** in this start folder. it contain Apps, examples, tutorials, references provided by us.
- APPs placed in AAAKernel are not supported anymore
- paths are defined in ../AAAUser/AAA_Who.txt (again please read the [README](./README.md))
## AAA files clarified
- executables are now **AAASeed_Metal.exe** (use this one) and **AAASeed_Wood.exe** for machines with no AVX2 
- there is a **c_dir_pool** object with symbolic dirs to deal with the new folder architecture (accessible in flatland CTRL F10).
- Folders AAA holding the MEUs and .bu_pos are now named **AAA_MEU**.
- Folder AAA in AAAKernel is now **AAA_PROTO**. it contains folders with names starting by MEU_PROTO_, these are parsed when an app start to find which MEU type exists. Other directories are not parsed for MEU prototypes.
- AAAUser up one level for the start folder now contains network and pref stuff (again please read the [README](./README.md)). There is an original folder **./AAAKernel/Dev/AAAUser** used by the installer when there no AAAUser yet.
- files with extension '.bu_pos' have now a '.bus' extension.
- more renames:
	- files *_MEU.bu_pos are now AAA_MUS.bus  
	- Back Ui.bu_pos renamed AAA_Uis.bus and move up one level  
	- GA.bu_pos renamed AAA_Ga.bus and move up one level
	- Garden.bu_pos renamed AAA_regular.bus and move up one level  
	- Garden_top.bu_pos renamed AAA_top.bus and move up one level  
	- Garden_Top_Top_.bu_pos removed now regrouped with AAA_BuGa.bus
	- Garden_Monitoring.bu_pos renamed AAA_Monitors.bus and move up one level  
	- Garden_MUS.bu_pos renamed AAA_MUS.bus  
	- GAVIZ.bu_pos renamed AAA_BuGa.bu and move up one level  
	- mus_def.lua renamed AAA_MUS_def.lua 
- global material are now in Pref
## OpenCV 4.10.0 with Cuda replace OpenCV 4.1.2
- Image Master improved (Flatland F10), now display right version
- MEU TutoOpenCV added as example for Dimitri/CERTH
- MEU OpenCVBase added
- in c_bdd_opencv Lukas-Kanade optical flow added (FarneBack need to be set to OFF)
## Bad bugs solved
- Mouse pointer should not vanish anymore
- Partial adjustement made to sound_spectrum and Beat (these should be rewrote later).
- C++ deal better with use of std::filesystem. Use try catch also now.
- No more bug when we create a dir with an empty name.
- yuv mode have being corrected and several conversion function to rbga8 added (less video capture bugs)
- some very low level OpenGl polygon mode stuff and texture matrix bug solved.
- the kinect depth "flashing capture monitor bug" is fixed (mipmap generation was missing).
- the random continuous compilation of shaders is fixed too.  
- Mouse coordonates don't jump anymore when we go left or up of the window.
## UI
- correct lua menus and add a sub menu utils.
- Undo/Redo rewrote and extended a lot, deal also with moving and resizing, all BU should function now.
- GA_HELP refined and extended: BU_COLOR, CAMERA flying.
- StarMenu (or UIF for UI Fast) partially rewrote and extended. All BU have one by default (for BUTTON, SELECTOR where it don't appear by default, click and go quickly out of BU rectangle). This way you can see the name of the BU and access 4 buttons: Name, Values, Dump, Doc.
- BUTTON and SLIDER can now have an associated SELECTOR, ui refined in this case (keyboard in particular).
- BU_MESS cleaned: slider is back, double click improved.
- refine dialogs for clarity. character can now be filtered.
- BU_MONITOR and BANK improved. All monitor have Open/Save/SaveAs Button'd accessible through StarMenu (UIF or UI Fast).
- BU_BANK improved and on top of monitoring.
- String Dialog can filter input in some cases.
- Hidden, Unused and Used Button are now mobiles.
- BU_VIZ can now be touched
- touching BU_ALIVE is like F3 key
- BU_EYE improved and can be touched...
- refine MULTI_CURVE.
### MEU
- MU/MEU display a link to each other.
- MEU top bar reorganized, have now Redef and Def button (Redef read saved state from preset0, Def just redefine the MEU UI).
- CAMERA selector show and control individual locking of camera (param ui_lock): shift click on selector.
- MEU have an about. This a first quick pass at it, feedback welcome.
- when MEU define_ui trigger an error now generate a better BU_PB (red circle pulsing).
### Visual stuff
- A lot of small visual detail changed for clarity.
- BU_RECT now in red when not rendering at all (BU_ALIVE Off). Text color in red for non 0. or 1. values.
- make flatland more visible by default.
- ALt key pushed change transparency on BU background.
- BU_CAM manifest when can move camera
### More
- camera flying mode expanded.
- when lua_master don't trigger dialog on errors, the editor is not trigger anymore to avoid making it unusable.
- StarMenu Name don't surround name with " anymore.
- BU_EYE really functionnal and no permanent message in terminal.
## APPs added
- APP_Tuto_ImgSend
- APP_Tuto_Control
- APP_Tuto_Kinect
- APP_Garden_Base
- APP_Garden_Boid
- and more
## MEUs
- MU can be moved and dropped around
- Instantiate, Copy (Ctrl Move), Destroy and rename seems all ok now, and function with undo/redo.
- name is now the way to access any GABU_OBJ (it replace gabu_name name_gabu and sname).sname is replaced by label and used only on UI for now.
- rename now check type name, allowed char and added to undo/redo.
- enforce unique name by folder (by MUS internally) instead of globally
- more documentations
### BU_CREATE_MEU is here and functionnal
- BU MEU is here to create new MEUs (they appear under the Unused Button). They can be selected/excluded by Tag and selected by Text pattern.
- Mew MEU appear undr Unused Button.
### new MEUS
- MEU TrainBU added for training show different BU
- MEU Tutos for drawing textured primitive
- MEU Tuto for line/maze Drawing
- MEU AAAUtils used by Mâa to perform changes to attain this version. Kept as example.
- MEU GeoUV added: display basic primitives mapped.
- MEU TutoMeuBuControl: show how a MEU can control a bu in another meu
- MEU TutoSliderMulti demonstrate a basic linear curve editor
- MEU ExSliderGrid:
- MEU ExSliderInstance: example of drawning using Instance drawing (everything is done by the shader) using local vertex and fragment shader
- MEU TutoOpenCv
- MEU TutoCurve3d
### MEUs by Abdalight
- HexCraze added(v13 or so now): play and enjoy with hexagon grid.
- ShaderLorenztAttractor (need more work)
- Paint in construction
### MEU Spout
- extend and refine UI.
- int8,int6,fp16,f32 type and R,G,RGB,RGBA formats now supported.
- receiver and sender now support changing size and format on the fly.
### MEU ImgAnal added
- used to analyse coverage or fing including rect (like the Manorine)
- some bugs corrected in c_bdd_img_analyse (the discrete case was not well handled)
- for now just find the barycenter (u/v) of the white pixels in a CPU image
### MEU video
- Display Grabber revived. This is a realtime screen capture. Croping is now ok (horizontaly modulo 32 only possible)
- c_tex_video directshow handle better speed change including zero speed
- In and Out are now loaded and saved
### MEUs Improved
- MEU using multiple buffers (JumpFlood, ReactDif, Branching, Ripple) redone: use only one texture from Fbo, deal with output color/alpha.
- MEU TK have now main, edge and particle separate colors.
- replace and improve MEU BasicShader: example of a grid drawn using a local vertex and fragment shader.
- Kinect: can now choose kinect type. "flashing bug" resolved.
- ImgSend: better image part, a UI to edit net_link used.
- Boid
- Voronoi
- Toy: refined
- MuBegin: add transfo
- Out: UI for display mode refined and fragment shader optimized, added transformations.
- Movie Writer is now functionnal and accept all fbo format except depth perhaps. Compress texture flux into avi mp4 file.
### MEUs Cleaned
- ColorCurve: checked, updated and move to PROTO
- Materials: retrieve the c_obj_ui matarial in a better way
- CaptureRect
- class MEU_FBO extended
## BU_DOC is functionnal and useful
- is used to display documentation (Which is still poor) and can also act as an inspector for lua gabu_obj and tables.
- UIF Menus have now a "Doc" button to send the corresponding object to the BU Doc.
- now on top of monitoring
## Trackers
- intel Lidar l515 depth can be read in 320x240
- 3dConnexion SpaceTraveler have been hack back in exe
## More
- WatchDog is back (see in [README](./README.md))
- CTRL F1 don't trigger error dialog anymore
- Noise Textures updated and added
- AAASeed new About
- check all the apps (restore them totally or partially) and resaved them
- SLIDER_XY improved
- Corrections while doing VJ: plenty of bugs
- New GridUv texture use now color instand of black grid. This is better to visualize, in particular zero uv size.
- AAASeed started with shift down is more verbose at start.
## for the lua coder
- BU_TEXTURE replace previous UI for selecting texture (it look the same but...). Old code removed, renamed and clarified. Absorb MEU_add_tex stuff.
- Move texture stuff to TEXS.
- MEU:load_tex() is now MEU:load_texture() and use TEXS:load_texture() as BU_TEXYURE too.
- BU_LIST have been added and BU_DOC and BU_CREATE_MEU use it.
- name and label instead of name_gabu, sname and name, a lot of change in the APPs file to propagate this.
- all BUS/BU api for add/create rewrote to use rect arguments instead of x,y, sx,sy, angle. Tested and stable now.
- BU_LIST introduced (used in BU_DOC).
- BU_WINDOW_LIST adde, base class for BU_DOC and New MEU Dialog soon.
- BU create strategy redone
- GABU_OBJ creation name kept, __name renamed __name_gabu and isolated, add change_gabu_name()
- BUS and MEU adding of BU have being fully cleaned up, APIs are now symetric.
- bu_group mecanism redone at BUS:add_bu() level instead of MEU, so it can be used in Customer UI.
- BU,BUS and MEU are more strict on private fields ( use prefix __ ).
- BU_COLOR was cleaned.
- BU_WINDOW was added to intercept ui stuff.
- aaa.layer extended and documented.
- BUS_CTX introduced to encapsulate encapsulate BUSS.
- file .bus now use BUS.set_bu_pos_value() and BUS.set_bu_pos_value_mini() with rect argument.
- add TIMER.
- find MU now check also reference for MEU Ref.
- A lot of small changes in particular how obj are retrived.
- add a Dev doc.
- MULTI_CURVE is now in a separeted file.
- GABU_OBJ have now apply_fn/method_down() instead of apply_down.
- preset save a version number.
- correction on __bus_offset which seems usable now.
- a lot of corrections on low level stuff in GABU_OBJ and CLASS.
- more handling of different dialogs in C, lua return now a boolean. In the change_value case used to filter input.
- __set_window_state,set_window_state() deal with undo/redo and replace set_mini() and other fns.
- aaa.img.move_to_gpu() replace aaa.img.load_in_board(), aaa.img.move_from_gpu() added.
- click long canceled when UIF appear.
- MU rename go thru checks and deal with undo/redo.
- SCOPE protected from Nan.
- remove_trailing_slash( str ) now in string instead of aaa.file.
- aaa.file.do_dialog_open/save now accept "image" as filter argument.
- MEU_CTX (MEU context) introduced and now used.
- MEU init sequence redone (dir_protos removed) and centralise in a new method.
- MEU initialisation improved. sequence of init in particular. Instance have now there lua info striped to avoid proto infos duplication.
- MEU free should be correct now, MEU_DIR_BASE now free its mus.
- in pure instance the lua info is erase.d
- in names of lua fn and field slot_ui replaced by ui_slot everywhere.
- BU_CREATE_MEU better display names.
## lua gol changed, extended and internally optimized
- gol.texcoor2_vertex2() removed please use gol.draw_uv_xy() instead
- add fns for drawing with uv using tables as arguments
- add gol.set_attrib_uv/normal() and gol.draw_xxx_uv/normal/uv_normal_xd() and restore user datum method
## aaalua Glue
- add bdd.do/enum_command() and refine aaa.bdd.set/get_control_point.
- aaa.bdd.flex glue functions arguments have been checked and improved
- aaa.time.restart() added
- gol.get_point_size() and gol.get_line_size() added
- add aaa.window fns to control window title
## for the C++ coder
- Compiled with Visual Studio Community 2022 (64-bit) Version 17.13.0 Preview 2.1.
- Compiled with more recent windows sdk 10.0.26100.0 (Solve Alain longuet Asus's problem).
- exe have being recompiled with declaration of bind params being explicit.
- Faster start up because of faster retrivial of c_obj_ui
- C MEMSET rewrote and optimize using AVX
- SpoutSDK now the current one: 2.007.015.
- c_cpu_ui show AVX
- image send ui, bdd and process improved
- keyboard modifiers now cached
- bdd_img_send() optimized and crop is now correct
- net: add img counters
- kinect v1 don't crash AAASeed anymore (not sure it is functional).
- inititialisation of obj members move to .cpp instead from .h
- c_bdd_boid inproved
- img format conversion extended and optimized, so all r rg rgb rgba can go to bgr, this way OpenCV MovieWriter can be used on all Fbo Type.
- c_cpu have now has/allow/use and is updated every frame, but not saved/loaded.
- c_bdd_grid_adjustable improved: undo and display.
## debug improved
- improve printing, ckecking and messaging
- print_traceback() is more readable and display full filename   
- build_table_header() used in table.print() was buggy
- remove bugs when using the global table
- aaa.debug.print_fn/method() extended
---
---

# --- installer.1.2.2---
## 1/ MEU NdcGrids
- UI refined end extended
- offset and scaling xy added
## 2/ MEU Trax
- deal now with connection with the mu slider too
## 3/ MEU Out
- collect different mode in one selector, adding Color and Black Mode
- correct shaders so it will function with Intel Gpu driver
## 4/ Move camera
- camera mouvement with mouse can now be smoothed with an interpolation (use pref/UI/ui_camera_inter). This allow live manipulation in a show without disturbing movements.
## 5/ Shaders
- random and noise extended in include. Text offset for error message is now 2500 lines.
- the "AMD" problem on Faycal machine should be gone (this was an installer problem)
## 6/ MEU timings
- MEU now display the time elapsed between the beginning and end of their render. This indicative, the underlying OpenGl driver
## 7/ LaGranja
- This a version cleaned up after the show in LaGranja on May 3th 2024
## 8/ internals
- add MEU:have_instance and change MEU.__bu_ui strategy

# --- installer.1.2.1---
## 1/ UI
- Extend MEU clipboard
- Redid c_shading param code for better param comment
## 2/ MEU FLex
- Clean Up and layout
## 3/ MEU KinMoveAuto
- Bad bug corrected: when move is not used (No) we now have the image back
## 4/ MEU ImgSend
- UI Improved, tested. exe now protected from reception of Image network packet at start.
## 5/ OSC
- C and lua stability much better
- deal with more tags on reception
- improve OSC_ndim doc
## 6/ Shader on Amd
- include_b.glsl amd compatible 
- still a problem with MEU Displace

# --- installer.1.1.9 ---
## 1/ Net
- net_link stop then start better
- 4 OSC destination now (was 3)
## 2/ MEU OSCSendLab and OSCReceiveLab
- implemented first version of ndim protocol see OSC_ndim.md in AAADoc
- AAASeed listen to UDP OSC on port 18010

# --- installer.1.1.81 ---
## 1/ MEU TRAX
- first usable version with persistent connexions (1 In and 3 out)
## 2/ UI
- Star menu: add Values and Name Buttons (to be refined and finished)
- BU_RECT slider bar at the top of BU_RECT
## 3/ MEU KinAutoMove
- Remap corrected
## 4/ Internals
- MEU KinMoveAuto: decimation test done after the push
- MEU Mondrian: private to Mâa and Christian Amoretti for the moment
- Add MEU Watcher for TextRiot/StableDif demo
- add BU:add_dialog_message() to display message close to bu
- SCOPE:draw() refined

# --- installer.1.1.7 ---BU:add_dialog_message
## 1/ UI
- SELECTOR and BUTTON have a star menu now. It appears when you go out of the inside area in a short delay. It is very fresh, so bugs and problems are around the corner, it need to be refined here and there, please give Mâa feedback.
- MEU Fbo icon are now marked and even animated, so they are easy to spot in the render chain.
- BU_CAM manifest itself when it is off and we try to use the camera. Special thanks and dedicace to Priam which as most users don't read manuals and keep on not doing it. This help me improve the software while this is frustrating to him.
- CTRL F2 function in more case but set up sometimes ui to non used places, in this case repeat operation
- SHIFT CTRL F2 added which work the same but go through the UI configuration in a reverse order
- BU_RECT created close to the BU_RECT which interpreted the creating command instead of center
- resolved bug on click_up when on Max for right side of SLIDER_TWOShaders: first AAA_get/transform fns definition and use 
## 2/ Shaders
- Big cleanup by avoiding ftexgen() (and similar) repetition, now in include_rest.glsl. Even removed some unused files.
## 3/ MEU KinMoveAuto
- Flip_x in transformation is now handled by vertex shader, geometry shader still deal with vertice order to respect front/back face. This way Remap (done also in the vertex ahder) is handled taking in account flip_x and don't need an extra Inversion to conpensate in the flip_x case.
- Shaders optimized
## 4/ c_bdd_grid_adjustable (MEU Out)
- Symetry in u and v can be used in continuous mode (no need to trig every time)
## 5/ MEU NdcErode
- more modes and parameters in particular to eliminate static stuff in a depth capture.
## 6/ MEU PIP
- the automatic fade was not functionning, in fact it was a bug in MEU:set_bind_2d() which was changing layer/texture bank/bind as if id was always 1.
## 7/ MEU Materials
- add access to alpha diffuse.
## 8/ MEU Displace
- better than DisplaceCL (depreciated: will be removed soon)
- it is a prototype now
- a tuto for MEU, in particular MEU with shaders. Use similar MEU DisplaceTuto to experiment. 

# --- installer.1.1.6 ---
## 1/ MEU Out and c_bdd_grid_adjustable
- Separate White and UV visualisation from Crop
- Change param and BU From Pure to Deform
- Fuzzy new UI
- 4 gamma values can be set/get: so different fuzzy side from the same or distinct MEU can share the gamma value. Before we had only a master value
- Fuzzy optimized
## 2/ MEU exploitation
- set window to NoTopMost when we stop forcing it to TopMost: window don't stay forced on top
## 3/ Tips and Tricks document added in AAADoc
- with a first try at a short doc to help you change which kinect version you use and some concepts around it.
## 4/ UI
- GA_HELP refined and extended
## 5/ Internals
- MU find prtotected from bad pattern
- Change default texture binds used by Kinect A/B/C
- MEU Kinect_A saved as using kinect v2 now


# --- installer.1.1.5 ---
## 1/ FBOs
- A lot of internal change
- a FBO can now loop through a serie of output textures. this way we can access a texture history and for example do a difference with a previous frame.
- FlipFlop/switching completely rewrote, MEU JumpFlood, ReactDif, Branching and Ripple adapted using do_fbo_begin/end(). they work fine also with 1 iteration now.
- texture copy version should work too now
## 2/ Kinect capture (KInectA for now B and C soon)
- use FBO switching in Dir_KinectA
- use new binds for KinectA/B/C
- use new FBOs and binds in Dir_KinectA
- add a FBO and NdcMerge (see below) MEU to process and output differences
## 3/ MEU KinMoveAuto
- improved UI
- add Inv Button to XYZ range
- add clear buttons with confirmation
- add the Remap functionality: use a rg map to distord space
## 4/ Capture
- param split in capture_id_used and asked. This avoid erasing precious id linked to a specific installation. WE need to do an explicit copy to loose it now.
## 5/ MEU NdcMerge
- made to Merge two Kinect Flux or compute difference between different frames
- access to which texture is used in a switching Fbos
## 6/ MEU NdcErode
- Specificly made to eliminate the noise left over when doing difference on successive Kinect Frames with NdcMerge.
## 7/ MEU Typo
- Display Sdf fonts (Signed Distance Field)
## 8/ UI
- Add keys and SHIFT clicks to change monitor Bind (see help)
- flip value now function on SLIDER_TWO
- improved default values on SLIDER_TWO (use SLIDER code)
- Key and key_special now process by BU, then APP, then GA. it was the reverse before, so specialized Bu used of keys was masked
## 9/ BU_TEXT
- they now load ans save their value
## 10/ c_bdd_grid_adjustable
- Added u/v symmetry trig
## 11/ Internals
- creation of c_obj_ui from GaBu (create_by_cid and clone) now deal better with setting a root to the new obj: MEU AAATree look now better and is more useful.
- register these objs, when GABU_OBJ is freed we use it for release and destruction of these objs. No more free_custom()
- confirmation button corrected
- Instantiation of MU now remove aaa.lua (symbolic name) in interface
- clean up MEU to call self:draw_layers/_begin/_end instead of fns when possible.
- Start to use more bind for Fbos and Monitor (need to be finished)
- M1-M32 Monitor defined in BIND_TEX
- refine fbo __copy_attachment_to_tex
- start to use PROTO_name.lua for MEU script name for clarity
- ...
## 12/ Maa's stuff
- Nimes particles handle touch / untouch logic now

# --- installer.1.1.4 ---
## 1/ Transformations
- transformation added to Obj3D
- add now a bu_group in transfo, this change some BU names and invalidate previous save of these BUs
- Improve UI layout
- Start experiment with multiple set of value (see MEU AAATree)
## 2/ Functions key
- Help now done against C and lua
- check that all corresponding path code are ok
- remove troubling keys on more exotic combination: e.g. F6 - > SHIFT CTRL F6
## 3/ Errors
- Remove most of the error at start of Garden Tutorial
## 4/ FTGL
- Updated to a more recent version of the Lib
- Probably supress a memory leak (in MEU Typo)
## 5/ UI
- Visual feeedback on Save and Save All
## 6/ MuBegin
- All modes should be functional now


# --- installer.1.1.3 ---
## 1/ Transformations
- The transformation was inactive in some MEU, it is now forced at init
- More MEU have it
## 2/ Edit string
- multiline by default now: could trigger side effects let me know
## 3/ MEU Typo
- promoted to a full proto, and remove a lot of instances
- display font name
- double click on selector let you load fonts in the global Font bind like already done for images/textures and video
- To edit text double click on first text line in MEU (below font name text info)
## 4/ lua script not copied anymore when instantiate mu
- only the proto script is used and this was creating unused files


# --- installer.1.1.2 ---
## 1/ Function Keys
- Some obscure legacy one removed
- All in the help now and checked against the C code
## 2/ Transformations
- A generic process was created to add translate(Tra), Rotate(Rot) and Scale(Sca) to MEU
- It have being added to some MEUs
- add_transfo() in MEU PIP could be used as examples
- Let Mâa Now if you need it in other MEU


# --- installer.1.1.1 --- (Setup.exe Updated to 1.1.0)
## 1/ MEU AAATree extended
- It works with the refined c_bdd_cell_draw (in Layer_A)
- Internal structure is improved for clarity in C and GaBu
- Transformation inside is a work in process which should go soon to most graphic MEU as a Tab
## 2/ MEU NetLab introduced
- Show the first experiences (than what MEU Lab are made for) Mâa and Louis done with socket and hhtp
- We download images from the web infact (try the Louis button) !
## 3/ Help Improved
- F1 toggle it too now
- Function keys documented in an help group, not finished yet need to complete and check with reality.
- Key documentation can be reused in GA_HELP
## 4/ Lua glue refined
- Some lua error message display argument position now
## 5/ Beginning of this document Added
- Please read


# --- installer.1.1.0 ---
A lot of underground work in the C++
## 1/ c_obj_ui id cleanup
- Clearer code and more crash guard. then start at 1 and end at 1024*1024-1
## 2/ A focus on the AAASeed tree structure
- You can see it using the experimental MEU AAATree_1 in Garden Tutorial.
- New intermediate c_node_ui where added to regroup c_obj_ui.
- GaBuZoMeu now name the c_obj_ui it create for more clarity.
- c_bdd_cell_draw extended and refined, More to come.
## 3/ Lua socket
- lua socket code have being absorbed in AAASeed. You can even do http request.
- More need to be done and tested in particular absorb also ssl, so we can do https too.


# --- installer.1.0.9 ---
Release Notes now done for current pool of users by Mâa then Louis
## 1/ New Pass on Radeon shader (Priam and Benjamin)
Depreciated OpenGl Fix path Light stuff still don't fonction on Radeon driver. But most APP function on Radeon now.
## 2/ MEU MuBegin/End (Tendayi and Jaime)
- loop mode refined and extended
- support now loops in loops
- most of it rewritten, other option than Skip and Loop should not be used for the moment
## 3/ MEU Typo (Priam)
- Add a way to edit edit text
- Add a font selector
- Load Priam fonts
## 4/ Pointer sorry Cursor (will change that later)
- param names have being changed and doc added in the help
- behavior refined in the C++ Kernel
## 5/ Bugs
- space key for monitor is ok now


# --- installer.1.0.8 ---
## 1/ refine lua_aaaseed docs
## 2/ x/y_offset for BUs is now __exp_x/y_offset to isolate and experiment
## 3/ Shaders
- now 4 shader include_a depending on platform
- try to correct all shader on the const uniform (using CONST) to make sure it compile on amd
- hack shader include_light/rest.glsl and vs_fixed_main.vert so Nimes will run on Radeon Vega (use AAA_is_NVIDIA)
- correct shader and check on a Radeon machine
## 4/ UIF
Make sure uif is on the top of all, hack bus offset and transfo to have a decent state on main bus offsets for GA
## 5/ cleaning
clean derviche APP trash, comment Meu Avatar, correct draw_icon ref bug


# --- installer.1.0.7 ---
## 1/ removed force MEU tuto in UI from APPs
## 2/ Instantiate have now a dialog
 when type have been changed or space in the name,
## 3/ Radeon
- change include_a to avoid Radeon pb
- make shader work on AMD by having a different include_a.glsl that the general one (define CONST as nothing on AMD)
- MEU Out and PIP shaders corrected
- adapt some base shaders for amd (kinect, JumpFlood, Branching, Obj3d, Sun)


# --- installer.1.0.6 ---
## 1/ tab
tab for flatland doesn't change the main zoom of the UI at the same time anymore
this is now done through CRTL TAB
## 2/ save
when doing a CTRL s or double ESC and so saving the global context
AAASeed will now save  the MU/MEU attached to the BU_MEU (Ui1, UI2, ... )
so you will find these as you left them
## 3/ corrections
- corrected a memory leak on basic gl object which was making bdd_gl_part leak when points where reallocated
- more info on gl buffer c_gol_ui
- better handing of shader names and param

