#version 460

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 Hdrcolor;

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

uniform float AveLum;
uniform int Pass;

uniform mat3 rgb2xyz = mat3 (0.4124564, 0.2126729, 0.0193339,
 0.3575761, 0.7151522, 0.1191920, 0.1804375, 0.0721750, 0.9503041);

uniform mat3 xyz2rgb = mat3(
 3.2404542, -0.9692660, 0.0556434,
 -1.5371385, 1.8760108, -0.2040259,
 -0.4985314, 0.0415560, 1.0572252 );

uniform float Exposure = 0.35;
uniform float White = 0.928;
uniform bool DoToneMap = true;

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

void pass1()
{
    vec3 n = normalize(Normal);

    // Compute shading and store result in high-res framebuffer
    Hdrcolor = vec3(0.0);
    
    Hdrcolor += blinnPhongSpot(Position, n); 

    float dist = abs(Position.z);

    // fogFactor calculation based on the formular
    float fogFactor = (Fog.MaxDist - dist) / (Fog.MaxDist - Fog.MinDist);

    fogFactor = clamp(fogFactor,0.0,1.0); // clamp values

    // colour we recieve from blinnPhong calculation
    vec3 shadeColour = Hdrcolor;

    // we assign a colour based on the fogFactor using mix
    vec3 colour = mix(Fog.Colour, shadeColour, fogFactor);
}

void pass2()
{
    // Retrieve high-res color from texture
    vec3 color = Hdrcolor.rgb;

    // Convert to XYZ
    vec3 xyzCol = rgb2xyz * vec3(color);

    // Convert to xyY
    float xyzSum = xyzCol.x + xyzCol.y + xyzCol.z;
    vec3 xyYCol = vec3( xyzCol.x / xyzSum, xyzCol.y / xyzSum, xyzCol.y);

    // Apply the tone mapping operation to the luminance (xyYCol.z or xyzCol.y)
    float L = (Exposure * xyYCol.z) / AveLum;
    L = (L * ( 1 + L / (White * White) )) / ( 1 + L );

    // Using the new luminance, convert back to XYZ
    xyzCol.x = (L * xyYCol.x) / (xyYCol.y);
    xyzCol.y = L;
    xyzCol.z = (L * (1 - xyYCol.x - xyYCol.y))/xyYCol.y;

    // Convert back to RGB and send to output buffer
    if( DoToneMap )
        FragColor = vec4( xyz2rgb * xyzCol, 1.0);
    else
        FragColor = vec4(color,1.0);
}

void main() {

    if(Pass == 1) { pass1(); }
    else if(Pass == 2) { pass2(); }
}
