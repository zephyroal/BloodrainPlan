   void DoF_Gaussian3x3VP_cg(  float4 worldPos      : POSITION,
                                float2 textCoordi : TEXCOORD0,
                                out float4 viewPos   : POSITION,
                                out float2 textCoordo : TEXCOORD0,
                                  uniform float4x4 worldViewProj
                                )       
    {
       viewPos = mul(worldViewProj, worldPos);
       textCoordo = textCoordi;
    }

    void DoF_Gaussian3x3FP_cg(  float2 tex  : TEXCOORD0,
                                out float4 color : COLOR,
                                uniform sampler2D source : register(s0),
                                uniform float3 pixelSize
                                )
    {
     
      #define KERNEL_SIZE 9
     
      float weights[KERNEL_SIZE];
      float2 offsets[KERNEL_SIZE];
     
      weights[0] = 1.0/16.0; weights[1] = 2.0/16.0; weights[2] = 1.0/16.0;
      weights[3] = 2.0/16.0; weights[4] = 4.0/16.0; weights[5] = 2.0/16.0;
      weights[6] = 1.0/16.0; weights[7] = 2.0/16.0; weights[8] = 1.0/16.0;

      offsets[0] = float2(-pixelSize.x, -pixelSize.y);
      offsets[1] = float2(0, -pixelSize.y);
      offsets[2] = float2(pixelSize.x, -pixelSize.y);
      offsets[3] = float2(-pixelSize.x, 0);
      offsets[4] = float2(0, 0);
      offsets[5] = float2(pixelSize.x, 0);
      offsets[6] = float2(-pixelSize.x, pixelSize.y);
      offsets[7] = float2(0,  pixelSize.y);
      offsets[8] = float2(pixelSize.x, pixelSize.y);

      color = float4(0,0,0,0);

      for (int i = 0; i < KERNEL_SIZE; ++i)
        color += weights[i] * tex2D(source, tex + offsets[i]);
    }


    void DoF_DepthOfFieldVP_cg(  float4 worldPos      : POSITION,
                                float2 textCoordi : TEXCOORD0,
                                out float4 viewPos   : POSITION,
                                out float2 textCoordo : TEXCOORD0,
                                  uniform float4x4 worldViewProj
                                )       
    {
       viewPos = mul(worldViewProj, worldPos);
       textCoordo = textCoordi;
    }

    void DoF_DepthOfFieldFP_cg( float2 tex  : TEXCOORD0,
                                out float4 color : COLOR,
                                uniform sampler2D scene : register(s0),             // full resolution image
                                uniform sampler2D blur  : register(s1),             // downsampled and blurred image
                                uniform float3 pixelSizeScene,                      // pixel size of full resolution image
                                uniform float3 pixelSizeBlur,                       // pixel size of downsampled and blurred image
                                uniform float4  dofParams                          
                                )
    {

      float  tmpDepth    = tex2D(scene, tex).w; 
      float4 tmpDepthVec = float4(0,0,tmpDepth,0);
      float  centerDepth = tmpDepth; 

      if(tmpDepth<dofParams.y)
      {   
        centerDepth = (tmpDepth - dofParams.y) / (dofParams.y - dofParams.x);                    // scale depth value between near blur distance and focal distance to [-1, 0] range
      }else{
        centerDepth = (tmpDepth - dofParams.y) / (dofParams.z - dofParams.y);                    // scale depth value between focal distance and far blur distance to [0, 1] range   
        centerDepth = clamp(centerDepth, 0.0, dofParams.w);                                             // clamp the far blur to a maximum blurriness
      }
     
      centerDepth = 0.5f*centerDepth + 0.5f;                                                            // scale and bias into [0, 1] range


     
      #define NUM_TAPS 12                     // number of taps the shader will use
     
      float2 poisson[NUM_TAPS];               // containts poisson-distributed positions on the unit circle
      float2 maxCoC;                          // maximum circle of confusion (CoC) radius and diameter in pixels
      float radiusScale;                      // scale factor for minimum CoC size on low res. image
     
      poisson[ 0] = float2( 0.00,  0.00);
      poisson[ 1] = float2( 0.07, -0.45);
      poisson[ 2] = float2(-0.15, -0.33);
      poisson[ 3] = float2( 0.35, -0.32);
      poisson[ 4] = float2(-0.39, -0.26);
      poisson[ 5] = float2( 0.10, -0.23);
      poisson[ 6] = float2( 0.36, -0.12);
      poisson[ 7] = float2(-0.31, -0.01);
      poisson[ 8] = float2(-0.38,  0.22);
      poisson[ 9] = float2( 0.36,  0.23);
      poisson[10] = float2(-0.13,  0.29);
      poisson[11] = float2( 0.14,  0.41);
     
      maxCoC = float2(5.0, 10.0);
      radiusScale = 0.4;

      // Get depth of center tap and convert it into blur radius in pixels
      
      float discRadiusScene = abs(centerDepth * maxCoC.y - maxCoC.x);
      float discRadiusBlur = discRadiusScene * radiusScale; // radius on low res. image

      float4 sum = float4(0.0,0.0,0.0,0.0);

      for (int i = 0; i < NUM_TAPS; ++i)
      {
        // compute texture coordinates
        float2 coordScene = tex + (pixelSizeScene.xy * poisson[i] * discRadiusScene);
        float2 coordBlur = tex + (pixelSizeBlur.xy * poisson[i] * discRadiusBlur);
     
        // fetch taps and depth
        float4 tapScene = tex2D(scene, coordScene);

        float tmpDepth = tex2D(scene, coordScene).w;
        float4 tmpDepthVec = float4(0,0,tmpDepth,0);
        float tapDepth  = tmpDepthVec.z; 
 
        if(tmpDepth<dofParams.y)
        {   
            tapDepth  = (tmpDepth - dofParams.y) / (dofParams.y - dofParams.x);                    // scale depth value between near blur distance and focal distance to [-1, 0] range
        }else{
        tapDepth  = (tmpDepth - dofParams.y) / (dofParams.z - dofParams.y);                    // scale depth value between focal distance and far blur distance to [0, 1] range   
        tapDepth  = clamp(tapDepth, 0.0, dofParams.w);                                             // clamp the far blur to a maximum blurriness
      }
     
      tapDepth = 0.5f*tapDepth + 0.5f;                                                            // scale and bias into [0, 1] range


        float4 tapBlur = tex2D(blur, coordBlur);
       
        // mix low and high res. taps based on tap blurriness
        float blurAmount = abs(tapDepth * 2.0 - 1.0); // put blurriness into [0, 1]
        float4 tap = lerp(tapScene, tapBlur, blurAmount);
     
        // "smart" blur ignores taps that are closer than the center tap and in focus
        float factor = (tapDepth >= centerDepth) ? 1.0 : abs(tapDepth * 2.0 - 1.0);
     
        // accumulate
        sum.rgb += tap.rgb * factor;
        sum.a += factor;
      }
     
      color = (sum / sum.a);
    }

