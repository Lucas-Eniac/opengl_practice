#version 460 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

uniform Material material;

void main()
{
    // Ambient / Diffuse
    vec4 texColor = texture(material.texture_diffuse1, TexCoords);
    vec4 specColor = texture(material.texture_specular1, TexCoords);

    // Simple lighting placeholder
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(1.0, 1.0, 2.0));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0); 
    
    vec3 result = (0.2 + diffuse) * texColor.rgb;
    FragColor = vec4(result, 1.0);
}