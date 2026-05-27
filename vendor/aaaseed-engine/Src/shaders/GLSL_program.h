
#ifdef AAA_GLSL_PROGRAM_H
#error "GLSL_PROGRAM_H included more than once."
#endif
#define AAA_GLSL_PROGRAM_H 1


#ifndef	AAA_GOL_SHADER_H
#	include "gol/gol_shader.h"
#endif
#ifndef AAA_OBJ_H
#	include		"infrastructure/obj/obj.h"
#endif

class c_shader;
class c_shader_glsl;
class c_uniforms_render_glsl;
class c_uniforms_compute_glsl;


class c_program_glsl : public c_obj
{
private:
	INT32	_program_id;
protected:
	bool	_b_valid;
	bool	_b_bind_first;

protected:
	FINLINE void	use_program()	CONST	{	GOL::use_program( _program_id );	}
			INT32	get_program_always();

public:
	c_program_glsl();
	~c_program_glsl();

	bool	is_valid()	CONST	{ return _b_valid; }

	FINLINE	GLuint	get_program_id()	CONST	{	return _program_id;		}

	//	Unbinds the current glsl Shader Program
	FINLINE static	void	unbind()	{	GOL::use_program( 0 );	}
					INT32	get_attribute_location( C_PCHAR_C name );
};

class c_program_render_glsl final : public c_program_glsl
{
public:
	//input:
	//	GL_POINTS, GL_LINES, GL_LINES_ADJACENCY, GL_TRIANGLES, GL_TRIANGLES_ADJACENCY
	enum GEOMETRY_SHADER_INPUT : INT32
	{
		GSI_POINTS = 0,
		GSI_LINES,
		GSI_LINES_ADJACENCY,
		GSI_TRIANGLES,
		GSI_TRIANGLES_ADJACENCY,
		GSI_MAX_NB
	};
	//output:
	//	GL_POINTS, GL_LINE_STRIP, GL_TRIANGLE_STRIP
	enum GEOMETRY_SHADER_OUTPUT : INT32
	{
		GSO_POINTS = 0,
		GSO_LINE_STRIP,
		GSO_TRIANGLE_STRIP,
		GSO_MAX_NB
	};

private:
	c_uniforms_render_glsl*	_uniforms_glsl;
	c_shader_glsl*			_p_vertex_shader;
	c_shader_glsl*			_p_geometry_shader;
	c_shader_glsl*			_p_fragment_shader;

	GEOMETRY_SHADER_INPUT	_geometry_in;
	GEOMETRY_SHADER_OUTPUT	_geometry_out;
	INT32					_vertices_max_nb;
	INT32					_vertices_nb;

public:
	c_program_render_glsl();
	~c_program_render_glsl();

			void						bind();	//	Bind the Shader Program
	FINLINE	c_uniforms_render_glsl*		get_glsl_uniforms()	CONST	{	return _uniforms_glsl;	}

			void	detach_shaders();
			bool	attach_shaders(	c_shader CONST * CONST vertex_shader, c_shader CONST * CONST fragment_shader, c_shader CONST * CONST geometry_shader = nullptr );
			bool	update_shaders(	c_shader CONST * CONST vertex_shader, c_shader CONST * CONST fragment_shader, c_shader CONST * CONST geometry_shader = nullptr );

			void	set_geometry( GEOMETRY_SHADER_INPUT geo_in, GEOMETRY_SHADER_OUTPUT geo_out, INT32 nb );
};

class c_program_compute_glsl final : public c_program_glsl
{
private:
	c_uniforms_compute_glsl*	_uniforms_glsl;
	c_shader_glsl*				_p_compute_shader;

public:
	c_program_compute_glsl();
	~c_program_compute_glsl();

			void						bind();	//	Bind the Shader Program
	FINLINE	c_uniforms_compute_glsl*	get_glsl_uniforms()	CONST	{	return _uniforms_glsl;	}

			void	detach_shader();
			bool	attach_shader(	c_shader CONST * CONST compute_shader	);
			bool	update_shader(	c_shader CONST * CONST compute_shader	);
};



