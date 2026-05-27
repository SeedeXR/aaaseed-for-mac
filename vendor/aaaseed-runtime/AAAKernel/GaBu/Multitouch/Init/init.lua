if not aaa.lua.global.get( "CALAGE" ) then
	aaa.print( "will activate needed modules by multitouch" )
	aaa.activate
	{
		"GaBu/GaBu_Util/default.layerss_param",
		"GaBu/GaBu_BU/default.layerss_param",
		"GaBu/GaBu_Input/default.layerss_param",
		"GaBu/GaBu_Calage/default.layerss_param"
	}
end

