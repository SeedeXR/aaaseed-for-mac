
#ifdef AAA_MOCAP_FORMAT_BVH_H
#error "MOCAP_FORMAT_BVH_H included more than once."
#endif
#define AAA_MOCAP_FORMAT_BVH_H 1


//#if AAA_OS_WINDOWS() && !AAA_WIN64() && (AAA_VSTOOL() < 140)
#define AAA_LIB_USE_ASSIMP() 1
//#endif


#if AAA_LIB_USE_ASSIMP()

#ifndef	AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef _CSTDINT_
#	include <cstdint>
#endif
#ifndef AAA_ERR_H
#	include "err.h"
#endif
#ifndef _STRING_
#	include <string>
#endif

namespace Assimp
{
	class Importer;
}
struct aiScene;
struct aiNode;

class c_mocap_segment;

namespace mocap
{
	class c_animator;

	/**
	* @class BVHFormat
	*
	* @brief load and parse BVH file
	*/
	class c_bvh_data
	{
	public:
		bool						_b_file_loaded;			// File loaded state

		INT32						_bones_nb;				// Number of body nodes
		INT32 *						_frame_nb;				// Number of frames per anim array
		REAL *						_frame_by_sec;			// Number of frames per second per anim array
		INT32						_anim_nb;				// Number of animations

//		Assimp::Importer *			_importer;				// Assimp base importer

		const aiScene *				_scene;					// Assimp loaded scene

		mocap::c_animator *			_animator;

		REAL *						_bones_weight;
	//	size_t						_anim_node_nb;

	protected:
		/** BVHFormat constructor. */
		c_bvh_data( void );


		/** Init and alloc class members */
		void init_members( void );
		/** Deallocate loaded datas. */
		void dealloc( void );


		/** Import motion capture datas based on file path \a file_path. */
		bool import_from_assimp( const std::string & file_path );


		/** Find bone offset based on its name \a name at target frame \a frame.
		*	Returns new float pointer (vec3) or NULL if no bone offset found
		*/
		void find_offset_by_name( const std::string & name, int32_t frame, float * dst );


	public:
		/** Create a new BVHFormat pointer. */
		static c_bvh_data * create_method( void );


		/** BVHFormat class destructor. */
		virtual ~c_bvh_data( void );


		/** Import motion capture datas based on file path \a file_path.
		*	Returns true on success, false otherwise.
		*/
		virtual bool import_data( const char * file_path );

		/** Retrieve target \a animation_index animation datas as a single NATIVE_float32_t array.
		*	Returns error code as AAA_ERR.
		*/
		AAA_ERR	retrieve_data( REAL* data, int32_t real_by_node, const INT32 & animation_index );


		///////////////////////////////////////////////////////////////////////////////////////////////
		//		GET / SET
		///////////////////////////////////////////////////////////////////////////////////////////////

		/** Get skeleton segments indices (1-2, 2-3, 3-4 etc...) as INT32 array. */
		//	hack should be cleaned
		INT32 *	get_segment_point_index( void );
		void	free_segment_point_index( INT32* array );

		/** Get segments amount as INT32. */
		INT32	get_segment_nb( void );

		/** Get frames amount for animation \a animation_index as NATIVE_Sint32_t. */
		INT32	get_frame_nb(		const INT32 animation_index )		{	return _b_file_loaded ? _frame_nb[ animation_index ] : 0; }
		/** Get frames amount by second for animation \a animation_index as NATIVE_float32_t. */
		REAL	get_frame_by_sec(	const INT32 animation_index )		{	return _b_file_loaded ? _frame_by_sec[ animation_index ] : 0; }

		/** Get nodes amount as NATIVE_Sint32_t. */
		int32_t get_node_nb( void );
	};


} // namespace mocap

#endif	//#if AAA_LIB_USE_ASSIMP()