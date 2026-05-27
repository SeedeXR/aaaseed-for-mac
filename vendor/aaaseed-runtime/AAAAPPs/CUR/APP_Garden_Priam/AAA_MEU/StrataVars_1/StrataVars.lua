-- un example vraiment vide
------------DEFINE
function meu:define_ui()


local param_set = param.set

local ref = self.ref
local ui = self.ui
local bu
local par

local ix = 1
local iy = 1
local ix,iy = 1,1
local SY,DY = 1,.2  
local sy = 1
local delay = self.delay 
app.activex = false
local ramp_info = {}
self.ramp = ramp_info
local vecPosA = {}
self.vecA = vecPosA
local vecPosB = {}
self.vecB = vecPosB


local layerA =	self:get_layer(1)
-- NOW you have to manually switch tranfo_trs / active to ON, it's inaccessible by code as far as i tried for 1h, P
-- ONLY when you manually switch tranfo_trs / active to ON, is the below going to work:
local transfo		=	aaa.obj.get_down_by_class	( layerA, "transfo_trs" )
local tra = {}
tra.x, tra.y, tra.z	=	param.get_ref( transfo, "translate_x" ), param.get_ref( transfo, "translate_y" ), param.get_ref( transfo, "translate_z" )
ref.tra = tra
param_set(  transfo, "active", true )
param_set(  layerA, "trs_1", 2 )
param_set(  transfo , "translate", true )

app.strataDateNew = 77
--local nana = app.strataDateNew
app.strataDateCur = 8
app.worldCenterX = 5
app.worldCenterY = 0
app.worldCenterZ = 0


bu = self:add_button({ix, iy + 2, SY, SY}, "on", self, "active", true):set_text_visible(true)
bu = self:add_button({ix, iy + 3, SY, SY}, "Loop", ramp_info, "loop_ramp", true):set_text_visible(true)
bu = self:add_slider_two({ix+3, iy + 2, 6, SY}, "Timer Range", ramp_info, "min", "max", 0, 1, 0, 1):set_color_back("u") 
bu = self:add_slider({ix+10, iy + 2, 6, SY}, "Timer Time", ramp_info, "time", 5, 0, 10):set_color_back("u") 
bu = self:add_slider({ix+4, iy + 3, 12, SY}, "Timer Out", ramp_info, "rampOut", 0, ramp_info.min, ramp_info.max):set_color_back("u") 
ramp_info.progress = 0

self:add_text_info(	{ix  ,iy ,	16,1}, "this holds the Strata world reference center in its LayerA trs" )

-- bu = self:add_slider( {ix,iy+2, 8,SY}, "Nunits", self, "N", 0,  0,20 )
-- bu = self:add_slider( {ix,iy+3, 8,SY}, "Radius", self, "R", 0,  0,20 )
-- bu = self:add_slider( {ix,iy+4, 8,SY}, "TransitionCheck", self, "TR", 0,  0,20 )
self:add_slider( {ix+8,iy+7, 8,SY}, "DateCurSldr", app, "strataDateCur", 3,  0,20 ) -- uses app.strataDateNew declaerd above
self:add_slider( {ix+8,iy+8, 8,SY}, "DateNewSldr", app, "strataDateNew", 3,  0,20 ) -- uses app.strataDateNew declaerd above


self:add_slider({ix, iy+7, 4, SY}, "TrsX", app, "worldCenterX", 0, 0, 8):set_color_back("u") -- just so it's different
self:add_slider({ix, iy+8, 4, SY}, "TrsY", app, "worldCenterY", 0, 0, 8):set_color_back("v") -- just so it's different
self:add_slider({ix, iy+9, 4, SY}, "TrsZ", app, "worldCenterZ", 0, 0, 8):set_color_back("w") -- just so it's different

bu = self:add_trig( {	ix,	iy+11,		4,SY },	"StartTransition",	app, "active", false	):set_text_visible(true) --regular ON-OFF button

bu = self:add_selector(	{ix+4,iy+10,	12,SY}, "DateCur" )
bu:set_item_text( 1, "1", "2", "3", "4", "5", "6", "7", "8", "9", "10")
bu:set_target_lua( app, "strataDateCur" )
bu:set_method_on_value_change( self, "call_animation", bu )

bu = self:add_selector(	{ix+4,iy+12,	12,SY}, "DateNew" )
bu:set_item_text( 1, "1", "2", "3", "4", "5", "6", "7", "8", "9", "10")
bu:set_target_lua( app, "strataDateNew" )
bu:set_method_on_value_change( self, "call_animation", bu )

end

function meu:call_animation()
    self:print("Call Animation")

if self:notEqual(app.strataDateNew, app.strataDateCur) then
            self:print("VarA is not equal to VarB and it is ".. self:notEqual(app.strataDateNew, app.strataDateCur).. " start animation")
            app.activex = true
        elseif self:notEqual(app.strataDateNew, app.strataDateCur) and app.activex == true then
            self:print("VarA is equal to VarB")
            app.activex = false
        elseif self:notEqual(app.strataDateNew, app.strataDateCur) == false then
            self:print("VarA is equal to VarB")
            app.activex = false
        -- elseif self:notEqual(app.strataDateNew, app.strataDateCur) == false and app.active == false then
        --     self:print("VarA is equal to VarB")
        -- app.active = true
    end
end

----RAMP TIMER
function meu:tick_ramp()
    local progress_step = aaa.time.dt/self.ramp.time
    self.ramp.progress = self.ramp.progress + progress_step
    -- Cap progress at 1
    if self.ramp.progress >= 1 then
        self.ramp.progress = 1
		self.ramp.rampOut = self.ramp.max
    end
    -- -- Set  ramp out value
self.ramp.rampOut = self.ramp.min + ((self.ramp.max - self.ramp.min) * self.ramp.progress)
    -- -- Reset if looping or stop if not
	if self.ramp.progress == 1 and self.ramp.loop_ramp then
    	self.ramp.progress = 0
		self.ramp.rampOut = self.ramp.min
    elseif self.ramp.progress == 1 then
        self.ramp.progress = 0
		--self.ramp.rampOut = self.ramp.min
		self.active = false
    end
end

function meu:notEqual(A, B)
    return A ~= B
end

function meu:update()
--    local ddd = self:notEqual(app.strataDateNew, app.strataDateCur)
--     self:print("Ddd "..ddd.. " and appacativex is "..app.activex)
--     ddd = self.inequalityResult

    local animateOne = self:get_bu_value("Timer Out")
    
    if self.active then
        self:tick_ramp()
    end
    
    if animateOne >= 0.1 and animateOne <= 0.11 then
        app.active2 = true
        self:print("caca "..animateOne)
    end

end








--  function meu:update()

--     -- local eq = self:notEqual(app.strataDateNew, app.strataDateCur)
--     -- self.equalityResult = eq
--     --self:print("DateCur is "..app.strataDateCur.." and DateNew is "..app.strataDateNew.." and TRIG is "..self:notEqual(app.strataDateNew, app.strataDateCur))
--     if self:notEqual(app.strataDateNew, app.strataDateCur) then
--         self:print("VarA is not equal to VarB and it is "..app.strataDateNew )
--         -- app.active = false
--     else
--         self:print("VarA is equal to VarB")
--         app.active = true
--         -- app.active = false
    
    
--      end