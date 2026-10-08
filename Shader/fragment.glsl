#version 330 core

in vec2 texCoord;
in vec3 Normal;
in vec3 Pos;
out vec4 frag;

uniform sampler2D tex;
uniform samplerCube shadowCube;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec4 baseColor;
uniform float farPlane;
uniform int shadowsEnabled;
uniform int ambientEnabled;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - Pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 viewDir = normalize(viewPos - Pos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    float shadow = 0.0;
    if (shadowsEnabled != 0) {
        vec3 fragToLight = Pos - lightPos;
        float closestDepth = texture(shadowCube, fragToLight).r * farPlane;
        float currentDepth = length(fragToLight);
        float bias = max(0.15 * (1.0 - dot(norm, lightDir)), 0.05);
        shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }

    vec3 ambient = ambientEnabled != 0 ? 0.1 * lightColor : vec3(0.0);
    vec3 diffuse = diff * lightColor;
    vec3 specular = spec * lightColor;
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);
    frag = texture(tex, texCoord) * baseColor * vec4(lighting, 1.0);
}
