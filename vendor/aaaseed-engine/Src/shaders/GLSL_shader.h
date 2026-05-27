
#ifdef AAA_GLSL_SHADER_H
#error "GLSL_SHADER_H included more than once."
#endif
#define AAA_GLSL_SHADER_H 1


#ifndef AAA_GOL_BASE_H
#	include "gol/gol_base.h"
#endif
#ifndef AAA_SHADER_H
#	include "shader.h"
#endif

class c_shader_glsl final : public c_shader
{
private:
	GLuint			_shader_id;

public:
	static	GLenum		get_gl_type( SHADER_TYPE shader_type_in );

	c_shader_glsl( c_shader::SHADER_TYPE shader_type );
	~c_shader_glsl();

			GLuint		get_gl_shader()	CONST	{	return _shader_id; }
			operator	GLuint () CONST;

	virtual	void		load();
	virtual	void		dealloc();
};

