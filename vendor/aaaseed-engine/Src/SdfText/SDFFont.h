
#ifdef AAA_SDFFONT_H
#error "SDFFONT_H included more than once."
#endif
#define AAA_SDFFONT_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#if !defined(AAA_AAA_STR_H)
#	include "aaa_str.h"
#endif
#if !defined(_UNORDERED_MAP_)
#	include <unordered_map>
#endif

namespace aaa::font
{


class c_sdf_font final : public c_obj
{
	friend class c_sdf_text;
public:
	enum class FontType
	{
		NONE,
		SDF,
		MSDF,
		MTSDF
	};

	struct PageTexInfo
	{
		INT32			index	{};
		INT32			width	{};
		INT32			height	{};
		INT32			bind	{};
		std::string		filename{};
	};

	class KerningInfo
	{
	public:
		INT32		First	{};
		INT32		Second	{};
		REAL		Amount	{};

		KerningInfo()	{}
	};

	class Metrics
	{
	public:
		INT32		x			{};		// left position of the character image in the texture
		INT32		y			{};		// top position of the character image in the texture
		INT32		Width		{};		// width of the character image in the texture
		INT32		Height		{};		// height of the character image in the texture
		REAL		Sx			{};		// width of the character.
		REAL		Sy			{};		// height of the character.
		REAL		charx		{};		// How much the current position should be offset when copying the image from the texture to the screen.
		REAL		chary		{};		// How much the current position should be offset when copying the image from the texture to the screen.
		REAL		XOffset		{};		// How much the current position should be offset when copying the image from the texture to the screen.
		REAL		YOffset		{};		// How much the current position should be offset when copying the image from the texture to the screen.
		REAL		XAdvance	{};		// How much the current position should be advanced after drawing the character.
		INT32		Page		{};		// texture page where the character image is found.
		INT32		Channel		{};		// texture channel where the character image is found (1 = blue, 2 = green, 4 = red, 8 = alpha, 15 = all channels).
		bool		isFlip		{};		// Char was flipped 90 during packing

		Metrics()	{}
	};
	using CharToGlyph = std::unordered_map< std::uint32_t, Metrics >;

	c_sdf_font();
	~c_sdf_font();

	bool							load_textures();
	bool CONST						is_loaded() CONST					{ return _b_loaded; }
	o_str							get_face_name() CONST				{ return _font_name; }
	INT32							get_bind_from_page( UINT32 CONST page_index );
	bool							open( o_str CONST& filename );

	INT32							get_char_count() CONST				{ return static_cast<INT32>(_chars.size()); }
	const PageTexInfo&				get_page_info( INT32 CONST page )	{ return _pages_info[ page ]; }
	REAL							get_kerning_pair( INT32 CONST, INT32 CONST );
	std::tuple<REAL, REAL, REAL>	get_string_width( std::string CONST & string );
	std::tuple<REAL, REAL, REAL>	get_string_width( std::u32string CONST & string );



	FontType						get_font_type() CONST				{ return _font_type; }

	void							set_char_advance( const REAL advance ) { _x_advance = advance; }
	std::vector<std::string>		calculate_line_breaks( std::string CONST string, REAL CONST maxWidth = 0. );
protected:
	o_str						_font_path;
	FontType					_font_type;
	REAL						_line_height;
	REAL						_scaling;
	REAL						_text_height;
	REAL						_ascender;
	REAL						_space_width;
	REAL						_smooth_pixels;

	REAL						_base;
	REAL						_scale_x;
	REAL						_scale_y;
	REAL						_x_advance;		// How much the current position should be advanced after drawing the character.
	INT32						_pages;
	INT32						_kern_count;
	std::string					_face_name;
	o_str						_font_name;

	CharToGlyph					_chars;
	std::vector<KerningInfo>	_kerning_pairs;
	std::vector<PageTexInfo>	_pages_info;
	bool						_b_loaded{};

	void	add_kerning_pair( INT32 CONST first, INT32 CONST second, INT32 CONST amount );
	void	add_char( INT32 CONST id, INT32 CONST x, INT32 CONST y, INT32 CONST w, INT32 CONST h, INT32 CONST xoffset, INT32 CONST yoffset, INT32 CONST xadvance, INT32 CONST page, INT32 CONST chnl );
};

namespace sdf
{
	extern	INT32 g_bind_2d_begin;
	extern	INT32 g_bind_2d_end;

	extern	AAA_ERR		init();
	extern	void		deinit();
	extern	c_sdf_font*	get(   INT32 font_bind );
	extern	void		erase( INT32 font_bind );
}

}	// namespace aaa::font


