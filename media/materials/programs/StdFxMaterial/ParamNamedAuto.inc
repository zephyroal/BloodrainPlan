#ifndef _JoyParamNamedAuto_INC_
#define _JoyParamNamedAuto_INC_

/// max foward light number
#ifndef MAX_LIGHT_NUM
#define MAX_LIGHT_NUM 8
#endif

/// max hardware skin bone number
#ifndef MAX_SKIN_BONE_NUM
#define MAX_SKIN_BONE_NUM 68
#endif

float4x4 world_matrix;											// ACT_WORLD_MATRIX
float4x4 inverse_world_matrix;							// ACT_INVERSE_WORLD_MATRIX
float4x4 transpose_world_matrix;						// ACT_TRANSPOSE_WORLD_MATRIX
float4x4 inverse_transpose_world_matrix;		// ACT_INVERSE_TRANSPOSE_WORLD_MATRIX
float4x4 world_matrix_array[MAX_SKIN_BONE_NUM];								// ACT_WORLD_MATRIX_ARRAY	

float4x4 view_matrix;												// ACT_VIEW_MATRIX
float4x4 inverse_view_matrix;								// ACT_INVERSE_VIEW_MATRIX
float4x4 transpose_view_matrix;							// ACT_TRANSPOSE_VIEW_MATRIX
float4x4 inverse_transpose_view_matrix;			// ACT_INVERSE_TRANSPOSE_VIEW_MATRIX

float4x4 projection_matrix;									// ACT_PROJECTION_MATRIX
float4x4 inverse_projection_matrix;					// ACT_INVERSE_PROJECTION_MATRIX
float4x4 transpose_projection_matrix;				// ACT_TRANSPOSE_PROJECTION_MATRIX
float4x4 inverse_transpose_projection_matrix;		// ACT_INVERSE_TRANSPOSE_PROJECTION_MATRIX

float4x4 viewproj_matrix;							// ACT_VIEWPROJ_MATRIX
float4x4 inverse_viewproj_matrix;			// ACT_INVERSE_VIEWPROJE_MATRIX
float4x4 transpose_viewproj_matrix;		// ACT_TRANSPOSE_VIEWPROJ_MATRIX
float4x4 inverse_transpose_viewproj_matrix;// ACT_INVERSE_TRANSPOSE_VIEWPROJ_MATRIX

float4x4 worldview_matrix;									// ACT_WORLDVIEW_MATRIX
float4x4 inverse_worldview_matrix;					// ACT_INVERSE_WORLDVIEW_MATRIX
float4x4 transpose_worldview_matrix;				// ACT_TRANSPOSE_WORLDVIEW_MATRIX
float4x4 inverse_transpose_worldview_matrix;		// ACT_INVERSE_TRANSPOSE_WORLDVIEW_MATRIX


float4x4 worldviewproj_matrix;						// ACT_WORLDVIEWPROJ_MATRIX
float4x4 inverse_worldviewrojection_matrix;			// ACT_INVERSE_WORLDVIEWROJECTION_MATRIX
float4x4 transpose_worldviewrojection_matrix;		// ACT_TRANSPOSE_WORLDVIEWROJECTION_MATRIX
float4x4 inverse_transpose_worldviewrojection_matrix;	// ACT_INVERSE_TRANSPOSE_WORLDVIEWROJECTION_MATRIX

float4 camera_position;										// ACT_CAMERA_POSITION
float4 camera_position_object_space;	// ACT_CAMERA_POSITION_OBJECT_SPACE

float4 light_diffuse_colour_power_scaled; 	// ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED
float4 light_specular_colour_power_scaled;	
float4 light_position; // ACT_LIGHT_POSITION
float4 ambient_light_colour; // ACT_AMBIENT_LIGHT_COLOUR
float4 surface_diffuse_colour; // ACT_SURFACE_DIFFUSE_COLOUR
float4 surface_specular_colour;	// ACT_SURFACE_SPECULAR_COLOUR
float4 surface_shininess; // ACT_SURFACE_SHININESS
float4 surface_emissive_colour; // ACT_SURFACE_EMISSIVE_COLOUR
float4 derived_scene_colour; // ACT_DERIVED_SCENE_COLOUR
float4 light_attenuation; // ACT_LIGHT_ATTENUATION
float4 light_attenuation_array[MAX_LIGHT_NUM]; // ACT_LIGHT_ATTENUATION_ARRAY

float light_number; // ACT_LIGHT_NUMBER
float light_count;
float4 light_position_array[MAX_LIGHT_NUM]; // ACT_LIGHT_POSITION_ARRAY
float4 light_diffuse_colour_power_scaled_array[MAX_LIGHT_NUM]; // ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED_ARRAY
float4 light_specular_colour_power_scaled_array[MAX_LIGHT_NUM]; // ACT_LIGHT_SPECULAR_COLOUR_POWER_SCALED_ARRAY
float4 derived_light_diffuse_colour_array[MAX_LIGHT_NUM]; // ACT_DERIVED_LIGHT_DIFFUSE_COLOUR_ARRAY
float4 derived_light_specular_colour_array[MAX_LIGHT_NUM]; // ACT_DERIVED_LIGHT_SPECULAR_COLOUR_ARRAY

float4 fog_colour; // ACT_FOG_COLOUR
float4 fog_params; // ACT_FOG_PARAMS;

float4 inverse_texture_size;

float time;
float time_0_x;
float time_0_1;
float pass_number;
float far_clip_distance;

float4x4 texture_matrix_0; // ACT_TEXTURE_MATRIX
float4x4 texture_matrix_1; // ACT_TEXTURE_MATRIX
float4x4 texture_matrix_2; // ACT_TEXTURE_MATRIX

float3x4 world_matrix_array_3x4[MAX_SKIN_BONE_NUM]; // ACT_WORLD_MATRIX_ARRAY_3x4;

#endif