local caller = aaa.get_caller()
local key = "maa_ui_"..caller


aaa[key] = {}
local	t = aaa[key]
t.obj = aaa.obj.get( "opencl_kinect2" )
param.set_comment( t.obj, "param_01", "gradient du" )
param.set_comment( t.obj, "param_02", "gradient dv" )
param.set_comment( t.obj, "param_03", "z factor" )
param.set_comment( t.obj, "param_04", "z min" )
param.set_comment( t.obj, "param_05", "normal factor" )
param.set_comment( t.obj, "param_07", "Kinect" )
param.set_comment( t.obj, "param_08", "Normal displace" )
