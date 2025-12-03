#version 330 core

in vec2 vUV;
in vec3 vNormal;

out vec4 FragColor;

uniform sampler2D uBaseColorTex; // texture
uniform bool uHasBaseColorTex;   // true if texture exists
uniform vec4 uBaseColorFactor;   // fallback color

void main()
{
    vec3 normal = normalize(vNormal);

    // --- base color ---
    vec4 baseColor = uBaseColorFactor;
    if (uHasBaseColorTex) {
        baseColor = texture(uBaseColorTex, vUV);
    }

    // --- lighting ---
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.5));
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 ambient = 0.2 * baseColor.rgb;
    vec3 diffuse = diff * baseColor.rgb;

    FragColor = vec4(ambient + diffuse, baseColor.a);
}
