local caller = aaa.get_caller()
local key = "maa_ui_"..caller
--aaa.print( key )
if aaa[key] then return end

aaa[key] = {}
local	t = aaa[key]
t.obj = aaa.obj.get( "opencl_plancton" )
param.set_comment( t.obj, "param_01", "time factor" )
param.set_comment( t.obj, "param_02", "cone deformation" )
--param.set_comment( t.obj, "param_03", "z factor" )
--param.set_comment( t.obj, "param_07", "Kinect" )
--param.set_comment( t.obj, "param_08", "Normal displace" )
