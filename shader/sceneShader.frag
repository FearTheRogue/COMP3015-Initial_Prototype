#version 460

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;

layout (binding = 0) uniform sampler2D Tex1;

uniform struct LightInfo {
    vec4 Position;
    vec3 La;
    vec3 L;
} Light;

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

vec3 phongModel(vec3 pos, vec3 norm) {
    
    // calculate ambient 
    vec3 ambient = Material.Ka * Light.La;

    // calculate diffuse
    vec3 s = vec3(0.0);

    if(Light.Position.w == 0) {
        s = Light.Position.xyz;
    } else {
        s = normalize(vec3(Light.Position) - pos);
    }

    float sDotN = max(dot(s, norm), 0.0);
    vec3 diffuse = Material.Kd * sDotN;

    // calculate specular
    vec3 spec = vec3(0.0);

    if(sDotN > 0.0) {
        vec3 v = normalize(-pos.xyz);
        vec3 r = reflect(-s, norm);
        spec = Material.Ks * pow(max(dot(r,v), 0.0), Material.Shininess);
    }

    return ambient + Light.L * (diffuse + spec);
}

vec3 blinnPhong(vec3 pos, vec3 norm) {
    
    vec3 texColour = texture(Tex1, TexCoord).rgb;

    // calculate ambient 
    vec3 ambient = Material.Ka * Light.La;

    // calculate diffuse
    vec3 s = vec3(0.0);

    if(Light.Position.w == 0) {
        s = Light.Position.xyz;
    } else {
        s = normalize(vec3(Light.Position) - pos);
    }

    float sDotN = max(dot(s, norm), 0.0);
    vec3 diffuse = Material.Kd * sDotN;

    // calculate specular
    vec3 spec = vec3(0.0);

    if(sDotN > 0.0) {
        vec3 v = normalize(-pos.xyz);
        vec3 h = normalize(v + s);
        spec = Material.Ks * pow(max(dot(h,norm), 0.0), Material.Shininess);
    }

    return ambient + Light.L * (diffuse + spec);
}

vec3 blinnPhongSpot(vec3 pos, vec3 norm)
{
    vec3 texColour = texture(Tex1, TexCoord).rgb;

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

    //FragColor = vec4(phongModel(Position, normalize(Normal)), 1);
    //FragColor = vec4(blinnPhongSpot(Position, normalize(Normal)),1);
    //FragColor = vec4(blinnPhong(Position, normalize(Normal)),1);

    float dist = abs(Position.z);

    // fogFactor calculation based on the formular
    float fogFactor = (Fog.MaxDist - dist) / (Fog.MaxDist - Fog.MinDist);

    fogFactor = clamp(fogFactor,0.0,1.0); // clamp values

    // colour we recieve from blinnPhong calculation
    vec3 shadeColour = blinnPhongSpot(Position, normalize(Normal));

    // we assign a colour based on the fogFactor using mix
    vec3 colour = mix(Fog.Colour, shadeColour, fogFactor);

    // final colour
    FragColor = vec4(colour, 1.0);
}
