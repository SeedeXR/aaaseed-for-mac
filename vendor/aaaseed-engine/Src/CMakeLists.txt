
###################################################################################################
# NOTE:
#	OPTION_BUILD_USE_VLD is defined in ROOT\CMakeLists.txt
###################################################################################################

# Project usefull vars.
###################################################################################################
set( SOURCE_FILES )
set( executable_name AAASeed )


# Parse files in project.
###################################################################################################

#################################################
# Boids
#################################################
set(BOIDS_SOURCE_FILES
		# System 
		${CMAKE_SOURCE_DIR}/src/boids/meca.h
		${CMAKE_SOURCE_DIR}/src/boids/meca.cpp
		${CMAKE_SOURCE_DIR}/src/boids/poid.h
		${CMAKE_SOURCE_DIR}/src/boids/poid.cpp
		)
set( SOURCE_FILES ${SOURCE_FILES} ${BOIDS_SOURCE_FILES} )
source_group("Boid" FILES ${BOIDS_SOURCE_FILES} )
		
		

#################################################
# Code Utils
#################################################
set(CODEUTILS_SOURCE_FILES
		# Buffer 
		${CMAKE_SOURCE_DIR}/src/code_utils/buffer/buffer_blk.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/buffer/buffer_blk.h
		# File
		${CMAKE_SOURCE_DIR}/src/code_utils/file/asc_parser.h
		${CMAKE_SOURCE_DIR}/src/code_utils/file/asc_parser.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/file/dirParser.h
		${CMAKE_SOURCE_DIR}/src/code_utils/file/dirParser.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/file/file_csv.h
		${CMAKE_SOURCE_DIR}/src/code_utils/file/file_csv.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/file/file_hrc.h
		${CMAKE_SOURCE_DIR}/src/code_utils/file/file_hrc.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/file/file_io.h
		${CMAKE_SOURCE_DIR}/src/code_utils/file/file_io.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/file/fname.h
		${CMAKE_SOURCE_DIR}/src/code_utils/file/fname.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/file/aaa_dir_lua.h
		${CMAKE_SOURCE_DIR}/src/code_utils/file/aaa_dir_lua.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/file/aaa_file.h
		${CMAKE_SOURCE_DIR}/src/code_utils/file/aaa_file.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/file/aaa_file_lua.h
		${CMAKE_SOURCE_DIR}/src/code_utils/file/aaa_file_lua.cpp
		# Thread
		# ${CMAKE_SOURCE_DIR}/src/code_utils/thread/our_mutex.h
		# ${CMAKE_SOURCE_DIR}/src/code_utils/thread/our_mutex.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/thread/aaa_thread.h
		${CMAKE_SOURCE_DIR}/src/code_utils/thread/aaa_thread.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/thread/scheduler.h
		${CMAKE_SOURCE_DIR}/src/code_utils/thread/scheduler.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/thread/thread1.cpp
		# Time
		${CMAKE_SOURCE_DIR}/src/code_utils/time/delta_t.h
		${CMAKE_SOURCE_DIR}/src/code_utils/time/delta_t.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/time/ourdate.h
		${CMAKE_SOURCE_DIR}/src/code_utils/time/ourdate.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/time/ourtime.h
		${CMAKE_SOURCE_DIR}/src/code_utils/time/ourtime.cpp
		
		# Main 
		${CMAKE_SOURCE_DIR}/src/code_utils/asc_line.h
		${CMAKE_SOURCE_DIR}/src/code_utils/asc_line.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/bdd_util.h
		${CMAKE_SOURCE_DIR}/src/code_utils/bdd_util.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/Callbacks.h
		${CMAKE_SOURCE_DIR}/src/code_utils/CallbacksAuto.h
		${CMAKE_SOURCE_DIR}/src/code_utils/checksum.h
		${CMAKE_SOURCE_DIR}/src/code_utils/checksum.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/console.h
		${CMAKE_SOURCE_DIR}/src/code_utils/console.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/CPU.h
		${CMAKE_SOURCE_DIR}/src/code_utils/CPU.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/data_2d.h
		${CMAKE_SOURCE_DIR}/src/code_utils/data_2d.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/dll_version.h
		${CMAKE_SOURCE_DIR}/src/code_utils/dll_version.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/err.h
		${CMAKE_SOURCE_DIR}/src/code_utils/err.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/fast_atof.h 
		${CMAKE_SOURCE_DIR}/src/code_utils/fn_types.h
		${CMAKE_SOURCE_DIR}/src/code_utils/gol.h
		${CMAKE_SOURCE_DIR}/src/code_utils/gol.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/gol_lua.h
		${CMAKE_SOURCE_DIR}/src/code_utils/gol_lua.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/id_unique.h
		${CMAKE_SOURCE_DIR}/src/code_utils/id_unique.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/md5.h
		# ${CMAKE_SOURCE_DIR}/src/code_utils/md5c.c
		${CMAKE_SOURCE_DIR}/src/code_utils/md5c.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/memcpy.h
		${CMAKE_SOURCE_DIR}/src/code_utils/memcpy.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/os_version.h
		${CMAKE_SOURCE_DIR}/src/code_utils/os_version.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/aaa_def.h
		${CMAKE_SOURCE_DIR}/src/code_utils/aaa_mem.h
		${CMAKE_SOURCE_DIR}/src/code_utils/aaa_mem.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/aaa_str.h
		${CMAKE_SOURCE_DIR}/src/code_utils/aaa_str.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/aaa_type.h
		${CMAKE_SOURCE_DIR}/src/code_utils/aaa_util.h
		${CMAKE_SOURCE_DIR}/src/code_utils/aaa_util.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/profile.h
		${CMAKE_SOURCE_DIR}/src/code_utils/profile.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/str_utils.h
		${CMAKE_SOURCE_DIR}/src/code_utils/str_utils.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/stream_util.h
		${CMAKE_SOURCE_DIR}/src/code_utils/stream_util.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/strnum.h
		${CMAKE_SOURCE_DIR}/src/code_utils/strnum.cpp
		${CMAKE_SOURCE_DIR}/src/code_utils/time_buf.h
		${CMAKE_SOURCE_DIR}/src/code_utils/time_buf.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${CODEUTILS_SOURCE_FILES})
source_group("Code Utils" FILES ${CODEUTILS_SOURCE_FILES})
		
		
#################################################
# Core
#################################################
set(CORE_SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/core/App.h
	${CMAKE_SOURCE_DIR}/src/core/App.cpp
	${CMAKE_SOURCE_DIR}/src/core/Controller.h 
	${CMAKE_SOURCE_DIR}/src/core/Controller.cpp 
	${CMAKE_SOURCE_DIR}/src/core/Controller_AAA.h 
	${CMAKE_SOURCE_DIR}/src/core/Controller_AAA.cpp 
	${CMAKE_SOURCE_DIR}/src/core/Renderer.h 
	${CMAKE_SOURCE_DIR}/src/core/Renderer.cpp 
	${CMAKE_SOURCE_DIR}/src/core/Renderer_AAA.h 
	${CMAKE_SOURCE_DIR}/src/core/Renderer_AAA.cpp 
	${CMAKE_SOURCE_DIR}/src/core/Runner.h 
	${CMAKE_SOURCE_DIR}/src/core/Runner.cpp 
	)
set(SOURCE_FILES ${SOURCE_FILES} ${CORE_SOURCE_FILES})		
source_group("Core" FILES ${CORE_SOURCE_FILES})
		
		
#################################################
# Draw
#################################################
set(DRAW_SOURCE_FILES
		# Geo
		${CMAKE_SOURCE_DIR}/src/draw/geo/curve_flat.h
		${CMAKE_SOURCE_DIR}/src/draw/geo/curve_flat.cpp
		${CMAKE_SOURCE_DIR}/src/draw/geo/points.h
		${CMAKE_SOURCE_DIR}/src/draw/geo/points.cpp
		${CMAKE_SOURCE_DIR}/src/draw/geo/polyline.h
		${CMAKE_SOURCE_DIR}/src/draw/geo/polyline.cpp
		${CMAKE_SOURCE_DIR}/src/draw/geo/polyline.h
		
		# Main
		${CMAKE_SOURCE_DIR}/src/draw/axe.h
		${CMAKE_SOURCE_DIR}/src/draw/axe.cpp
		${CMAKE_SOURCE_DIR}/src/draw/bind_img.h
		${CMAKE_SOURCE_DIR}/src/draw/bind_img.cpp
		${CMAKE_SOURCE_DIR}/src/draw/Bspfile.h
		${CMAKE_SOURCE_DIR}/src/draw/bspfile.cpp
		${CMAKE_SOURCE_DIR}/src/draw/BspFileStructs.h
		${CMAKE_SOURCE_DIR}/src/draw/Camera.h
		${CMAKE_SOURCE_DIR}/src/draw/camera.cpp
		${CMAKE_SOURCE_DIR}/src/draw/camera_format.h
		${CMAKE_SOURCE_DIR}/src/draw/camera_format.cpp
		${CMAKE_SOURCE_DIR}/src/draw/clip.h
		${CMAKE_SOURCE_DIR}/src/draw/clip.cpp
		${CMAKE_SOURCE_DIR}/src/draw/color.h
		${CMAKE_SOURCE_DIR}/src/draw/color.cpp
		${CMAKE_SOURCE_DIR}/src/draw/colorrnd.h
		${CMAKE_SOURCE_DIR}/src/draw/colorrnd.cpp
		${CMAKE_SOURCE_DIR}/src/draw/dust.h
		${CMAKE_SOURCE_DIR}/src/draw/dust.cpp
		${CMAKE_SOURCE_DIR}/src/draw/face.h
		${CMAKE_SOURCE_DIR}/src/draw/face.cpp
		# ${CMAKE_SOURCE_DIR}/src/draw/feedback.h
		# ${CMAKE_SOURCE_DIR}/src/draw/feedback.cpp
		${CMAKE_SOURCE_DIR}/src/draw/fog.h
		${CMAKE_SOURCE_DIR}/src/draw/fog.cpp
		${CMAKE_SOURCE_DIR}/src/draw/fracglut.h
		${CMAKE_SOURCE_DIR}/src/draw/fracglut.cpp
		${CMAKE_SOURCE_DIR}/src/draw/gol_buffer.h
		${CMAKE_SOURCE_DIR}/src/draw/gol_buffer.cpp
		${CMAKE_SOURCE_DIR}/src/draw/guf.h
		${CMAKE_SOURCE_DIR}/src/draw/guf.cpp
		${CMAKE_SOURCE_DIR}/src/draw/leveldata.h
		${CMAKE_SOURCE_DIR}/src/draw/leveldata.cpp
		${CMAKE_SOURCE_DIR}/src/draw/light.h
		${CMAKE_SOURCE_DIR}/src/draw/light.cpp
		${CMAKE_SOURCE_DIR}/src/draw/lights.h
		${CMAKE_SOURCE_DIR}/src/draw/lights.cpp
		${CMAKE_SOURCE_DIR}/src/draw/map.h
		${CMAKE_SOURCE_DIR}/src/draw/map.cpp
		${CMAKE_SOURCE_DIR}/src/draw/mat.h
		${CMAKE_SOURCE_DIR}/src/draw/mat.cpp
		${CMAKE_SOURCE_DIR}/src/draw/mesh.h
		${CMAKE_SOURCE_DIR}/src/draw/mesh.cpp
		${CMAKE_SOURCE_DIR}/src/draw/model.h
		${CMAKE_SOURCE_DIR}/src/draw/model.cpp
		${CMAKE_SOURCE_DIR}/src/draw/multiple.h
		${CMAKE_SOURCE_DIR}/src/draw/multiple.cpp
		${CMAKE_SOURCE_DIR}/src/draw/NSight.cpp
		${CMAKE_SOURCE_DIR}/src/draw/NSightEvents.h
		${CMAKE_SOURCE_DIR}/src/draw/aaa_glut.h
		${CMAKE_SOURCE_DIR}/src/draw/aaa_glut.cpp
		${CMAKE_SOURCE_DIR}/src/draw/picked.h
		${CMAKE_SOURCE_DIR}/src/draw/picked.cpp
		${CMAKE_SOURCE_DIR}/src/draw/picking.h
		${CMAKE_SOURCE_DIR}/src/draw/picking.cpp
		${CMAKE_SOURCE_DIR}/src/draw/render.h
		${CMAKE_SOURCE_DIR}/src/draw/render.cpp
		# ${CMAKE_SOURCE_DIR}/src/draw/screen_def.h
		# ${CMAKE_SOURCE_DIR}/src/draw/screen_def.cpp
		${CMAKE_SOURCE_DIR}/src/draw/seedcam.h
		${CMAKE_SOURCE_DIR}/src/draw/seedcam.cpp
		${CMAKE_SOURCE_DIR}/src/draw/seeddraw.h
		${CMAKE_SOURCE_DIR}/src/draw/seeddraw.cpp
		${CMAKE_SOURCE_DIR}/src/draw/box.h
		${CMAKE_SOURCE_DIR}/src/draw/box.cpp
		${CMAKE_SOURCE_DIR}/src/draw/shape.h
		${CMAKE_SOURCE_DIR}/src/draw/shape.cpp
		${CMAKE_SOURCE_DIR}/src/draw/ship.h
		${CMAKE_SOURCE_DIR}/src/draw/ship.cpp
		${CMAKE_SOURCE_DIR}/src/draw/stencil.h
		${CMAKE_SOURCE_DIR}/src/draw/stencil.cpp
		${CMAKE_SOURCE_DIR}/src/draw/stereo.h
		${CMAKE_SOURCE_DIR}/src/draw/stereo.cpp
		${CMAKE_SOURCE_DIR}/src/draw/tex.h
		${CMAKE_SOURCE_DIR}/src/draw/tex.cpp
		${CMAKE_SOURCE_DIR}/src/draw/tex_anim.h
		${CMAKE_SOURCE_DIR}/src/draw/tex_anim.cpp
		${CMAKE_SOURCE_DIR}/src/draw/texture.h
		${CMAKE_SOURCE_DIR}/src/draw/texture.cpp
		${CMAKE_SOURCE_DIR}/src/draw/vertex_data.h
		${CMAKE_SOURCE_DIR}/src/draw/vertex_data.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${DRAW_SOURCE_FILES})	
source_group("Draw" FILES ${DRAW_SOURCE_FILES})



#################################################
# Event
#################################################
set(SOURCE_FILE_EVENT
	${CMAKE_SOURCE_DIR}/src/evt/Delegate.h 
	${CMAKE_SOURCE_DIR}/src/evt/Event.h 
	${CMAKE_SOURCE_DIR}/src/evt/Event.cpp 
	${CMAKE_SOURCE_DIR}/src/evt/Utils.h 
	)
set(SOURCE_FILES ${SOURCE_FILES} ${SOURCE_FILE_EVENT})	
source_group("Event" FILES ${SOURCE_FILE_EVENT})
		
		
#################################################
# Exemple
#################################################
set(EXAMPLE_SOURCE_FILES
		# ${CMAKE_SOURCE_DIR}/src/exemple/Boid8ex.cpp
		# ${CMAKE_SOURCE_DIR}/src/exemple/BoidNamedObject.cpp
		# ${CMAKE_SOURCE_DIR}/src/exemple/BoidObstacle.cpp
		)
# set(SOURCE_FILES ${SOURCE_FILES} ${EXAMPLE_SOURCE_FILES})	
# source_group("Example" FILES ${EXAMPLE_SOURCE_FILES})
		
		
#################################################
# FBO
#################################################
set(FBO_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/FBO/fbo.h
		${CMAKE_SOURCE_DIR}/src/FBO/fbo.cpp
		${CMAKE_SOURCE_DIR}/src/FBO/framebufferobject.h
		${CMAKE_SOURCE_DIR}/src/FBO/framebufferobject.cpp
		${CMAKE_SOURCE_DIR}/src/FBO/render_buffer.h
		${CMAKE_SOURCE_DIR}/src/FBO/render_buffer.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${FBO_SOURCE_FILES})	
source_group("FBO" FILES ${FBO_SOURCE_FILES})
		
		
#################################################
# FFGL
#################################################
set(FFGL_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/FFGL/FFGLPluginInstance.h
		${CMAKE_SOURCE_DIR}/src/FFGL/FFGLPluginInstance.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${FFGL_SOURCE_FILES})	
source_group("FFGL" FILES ${FFGL_SOURCE_FILES})
		
		
#################################################
# FTGL
#################################################
set(FTGL_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/ftgl/aaa_ftgl.h
		${CMAKE_SOURCE_DIR}/src/ftgl/aaa_ftgl.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${FTGL_SOURCE_FILES})	
source_group("FTGL" FILES ${FTGL_SOURCE_FILES})
		
		
#################################################
# Gypsy
#################################################
set(GYPSY_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/Gypsy/Gypsy.h
		${CMAKE_SOURCE_DIR}/src/Gypsy/Matrix.h
		${CMAKE_SOURCE_DIR}/src/Gypsy/wrap_gypsy.h
		${CMAKE_SOURCE_DIR}/src/Gypsy/wrap_gypsy.cpp
		${CMAKE_SOURCE_DIR}/src/Gypsy/wrap_gypsy_api_Macro.h
		)
set(SOURCE_FILES ${SOURCE_FILES} ${GYPSY_SOURCE_FILES})	
source_group("Gypsy" FILES ${GYPSY_SOURCE_FILES})
		
		
#################################################
# Image
#################################################
set(IMAGE_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/image/bitmap_convert.h
		${CMAKE_SOURCE_DIR}/src/image/bitmap_convert.cpp
		${CMAKE_SOURCE_DIR}/src/image/color_space.h
		${CMAKE_SOURCE_DIR}/src/image/color_space_lua.h 
		${CMAKE_SOURCE_DIR}/src/image/color_space_lua.cpp 
		${CMAKE_SOURCE_DIR}/src/image/convert.h
		${CMAKE_SOURCE_DIR}/src/image/image_fill.cpp
		${CMAKE_SOURCE_DIR}/src/image/img.h
		${CMAKE_SOURCE_DIR}/src/image/img.cpp
		${CMAKE_SOURCE_DIR}/src/image/img_getset.cpp 
		${CMAKE_SOURCE_DIR}/src/image/img_jpg.h
		${CMAKE_SOURCE_DIR}/src/image/img_jpg.cpp
		${CMAKE_SOURCE_DIR}/src/image/img_lua.h
		${CMAKE_SOURCE_DIR}/src/image/img_lua.cpp
		${CMAKE_SOURCE_DIR}/src/image/img_sgi.h
		${CMAKE_SOURCE_DIR}/src/image/img_sgi.cpp
		${CMAKE_SOURCE_DIR}/src/image/img_tga.h
		${CMAKE_SOURCE_DIR}/src/image/img_tga.cpp
		${CMAKE_SOURCE_DIR}/src/image/img_util.h
		${CMAKE_SOURCE_DIR}/src/image/img_xnview.h
		${CMAKE_SOURCE_DIR}/src/image/img_xnview.cpp
		${CMAKE_SOURCE_DIR}/src/image/img_yuv.h
		${CMAKE_SOURCE_DIR}/src/image/img_yuv.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${IMAGE_SOURCE_FILES})	
source_group("Image" FILES ${IMAGE_SOURCE_FILES})
		
		
#################################################
# Infrastructure
#################################################
set(INFRASTRUCTURE_SOURCE_FILES
		# Bind
		${CMAKE_SOURCE_DIR}/src/infrastructure/bind/bind.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/bind/bind.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/bind/list_fix.h
		#${CMAKE_SOURCE_DIR}/src/infrastructure/bind/list_fix.cpp
		# Data
		${CMAKE_SOURCE_DIR}/src/infrastructure/data/aaa_global.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/data/aaa_global.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/data/data_src.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/data/data_src.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/data/datacube.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/data/datacube.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/data/datagrid.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/data/datagrid.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/data/dataline.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/data/dataline.cpp
		# Factory
		${CMAKE_SOURCE_DIR}/src/infrastructure/factory/factory.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/factory/factory.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/factory/factory_menu.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/factory/factory_menu.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/factory/registry.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/factory/registry.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/factory/registry_generic.h
		# Layer
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/app.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/app.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/layer.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/layer.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/layer_att.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/layer_att.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/layer_lua.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/layer_lua.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/layers.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/layers.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/layers_att.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/layers_att.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/layers_lua.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/layers_lua.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/module.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/module.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/modules.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/layer/modules.cpp
		# Obj
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/command.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/command.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/node.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/node.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/node_list_ui.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/obj_ui_array.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/obj.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/obj.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/obj_lua.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/obj_lua.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/obj_ui.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/obj_ui.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/aaa_list.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/root.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/root.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/state.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/obj/state.cpp
		# Param
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/connex.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/connex.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/param.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/param.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/param_class_branch.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/param_class_branch.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/param_data.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/param_data.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/param_def.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/param_draw.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/param_draw.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/param_focus.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/param_focus.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/param_lua.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/param_lua.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/param_st.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/param_st.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/params.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/params.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/Trax.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/trax.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/trax_name.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/trax_name.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/traxs.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/param/traxs.cpp
		
		# Main
		${CMAKE_SOURCE_DIR}/src/infrastructure/Averager.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/averager.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/bind_text.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/bind_text.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/cameras.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/cameras.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/clipboard.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/clipboard.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/compute_master.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/compute_master.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/flux_filter.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/flux_filter.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/namer.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/namer.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/aaa_const.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/aaa_window.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/aaa_window.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/seed_stop.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/seed_stop.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/seedfile.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/seedfile.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/viewport.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/viewport.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/watchdog.h
		${CMAKE_SOURCE_DIR}/src/infrastructure/watchdog.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${INFRASTRUCTURE_SOURCE_FILES})	
source_group("Infrastructure" FILES ${INFRASTRUCTURE_SOURCE_FILES})
		
		
#################################################
# Language -> LUA
#################################################
set(LUA_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/language/lua/aaalua.h
		${CMAKE_SOURCE_DIR}/src/language/lua/aaalua.cpp
		${CMAKE_SOURCE_DIR}/src/language/lua/aaalua_debug.h
		${CMAKE_SOURCE_DIR}/src/language/lua/aaalua_debug.cpp
		${CMAKE_SOURCE_DIR}/src/language/lua/aaalua_draw.h
		${CMAKE_SOURCE_DIR}/src/language/lua/aaalua_draw.cpp
		${CMAKE_SOURCE_DIR}/src/language/lua/aaalua_glue.h
		${CMAKE_SOURCE_DIR}/src/language/lua/aaalua_glue.cpp
		${CMAKE_SOURCE_DIR}/src/language/lua/aaalua_util.h
		${CMAKE_SOURCE_DIR}/src/language/lua/aaalua_util.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/tos_lua.cpp
		${CMAKE_SOURCE_DIR}/src/infrastructure/tos_lua.h
		)
set(SOURCE_FILES ${SOURCE_FILES} ${LUA_SOURCE_FILES})	
source_group("LUA" FILES ${LUA_SOURCE_FILES})
		
		
#################################################
# Lib Wrappers
#################################################
set(LIBWRAPPERS_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_class.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_loader.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_loader.cpp
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_loader_api_Macro.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_bass.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_bass.cpp
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_bass_asio.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_bass_asio.cpp
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_bass_api_Macro.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_bird.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_bird.cpp
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_bird_api_Macro.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_cg.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_cg.cpp
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_ffmpeg.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_ffmpeg.cpp
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_fftw3.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_fftw3.cpp
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_fftw3_api_Macro.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_fglove.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_fglove.cpp
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_fglove_api_Macro.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_ftdi.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_ftdi.cpp
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_ftdi_api_Macro.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_gfl.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_gfl.cpp
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_gfl_api_Macro.h
		# ${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_pqlabs.h
		# ${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_pqlabs.cpp
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_pqlabs_api_Macro.h
		# ${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_qlib.h
		# ${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_qlib.cpp
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_qlib_api_Macro.h
		# ${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_qt.h
		# ${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_qt.cpp
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_qt_api_Macro.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_touchco.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_touchco.cpp
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_touchco_api_Macro.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_wke.h
		${CMAKE_SOURCE_DIR}/src/lib_wrappers/wrap_wke_api_Macro.h
		)
set(SOURCE_FILES ${SOURCE_FILES} ${LIBWRAPPERS_SOURCE_FILES})	
source_group("Lib Wrappers" FILES ${LIBWRAPPERS_SOURCE_FILES})
		
		
#################################################
# License
#################################################
set(LICENSE_SOURCE_FILES
		# Resources
		${OPTION_BUILD_PATH_RESSOURCES}/resource.h
		# ${OPTION_BUILD_PATH_RESSOURCES}/Seed.rc
		# License
		${CMAKE_SOURCE_DIR}/src/license/info.h
		${CMAKE_SOURCE_DIR}/src/license/info.cpp
		${CMAKE_SOURCE_DIR}/src/license/license.h
		${CMAKE_SOURCE_DIR}/src/license/license.cpp
		${CMAKE_SOURCE_DIR}/src/license/license_ui.h
		${CMAKE_SOURCE_DIR}/src/license/license_ui.cpp
		${CMAKE_SOURCE_DIR}/src/license/LogoMaa.h
		${CMAKE_SOURCE_DIR}/src/license/LogoMaa.cpp
		${CMAKE_SOURCE_DIR}/src/license/version.h
		${CMAKE_SOURCE_DIR}/src/license/version.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${LICENSE_SOURCE_FILES})	
source_group("License" FILES ${LICENSE_SOURCE_FILES})
		
		
#################################################
# Math
#################################################
set(MATH_SOURCE_FILES
		# Function 1D
		${CMAKE_SOURCE_DIR}/src/math/fn1d/fn1d.h
		${CMAKE_SOURCE_DIR}/src/math/fn1d/fn1d.cpp
		${CMAKE_SOURCE_DIR}/src/math/fn1d/fn1d_fm.h
		${CMAKE_SOURCE_DIR}/src/math/fn1d/fn1d_fm.cpp
		${CMAKE_SOURCE_DIR}/src/math/fn1d/fn1d_pp.h
		${CMAKE_SOURCE_DIR}/src/math/fn1d/fn1d_pp.cpp
		
		# Main
		${CMAKE_SOURCE_DIR}/src/math/billboard.h
		${CMAKE_SOURCE_DIR}/src/math/billboard.cpp
		${CMAKE_SOURCE_DIR}/src/math/curves.h
		${CMAKE_SOURCE_DIR}/src/math/curves.cpp
		${CMAKE_SOURCE_DIR}/src/math/gainbias.h
		${CMAKE_SOURCE_DIR}/src/math/math_lua.h
		${CMAKE_SOURCE_DIR}/src/math/math_lua.cpp
		${CMAKE_SOURCE_DIR}/src/math/matrix.h
		${CMAKE_SOURCE_DIR}/src/math/matrix.cpp
		#${CMAKE_SOURCE_DIR}/src/math/noiscurv.cpp # Do not include in build
		${CMAKE_SOURCE_DIR}/src/math/noisdist.h
		${CMAKE_SOURCE_DIR}/src/math/noisdist.cpp
		${CMAKE_SOURCE_DIR}/src/math/noisturb.h
		${CMAKE_SOURCE_DIR}/src/math/noisturb.cpp
		${CMAKE_SOURCE_DIR}/src/math/aaa_math.h
		${CMAKE_SOURCE_DIR}/src/math/aaa_math.cpp
		${CMAKE_SOURCE_DIR}/src/math/aaa_quat.cpp
		${CMAKE_SOURCE_DIR}/src/math/aaa_rand.h
		${CMAKE_SOURCE_DIR}/src/math/aaa_rand.cpp
		# ${CMAKE_SOURCE_DIR}/src/math/predicates.c
		# ${CMAKE_SOURCE_DIR}/src/math/quaternions.h
		# ${CMAKE_SOURCE_DIR}/src/math/quaternions.cpp
		${CMAKE_SOURCE_DIR}/src/math/rand.h
		${CMAKE_SOURCE_DIR}/src/math/rand.cpp
		${CMAKE_SOURCE_DIR}/src/math/spots.h
		${CMAKE_SOURCE_DIR}/src/math/spots.cpp
		${CMAKE_SOURCE_DIR}/src/math/TArea.h 
		${CMAKE_SOURCE_DIR}/src/math/TBox.h 
		${CMAKE_SOURCE_DIR}/src/math/TMath.h 
		${CMAKE_SOURCE_DIR}/src/math/TMath.cpp 
		${CMAKE_SOURCE_DIR}/src/math/TMatrix.h 
		${CMAKE_SOURCE_DIR}/src/math/TMatrix22.h 
		${CMAKE_SOURCE_DIR}/src/math/TMatrix33.h 
		${CMAKE_SOURCE_DIR}/src/math/TMatrix44.h 
		${CMAKE_SOURCE_DIR}/src/math/TMatrixAffine2.h 
		${CMAKE_SOURCE_DIR}/src/math/TMatrixAlgo.h 
		${CMAKE_SOURCE_DIR}/src/math/TQuaternion.h 
		${CMAKE_SOURCE_DIR}/src/math/TRay.h 
		${CMAKE_SOURCE_DIR}/src/math/TRect.h 
		${CMAKE_SOURCE_DIR}/src/math/TVector.h 
		${CMAKE_SOURCE_DIR}/src/math/v.h
		${CMAKE_SOURCE_DIR}/src/math/v.cpp
		${CMAKE_SOURCE_DIR}/src/math/v_xna.h
		${CMAKE_SOURCE_DIR}/src/math/v_xna.cpp
		${CMAKE_SOURCE_DIR}/src/math/VoronoiDiagramGenerator.h
		${CMAKE_SOURCE_DIR}/src/math/VoronoiDiagramGenerator.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${MATH_SOURCE_FILES})	
source_group("Math" FILES ${MATH_SOURCE_FILES})
		
		
#################################################
# Media -> Sound
#################################################
set(MEDIASOUND_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/media/sound/snd_input.h
		${CMAKE_SOURCE_DIR}/src/media/sound/snd_input.cpp
		${CMAKE_SOURCE_DIR}/src/media/sound/snd_input_bass.h
		${CMAKE_SOURCE_DIR}/src/media/sound/snd_input_bass.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/sound/snd_input_bassasio.h
		# ${CMAKE_SOURCE_DIR}/src/media/sound/snd_input_bassasio.cpp
		${CMAKE_SOURCE_DIR}/src/media/sound/snd_input_directsound.h
		${CMAKE_SOURCE_DIR}/src/media/sound/snd_input_directsound.cpp
		${CMAKE_SOURCE_DIR}/src/media/sound/snd_player.h
		${CMAKE_SOURCE_DIR}/src/media/sound/snd_player.cpp
		${CMAKE_SOURCE_DIR}/src/media/sound/snd_player_bass.h
		${CMAKE_SOURCE_DIR}/src/media/sound/snd_player_bass.cpp
		${CMAKE_SOURCE_DIR}/src/media/sound/snd_player_directsound.h
		${CMAKE_SOURCE_DIR}/src/media/sound/snd_player_directsound.cpp
		${CMAKE_SOURCE_DIR}/src/media/sound/sound_bass.h
		${CMAKE_SOURCE_DIR}/src/media/sound/sound_bass.cpp
		${CMAKE_SOURCE_DIR}/src/media/sound/sound_bass_asio.h
		${CMAKE_SOURCE_DIR}/src/media/sound/sound_bass_asio.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/sound/sound_capture.h
		# ${CMAKE_SOURCE_DIR}/src/media/sound/sound_capture.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/sound/sound_capture_bass.h
		# ${CMAKE_SOURCE_DIR}/src/media/sound/sound_capture_bass.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/sound/sound_capture_bassasio.h
		# ${CMAKE_SOURCE_DIR}/src/media/sound/sound_capture_bassasio.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/sound/sound_capture_directsound.h
		# ${CMAKE_SOURCE_DIR}/src/media/sound/sound_capture_directsound.cpp
		${CMAKE_SOURCE_DIR}/src/media/sound/sound_directsound.h
		${CMAKE_SOURCE_DIR}/src/media/sound/sound_directsound.cpp
		${CMAKE_SOURCE_DIR}/src/media/sound/sound_input.h
		${CMAKE_SOURCE_DIR}/src/media/sound/sound_input.cpp
		${CMAKE_SOURCE_DIR}/src/media/sound/sound_new.h
		${CMAKE_SOURCE_DIR}/src/media/sound/sound_new.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/sound/sound_player.h
		# ${CMAKE_SOURCE_DIR}/src/media/sound/sound_player.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/sound/sound_player_directsound.h
		# ${CMAKE_SOURCE_DIR}/src/media/sound/sound_player_directsound.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${MEDIASOUND_SOURCE_FILES})	
source_group("Media Sound" FILES ${MEDIASOUND_SOURCE_FILES})
		
		
#################################################
# Media -> Video -> Capture
#################################################
set(MEDIAVIDEOCAPTURE_SOURCE_FILES
		# Capture -> Kinect -> Libusb
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/libusb/include/usb.h
		# Capture -> Kinect
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/capture_kinect.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/capture_kinect.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/init.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/Kinect-Driver.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/Kinect-FrameInput.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/Kinect-Utility.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/Kinect-Utility.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/Kinect-win32.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/Kinect-win32.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/Kinect-win32-internal.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/wrap_kinect.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/wrap_kinect.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/wrap_kinect_api_Macro.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/wrap_libusb.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/wrap_libusb.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/Kinect/wrap_libusb_api_Macro.h
		# uEye
		${CMAKE_SOURCE_DIR}/src/media/video/capture/uEye/capture_ueye.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/uEye/capture_ueye.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/uEye/Dynamic_uEye_api.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/uEye/Dynamic_uEye_api.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/uEye/DynamicuEyeTools.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/uEye/DynamicuEyeTools.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/uEye/uEye.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/uEye/uEye_api_Macro.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/uEye/ueye_deprecated.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/uEye/uEye_tools.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/uEye/uEyeCaptureInterface.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/uEye/version.h
		# Capture
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_censys.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_censys.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_censys_bumblebee.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_censys_bumblebee.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_censys_tracking.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_censys_tracking.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_dshow.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_dshow.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_flycapture.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_flycapture.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_flycapture2.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_flycapture2.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_ps3_eye.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_ps3_eye.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_triclops.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_triclops.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_vfw.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_vfw.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_video.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/capture_video.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/ptgrey_utils.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/ptgrey_utils.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_censys.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_censys.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_censys_api_Macro.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_digiclops.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_digiclops.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_digiclops_api_Macro.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_PGRFlyCapture.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_PGRFlyCapture.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_PGRFlyCaptureGui.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_PGRFlyCaptureGui.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_PS3EyeLib.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_PS3EyeLib.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_PS3EyeMulticam.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_PS3EyeMulticam.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_ps3eyemulticam_api_Macro.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_triclops.h
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_triclops.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/capture/wrap_triclops_api_Macro.h
		)
set(SOURCE_FILES ${SOURCE_FILES} ${MEDIAVIDEOCAPTURE_SOURCE_FILES})	
source_group("Media Video Capture" FILES ${MEDIAVIDEOCAPTURE_SOURCE_FILES})
		
		
#################################################
# Media -> Video
#################################################
set(MEDIAVIDEO_SOURCE_FILES
		# # Deinterlace -> Leak Kernel
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/CopyLine_C.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/CopyLine_MMX.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/Deinterlace_0.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/DeinterlaceLine_1.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/DeinterlaceLine_2_C.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/DeinterlaceLine_2_MMX.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/DeinterlaceLoad.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/MotionMask_YUY2+RGB_0.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/MotionMask_YV12_0.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/MotionMaskLine_RGB32_2_C.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/MotionMaskLine_RGB32_2_MMX.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/MotionMaskLine_YUY2_2_C.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/MotionMaskLine_YUY2_2_MMX.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/MotionMaskLine_YUY2+RGB_1.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/MotionMaskLine_YV12_1.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/MotionMaskLine_YV12_2_C.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/leak_kernel/MotionMaskLine_YV12_2_MMX.cpp
		# Deinterlace
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/deinterlace_area.h
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/deinterlace_area.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/deinterlace_bob.h
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/deinterlace_bob.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/deinterlace_kernel.h
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/deinterlace_kernel.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/deinterlace_leak_kernel.h
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/deinterlace_leak_kernel.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/deinterlace_smart.h
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/deinterlace_smart.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/deinterlace_smartbob.h
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/deinterlace_smartbob.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/deinterlace_smooth.h
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/deinterlace_smooth.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/deinterlace_weave.h
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace/deinterlace_weave.cpp
		
		# Main
		${CMAKE_SOURCE_DIR}/src/media/video/bitmap.h
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace.h
		${CMAKE_SOURCE_DIR}/src/media/video/deinterlace.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/directshow_util.h
		${CMAKE_SOURCE_DIR}/src/media/video/directshow_util.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/ds_avi.h
		${CMAKE_SOURCE_DIR}/src/media/video/ds_avi.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/file_flux.h
		${CMAKE_SOURCE_DIR}/src/media/video/file_flux.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/image_flux.h
		${CMAKE_SOURCE_DIR}/src/media/video/image_flux.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/mov_ffmpeg.h
		${CMAKE_SOURCE_DIR}/src/media/video/mov_ffmpeg.cpp
		# ${CMAKE_SOURCE_DIR}/src/media/video/mov_qt.h
		# ${CMAKE_SOURCE_DIR}/src/media/video/mov_qt.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/movie_ms.h
		${CMAKE_SOURCE_DIR}/src/media/video/movie_ms.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/aaa_bitmap.h
		${CMAKE_SOURCE_DIR}/src/media/video/aaa_bitmap.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/aaa_video.h
		${CMAKE_SOURCE_DIR}/src/media/video/aaa_video.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/pixel_format.h
		${CMAKE_SOURCE_DIR}/src/media/video/pixel_format.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/tex_video.h
		${CMAKE_SOURCE_DIR}/src/media/video/tex_video.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/tex_video_master.h
		${CMAKE_SOURCE_DIR}/src/media/video/tex_video_master.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/texture_flux_master.h
		${CMAKE_SOURCE_DIR}/src/media/video/texture_flux_master.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/TextureBuffer.h
		${CMAKE_SOURCE_DIR}/src/media/video/TextureBuffer.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/vbl.h
		${CMAKE_SOURCE_DIR}/src/media/video/vbl.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/video_player.h
		${CMAKE_SOURCE_DIR}/src/media/video/video_player.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/win_avi.h
		${CMAKE_SOURCE_DIR}/src/media/video/win_avi.cpp
		${CMAKE_SOURCE_DIR}/src/media/video/Win_DV.h
		${CMAKE_SOURCE_DIR}/src/media/video/Win_DV.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${MEDIAVIDEO_SOURCE_FILES})	
source_group("Media Video" FILES ${MEDIAVIDEO_SOURCE_FILES})
		
		
#################################################
# MOCAP
#################################################
set(MOCAP_SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/mocap/Animator.h
	${CMAKE_SOURCE_DIR}/src/mocap/Animator.cpp
	${CMAKE_SOURCE_DIR}/src/mocap/BVHFormat.h
	${CMAKE_SOURCE_DIR}/src/mocap/BVHFormat.cpp
	)
set(SOURCE_FILES ${SOURCE_FILES} ${MOCAP_SOURCE_FILES})	
source_group("Mocap" FILES ${MOCAP_SOURCE_FILES})
		
		
#################################################
# MSA
#################################################
set(MSA_SOURCE_FILES
		# Core -> Cinder Lite
		${CMAKE_SOURCE_DIR}/src/MSA/MSACore/cinder-lite/ChanTraits.h
		${CMAKE_SOURCE_DIR}/src/MSA/MSACore/cinder-lite/CinderMath.h
		${CMAKE_SOURCE_DIR}/src/MSA/MSACore/cinder-lite/Color.h
		${CMAKE_SOURCE_DIR}/src/MSA/MSACore/cinder-lite/Color.h
		${CMAKE_SOURCE_DIR}/src/MSA/MSACore/cinder-lite/Vector.h
		# Core 
		${CMAKE_SOURCE_DIR}/src/MSA/MSACore/MSACore.h
		${CMAKE_SOURCE_DIR}/src/MSA/MSACore/MSACore-AAASeed.h
		${CMAKE_SOURCE_DIR}/src/MSA/MSACore/MSACore-Cinder.h
		${CMAKE_SOURCE_DIR}/src/MSA/MSACore/MSACoreCommon.h
		${CMAKE_SOURCE_DIR}/src/MSA/MSACore/MSACoreGL.h
		# ${CMAKE_SOURCE_DIR}/src/MSA/MSACore/MSACoreGL.cpp
		${CMAKE_SOURCE_DIR}/src/MSA/MSACore/MSACoreMath.h
		${CMAKE_SOURCE_DIR}/src/MSA/MSACore/MSACore-OF.h
		# Fluid
		${CMAKE_SOURCE_DIR}/src/MSA/MSAFluid/MSAFluid.h
		${CMAKE_SOURCE_DIR}/src/MSA/MSAFluid/MSAFluidDrawerBase.h
		${CMAKE_SOURCE_DIR}/src/MSA/MSAFluid/MSAFluidDrawerBase.cpp
		${CMAKE_SOURCE_DIR}/src/MSA/MSAFluid/MSAFluidDrawerGl-AAASeed.h
		${CMAKE_SOURCE_DIR}/src/MSA/MSAFluid/MSAFluidDrawerGl-AAASeed.cpp
		# ${CMAKE_SOURCE_DIR}/src/MSA/MSAFluid/MSAFluidDrawerGl-OF.h
		${CMAKE_SOURCE_DIR}/src/MSA/MSAFluid/MSAFluidParticleUpdater.h
		${CMAKE_SOURCE_DIR}/src/MSA/MSAFluid/MSAFluidSolver.h
		${CMAKE_SOURCE_DIR}/src/MSA/MSAFluid/MSAFluidSolver.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${MSA_SOURCE_FILES})	
source_group("MSA" FILES ${MSA_SOURCE_FILES})
		
		
#################################################
# OBJ UI -> BDD
#################################################
set(OBJUIBDD_SOURCE_FILES
		# Edit
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_edit/bdd_curve_edit.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_edit/bdd_curve_edit.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_edit/bdd_curve3d.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_edit/bdd_curve3d.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_edit/bdd_edit.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_edit/bdd_edit.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_edit/bdd_grid_adjustable.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_edit/bdd_grid_adjustable.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_edit/bdd_tree.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_edit/bdd_tree.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_edit/keys.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_edit/keys.cpp
		# Geo
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_algo_maa.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_algo_maa.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_circle.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_circle.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_cone.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_cone.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_face.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_face.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_gene.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_gene.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_grid.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_grid.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_line.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_line.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_maa.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_maa.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_null.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_null.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_polygon.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_polygon.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_proj_cone.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_proj_cone.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_quak.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_quak.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_sphere.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_sphere.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_terrain.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_terrain.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_text.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_text.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_torus.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_torus.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_tri.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_tri.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_tri_file.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_tri_svg.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_tube.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_tube.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_tube_path.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_tube_path.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_voronoi.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bdd_voronoi.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bddtex2d.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_geo/bddtex2d.cpp
		# Hardware
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_hardware/bdd_irtouch.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_hardware/bdd_irtouch.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_hardware/bdd_kramer_p2k.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_hardware/bdd_kramer_p2k.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_hardware/bdd_lumio.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_hardware/bdd_lumio.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_hardware/bdd_pjlink.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_hardware/bdd_pjlink.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_hardware/bdd_pqlabs.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_hardware/bdd_pqlabs.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_hardware/bdd_touchco.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_hardware/bdd_touchco.cpp
		# Image
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_feedback.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_feedback.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_analyse.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_analyse.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_cv_background.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_cv_background.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_cv_calibrate.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_cv_calibrate.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_cv_mix.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_cv_mix.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_cv_stereo.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_cv_stereo.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_cv_stereo_point.h 
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_cv_stereo_point.cpp 
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_detect_face.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_detect_face.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_erode_dilate.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_erode_dilate.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_gradient.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_gradient.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_opencv.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_opencv.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_proc.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_proc.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_smooth.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_smooth.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_water.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_water.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_gene.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_gene.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_substrate.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_img_substrate.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_opencl_img.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_image/bdd_opencl_img.cpp
		# Multitouch
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_multitouch/bdd_blob_tracking.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_multitouch/bdd_blob_tracking.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_multitouch/bdd_img_contour.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_multitouch/bdd_img_contour.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_multitouch/bdd_img_cv_multitouch.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_multitouch/bdd_img_cv_multitouch.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_multitouch/blob.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_multitouch/blob.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_multitouch/rect2d.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_multitouch/vector2d.h
		# Old
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/aichi.h
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/aichi.cpp
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/belga.h
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/belga.cpp
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/boulogne.h
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/boulogne.cpp
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/bpi.h
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/bpi.cpp
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/lcp.h
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/lcp.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/lenticular.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/lenticular.cpp
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/nib.h
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/nib.cpp
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/orgia.h
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/orgia.cpp
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/pay6.h
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/pay6.cpp
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/peripherie.h
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/peripherie.cpp
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/plage.h
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/plage.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/special.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/special.cpp
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/strato.h
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/strato.cpp
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/upstream.h
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_old/upstream.cpp
		# Point
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_blob.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_blob.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_boid.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_boid.cpp
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_data_upstream.h
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_data_upstream.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_fluid.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_fluid.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_mocap.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_mocap.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_part.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_part.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_point.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_point.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_point_network.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_point_network.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_voxel.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/bdd_voxel.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/boid.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/boid.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/particle.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/particle.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/placer.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_point/placer.cpp
		# Sound
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_sound/bdd_snd_synth_spectrum.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_sound/bdd_snd_synth_spectrum.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_sound/bdd_snd_wave.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_sound/bdd_snd_wave.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_sound/bdd_sound.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_sound/bdd_sound.cpp
		# SPE
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_array.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_array.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_boxes.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_boxes.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_cell_automata.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_cell_automata.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_chat.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_chat.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_chryzode.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_chryzode.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_def_vertex.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_def_vertex.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_dll.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_dll.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_ffgl.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_ffgl.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_flexus.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_flexus.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_FT_face.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_FT_face.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_layer_selector.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_layer_selector.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_miguel.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_miguel.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_movie.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_movie.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_sign.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_sign.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_special.h
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_special.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_time_watcher.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_time_watcher.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_watchdog.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_watchdog.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_web.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/bdd_web.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/web_util.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/web_util_awesomium.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/web_util_awesomium.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/web_util_wke.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_spe/web_util_wke.cpp
		# Util
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/util/bbox.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/util/bbox.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/util/bdd.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/util/bdd.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/util/bdd_lua.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/util/bdd_lua.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/util/bdd_uv.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/util/bdd_uv.cpp
		# Com
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/com/bdd_smtp.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/com/bdd_smtp.cpp

		# Main
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_arbre.h
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_arbre.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_ben.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_ben.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_cell_draw.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_cell_draw.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_clear_screen.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_clear_screen.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_opencl_part.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_opencl_part.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_ui_pref.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/bdd_ui_pref.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/bdd/rectangle2d.h
		)
set(SOURCE_FILES ${SOURCE_FILES} ${OBJUIBDD_SOURCE_FILES})	
source_group("Obj UI BDD" FILES ${OBJUIBDD_SOURCE_FILES})
		
		
#################################################
# OBJ UI -> COM
#################################################
set(OBJUICOM_SOURCE_FILES
		# OSC
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/osc/MessageMappingOscPacketListener.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/osc/OscException.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/osc/OscHostEndianness.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/osc/OscOutboundPacketStream.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/osc/OscOutboundPacketStream.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/osc/OscPacketListener.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/osc/OscPrintReceivedElements.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/osc/OscPrintReceivedElements.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/osc/OscReceivedElements.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/osc/OscReceivedElements.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/osc/OscTypes.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/osc/OscTypes.cpp
		
		# Main
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/com/aka.h
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/com/aka.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/midi.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/midi.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/midi_data.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/midi_data.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/midi_lua.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/midi_lua.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/net.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/net.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/net_blk.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/net_blk.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/net_link.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/net_link.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/net_lua.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/net_lua.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/net_osc.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/net_util_obj.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/net_util_obj.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/serial.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/serial.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/socket.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/com/socket.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${OBJUICOM_SOURCE_FILES})	
source_group("Obj UI COM" FILES ${OBJUICOM_SOURCE_FILES})
		
		
#################################################
# OBJ UI -> Deformer
#################################################
set(OBJUIDEFORMER_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_bdd_blob.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_bdd_blob.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_bdd_uv.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_bdd_uv.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_blob.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_blob.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_cartogram.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_cartogram.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_cylinder.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_cylinder.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_data.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_data.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_fn.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_fn.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_fold.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_fold.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_image.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_image.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_interpolate.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_interpolate.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_nib.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_nib.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_node.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_node.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_panoscope.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_panoscope.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_path.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_path.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_repulsor.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_repulsor.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_rot.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_rot.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_scale.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_scale.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_skew.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_skew.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_spherical.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_spherical.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_spin.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_spin.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_spiral.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_spiral.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_tra.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_tra.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_turb.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_turb.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_vort.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_vort.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_vortex.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_vortex.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_water.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_water.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_wrap.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/def_wrap.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/defcurve.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/defcurve.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/deformer.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/deformer.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/wave.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/deformer/wave.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${OBJUIDEFORMER_SOURCE_FILES})	
source_group("Obj UI Deformer" FILES ${OBJUIDEFORMER_SOURCE_FILES})
		
		
#################################################
# OBJ UI -> Tracker
#################################################
set(OBJUITRACKER_SOURCE_FILES
		# Glove 5DT
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/Glove5DT/fglove.h
		
		# Main
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/analog_way.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/analog_way.cpp
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/arcom88.cpp
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/arcom88.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/dmx.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/dmx.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/dmx_try.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/doremi.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/doremi.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/ftdi.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/ftdi.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/glove5DT.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/glove5DT.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/hmd.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/hmd.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/hydra.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/hydra.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/iglasses.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/iglasses.cpp
		# ${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/isense.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/joy.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/joy.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/mackie.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/mackie.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/magellan.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/magellan.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/MGLDLL.H
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/MGLDLL.C
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/neat.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/neat.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/nvision.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/nvision.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/aaa_ascension.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/aaa_ascension.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/aaa_gypsy.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/aaa_gypsy.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/aaa_isense.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/aaa_isense.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/polhemus.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/polhemus.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/sensor_6dof.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/sensor_6dof.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/spaceball.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/spaceball.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/toaster.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/toaster.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/tracker.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/tracker.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/trackers.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/trackers.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/wrap_isense.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/wrap_isense.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/wrap_isense_api_Macro.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/wrap_sixense.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/wrap_sixense_api_Macro.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/xps.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tracker/xps.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${OBJUITRACKER_SOURCE_FILES})	
source_group("Obj UI Tracker" FILES ${OBJUITRACKER_SOURCE_FILES})
		
		
#################################################
# OBJ UI -> Misc + Main
#################################################
set(OBJUI_SOURCE_FILES
		# Nvidia CPL
		${CMAKE_SOURCE_DIR}/src/obj_ui/nvidia_cpl/NvApiError.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/nvidia_cpl/NvCpl.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/nvidia_cpl/nvidia_cpl.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/nvidia_cpl/nvidia_cpl.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/nvidia_cpl/NvPanelApi.h
		# Tessellation
		${CMAKE_SOURCE_DIR}/src/obj_ui/tessellation/triangle.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/tessellation/triangle.cpp
		# Transfo
		${CMAKE_SOURCE_DIR}/src/obj_ui/transfo/transfo.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/transfo/transfo.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/transfo/transfo_three.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/transfo/transfo_three.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/transfo/transfo_trs.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/transfo/transfo_trs.cpp
		
		# Main
		${CMAKE_SOURCE_DIR}/src/obj_ui/aaarect.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/demo.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/demo.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/demo.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/display_info.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/display_info.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/flexus.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/flexus.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/fn_3d.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/fn_3d.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/multi_screen.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/multi_screen.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/print_master.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/print_master.h
		${CMAKE_SOURCE_DIR}/src/obj_ui/snap.cpp
		${CMAKE_SOURCE_DIR}/src/obj_ui/snap.h
		)
set(SOURCE_FILES ${SOURCE_FILES} ${OBJUI_SOURCE_FILES})	
source_group("Obj UI" FILES ${OBJUI_SOURCE_FILES})
		
		
#################################################
# OpenCL
#################################################
set(OPENCL_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/OpenCL/aaa_opencl.h
		${CMAKE_SOURCE_DIR}/src/OpenCL/aaa_opencl.cpp
		${CMAKE_SOURCE_DIR}/src/OpenCL/aaa_prec.h
		${CMAKE_SOURCE_DIR}/src/OpenCL/opencl_types.h
		${CMAKE_SOURCE_DIR}/src/OpenCL/wrap_OpenCL.h
		${CMAKE_SOURCE_DIR}/src/OpenCL/wrap_OpenCL.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${OPENCL_SOURCE_FILES})	
source_group("OpenCL" FILES ${OPENCL_SOURCE_FILES})
		
		
#################################################
# OpenGL
#################################################
set(OPENGL_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/gl/Engine.h
		${CMAKE_SOURCE_DIR}/src/gl/Engine.cpp
		${CMAKE_SOURCE_DIR}/src/gl/Item.h
		${CMAKE_SOURCE_DIR}/src/gl/Item.cpp
		${CMAKE_SOURCE_DIR}/src/gl/OGLExternal.h
		${CMAKE_SOURCE_DIR}/src/gl/Utilities.h 
		${CMAKE_SOURCE_DIR}/src/gl/Utilities.cpp 
		# ${CMAKE_SOURCE_DIR}/src/gl/Viewport.h 
		# ${CMAKE_SOURCE_DIR}/src/gl/Viewport.cpp 
		)
set(SOURCE_FILES ${SOURCE_FILES} ${OPENGL_SOURCE_FILES})	
source_group("gl" FILES ${OPENGL_SOURCE_FILES})
		
		
#################################################
# OpenFramework
#################################################
set(OPENFRAMEWORK_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/OpenFramework/MsaFluids/aaa_msa_ofconstants.h
		${CMAKE_SOURCE_DIR}/src/OpenFramework/MsaFluids/aaa_msa_ofmain.h
		${CMAKE_SOURCE_DIR}/src/OpenFramework/MsaFluids/ofxMSAFluid.h
		${CMAKE_SOURCE_DIR}/src/OpenFramework/MsaFluids/ofxMSAFluidDrawer.h
		${CMAKE_SOURCE_DIR}/src/OpenFramework/MsaFluids/ofxMSAFluidDrawer.cpp
		${CMAKE_SOURCE_DIR}/src/OpenFramework/MsaFluids/ofxMSAFluidParticleUpdater.h
		${CMAKE_SOURCE_DIR}/src/OpenFramework/MsaFluids/ofxMSAFluidSolver.h
		${CMAKE_SOURCE_DIR}/src/OpenFramework/MsaFluids/ofxMSAFluidSolver.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${OPENFRAMEWORK_SOURCE_FILES})	
source_group("Open Frameworks" FILES ${OPENFRAMEWORK_SOURCE_FILES})
		
		
#################################################
# Platform
#################################################
set(PLATFORM_SOURCE_FILES
		# Win32
		${CMAKE_SOURCE_DIR}/src/platform/win32/COM.h
		${CMAKE_SOURCE_DIR}/src/platform/win32/COM.cpp
		${CMAKE_SOURCE_DIR}/src/platform/win32/touch_windows.h
		${CMAKE_SOURCE_DIR}/src/platform/win32/touch_windows.cpp
		${CMAKE_SOURCE_DIR}/src/platform/win32/win_dlg_util.h
		${CMAKE_SOURCE_DIR}/src/platform/win32/win_dlg_util.cpp
		${CMAKE_SOURCE_DIR}/src/platform/win32/wrap_dll.h
		${CMAKE_SOURCE_DIR}/src/platform/win32/wrap_dll.cpp
		
		# Main
		${CMAKE_SOURCE_DIR}/src/platform/power_master.h
		${CMAKE_SOURCE_DIR}/src/platform/power_master.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${PLATFORM_SOURCE_FILES})	
source_group("Platform" FILES ${PLATFORM_SOURCE_FILES})
		
		
#################################################
# Shaders
#################################################
set(SHADERS_SOURCE_FILES
		# ${CMAKE_SOURCE_DIR}/src/shaders/cg_progam.h
		# ${CMAKE_SOURCE_DIR}/src/shaders/cg_progam.cpp
		# ${CMAKE_SOURCE_DIR}/src/shaders/cg_shader.h
		# ${CMAKE_SOURCE_DIR}/src/shaders/cg_shader.cpp
		# ${CMAKE_SOURCE_DIR}/src/shaders/cg_uniforms.h
		# ${CMAKE_SOURCE_DIR}/src/shaders/cg_uniforms.cpp
		# ${CMAKE_SOURCE_DIR}/src/shaders/cg_utils.h
		# ${CMAKE_SOURCE_DIR}/src/shaders/cg_utils.cpp
		${CMAKE_SOURCE_DIR}/src/shaders/GLSL_program.h
		${CMAKE_SOURCE_DIR}/src/shaders/GLSL_program.cpp
		${CMAKE_SOURCE_DIR}/src/shaders/GLSL_shader.h
		${CMAKE_SOURCE_DIR}/src/shaders/GLSL_shader.cpp
		${CMAKE_SOURCE_DIR}/src/shaders/GLSL_uniforms.h
		${CMAKE_SOURCE_DIR}/src/shaders/GLSL_uniforms.cpp
		${CMAKE_SOURCE_DIR}/src/shaders/aaa_shader.h
		${CMAKE_SOURCE_DIR}/src/shaders/aaa_shader.cpp
		${CMAKE_SOURCE_DIR}/src/shaders/shader.h
		${CMAKE_SOURCE_DIR}/src/shaders/shader.cpp
		${CMAKE_SOURCE_DIR}/src/shaders/shader_const.h
		${CMAKE_SOURCE_DIR}/src/shaders/shader_master.h
		${CMAKE_SOURCE_DIR}/src/shaders/shader_master.cpp
		${CMAKE_SOURCE_DIR}/src/shaders/shader_noise3d.h
		${CMAKE_SOURCE_DIR}/src/shaders/shader_noise3d.cpp
		${CMAKE_SOURCE_DIR}/src/shaders/shader_uniforms.h
		${CMAKE_SOURCE_DIR}/src/shaders/shading.h
		${CMAKE_SOURCE_DIR}/src/shaders/shading.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${SHADERS_SOURCE_FILES})	
source_group("Shaders" FILES ${SHADERS_SOURCE_FILES})
		
		
#################################################
# System
#################################################
set( SYSTEM_SOURCE_FILES
	# Base
	${AAASeedProject_SOURCE_DIR}/src/system/Native_System.h
	${AAASeedProject_SOURCE_DIR}/src/system/Native_Types.h
	# Shared
	${AAASeedProject_SOURCE_DIR}/src/system/shared/SystemCursor.h
	${AAASeedProject_SOURCE_DIR}/src/system/shared/SystemEvent.h
	${AAASeedProject_SOURCE_DIR}/src/system/shared/SystemMouse.h
	${AAASeedProject_SOURCE_DIR}/src/system/shared/SystemMouse.cpp
	${AAASeedProject_SOURCE_DIR}/src/system/shared/SystemUtils.h 
	${AAASeedProject_SOURCE_DIR}/src/system/shared/SystemView.h
	${AAASeedProject_SOURCE_DIR}/src/system/shared/SystemView.cpp
	${AAASeedProject_SOURCE_DIR}/src/system/shared/SystemView_AAA.h 
	${AAASeedProject_SOURCE_DIR}/src/system/shared/SystemView_AAA.cpp 
	)
	
if( WIN32 )
	list( APPEND SYSTEM_SOURCE_FILES
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemContext.h
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemContext.cpp
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemContextMenu.h 
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemContextMenu.cpp 
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemError.h
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemError.cpp
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemEventHandler.h
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemEventHandler.cpp
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemEventListener.h
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemEventListener.cpp
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemEventReader.h
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemEventReader.cpp
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemKeyboard.h
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemKeyboard.cpp
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemKeymap.h
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemKeymap.cpp
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemNode.h
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemNode.cpp
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemPixelFormat.h
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemPixelFormat.cpp
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemUtils.cpp 
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemWindow.h
		${AAASeedProject_SOURCE_DIR}/src/system/win32/SystemWindow.cpp
		)
elseif( APPLE )
endif()
set(SOURCE_FILES ${SOURCE_FILES} ${SYSTEM_SOURCE_FILES})	
source_group("System" FILES ${SYSTEM_SOURCE_FILES})
		
		
#################################################
# Thread
#################################################
set(THREAD_SOURCE_FILES
	# Shared
	${CMAKE_SOURCE_DIR}/src/thread/shared/Native_Barrier.h
	${CMAKE_SOURCE_DIR}/src/thread/shared/Native_Barrier.cpp
	${CMAKE_SOURCE_DIR}/src/thread/shared/Native_CriticalSectionLock.h
	${CMAKE_SOURCE_DIR}/src/thread/shared/Native_Lock.h
	${CMAKE_SOURCE_DIR}/src/thread/shared/Native_Lockable.h
	${CMAKE_SOURCE_DIR}/src/thread/shared/Native_ScopedMutex.h
	)
	
if( WIN32 )
	list( APPEND THREAD_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/thread/win32/Native_Condition.h
		${CMAKE_SOURCE_DIR}/src/thread/win32/Native_Lock.cpp
		${CMAKE_SOURCE_DIR}/src/thread/win32/Native_Mutex.h
		${CMAKE_SOURCE_DIR}/src/thread/win32/Native_Thread.h
		${CMAKE_SOURCE_DIR}/src/thread/win32/Native_Thread.cpp
		${CMAKE_SOURCE_DIR}/src/thread/win32/Native_ThreadSimple.h
		${CMAKE_SOURCE_DIR}/src/thread/win32/Native_ThreadSimple.cpp
		)
elseif( APPLE )
	list( APPEND THREAD_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/thread/osx/Native_Lock.cpp
		${CMAKE_SOURCE_DIR}/src/thread/pthread/Native_Condition.h
		${CMAKE_SOURCE_DIR}/src/thread/pthread/Native_Lock.cpp
		${CMAKE_SOURCE_DIR}/src/thread/pthread/Native_Mutex.h
		${CMAKE_SOURCE_DIR}/src/thread/pthread/Native_Thread.h
		${CMAKE_SOURCE_DIR}/src/thread/pthread/Native_Thread.cpp
		${CMAKE_SOURCE_DIR}/src/thread/pthread/Native_ThreadSimple.h
		${CMAKE_SOURCE_DIR}/src/thread/pthread/Native_ThreadSimple.cpp
		)
endif()
set(SOURCE_FILES ${SOURCE_FILES} ${THREAD_SOURCE_FILES})	
source_group("Thread" FILES ${THREAD_SOURCE_FILES})
		
		
#################################################
# Tiny XML
#################################################
set(TINYXML_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/tinyxml/tinystr.h
		${CMAKE_SOURCE_DIR}/src/tinyxml/tinystr.cpp
		${CMAKE_SOURCE_DIR}/src/tinyxml/tinyxml.h
		${CMAKE_SOURCE_DIR}/src/tinyxml/tinyxml.cpp
		${CMAKE_SOURCE_DIR}/src/tinyxml/tinyxmlerror.cpp
		${CMAKE_SOURCE_DIR}/src/tinyxml/tinyxmlparser.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${TINYXML_SOURCE_FILES})	
source_group("TinyXML" FILES ${TINYXML_SOURCE_FILES})
		
		
#################################################
# Tracking
#################################################
set(TRACKING_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/tracking/tracking_matrix.h
		${CMAKE_SOURCE_DIR}/src/tracking/tracking_matrix.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${TRACKING_SOURCE_FILES})	
source_group("Tracking" FILES ${TRACKING_SOURCE_FILES})
		
		
#################################################
# True Type
#################################################
set(TRUETYPE_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/TrueType/AAA_font_tmp.h
		${CMAKE_SOURCE_DIR}/src/TrueType/AAA_font_tmp.cpp
		${CMAKE_SOURCE_DIR}/src/TrueType/FTEngine.h
		${CMAKE_SOURCE_DIR}/src/TrueType/FTEngine.cpp
		${CMAKE_SOURCE_DIR}/src/TrueType/AAA_FTFace.h
		${CMAKE_SOURCE_DIR}/src/TrueType/AAA_FTFace.cpp
		${CMAKE_SOURCE_DIR}/src/TrueType/AAA_FTGlyph.h
		${CMAKE_SOURCE_DIR}/src/TrueType/AAA_FTGlyph.cpp
		${CMAKE_SOURCE_DIR}/src/TrueType/FTGlyphVectorizer.h
		${CMAKE_SOURCE_DIR}/src/TrueType/FTGlyphVectorizer.cpp
		${CMAKE_SOURCE_DIR}/src/TrueType/FTInstance.h
		${CMAKE_SOURCE_DIR}/src/TrueType/FTInstance.cpp
		${CMAKE_SOURCE_DIR}/src/TrueType/GLTTFont.h
		${CMAKE_SOURCE_DIR}/src/TrueType/GLTTFont.cpp
		${CMAKE_SOURCE_DIR}/src/TrueType/GLTTGlyphPolygonizer.h
		${CMAKE_SOURCE_DIR}/src/TrueType/GLTTGlyphPolygonizer.cpp
		${CMAKE_SOURCE_DIR}/src/TrueType/GLTTGlyphPolygonizerHandler.h
		${CMAKE_SOURCE_DIR}/src/TrueType/GLTTGlyphPolygonizerHandler.cpp
		${CMAKE_SOURCE_DIR}/src/TrueType/GLTTminmax.h
		# ${CMAKE_SOURCE_DIR}/src/TrueType/GLTTOutlineFont.C
		${CMAKE_SOURCE_DIR}/src/TrueType/GLTTOutlineFont.h
		${CMAKE_SOURCE_DIR}/src/TrueType/GLTTOutlineFont.cpp
		${CMAKE_SOURCE_DIR}/src/TrueType/OurTrueType.h
		${CMAKE_SOURCE_DIR}/src/TrueType/OurTrueType.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${TRUETYPE_SOURCE_FILES})	
source_group("True Type" FILES ${TRUETYPE_SOURCE_FILES})
		
		
#################################################
# UI
#################################################
set(UI_SOURCE_FILES
		# Event
		${CMAKE_SOURCE_DIR}/src/ui/event/event_keyboard.h
		${CMAKE_SOURCE_DIR}/src/ui/event/event_keyboard.cpp
		${CMAKE_SOURCE_DIR}/src/ui/event/event_midi.h
		${CMAKE_SOURCE_DIR}/src/ui/event/event_midi.cpp
		${CMAKE_SOURCE_DIR}/src/ui/event/event_mouse.h
		${CMAKE_SOURCE_DIR}/src/ui/event/event_mouse.cpp		
		# Main
		${CMAKE_SOURCE_DIR}/src/ui/alphabet.h
		${CMAKE_SOURCE_DIR}/src/ui/alphabet.cpp
		${CMAKE_SOURCE_DIR}/src/ui/alphabet_roman.h
		${CMAKE_SOURCE_DIR}/src/ui/alphabet_roman.cpp
		${CMAKE_SOURCE_DIR}/src/ui/dialog.h
		${CMAKE_SOURCE_DIR}/src/ui/dialog.cpp
		${CMAKE_SOURCE_DIR}/src/ui/dialog_number.h
		${CMAKE_SOURCE_DIR}/src/ui/dialog_number.cpp
		${CMAKE_SOURCE_DIR}/src/ui/dialog_str.h
		${CMAKE_SOURCE_DIR}/src/ui/dialog_str.cpp
		${CMAKE_SOURCE_DIR}/src/ui/dialog_wrapper.h
		${CMAKE_SOURCE_DIR}/src/ui/dialog_wrapper.cpp
		${CMAKE_SOURCE_DIR}/src/ui/AAA_event.h
		${CMAKE_SOURCE_DIR}/src/ui/AAA_event.cpp
		${CMAKE_SOURCE_DIR}/src/ui/file_dlg.h
		${CMAKE_SOURCE_DIR}/src/ui/file_dlg.cpp
		${CMAKE_SOURCE_DIR}/src/ui/flatland.h
		${CMAKE_SOURCE_DIR}/src/ui/flatland.cpp
		${CMAKE_SOURCE_DIR}/src/ui/flatland_lua.h
		${CMAKE_SOURCE_DIR}/src/ui/flatland_lua.cpp
		${CMAKE_SOURCE_DIR}/src/ui/font.h
		${CMAKE_SOURCE_DIR}/src/ui/font.cpp
		${CMAKE_SOURCE_DIR}/src/ui/keyboard.h
		${CMAKE_SOURCE_DIR}/src/ui/keyboard.cpp
		${CMAKE_SOURCE_DIR}/src/ui/menu_lua.h
		${CMAKE_SOURCE_DIR}/src/ui/menu_lua.cpp
		${CMAKE_SOURCE_DIR}/src/ui/obj_value.h
		${CMAKE_SOURCE_DIR}/src/ui/obj_value.cpp
		${CMAKE_SOURCE_DIR}/src/ui/aaa_menu.h
		${CMAKE_SOURCE_DIR}/src/ui/aaa_menu.cpp
		${CMAKE_SOURCE_DIR}/src/ui/pref.h
		${CMAKE_SOURCE_DIR}/src/ui/pref.cpp
		${CMAKE_SOURCE_DIR}/src/ui/seed_ui.h
		${CMAKE_SOURCE_DIR}/src/ui/seed_ui.cpp
		${CMAKE_SOURCE_DIR}/src/ui/seedkey.h
		${CMAKE_SOURCE_DIR}/src/ui/seedkey.cpp
		${CMAKE_SOURCE_DIR}/src/ui/seedmenu.h
		${CMAKE_SOURCE_DIR}/src/ui/seedmenu.cpp
		${CMAKE_SOURCE_DIR}/src/ui/SeedMenuDefine.h
		${CMAKE_SOURCE_DIR}/src/ui/strsymbo.h
		${CMAKE_SOURCE_DIR}/src/ui/strsymbo.cpp
		${CMAKE_SOURCE_DIR}/src/ui/text_dlg.h
		${CMAKE_SOURCE_DIR}/src/ui/text_dlg.cpp
		${CMAKE_SOURCE_DIR}/src/ui/ui_lua.h
		${CMAKE_SOURCE_DIR}/src/ui/ui_lua.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${UI_SOURCE_FILES})	
source_group("UI" FILES ${UI_SOURCE_FILES})
		
		
#################################################
# Wrapper
#################################################
set(WRAPPER_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/wrapper/aaa_opencv.h
		${CMAKE_SOURCE_DIR}/src/wrapper/aaa_opencv.cpp
		${CMAKE_SOURCE_DIR}/src/wrapper/open_cv_image.h
		${CMAKE_SOURCE_DIR}/src/wrapper/open_cv_image.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${WRAPPER_SOURCE_FILES})	
source_group("Wrappers" FILES ${WRAPPER_SOURCE_FILES})
		
		
#################################################
# Main
#################################################
set(MAIN_SOURCE_FILES
		${CMAKE_SOURCE_DIR}/src/action.h
		${CMAKE_SOURCE_DIR}/src/action.cpp
		${CMAKE_SOURCE_DIR}/src/GLOS.H
		#${CMAKE_SOURCE_DIR}/src/seed.h
		#${CMAKE_SOURCE_DIR}/src/seed.cpp
		)
set(SOURCE_FILES ${SOURCE_FILES} ${MAIN_SOURCE_FILES})	
source_group("Main" FILES ${MAIN_SOURCE_FILES})
		
	
#################################################
# Assembler files
#################################################	
# enable_language( ASM )
ENABLE_LANGUAGE( ASM_MASM )
set(ASM_SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/image/a_convert.asm 
	${CMAKE_SOURCE_DIR}/src/image/a_yuv2rgbhq.asm 
	${CMAKE_SOURCE_DIR}/src/image/a_yuv422torgb.asm 
	${CMAKE_SOURCE_DIR}/src/image/a_yuvtable.asm 
	)
SET_SOURCE_FILES_PROPERTIES(${ASM_SOURCE_FILES} PROPERTIES LANGUAGE ASM)
set(SOURCE_FILES ${SOURCE_FILES} ${ASM_SOURCE_FILES})	
source_group("Assembler" FILES ${ASM_SOURCE_FILES})

	
# Visual Leak Detector.
#################################################
if( WIN32 AND OPTION_BUILD_USE_VLD )
	include("${CMAKE_SOURCE_DIR}/CMake/FindVLD.cmake")
	if( VLD_FOUND )
		set( AAA_LEAK_DETECTOR ON )
		include_directories( ${VLD_INCLUDE_DIRS} )
	endif()
endif()
			
# External.h
#################################################
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/generated/External.h.in
				${CMAKE_CURRENT_SOURCE_DIR}/generated/External.h )
set( SOURCE_FILES ${SOURCE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/generated/External.h )
set_source_files_properties( ${CMAKE_CURRENT_SOURCE_DIR}/generated/External.h PROPERTIES GENERATED true )


# Windows Resource file.
###################################################################################################
if( WIN32 )
	# Generate rc files
	configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/../Ressource/Seed.rc ${CMAKE_CURRENT_BINARY_DIR}/Seed.rc )
	# Set properties to generated files
	set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/${PRODUCT_PRODUCT_NAME}.rc PROPERTIES GENERATED TRUE )	
	set( SOURCE_FILES ${SOURCE_FILES} ${CMAKE_CURRENT_BINARY_DIR}/Seed.rc )
endif()


#################################################
# Set the source files properties
#################################################
# SET_SOURCE_FILES_PROPERTIES( ${SOURCE_FILES} PROPERTIES GENERATED true )
		
	
###################################################################################################
# Build the executable if requested by the end-user
###################################################################################################

#############################################
# Build getopt library 			
#############################################

set(GETOPT_SOURCES
	${BASE_SOURCE_PATH}/src/executable/getopt.c
	${BASE_SOURCE_PATH}/src/executable/getopt1.c
	${BASE_SOURCE_PATH}/src/executable/getopt.h 
	)
ADD_LIBRARY(getopt STATIC ${GETOPT_SOURCES})
SET_TARGET_PROPERTIES(getopt PROPERTIES
	COMPILE_FLAGS "${EXECUTABLE_C_COMPILE_FLAGS}"
	PREFIX ""
	OUTPUT_NAME "libgetopt" )
	
	
#############################################
# Add include directories			
#############################################
INCLUDE_DIRECTORIES(	
	${AAASeed_SOURCE_DIR}
	${AAASeedProject_SOURCE_DIR}     
	${AAASeedProject_BINARY_DIR} 		
	)

	
##########################################################################################
# Build main executables			
##########################################################################################
add_executable(${executable_name} 
			   ${WINDOW_COMMON_HEADER} 
			   ${WINDOW_COMMON_SOURCES} 
			   ${WINDOW_WIN32_SOURCES} 
			   ${WINDOW_WGL_SOURCES} 
			   ${SOURCE_FILES} 
			   ${AAASeedProject_SOURCE_DIR}/src/seed.h 
			   ${AAASeedProject_SOURCE_DIR}/src/seed.cpp 
			   )
# TARGET_LINK_LIBRARIES(${executable_name} getopt)

if( WIN32 AND OPTION_BUILD_USE_VLD AND VLD_FOUND )
	target_link_libraries( ${executable_name} ${VLD_LIBRARIES} )
endif()

SET_TARGET_PROPERTIES(${executable_name} PROPERTIES OUTPUT_NAME "${executable_name}")

# ADD_CUSTOM_COMMAND(TARGET ${executable_name}
				   # POST_BUILD
				   # COMMAND ${executable_name}
				   # WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
				   # )


##################################################################################################
# External dependencies
##################################################################################################

#################################################
# Link libraries
#################################################
include( LinkLibraries.cmake )

#################################################
# Copy DLLs
#################################################
include( CopyDLL.cmake )

#################################################
# Create environment
#################################################
include( CreateEnvironment.cmake )

#################################################
# Copy assets (icons etc)
#################################################
include( CopyAssets.cmake )


#################################################
# Main library .pdb file (Windows only)
#################################################
if( WIN32)
	if( OPTION_BUILD_TYPE_RELEASE OR OPTION_BUILD_TYPE_DEBUG )
		install(FILES ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/${executable_name}.pdb DESTINATION . COMPONENT AAASeed)
	endif()
endif()

 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
