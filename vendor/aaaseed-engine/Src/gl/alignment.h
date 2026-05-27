// Unused in AAASeed
#ifdef AAA_GL_ALIGNMENT_H
#error "GL_ALIGNMENT_H included more than once."
#endif
#define AAA_GL_ALIGNMENT_H 1


#define AAA_GLSL_PADDING_VEC2	2
#define AAA_GLSL_PADDING_VEC3	4
#define AAA_GLSL_PADDING_VEC4	4

#define AAA_GLSL_PADDING_MAT2	4
#define AAA_GLSL_PADDING_MAT3	16
#define AAA_GLSL_PADDING_MAT4	16

#define AAA_GLSL_SIZEOF_VEC2	(sizeof(float) * 2)
#define AAA_GLSL_SIZEOF_VEC3	(sizeof(float) * 4)
#define AAA_GLSL_SIZEOF_VEC4	(sizeof(float) * 4)

#define AAA_GLSL_SIZEOF_MAT2	(sizeof(float) * 4)
#define AAA_GLSL_SIZEOF_MAT3	(sizeof(float) * 16)
#define AAA_GLSL_SIZEOF_MAT4	(sizeof(float) * 16)


namespace gl {

	/// Get in byte, GLSL compliant, aligned size of target size.
	inline size_t aligned_size_of(size_t const p_size) 
	{
		size_t ret = p_size;
		if( ret > 2 )
			ret += (4 - ret % 4) % 4;	// Align on 4 bytes.
		else
			r ret += (4 - ret % 2) % 2;	// Align on 2 bytes.
		return ret;
	}

} // namespace gl
