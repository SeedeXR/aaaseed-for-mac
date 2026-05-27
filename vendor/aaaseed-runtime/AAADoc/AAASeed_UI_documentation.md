# GaBuZoMeu

The main environment for your projects and apps. It can be switched to edition mode.

This is a basic hello world app in our project, this is GaBuZoMeu running, you won’t see anything but the resulting render of your APP.

![UIHello](./images/ui_hellow.png)


This is the same project / APP in edition mode: 

![UIEDMODE](./images/ui_edition.png)


You can see different UI elements and zones. 


The overall design may seem “vintage”, and it looks like it, but keep in mind that this UI is working on massive multitouch situations, has to be very efficient, it can work with a mouse, and without it, it is totally flexible and for sure it is skinnable if you want to change it!

You will notice: 

- UIs (1 to 6)
- The “always”, “Pre” and “Dif” zones
- There are Mus, Monitors and MEUs on the screen.

Here is the same with flatland displayed (tab)

![UIFLAT](./images/ui_flatland.png)


# UI

UIs are the zones where you get the details of the project’s MEUs, to setup your MUs. When you click on a MU, this activates its MEU inside UI5 by default.

![UIZ5](./images/ui_zone5.png)

In the middle of GaBuZoMeu, you have 6 UI buttons (UI1…UI6), you can click on them to move the MEU to a new UI zone.

Each UI Zone has a full screen mode, a standard mode, and a mini mode. You can switch from one to another using a double click in the center for the full screen mode, and in the upper part for the mini mode.

- UI 1 and UI2 are minimized.
- UI 3 to 5 are the blue zones.
- UI 6 is the smaller blue zone. 

# Always, Pre, Dif

These are the rendering zones for the MUs, the space where they will display in the following order: from bottom to top, from left to right.

You can activate or deactivate the zones using a button.

As you can see, when you activate the MU:Dif, the MU:TutoLuaHello, the Hello world app, will not display in the main (Always) screen anymore, it will display in the Dif.

![UIRENDZ](./images/ui_rendzones.png)

If you press F2 to exit GaBuZoMeu edition mode, your screen will be black (as your Hello World MU is displaying in the Dif)

Note that you can change the parameters of the MU:Dif using its MEU that is displayed as always, by default, on UI5.


# Mu

MUs are your apps on the project. You often will need more than one to achieve what you want to do. Technically speaking, they are the encapsulations of an object, which generates a graphic or not, which will execute during one frame.


Clicking on a MU: 

![UICLICKMU](./images/ui_clickmu.png)

*You will find the MUs on the right side of GaBuZoMeu. The typical MU has two parts, the left one, on which you can click to move it and display its MEU and the right one which will display a StarMenu which will allow you to edit the code, activate the MU or deactivate it, and choose on which UI to open its MEU.*


Clicking on the left side of the MU will display its MEU on UI5 as you will see on the following screenshot.

![UIMEUUI5](./images/ui_meuui5.png)

Clicking on the right side of the MU will open its StarMenu, on which you will be able to open the Directory containing its file, open its MEU in a UI Zone, open the Lua code, and activate(max)/deactivate(min) it: 

![UISTARMENU](./images/ui_starmenu.png)


# Monitors

Monitors are used to display some of your MU’s production. They are windows/zones (min, fullscreen, …)

You can find them in the middle of the screen. In this capture, Monitor A is opened.

![UIMONA](./images/ui_monitora.png)

# MEU

They are the UIs for the MUs.

This is a MEU for the MU:Dif:

![UIMEUMUDIF](./images/ui_meumudif.png)

When you click on a MEU when in Flatland, it will display information about the corresponding MU.

# ZO

Contains BUS, that are attached to BU(s). They also can be resizable. 

UI Zones are ZO.

# GA

Global Actions, this regroups every global variable on AAASeed. You will find the UI on the top zone in the middle of the screen: 

![UIGA](./images/ui_globalaction.png)

The UI allows you to quickly modify the variables values: 

![UIGAO](./images/ui_gaopened.png)


# Flatland

When you hit “Tab”, this will get you into Flatland, where you can set all parameters, displayed in a tree list.

If you click on a MU, it will display the corresponding parameters: 

![UIFLATMU](./images/ui_flatlandmu.png)

If you hit F10, it will display the system information and parameters: 

![UIFLATF10](./images/ui_flatlandf10.png)

If you hit alt-F10, it will display the MUs tree so that you can activate them: 

![UIFLATALTF10](./images/ui_flatlandaltf10.png)

The interface visually represents the hierarchy in a textual format, with each level indented. You can open/close each branch of the tree. The tracks (traxs) are represented as new branches where they connect. Numerous keyboard shortcuts allow for navigation and parameter modification. By simply right-clicking and moving the mouse, one can alter a parameter.

The effect is immediate since the rendering loop is generally called between 120 to 25 times per second, even when menus are in use or traditional dialog boxes are activated. This real-time feature is a significant characteristic of the software and offers a tangible advantage. There is no distinction between "editing" and "using."

The left button acts on parameters like a context menu, which allows one to connect and disconnect the tracks. The right button unfolds the general menu.

You can also press ‘0’ (the zero key) to display the textures information in Flatland.

# BU

BU are the graphical elements which can be seen by the user.  BU stands for Box User: Slider Button Selector, Window…

BUS are coordinates system and can be attached to a BU (several BUS can be attached to a BU.

BU lives in BUS so a BU can have other BU inside BUS/BU/BUS/BU….

ZO regroups several BUS, they are a way to control several BUS at the same time.

## BU can be resizable (a BU can be a window)

If BU is resizable pressing ALT will show 9 areas over the BU.

These areas can be used to move and resize the BU.

![UIRESIZEBU](./images/ui_resizebu.png)

BU_MONITOR and BU_MEU are good examples of resizable BU(s)

## Every resizable BU has 3 states

Each resizable BU/ZO has a full screen mode, a standard mode, and a mini mode. You can switch from one to another using a double click in the center for the full screen mode, and in the upper part for the mini mode.

Resizable BU(s) are like window they can be regular or minimized. Some of them, like BU_MONITOR, can also be full screen in this case a double click switches from normal to full screen and double click at the top switches from normal to Minimized. The space key alternates between the 3 states.

# Many thanks to AAA Productions

![UIAAA](./images/ui_wrapup.jpeg)