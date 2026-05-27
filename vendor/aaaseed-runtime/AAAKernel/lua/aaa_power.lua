--UTIL
aaa.show_file_begin( "aaa_power" )

if not aaa.power then
	aaa.power = {}
	aaa.power.ref = {}
	local ref = aaa.power.ref
	ref.obj = aaa.obj.get_from_top_by_class( "power_master" )
		ref.active			= param.get_ref( ref.obj, "active" )
		ref.plugged			= param.get_ref( ref.obj, "plugged" )
		ref.charging		= param.get_ref( ref.obj, "charging" )
		ref.battery_level	= param.get_ref( ref.obj, "battery_level" )
		ref.time_left		= param.get_ref( ref.obj, "time_left" )
		ref.time_max		= param.get_ref( ref.obj, "time_max" )
		ref.state_plugged		= {}
			ref.state_plugged.trig		= 	param.get_ref( ref.obj, "cpu_state_plugged_trig" )
			ref.state_plugged.min		= 	param.get_ref( ref.obj, "cpu_state_plugged_min" )
			ref.state_plugged.max		= 	param.get_ref( ref.obj, "cpu_state_plugged_max" )
		ref.state_battery		= {}
			ref.state_battery.trig		= 	param.get_ref( ref.obj, "cpu_state_battery_trig" )
			ref.state_battery.min		= 	param.get_ref( ref.obj, "cpu_state_battery_min" )
			ref.state_battery.max		= 	param.get_ref( ref.obj, "cpu_state_battery_max" )
		ref.bright				= {}
			ref.bright.trig				= 	param.get_ref( ref.obj, "display_brightness_trig" )
			ref.bright.plugged			= 	param.get_ref( ref.obj, "display_brightness_plugged" )
			ref.bright.battery			= 	param.get_ref( ref.obj, "display_brightness_battery" )
end

function aaa.power.update()
	local self = aaa.power
	local ref = self.ref
	self.b_plugged		= param.get_bool( ref.plugged )
	self.b_charging		= param.get_bool( ref.charging )
	self.battery_level	= math.floor( param.get( ref.battery_level ) * 1000 ) / 10
	self.time_left		= param.get( ref.time_left )
	self.time_max		= param.get( ref.time_max )
end
function aaa.power.get_str()
	local self = aaa.power
	self.update()
	local str = "Power : "
	if self.b_plugged then		str = str.."Plugged " end
	if self.b_charging then		str = str.."Charging " end
	str = str..self.battery_level.."% "
	if self.time_left > 0 then
		local mins = math.floor( self.time_left / 60 )
		str = str..mins.." mins left"
	end
	return str
end
function aaa.power.print()
	aaa.print( aaa.power.get_str() )
end
function aaa.power.set_battery_cpu( min, max )
	aaa.print_inverse( "Set Cpu on Battery [ "..min.." , "..max.." ]" )
	local self = aaa.power
	local ref = self.ref.state_battery
	param.set( ref.min, min )
	param.set( ref.max, max )
	param.set( ref.trig, true )
end
function aaa.power.set_brightness_battery( bright )
	aaa.print_inverse( "Set Brightness on Battery to "..bright )
	local self = aaa.power
	local ref = self.ref.bright
	param.set( ref.battery, bright )
	param.set( ref.trig, true )
end
function aaa.power.set_plugged_cpu( min, max )
	aaa.print_inverse( "Set Cpu Plugged [ "..min.." , "..max.." ]" )
	local self = aaa.power
	local ref = self.ref.state_plugged
	param.set( ref.min, min )
	param.set( ref.max, max )
	param.set( ref.trig, true )
end
function aaa.power.set_brightness_plugged( bright )
	aaa.print_inverse( "Set Brightness Plugged to "..bright )
	local self = aaa.power
	local ref = self.ref.bright
	param.set( ref.plugged, bright )
	param.set( ref.trig, true )
end

aaa.show_file_end( "aaa_power" )
