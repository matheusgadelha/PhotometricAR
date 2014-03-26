//[FRAGMENT SHADER]
#version 330 

in vec3 OutNormal;
in vec4 ShadowCoord;

uniform vec3 LightDirection;
uniform sampler2D shadowMap;

out vec4 FragColor;

float random( vec4 seed4 )
{
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}
 
void main()
{
    vec2 poissonDisk[4] = vec2[](
        vec2( -0.94201624, -0.39906216 ),
        vec2( 0.94558609, -0.76890725 ),
        vec2( -0.094184101, -0.92938870 ),
        vec2( 0.34495938, 0.29387760 )
    );

    float bias = 0.01;

    float visibility = 1.0;
    for (int i=0;i<4;i++){
        int index = int(16.0*random(gl_FragCoord.xyzw))%16;
        if ( texture2D( shadowMap, ShadowCoord.xy + poissonDisk[i]/700.0 ).x  <  ShadowCoord.z-bias ){
          visibility -= 0.2*
            (1.0-texture( shadowMap, 
                            vec3(ShadowCoord.xy + poissonDisk[index]/700.0,(ShadowCoord.z-bias))
                          ).x
            );
        }
    }

    vec4 environmentComponent = vec4( 0.0f,0.0f,0.0f, 1-visibility );

    FragColor =	environmentComponent;
}
