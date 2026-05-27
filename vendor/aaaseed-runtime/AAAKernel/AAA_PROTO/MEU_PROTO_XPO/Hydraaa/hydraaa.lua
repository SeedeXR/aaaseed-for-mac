--	HYDRAAA MEU
--	Loads a Hydra (https://hydra.ojack.xyz) source string from a .txt file in
--	the MEU directory, transpiles it to a GLSL fragment shader and writes the
--	result to sha.frag. AAASeed's shader file watcher then picks the new
--	source up on the next frame (mtime check in c_shading::update).

function meu:define_meu_infos()
	return {
		author	= "Louis Montagne (Askem) + AAASeed",
		date	= "2026-04",
		tags	= { "2d", "Art", "Hydra", "Procedural", "Shader", "Texture", "VJ" },
		help	= "Live-codable Hydra (hydra.ojack.xyz) shader.\n"..
				  "Drop .txt files with Hydra source in this MEU's folder,\n"..
				  "select one in the dropdown : the file is transpiled to GLSL\n"..
				  "and applied as the fragment shader."
	}
end

----------------------------------------------------------------
--	Transpiler load
----------------------------------------------------------------
--	The transpiler is in a sibling Lua file. We load it once via dofile
--	using the MEU's own directory.

function meu:__load_transpiler()
	--	declare global before reading it (strict.lua)
	if aaa and aaa.lua and aaa.lua.global then
		aaa.lua.global.declare_table("HYDRAAA_TRANSPILE")
	end
	if _G.HYDRAAA_TRANSPILE and _G.HYDRAAA_TRANSPILE.transpile then return end
	local dir = self:get_dir_absolute()
	local path = dir.."/hydraaa_transpile.lua"
	local ok, err = pcall(dofile, path)
	if not ok then
		self:print_error("Hydraaa : failed to load transpiler at "..path.." : "..tostring(err))
	end
end

----------------------------------------------------------------
--	File listing
----------------------------------------------------------------
--	List .txt files in the MEU directory (excluding hydraaa_transpile.lua etc.)
--	Returns a Lua array of file basenames, sorted alphabetically.

function meu:__list_hydra_files()
	local dir = self:get_dir_absolute()
	local files = aaa.dir.get_files(dir) or {}
	local out = {}
	for _, name in ipairs(files) do
		--	keep only .txt
		if name:match("%.txt$") then
			out[#out+1] = name
		end
	end
	table.sort(out)
	return out
end

----------------------------------------------------------------
--	Transpile + write
----------------------------------------------------------------

function meu:__read_text_file(path)
	local f = aaa.file.open_no_error(path, "r")
	if not f then return nil end
	local txt = f:read("*a")
	f:close()
	return txt
end

function meu:__write_text_file(path, txt)
	local f = aaa.file.open(path, "w")
	if not f then return false end
	f:write(txt)
	f:close()
	return true
end

function meu:__transpile_selected()
	self:__load_transpiler()
	if not (_G.HYDRAAA_TRANSPILE and _G.HYDRAAA_TRANSPILE.transpile) then
		self:__set_status("transpiler not loaded", true)
		return false
	end

	local list = self.__hydra_files
	if not list or #list == 0 then
		self:__set_status("no .txt files in MEU folder", true)
		return false
	end

	local idx = self.s_hydra_file or 1
	if idx < 1 then idx = 1 end
	if idx > #list then idx = #list end
	local name = list[idx]

	local dir = self:get_dir_absolute()
	local src_path = dir.."/"..name
	local dst_path = dir.."/sha.frag"

	local src = self:__read_text_file(src_path)
	if not src then
		self:__set_status("read failed : "..src_path, true)
		return false
	end

	local res = _G.HYDRAAA_TRANSPILE.transpile(src)
	if not res.ok then
		--	keep the source visible : write a magenta placeholder shader
		--	with the error message in a comment, so the user gets a clear
		--	visual signal AND can read the error in sha.frag.
		local err_short = tostring(res.err):gsub(".*:%d+:%s*", "")
		local placeholder = ""
			.. "//	HYDRAAA : transpile error in "..name.."\n"
			.. "//	"..tostring(res.err):gsub("\n", "\n//\t").."\n"
			.. "uniform float aaa_fu_float[16];\n"
			.. "uniform int   aaa_fu_int[8];\n"
			.. "uniform vec4  aaa_fu_vec4[8];\n"
			.. "in VS_out { vec4 color; vec2 tex_coor; } vs_in;\n"
			.. "out vec4 out_result;\n"
			.. "void main(void) { out_result = vec4(1.0, 0.0, 1.0, 1.0); }\n"
		self:__write_text_file(dst_path, placeholder)
		self:__set_status("ERR "..name.." : "..err_short, true)
		return false
	end

	local ok = self:__write_text_file(dst_path, res.glsl)
	if not ok then
		self:__set_status("write failed : "..dst_path, true)
		return false
	end

	self:__set_status("OK "..name.." ("..#res.glsl.." bytes)", false)
	return true
end

function meu:__set_status(msg, b_err)
	self.__last_status = msg
	if self.ui and self.ui.bu_status and self.ui.bu_status.set_text then
		self.ui.bu_status:set_text(msg)
	end
	if b_err then
		self:print_error("Hydraaa : "..msg)
	else
		self:print("Hydraaa : "..msg)
	end
end

function meu:__on_file_changed()
	self:__transpile_selected()
end

function meu:__refresh_file_list()
	self.__hydra_files = self:__list_hydra_files()
	local bu = self.ui.bu_file
	if bu then
		local items = self.__hydra_files
		if #items == 0 then
			items = { "(no .txt)" }
		end
		bu:set_menu(items)
	end
end

----------------------------------------------------------------
--	UI
----------------------------------------------------------------

function meu:define_ui()
	local ix, iy = 1, 1
	local SY, DY = .99, .2

	self:add_shading_ui( {ix,iy, 8,SY} )
	iy = iy + SY + DY

	local y_begin = iy

------------------------------
	self:set_tab_key("Main")
	ix, iy = 1, y_begin

	self:add_button(			{ix,iy, 1,SY},	"Time Active",	self, "b_time", true ):set_text_visible(false)
	self:add_slider(			{ix+1,iy, 5,SY},	"Time",			self, "time",	0,	0,1000)
	self:add_trig_method(		{ix+6,iy, 2,SY},	"Restart",		self, "restart")
	iy = iy + SY + DY

	--	file selector as a dropdown menu (pattern from MEU_PROTO/Out).
	--	Build the menu list NOW with the actual files; initialize the
	--	target value before set_target_lua, exactly like PROTO_Out does
	--	for s_see, otherwise the menu never writes back.
	self.__hydra_files = self:__list_hydra_files()
	local items = {}
	if #self.__hydra_files == 0 then
		items[1] = "(no .txt)"
	else
		for i, name in ipairs(self.__hydra_files) do items[i] = name end
	end
	self:print("Hydraaa : "..#self.__hydra_files.." files for menu : "..table.concat(items, ", "))
	local bu = self:add_button( {ix,iy, 7,SY}, "Hydra File" )
	bu:set_menu( items )
	bu:set_target_lua(self, "s_hydra_file")
	self.s_hydra_file = self.s_hydra_file or 1
	bu:set_text_selector(true)
	bu:set_method_on_value_change(self, "__on_file_changed")
	self.ui.bu_file = bu
	self:add_trig_method( {ix+7,iy, 1,SY}, "Refresh", self, "__refresh_file_list" )
	iy = iy + SY + DY

	--	4 FFT bins exposed as sliders so Hydra audio code "() => a.fft[N]"
	--	can be driven by MIDI/OSC bound to these. They map to
	--	aaa_fu_float[5..8] in the generated shader, pushed each frame
	--	in update().
	self:add_slider( {ix,iy,   2,SY}, "fft0", self, "fft_0", 0, 0, 1 )
	self:add_slider( {ix+2,iy, 2,SY}, "fft1", self, "fft_1", 0, 0, 1 )
	self:add_slider( {ix+4,iy, 2,SY}, "fft2", self, "fft_2", 0, 0, 1 )
	self:add_slider( {ix+6,iy, 2,SY}, "fft3", self, "fft_3", 0, 0, 1 )
	iy = iy + SY + DY

	--	FBO target : type the name of an FBO MEU in your scene to enable
	--	feedback (Hydra src(o0)). Empty = no feedback, render goes direct
	--	to the show. The text widget value is read in update() and
	--	applied via set_meu_fbo.
	self.ui.bu_fbo = self:add_text( {ix,iy, 6,SY}, "FBO target" )
	self:add_trig_method( {ix+6,iy, 2,SY}, "Apply FBO", self, "__apply_fbo_target" )
	iy = iy + SY + DY

	--	Transpile status : displays last transpile result (OK or error
	--	message) so the user can see why a .txt failed without scrolling
	--	the console. Set in __transpile_selected.
	self.ui.bu_status = self:add_text_info( {ix,iy, 8,SY}, "Status" )
	self.ui.bu_status:set_text( self.__last_status or "ready" )
	iy = iy + SY + DY

	self:add_bu_texture_target_unit_nb( {ix,iy, 8,4}, "Tex_", 2,2 )
	iy = iy + 4 + DY

	ix, iy = 9, y_begin
	self:add_camera()
	self:add_size_uvf_video( {ix,iy, 8,3} )
	iy = iy + 3 + DY
	self:add_transfo( {ix,iy, 8,3} )
	iy = iy + 3 + DY
	self:add_mapping_by_side_only( {ix,iy, 8,SY} )
	iy = iy + SY + DY
	self:add_rgbf( {ix,iy, 8,SY} )
	iy = iy + SY + DY
	self:add_blending( {ix,iy} )
	iy = iy + SY + DY
	self:add_rendering()

------------------------------
	--	defensive : if init() failed earlier (bad fx config etc), get_shading()
	--	returns nil and add_shading_sliders_tab indexes a nil. Skip the
	--	shader sliders panel in that case so the rest of the UI still loads.
	local sha = self:get_shading()
	if sha then
		self:add_shading_sliders_tab( sha, {"Vert","Frag"}, y_begin, SY )
		self.ui.bu_time_vert = self:get_bu_by_key("vertex_float_1")
		self.ui.bu_time_frag = self:get_bu_by_key("fragment_float_1")
	else
		self:print_error("Hydraaa : no shading object available; skipping shader UI")
	end
end

----------------------------------------------------------------
--	Lifecycle
----------------------------------------------------------------

function meu:init()
	--	single-layer pattern matching TutoMapOSM (which renders correctly
	--	on this Mac build): Layer_A has full model+rendering+color+mapping
	--	+ shading, with bank_2d/bind_2d binding to a known texture so
	--	the bdd_grid produces visible output.
	self.layer_shading = 1
	self:add_shading(self.layer_shading)

	self:__load_transpiler()
	self:__refresh_file_list()
	if #self.__hydra_files > 0 then
		self.s_hydra_file = self.s_hydra_file or 1
		self:__transpile_selected()
	end
end

function meu:restart()
	self.time = 0
end

----------------------------------------------------------------
--	FBO target : enable Hydra src(o0) feedback by routing our render
--	to a separate FBO MEU. The user types the FBO MEU instance name
--	in the "FBO target" text input and clicks Apply.
----------------------------------------------------------------

function meu:__apply_fbo_target()
	local bu = self.ui.bu_fbo
	if not bu then return end
	local name = bu:get_value()
	if type(name) ~= "string" then name = "" end
	name = name:gsub("^%s+", ""):gsub("%s+$", "")	--	trim
	if name == "" then
		self.__fbo_target = nil
		self:print("Hydraaa : FBO target cleared, rendering to current FBO")
	else
		self.__fbo_target = name
		self:set_meu_fbo(name)
		self:print("Hydraaa : FBO target set to "..name..
			" (create a MEU FBO with that instance name in your scene)")
	end
end

function meu:update()
	--	add_button + set_menu does not fire set_method_on_value_change
	--	in this AAASeed build : poll the target value and re-transpile
	--	when the user picks a new file in the menu.
	local sel = self.s_hydra_file or 1
	if sel ~= (self.__last_sel or -1) then
		self.__last_sel = sel
		self:__transpile_selected()
	end

	--	hot reload of the selected .txt file every ~1s : if the user edits
	--	the Hydra source on disk, re-transpile so the shader updates live.
	self.__mtime_poll_n = (self.__mtime_poll_n or 0) + 1
	if (self.__mtime_poll_n % 60) == 0 then
		local list = self.__hydra_files
		if list and list[sel] then
			local path = self:get_dir_absolute().."/"..list[sel]
			local mt = aaa.file.is_exist(path) and (aaa.file.get_mtime and aaa.file.get_mtime(path) or os.time())
			--	Without aaa.file.get_mtime, fall back to comparing file size +
			--	first bytes via a cheap fingerprint (read again only if hash
			--	differs). For simplicity we just re-read and compare strings.
			local txt = self:__read_text_file(path)
			if txt and txt ~= self.__last_src then
				self.__last_src = txt
				self:__transpile_selected()
			end
		end
	end

	local t = self.time or 0
	if self.b_time then
		t = t + aaa.time.dt
		self.time = t
	end

	--	push uniforms : aaa_fu_float[0] = time, [1..2] = mouse, [3..4] = resolution
	local sha = self:get_shading()
	if sha then
		--	float 1 -> aaa_fu_float[0] (time)
		sha:set_frag_float(1, t)
		--	float 2..3 -> aaa_fu_float[1..2] (mouse), best-effort, default to 0
		local mx, my = 0, 0
		if aaa.mouse and aaa.mouse.get_xy then
			local ok, x, y = pcall(aaa.mouse.get_xy)
			if ok and x and y then mx, my = x, y end
		end
		sha:set_frag_float(2, mx)
		sha:set_frag_float(3, my)
		--	float 4..5 -> aaa_fu_float[3..4] (resolution), 1 by default to avoid /0
		local rx, ry = 1, 1
		if aaa.gol and aaa.gol.get_resolution then
			local ok, w, h = pcall(aaa.gol.get_resolution)
			if ok and w and h then rx, ry = w, h end
		end
		sha:set_frag_float(4, rx)
		sha:set_frag_float(5, ry)
		--	float 6..9 -> aaa_fu_float[5..8] (FFT bins for "a.fft[0..3]")
		sha:set_frag_float(6, self.fft_0 or 0)
		sha:set_frag_float(7, self.fft_1 or 0)
		sha:set_frag_float(8, self.fft_2 or 0)
		sha:set_frag_float(9, self.fft_3 or 0)
	end

	if self.ui.bu_time_vert then self.ui.bu_time_vert:set_value(t) end
	if self.ui.bu_time_frag then self.ui.bu_time_frag:set_value(t) end
end

function meu:draw()
	--	if a FBO target was set via the UI, switch render output to it
	--	BEFORE draw_layers_begin so the layer + our quad render into
	--	the FBO MEU's texture instead of the show. Other MEUs (or this
	--	one's src(o0)) can sample the result.
	if self.__fbo_target then
		pcall(function() self:do_fbo() end)
	end

	self:draw_layers_begin()
		--	draw_layer(1) sets up state (camera, color, shader binding).
		self:draw_layer(1)

		--	rebind the four user textures to GL_TEXTURE0..3.
		for i = 1, 4 do
			pcall(function() self:bind_texture_to_unit(i, i - 1) end)
		end

		--	bind the FBO target's output texture to GL_TEXTURE4 so the
		--	shader's g_fbo_out sampler reads it for src(o0..o3).
		--	This is the previous frame (since we are about to write the
		--	new one), enabling Hydra-style feedback chains.
		if self.__fbo_target then
			pcall(function()
				local mfbo = self:get_meu_fbo()
				if mfbo then
					local bind = mfbo:get_texture_bind_2d()
					if bind then
						gol.set_tex_unit_2d_bind(4, bind)
					end
				end
			end)
		end

		gol.color_white( 1 )
		aaa.draw_rect_uv( -1,-1, 1,1 )
	self:draw_layers_end()
end
