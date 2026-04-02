#version 460 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;

void main()
{
    // A simple basic diffuse color, since we didn't hook up a full texture for the prompt.
    // Or we just display the normals scaled to 0-1 for a quick placeholder color.
    vec3 color = normalize(Normal) * 0.5 + 0.5;
    
    // Add simple directional light
    vec3 lightDir = normalize(vec3(1.0, 1.0, 2.0));
    float diff = max(dot(normalize(Normal), lightDir), 0.0);
    vec3 diffuse = diff * vec3(0.3, 0.8, 0.4); // soft turtle green
    
    vec3 result = (0.2 + diffuse) * color;
    FragColor = vec4(result, 1.0);
}