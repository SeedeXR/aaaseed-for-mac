--test
--toto



upstream_polygon = upstream_polygon or aaa.obj.get_by_name( "Module/Dev/Door/fx_a.bdd_polygon" )
--prepare ref to speed accees
upstream_p1x = param.get_ref( upstream_polygon, "point_01_u" )
upstream_p1y = param.get_ref( upstream_polygon, "point_01_v" )
upstream_p2x = param.get_ref( upstream_polygon, "point_02_u" )
upstream_p2y = param.get_ref( upstream_polygon, "point_02_v" )
upstream_p3x = param.get_ref( upstream_polygon, "point_03_u" )
upstream_p3y = param.get_ref( upstream_polygon, "point_03_v" )
upstream_p4x = param.get_ref( upstream_polygon, "point_04_u" )
upstream_p4y = param.get_ref( upstream_polygon, "point_04_v" )

function upstream_door_build ()
  -- example to get trax freq
  -- param.get( aaa.obj.get_caller(), "freq" )

  for i=10000,1,-1 do
  --left
  local tmp = aaa.tin[1] + aaa.tin[5]
  param.set( upstream_p1x, tmp)
  param.set( upstream_p2x, tmp)
  --right
  tmp = aaa.tin[1] + aaa.tin[3]file
  param.set( upstream_p3x, tmp)
  param.set( upstream_p4x, tmp)

  --right top then bottom
  tmp = aaa.tin[2] + aaa.tin[4]
  param.set( upstream_p3y, tmp)
  param.set( upstream_p4y, tmp+aaa.tin[6])
  --left top then bottom
  tmp = aaa.tin[2] + aaa.tin[5] * aaa.tin[4]/aaa.tin[3]
  param.set( upstream_p2y, tmp)
  param.set( upstream_p1y, tmp+aaa.tin[6])
  end

  return 0
end