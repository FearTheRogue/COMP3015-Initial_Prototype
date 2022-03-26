#version 460

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;
layout (binding = 0) uniform sampler2D RenderTex;

uniform struct SpotLightInfo {
    vec3 Position;
    vec3 L;
    vec3 La;
    vec3 Direction;
    float Exponent;
    float Cutoff;
} Spot;

uniform struct MaterialInfo {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Shininess;
} Material;

uniform struct FogInfo {
    float MaxDist;
    float MinDist;
    vec3 Colour;
} Fog;

vec3 blinnPhongSpot(vec3 pos, vec3 norm)
{
    vec3 texColour = texture(RenderTex, TexCoord).rgb;

    vec3 ambient = texColour * Spot.La;

    vec3 s = normalize(Spot.Position.xyz - pos); // point light

    float cosAng = dot(-s, normalize(Spot.Direction)); // cosine of the angle
    float angle = acos(cosAng); // gives you the actual angle
    float spotScale = 0.0;
    
    vec3 diffuse = vec3(0.0);
    vec3 spec = vec3(0.0);

    if(angle < Spot.Cutoff) {
        spotScale = pow(cosAng, Spot.Exponent);
        float sDotN =  max(dot(s, norm), 0.0);
        diffuse = texColour * sDotN;
    
        if(sDotN > 0.0) {
            vec3 v = normalize(-pos.xyz);
            vec3 h = normalize(v + s);
            spec = Material.Ks * pow(max(dot(h, norm), 0.0), Material.Shininess);
        }
    }

    return ambient + spotScale * Spot.L * (diffuse + spec);
}

void main() {
   
    /// <summary>
    /// 
    /// CALCULATING FOG
    /// 
    /// </summary>
   
    // setting fog on the z axis
    float dist = abs(Position.z);

    // fogFactor calculation based on the formular
    float fogFactor = (Fog.MaxDist - dist) / (Fog.MaxDist - Fog.MinDist);

    // clamp values between 0.0 and 1.0
    fogFactor = clamp(fogFactor,0.0,1.0); 

    // colour we recieve from blinnPhong calculation
    vec3 shadeColour = blinnPhongSpot(Position, normalize(Normal));

    // we assign a colour based on the fogFactor using mix
    vec3 colour = mix(Fog.Colour, shadeColour, fogFactor);

    // final colour
    FragColor = vec4(colour, 1.0);
}
