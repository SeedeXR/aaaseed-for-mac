// tests/unit/path_a_catalog_test.cpp
//
// Path A catalog test : iterate every `.metal` file in
// src/shaders/msl/ , feed each to GOL::MetalBackend::create_program_msl,
// fail if any one is rejected. Auto-discovers ports landed by the
// glsl_to_msl tool (continuation 29) so each new port doesn't need a
// dedicated test entry -- just drop the .metal into the directory.
//
// Why a single iterating test instead of one TEST per shader :
//   - .metal files are produced one per source shader ; per-shader
//     duplication of test boilerplate scales poorly at 50-250 files.
//   - Per-shader failure context comes from SCOPED_TRACE on the
//     filename + the backend's get_last_error() string. Reporting is
//     just as useful as per-test failures, plus we get a count
//     guarantee : "at least N shaders must port".
//
// The catalog is the Path A regression corpus : adding a new port
// extends coverage automatically, and any future Metal-backend or
// MSL-grammar regression that breaks a previously-clean port surfaces
// here on the next CTest run.
//
// CTest label : `regression` -- the catalog acts like a golden corpus
// for shader compilation.

#include <gtest/gtest.h>

#include "src/gol/gol_backend.h"
#include "src/gol/metal/metal_backend.h"

#include <filesystem>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#ifndef AAA_SHADERS_MSL_DIR
#error "AAA_SHADERS_MSL_DIR must be defined by CMake -- points at src/shaders/msl/"
#endif

namespace
{
    std::string slurp( std::filesystem::path const& p )
    {
        std::ifstream in( p, std::ios::binary );
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }

    std::vector< std::filesystem::path > collect_metal_files()
    {
        std::vector< std::filesystem::path > out;
        std::filesystem::path const root( AAA_SHADERS_MSL_DIR );
        if( !std::filesystem::is_directory( root ) )
            return out;
        for( auto const& entry : std::filesystem::directory_iterator( root ) )
        {
            if( entry.is_regular_file() && entry.path().extension() == ".metal" )
                out.push_back( entry.path() );
        }
        //	Deterministic order so test failures point at the same file
        //	across runs / CI hosts.
        std::sort( out.begin(), out.end() );
        return out;
    }
}

TEST( PathACatalog, EveryMetalFileCompilesViaMetalBackend )
{
    auto const metal_files = collect_metal_files();

    //	Floor : every Path A port lands as one .metal -- the catalog
    //	should never shrink. As of continuation 31 we have 4 ports in
    //	src/shaders/msl/ that survive Apple Metal compilation :
    //	ps_edgeoverlay (hand), ps_Maa_add_scale (hand), ps_Maa_4tex
    //	(tool), ps_mrt_test (tool). The Path A tool's helper-function
    //	scoping + singleton-sampler-binding limitations were caught by
    //	this test in continuation 31 ; until those are fixed, ports
    //	requiring those features stay deferred. Bump the floor as the
    //	catalog grows ; never lower it without a documented removal.
    //	c128 : floor bumped 158 -> 159 -- Path A REVIVAL #1 added
    //	`fxaa_lottes.metal` (real Timothy Lottes 3.11 FXAA algorithm)
    //	alongside the c127-B fxaa.metal passthrough stub (the existing
    //	stub is preserved verbatim to anchor its regression #27 golden).
    //	c129-A : floor bumped 159 -> 160 -- Path A REVIVAL #2 added
    //	`ifs_de_library.metal` (real 8-variant DE library : Cubefield /
    //	Sponge / Sierpinski / Mandelbulb / Mandelbox / Dodecahedron /
    //	Knot / Quaternion) alongside the c102 ifs.metal sphere-DE stub
    //	(the stub is preserved verbatim to anchor its regression #34 golden).
    //	c132-A : third Path A revival, lights_deferred_real.metal
    //	(real per-light Lambert+Phong over SSBO of up to 8 lights) lives
    //	alongside the c100 lights_deferred_v0 / lights_deferred_v1 stubs
    //	(both stubs preserved verbatim to anchor c128-B / c129-B goldens).
    //	Floor 160 -> 161.
    //	c133-B : fourth Path A revival, aaa_cam_real.metal (real
    //	perspective camera constant block : view + proj + view_inv + vp)
    //	lives alongside the per-shader file-scope identity-matrix aaa_cam
    //	stubs inside depth_coc.metal / debug_world_map.metal /
    //	debug_world_map_color_distance.metal / lights_deferred_v1.metal
    //	(stubs preserved verbatim to anchor c121-B / c121-A / c128-B /
    //	c129-B goldens). Floor 161 -> 162.
    //	c134-B : fifth Path A revival, aaa_material_pbr.metal (real
    //	Cook-Torrance PBR : GGX NDF + Smith Schlick-GGX G + Schlick
    //	Fresnel + Lambert diffuse , single directional light) lives
    //	alongside the historic flat-albedo `aaa_material.material` stub
    //	plumbed through gbuffer.metal / gbuffer_generic.metal (stub
    //	behaviour preserved verbatim ; c121-A / c121-B / c128-B goldens
    //	for every aaa_material-consuming shader stay intact). Floor
    //	162 -> 163.
    //	c135-A : sixth Path A revival, aaa_noise_real.metal (real
    //	Perlin 1985/2002 classic 3D noise + Perlin 2001 / Gustavson
    //	2005 Simplex 3D noise sharing Ken Perlin's canonical 256-entry
    //	permutation table ; 4-quadrant 256x256 layout with scalar +
    //	5-octave fbm for both) lives alongside the c80 ps_Maa_noise
    //	and ps_Maa_noise_cheap stubs (both stubs preserved verbatim
    //	to anchor c80 / c81 goldens). Floor 163 -> 164.
    //	c136-A : seventh Path A revival, aaa_gol_real.metal -- real
    //	2D cellular automata (Conway 1970 B3/S23 + Silverman 1991
    //	Brian's Brain 3-state) sharing one shader switchable via
    //	uniform. 8-neighbour Moore topology, toroidal wrap via
    //	manual fract(uv+1) on the previous-state texture, CPU-side
    //	ping-pong over N iterations from a deterministic 16x16
    //	stencil seed (R-pentomino + glider + Brian's spaceship).
    //	Path A catalog has historically only had GOL-stub family
    //	members rendering passthrough flat textures with no neighbour
    //	evaluation ; this is the canonical real-rules port. Floor
    //	164 -> 165.
    //	c137-A : eighth Path A revival, aaa_curl_noise_real.metal --
    //	divergence-free velocity field from the curl of c135-A's
    //	Perlin 3D noise (Bridson 2007 ACM SCA "Curl-Noise for
    //	Procedural Fluid Flow"). 12-sample central finite-difference
    //	curl with eps = 1e-3 on a 3D vector potential (3 uncorrelated
    //	noise samples). 4-quadrant 256x256 layout : |vel| greyscale ;
    //	vel direction as RGB ; 50-step RK2 advected displacement ;
    //	100-step RK2 advected displacement. The c135-A Perlin
    //	permutation table + perlin3_ helpers are BYTE-EQUIVALENT
    //	COPIES (MSL has no cross-TU includes ; doctrine forbids
    //	touching c135-A to preserve its golden). Floor 165 -> 166.
    //	c138-A : ninth Path A revival, aaa_bloom_real.metal -- real
    //	4-pass bloom post-process : (1) rec.709 luminance threshold
    //	with smoothstep soft knee , (2) horizontal 13-tap separable
    //	Gaussian (Pascal triangle row 12 normalised by 1/4096 ,
    //	sigma ~ 4 px) , (3) vertical 13-tap separable Gaussian ,
    //	(4) additive composite final = base + intensity * blurred.
    //	One MSL file ; pass selector via AaaFuInts[3] ; CPU driver
    //	chains 4 sequential renders ping-ponging across 3 render
    //	targets. Lives alongside the historic bloom.metal /
    //	darkglow.metal / ps_Maa_threshold stubs (all preserved
    //	verbatim to anchor their goldens). Citations : Bjorke 2007
    //	GPU Gems 3 Ch.40 (7-tap linear trick noted) ; Real-Time
    //	Rendering 4th ed §10.6 Bloom ; Sousa 2008 GDC "Crysis 2
    //	Next Gen Effects". Floor 166 -> 167.
    //	c139-A : tenth Path A revival, aaa_motion_blur_real.metal --
    //	real camera-velocity-projected motion blur. Builds directly
    //	on c133-B's aaa_cam_real.metal AaaCamReal struct ; adds a
    //	SECOND constant block `cam_prev` (previous-frame view + proj
    //	+ vp matrices) so a per-pixel screen-space velocity vector
    //	can be derived by reconstructing world position from current
    //	UV + linear depth + cam.view_inv + fov scalars , reprojecting
    //	through cam_prev.vp , and differencing the resulting UV.
    //	N-tap (default N=16) symmetric accumulation along the velocity
    //	vector with a soft 1 - 0.5*|t| falloff produces the canonical
    //	streak / smear. Mode-flag stub variant : AaaFuInts[0] = 0
    //	collapses to flat mid-grey (FIFTH consecutive session locking
    //	this doctrine -- c135-A / c136-A / c137-A / c138-A / THIS).
    //	Lives alongside historic stubs that returned source unchanged ;
    //	no catalog file is touched. Citations : McGuire et al. 2012
    //	I3D "A Reconstruction Filter for Plausible Motion Blur" ;
    //	Rosado 2007 GPU Gems 3 Ch.27 "Motion Blur as a Post-Processing
    //	Effect" ; Real-Time Rendering 4th ed §12.5 Motion Blur.
    //	Floor 167 -> 168. Revival count 9 -> 10 (DOUBLE-DIGIT CROSSED).
    //	c140-A : eleventh Path A revival, aaa_dof_hex_bokeh_real.metal --
    //	real depth-of-field with hexagonal bokeh. 5-pass pipeline :
    //	(1) thin-lens CoC compute (Karis 2014 / RTR 4th ed §12.4
    //	formula coc = |f * (z - z_f) / (A * (z_f - f) * z)|) packed
    //	into alpha of a colour RT , (2) vertical-down N-tap sweep
    //	at angle 270 deg , (3) diagonal-down-right sweep at 330 deg ,
    //	(4) diagonal-down-left sweep at 210 deg , (5) composite via
    //	McIntosh 2012's hex-preserving combiner final = min(P1 ,
    //	max(P2 , P3)). Box kernel preserves the hex bokeh hard edge ;
    //	N=8 samples per direction. Mode-flag stub variant
    //	(AaaFuInts[0] = 0 -> flat mid-grey) -- SIXTH consecutive
    //	session locking this doctrine. Lives alongside historic DOF
    //	stubs that returned the source unchanged ; no catalog file
    //	is touched. Citations : McIntosh 2012 "Bokeh Effects in Adobe
    //	Premiere Pro CS5" (3-directional hex) ; Lottes ~2015 slides
    //	"Filmic Bokeh" (cited variant of 3-pass hex) ; Karis 2014
    //	SIGGRAPH "Physically Based Shading at Epic Games" (CoC /
    //	thin-lens reference) ; Real-Time Rendering 4th ed §12.4
    //	Depth of Field. Floor 168 -> 169.
    ASSERT_GE( metal_files.size(), size_t( 169 ) )
        << "Expected at least 169 .metal files in " << AAA_SHADERS_MSL_DIR
        << " ; found " << metal_files.size();

    GOL::MetalBackend backend;
    ASSERT_TRUE( backend.init() );

    std::set< std::string > failed;
    for( auto const& p : metal_files )
    {
        SCOPED_TRACE( "shader = " + p.filename().string() );

        std::string const source = slurp( p );
        ASSERT_FALSE( source.empty() ) << "Empty .metal file ?";

        GOL::ProgramId prog = backend.create_program_msl(
            source.c_str(), "vs_main", "fs_main" );

        if( prog == GOL::kInvalidProgramId )
        {
            ADD_FAILURE() << "MetalBackend rejected " << p.filename().string()
                          << " : " << backend.get_last_error();
            failed.insert( p.filename().string() );
        }
        else
        {
            backend.delete_program( prog );
        }
    }

    if( !failed.empty() )
    {
        std::string list;
        for( auto const& f : failed )
            list += "\n  " + f;
        FAIL() << failed.size() << " of " << metal_files.size()
               << " catalog shaders failed Metal compilation :" << list;
    }
}

TEST( PathACatalog, ReportsCountForVisibility )
{
    //	Plain stdout report so a CI log shows the catalog size growing
    //	session by session. Non-failure.
    auto const metal_files = collect_metal_files();
    std::printf( "[PathACatalog] %zu shader(s) in %s :\n",
                 metal_files.size(), AAA_SHADERS_MSL_DIR );
    for( auto const& p : metal_files )
        std::printf( "    %s\n", p.filename().string().c_str() );
    SUCCEED();
}
