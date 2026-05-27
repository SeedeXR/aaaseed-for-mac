function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu
    local param_set = param.set

	local ix,iy = 1,1
    local SY,DY = 1,.2
    local size = {}
    self.size = size

    self:add_slider({ix,iy,4,SY},"X", size, "x",0,0,10 ):set_text("x"):set_color_back()
    self:add_slider({ix+4,iy,4,SY},"Y", size, "y",0,0,10 ):set_text("y"):set_color_back()
    self:add_slider({ix+8,iy,4,SY},"Z", size, "z",0,0,10 ):set_text("z"):set_color_back()
    self:add_slider({ix,iy+1,4,SY},"W", size, "w",0,1,10 ):set_text("width"):set_color_back()
    self:add_slider({ix+4,iy+1,4,SY},"H", size, "h",0,1,10 ):set_text("height"):set_color_back()
    -- Insert reference into global table of StrataDeadzones
    if (app.StrataDeadzones == nil) then
        app.StrataDeadzones = {}
    end
    local idx = table.getn(app.StrataDeadzones) + 1
    self.idx = idx
    app.StrataDeadzones[idx] = self.size
    self:print("Added a zone to StrataDeadzones"..idx)
end

function meu:draw()
    local size = self.size
    gol.set_texture_dim(0)
    gol.disable_lighting(false)
    gol.set_line_width( 2 )
    gol.color( 1,0,0,1 )   -- red green blue alpha
    gol.draw_line_loop_3d(  size.x,size.y,size.z,      size.w+size.x,size.y,size.z,
                            size.w+size.x,size.y,size.h+size.z,  size.x,size.y,size.h+size.z, 
                            size.x,size.y,size.z )
end

function meu:update()
    
end