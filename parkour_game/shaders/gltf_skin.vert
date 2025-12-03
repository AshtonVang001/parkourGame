#version 330 core

// Vertex attributes
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(location = 3) in vec4 aJoints;  // joint indices
layout(location = 4) in vec4 aWeights; // joint weights

// Uniforms
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 u_Joints[100]; // adjust MAX_JOINTS if needed

out vec2 vUV;
out vec3 vNormal;

void main() 
{
    // --- compute skinning matrix ---
    mat4 skinMat = mat4(0.0);
    skinMat += aWeights.x * u_Joints[int(aJoints.x)];
    skinMat += aWeights.y * u_Joints[int(aJoints.y)];
    skinMat += aWeights.z * u_Joints[int(aJoints.z)];
    skinMat += aWeights.w * u_Joints[int(aJoints.w)];

    // --- apply skinning + model/view/projection ---
    vec4 worldPos = uModel * skinMat * vec4(aPos, 1.0);
    gl_Position = uProj * uView * worldPos;

    // --- transform normal safely ---
    vec3 n = aNormal;
    if(length(n) < 0.001) n = vec3(0.0, 1.0, 0.0); // fallback normal
    mat3 normalMat = transpose(inverse(mat3(uModel * skinMat)));
    vNormal = normalize(normalMat * n);

    // pass UV (default 0 if missing)
    vUV = aUV;
}
