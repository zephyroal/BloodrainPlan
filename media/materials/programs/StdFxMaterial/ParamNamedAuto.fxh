#define __FX__

#ifdef _3DSMAX_ 
    #ifndef __FX__
        #define __FX__ 
    #endif
#endif


#define NO_UI  < string UIWidget="None"; >

int g_RenderQueueType
< 
	string UIName = "RenderQueueType";
	int UIMin=-1;
	int UIMax=1000;
	int UIStep=1;
> = -1;

int g_RenderQueueDetail
< 
	string UIName = "RenderQueueDetail";
	int UIMin=-1;
	int UIMax=1000;
	int UIStep=1;
> = -1;


#ifdef __FX__
  	float4x4 world_matrix : world NO_UI;
#ifdef _3DSMAX_
#ifdef _DEBUG_MAX_
	float4x4 inverse_world_matrix : worldinverse  NO_UI;
#else
	float4x4 inverse_world_matrix : worldi  NO_UI;
#endif
#else
    float4x4 inverse_world_matrix : worldinverse  NO_UI;
#endif	
    float4x4 transpose_world_matrix : worldtranspose  NO_UI;
    float4x4 inverse_transpose_world_matrix : worldinversetranspose  NO_UI;
    float4x4 view_matrix : view  NO_UI;
    float4x4 inverse_view_matrix : ViewInverse  NO_UI;
    float4x4 transpose_view_matrix : viewtranspose  NO_UI;
    float4x4 inverse_transpose_view_matrix : viewinversetranspose  NO_UI;
    float4x4 projection_matrix : projection  NO_UI;
    float4x4 inverse_projection_matrix : projectioninverse  NO_UI;
    float4x4 transpose_projection_matrix : projectiontranspose  NO_UI;
    float4x4 inverse_transpose_projection_matrix : projectioninversetranspose  NO_UI;
    float4x4 worldview_matrix : worldview  NO_UI;
    float4x4 inverse_worldview_matrix : worldviewinverse  NO_UI;
    float4x4 transpose_worldview_matrix : worldviewtranspose  NO_UI;
    float4x4 inverse_transpose_worldview_matrix : worldviewinversetranspose  NO_UI;
#ifdef _3DSMAX
	//max no viewprojection
#else
	float4x4 viewproj_matrix : viewprojection  NO_UI;
#endif	
    
	float4x4 inverse_viewproj_matrix : viewprojectioninverse  NO_UI;
    float4x4 transpose_viewproj_matrix : viewprojectiontranspose  NO_UI;
    float4x4 inverse_transpose_viewproj_matrix : viewprojectioninversetranspose  NO_UI;
    float4x4 worldviewproj_matrix : worldviewprojection  NO_UI;
    float4x4 inverse_worldviewproj_matrix : worldviewprojectioninverse  NO_UI;
    float4x4 transpose_worldviewproj_matrix : worldviewprojectiontranspose  NO_UI;
    float4x4 inverse_transpose_worldviewproj_matrix : worldviewprojectioninversetranspose  NO_UI;
#else
    float4x4 world_matrix : world_matrix;
    float4x4 inverse_world_matrix : inverse_world_matrix;
    float4x4 transpose_world_matrix : transpose_world_matrix;
    float4x4 inverse_transpose_world_matrix : inverse_transpose_world_matrix;
    float4x4 view_matrix : view_matrix;
    float4x4 inverse_view_matrix : inverse_view_matrix;
    float4x4 transpose_view_matrix : transpose_view_matrix;
    float4x4 inverse_transpose_view_matrix : inverse_transpose_view_matrix;
    float4x4 projection_matrix : projection_matrix  NO_UI;
    float4x4 inverse_projection_matrix : inverse_projection_matrix;
    float4x4 transpose_projection_matrix : transpose_projection_matrix;
    float4x4 inverse_transpose_projection_matrix : inverse_transpose_projection_matrix;
    float4x4 worldview_matrix : worldview_matrix;
    float4x4 inverse_worldview_matrix : inverse_worldview_matrix;
    float4x4 transpose_worldview_matrix : transpose_worldview_matrix;
    float4x4 inverse_transpose_worldview_matrix : inverse_transpose_worldview_matrix;
    float4x4 viewproj_matrix : viewproj_matrix;
    float4x4 inverse_viewproj_matrix : inverse_viewproj_matrix;
    float4x4 transpose_viewproj_matrix : transpose_viewproj_matrix;
    float4x4 inverse_transpose_viewproj_matrix : inverse_transpose_viewproj_matrix;
    float4x4 worldviewproj_matrix : worldviewproj_matrix;
    float4x4 inverse_worldviewproj_matrix : inverse_worldviewproj_matrix;
    float4x4 transpose_worldviewproj_matrix : transpose_worldviewproj_matrix;
    float4x4 inverse_transpose_worldviewproj_matrix : inverse_transpose_worldviewproj_matrix;
#endif


#ifdef __FX__

    //The current cameras position in view space. 
    float3 get_camera_position() {return inverse_view_matrix[3].xyz;}
    #define camera_position get_camera_position()

    //The current cameras position in object space (ie when the object is at (0,0,0)). 
    float4 get_camera_position_object_space() 
    { 
        float4 c_pos=float4(camera_position.xyz,1);
        float4 pos=mul(c_pos,inverse_world_matrix);
        return float4( pos.xyz,1);
	
	/*
	float4 g_CamPos = mul(ViewI[3],WorldI);
	float3 ViewDir = g_CamPos.xyz - vsIn.Position.xyz;
	
	*/
	
    }
    #define camera_position_object_space get_camera_position_object_space()

    float2 viewportpixelsize : viewportpixelsize NO_UI;
    float4 get_viewport_size()
    {
        float4 f=float4(0,0,0,0);
        f.xy=viewportpixelsize.xy;
        f.zw=float2(1,1)/viewportpixelsize.xy;
        return f;
    }
    #define viewport_size get_viewport_size()
    
    float3 get_view_up_vector()
    {
         return view_matrix[1].xyz;
    }
    #define view_up_vector get_view_up_vector()
    
    float3 get_view_side_vector()
    {
        return view_matrix[0].xyz;
    }
    #define view_side_vector get_view_side_vector()
    
	float2 get_viewport_offset()
	{
		return (float2(0.5f,0.5f)/viewport_size);
	}
	
	#define viewport_offset get_viewport_offset()
	
#else
    float4 camera_position : camera_position;
    float4 camera_position_object_space : camera_position_object_space;
    float4 viewport_size : viewport_size;
    float4 view_up_vector : view_up_vector;
    float4 view_side_vector : view_side_vector;
#endif

//light 

#ifdef __FX__

		/*
		#ifdef _3DSMAX_
		    float4 ambient : Ambient <
		#else
		    float4 ambient : Ambient <
		#endif
		    string UIName =  "Ambient_Color!";
		    //string Object = "AmbientLight";
		    //string UIWidget = "Color";
		    //int RefID = 0;
        
		> = {0.2f, 0.2f, 0.2f,1};
		*/

		float4 light_position_0 : POSITION <
		
		#ifdef _3DSMAX_		
			string Object = "PointLight";
		#else
		    string Object = "PointLight";
		#endif    
		    string UIName =  "light Position 0";
		    string Space = "World";
		    int RefID = 0;
		> = {-0.5f,2.0f,1.25f,0.0f};
	
		float4 get_light_position_object_space_0()
		{
				return mul(light_position_0,inverse_world_matrix);
		}

		#define light_position_object_space_0 get_light_position_object_space_0()
		
		#ifdef _3DSMAX_
			#ifdef _DEBUG_MAX_
				float3 light_color_0 : DIFFUSE <
			#else
				float3 light_color_0 : LIGHTCOLOR <
			#endif
		#else
		    float3 light_color_0 : DIFFUSE <
		#endif
		    string UIWidget = "None";
			int LightRef = 0;
		> = {1.0f, 1.0f, 1.0f};
    
#else
    float4 ambient : ambient;
#endif

#ifdef __FX__

    //The current time, factored by the optional parameter (or 1.0f if not supplied). 
#ifdef _3DSMAX_    
    float __time : TIME NO_UI;
    float get_time()
    {
    	//3ds max 
    	//#define TIME_TICKSPERSEC	4800
			//#define TicksToSec( ticks ) ((float)(ticks)/(float)TIME_TICKSPERSEC)
			
			//3ds max 's time is 4800 ticks per second
			//so we get second need div 4.8
			
        return __time/4.8f;
    }
    #define time get_time()
#else
    float time : TIME NO_UI;
#endif

    float get_time_0_x( float x )
    {
        return fmod(time,x);
    }
    
    #define time_0_x get_time_0_x(300.0)
    #define time_0_1 get_time_0_x(1.0)
    
#else
    float time : time;
    //The current time, factored by the optional parameter (or 1.0f if not supplied). 
    
    float time_0_x : time_0_x;
    //Single float time value, which repeats itself based on "cycle time" given as an 'extra_params' field 
    
    float costime_0_x : costime_0_x;
    float sintime_0_x : sintime_0_x;
    float tantime_0_x : tantime_0_x;

    float time_0_x_packed : time_0_x_packed;
    //4-element vector of time0_x, sintime0_x, costime0_x, tantime0_x 
    
    float time_0_1 : time_0_1 ;
    //As time0_x but scaled to [0..1] 
    
    float costime_0_1 : costime_0_1 ;
    float sintime_0_1 : sintime_0_1 ;
    float tantime_0_1 : tantime_0_1 ;
    
    float time_0_1_packed : time_0_1_packed ;
    //As time0_x_packed but all values scaled to [0..1] 

    float time_0_2pi : time_0_2pi ;
    //As time0_x but scaled to [0..2*Pi] 
    
    float costime_0_2pi : costime_0_2pi ;
    //As costime0_x but scaled to [0..2*Pi] 
    
    float sintime_0_2pi : sintime_0_2pi ;
    //As sintime0_x but scaled to [0..2*Pi] 
    
    float tantime_0_2pi : tantime_0_2pi ;
    //As tantime0_x but scaled to [0..2*Pi] 
    
    float time_0_2pi_packed : time_0_2pi_packed ;
    //As time0_x_packed but scaled to [0..2*Pi] 
    
#endif

