if aaa.lua.global.declare_table( "AAAWelcome" ) then
	AAAWelcome.ref = {}
	local ref = AAAWelcome.ref
	ref.layers = aaa.layers.get_cur()
	--todo use userfata ?
	--ref.values = aaa.obj.get_with_ud( "GaBu/AAA_Welcome/Background/fx.values" )
	ref.values = aaa.obj.get_by_name( "GaBu/AAA_Welcome/Background/fx.values" )
	ref.value = param.get_ref( ref.values, "value_01" )
end

if bus_cur then
	local t = AAAWelcome.page_active
	if t then
		local v = 0
		local p = bus_cur:get_page()
		for i=1, #t do
			if t[i]==p then
				v=1
				break
			end
		end
		param.set( AAAWelcome.ref.value, v )
	end
end
