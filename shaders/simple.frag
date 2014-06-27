//[FRAGMENT SHADER]
#version 330 

in vec3 OutNormal;
in float OutVisibility;
in vec4 ShadowCoord;

uniform vec3 LightDirection;
uniform vec3 LightColor;
uniform vec3 ambientLight;
uniform sampler2D shadowMap;

out vec4 FragColor;

vec3 eye = vec3(0);
float shininess = 3.0f;
vec4 mColor = vec4(0.6,0.5,0.4,1.0);
float d_intensity = 0.2;
float s_intensity = 0.2;
float a_intensity = 0.2;
 
void main()
{
    vec2 poissonDisk[4] = vec2[](
        vec2( -0.94201624, -0.39906216 ),
        vec2( 0.94558609, -0.76890725 ),
        vec2( -0.094184101, -0.92938870 ),
        vec2( 0.34495938, 0.29387760 )
    );

    float bias = 0.01;

    float shadow_visibility = 1.0;
    for (int i=0;i<4;i++){
        if ( texture2D( shadowMap, ShadowCoord.xy + poissonDisk[i]/700.0 ).x  <  ShadowCoord.z-bias ){
          shadow_visibility-=0.2;
        }
    }

    vec3 reflected = normalize(reflect( -LightDirection, OutNormal ));
    eye = normalize(eye - gl_FragCoord.xyz);

    vec4 environmentComponent = vec4( a_intensity * ambientLight,1.0f );
    vec4 diffuseComponent = vec4( d_intensity * LightColor * max(dot(-LightDirection,OutNormal),0), 1.0f);
    // vec4 diffuseComponent = vec4( d_intensity * vec3(0.9,0.5,0.5) * max(dot(-LightDirection,OutNormal),0), 1.0f);
    vec4 specularComponent = vec4( s_intensity * LightColor * pow(max(dot(reflected,eye),0), shininess), 1.0f);

//    FragColor =	vec4(LightColor,1.0f);
    FragColor =	mColor*(shadow_visibility * diffuseComponent + 
                vec4(OutVisibility*environmentComponent.xyz,1.0f) +
                0.2*environmentComponent + 
                shadow_visibility*specularComponent);
// FragColor = mColor*(1.0 * diffuseComponent + 
//                 vec4(OutVisibility*environmentComponent.xyz,1.0f) +
//                 0.5*environmentComponent + 
//                 shadow_visibility*specularComponent);
}
