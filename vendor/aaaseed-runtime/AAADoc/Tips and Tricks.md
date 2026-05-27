# Switching between Kinect model 
Locate MEU Kinect_A and display MEU interface 

![MEU_Kinect](./images/MEU_Kinect.png)

This meu rely internally on a tex_video object that you can access using the Tex_video button, in particular the capture section is what you use to choose which type of captor you want to use (capture_ type param):

![Tex_video_capture_param](./images/Tex_video_capture_param.png)

The capture_type is probably correct: Kinect.
But eventually you will need to choose which kinect you are using (https://en.wikipedia.org/wiki/Kinect):
- Kinect v1 or Kinect for Xbox 360 (2010). It should be usable but we don't really support it anymore
- Kinect v2 or Kinect for Xbox One, the big black rectangular one with a 70 by 50 degree field
- Kinect Azure, the small white one with a 120 degree field

To select which kinect typer you want to use there is two ways:
1/ use the MEU Kinect Button, and flatland will be focused on the kinect object
1/ navigate in the tex_video object to the Kinect section to access the same Kinect Object. In Fact at the bottom of the capture section there is a serie of param and objects attached for the diffrent type of capture handled.

![Kinect_ui_param](./images/Kinect_ui_param.png)

But first make sure the capture is closed: Open button should be off (see below)
![MEU_KInect_open_run_off](./images/MEU_KInect_open_run_off.png)

Change the kinect_ui object param version_asked to the one you want.
Then push the MEU button Open Button to open the library corresponding to the version you selected (see below)

![MEU_KInect_open_on](./images/MEU_KInect_open_on.png)

Then push the MEU button Run Button so AAASeed wikk start streaming data (see below)

![MEU_Kinect_running_v2](./images/MEU_Kinect_running_v2.png)

or if RGB image was requested (param under RBG stuff in kinect _ui)

![MEU_Kinect_running_v2_rgb](./images/MEU_Kinect_running_v2_rgb.png)

The Control Next Button id on make sur the next MEU (usually a MEU dir) oin the render chain is execurted or not depending on the fact a next frame arrived. This way we avoid consuming processing power when the data from the captor is still the same. when AAASeed is running fater than the captor you should see the info text (yellow) on the right of the button switching state and the render chain switch too, the orange line below

![Render_chain_Kinect](./images/Render_chain_Kinect.png)

At this point Mâa will recommend you to push the Save Button at the top Right of the MEU so next time you open AAASeed the MEU Kinect_A will be in this state.

The MEU Kinect just read the data from the captor, the real processing is done here by the MEU in the Dir_KinectA

