#version 460

in vec3 Position;
in vec3 Normal;

layout (location = 0) out vec4 FragColor;

uniform struct LightInfo {
    vec4 Position;
    vec3 La;
    vec3 L;
} Light;

uniform struct MaterialInfo {
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float Shininess;
} Material;

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

void main() {
    FragColor = vec4(phongModel(Position, normalize(Normal)), 1);
}
