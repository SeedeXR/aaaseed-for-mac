function meu:init()
	local ref = self.ref

	local layers_ref = self:get_layers()

	local function build_anal_ref( name )
		local tab = {}
		tab.obj = aaa.obj.get_down( layers_ref, name )
			tab.coverage = param.get_ref( tab.obj, "out_coverage" )
		return tab
	end
	ref.anal = {}
	ref.anal[1] = build_anal_ref( "qw_face_anal_left" )
	ref.anal[2] = build_anal_ref( "qw_face_anal_center" )
	ref.anal[3] = build_anal_ref( "qw_face_anal_right" )
end

function meu:update()
	local ref = self.ref.anal
	local tot = 0
	for i=1,3 do
		--self:print(i)
		local cov = param.get( ref[i].coverage )
		tot =  tot + cov
		app.coverage[i] = cov
	end
	app.coverage_all = tot
	app.b_coverage = tot > .001
end

