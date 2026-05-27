local caller = aaa.get_caller()
local key = "maa_ui_"..caller

if aaa[key] then return end

aaa[key] = {}
local	t = aaa[key]
t.obj = aaa.obj.get( "opencl_facade" )
param.set_comment( t.obj, "param_01", "gradient du" )
param.set_comment( t.obj, "param_02", "gradient dv" )
param.set_comment( t.obj, "param_03", "" )
param.set_comment( t.obj, "param_05", "to end strange factor" )
param.set_comment( t.obj, "param_06", "Touch influence" )
param.set_comment( t.obj, "param_07", "Map influence" )
param.set_comment( t.obj, "param_08", "Mouse Influence" )
param.set_comment( t.obj, "param_09", "Damp" )
param.set_comment( t.obj, "param_10", "Speed factor" )
