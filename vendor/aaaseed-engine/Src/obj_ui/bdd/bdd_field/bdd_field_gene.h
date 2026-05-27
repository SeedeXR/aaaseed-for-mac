
#ifdef AAA_BDD_FIELD_GENE_H
#error "BDD_FIELD_GENE_H included more than once."
#endif
#define AAA_BDD_FIELD_GENE_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
// Used to access glm::mat4, not super clean. ED
#ifndef AAA_BDD_MESH_STATIC_H
#	include "obj_ui/bdd/bdd_mesh_static.h"
#endif

class	c_bdd_field_gene final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_field_gene, c_bdd );

public:
	friend class c_bdd_field_gene_file;

private:
	FP32*			_data					{nullptr};
	INT64			_data_size				{ 0 };
	FP32 *			_col					{ nullptr };
	INT32			_sx						{ 0 };
	INT32			_sy						{ 0 };
	INT32			_sz						{ 0 };
	INT32			_sxy					{ 0 };

	INT32			_ssx					{ 0 };
	INT32			_ssy					{ 0 };
	INT32			_ssz					{ 0 };

	INT32			_size_pixel_asked[3]	{0, 0, 0};
	INT32			_size_pixel[3]			{0, 0, 0};

	REAL			_origin[3]				{ 0., 0., 0. };
	REAL			_field_draw_size_factor { 1. };

	o_str			_target_name_symbo;
	c_bdd*			_bdd_target_src			{ nullptr };

	bool			_b_draw_normals			{ false };	//todo always false for now

	REAL			_scale					{ 1. };
	REAL			_max_threshold			{ 1. };
	REAL			_min_threshold			{ 0. };
	REAL			_decay_factor			{ 1. };
	REAL			_precision				{ 0.2f };

	REAL			_draw_count				{ 1.0f };
	INT32			_draw_steps_ui				{ 1 };
	bool			_b_draw					{ false };
	bool			_b_draw_particles		{ false };

	FP32			_bounding_box[3];

	bool			_b_compute_trig_ui		{ false };
	bool			_b_save_mesh			{ false };

	bool			_b_loaded				{ false };

	glm::mat4		_transformation_matrix;

	FINLINE	INT32	get_index_3d( INT32 CONST x, INT32 CONST y, INT32 CONST z ) CONST NOEXCEPT;
	FINLINE INT32	get_index_3d( INT32 CONST * CONST p_3i ) CONST NOEXCEPT;

	// Write to tex3d
	void	write_3d_v3( INT32 CONST * CONST pos, REAL CONST* CONST vec );
	void	draw_as_vectors( REAL CONST * CONST size );

	// Write to tex3d
	void	compute_test_field( );

	// Normals.
	FP32*	compute_normals( );
	void	draw_normals( REAL CONST * CONST size );

	// Helpers.
	void	compute_size( );
	void	map_image_point( FP32 * CONST dst, INT32 CONST * CONST pos );
	void	map_image_point( FP32 * CONST dst, INT32 CONST x, INT32 CONST y, INT32 CONST z );

	void	snap_to_grid( INT32 * CONST dst, FP32 CONST * CONST vec );

//	FP32	get_new_distance(FP32 CONST * CONST lhs, FP32 CONST * CONST rhs, FP32 CONST current_dist);

public:
	void	get_gradient( FP32 * CONST dst, INT32 CONST * CONST pos );
	void	get_gradient( FP32 * CONST dst, INT32 CONST x, INT32 CONST y, INT32 CONST z );
	void	get_gradient( FP32 * CONST dst, REAL CONST * CONST pos );

	REAL	get_distance(		REAL CONST * CONST pos );
	REAL	get_gradient_norm(	REAL CONST * CONST pos );
	REAL	get_gradient_norm(
						REAL CONST * CONST pos,
						REAL CONST fish_length,
						REAL CONST scale,
						REAL CONST decay_factor);
	REAL	get_gradient_norm(
						REAL CONST * CONST pos,
						REAL CONST min_threshold,
						REAL CONST max_threshold,
						REAL CONST scale,
						REAL CONST decay_factor);

public:
			bool	is_valid()	{	return _data != nullptr;	}
			void	init();

	virtual	void	update();
	virtual	void	draw();

	virtual	void	param_init_pt();

			void	save();
			void	load();


			bool	is_inside( REAL CONST x, REAL CONST y, REAL CONST z ) const;
			bool	is_inside( REAL CONST * CONST vec ) const;

			void	add_sphere( REAL* center, REAL radius );
};
