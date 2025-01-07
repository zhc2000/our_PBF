#version 450
layout(location=0) in vec3 inPosition; // 输入位置

// 输出到片段着色器的插值变量
layout(location=0) out vec3 outWorldPos; // 世界空间位置

layout(binding=0) uniform UniformRenderingObject{
    float zNear;
    float zFar;
    float fovy;
    float aspect;

    mat4 model;
    mat4 view;
    mat4 projection;
    mat4 invProjection;

    float particleRadius;
};

void main(){
    // 将位置从模型空间转换到裁剪空间
    vec4 worldPosition = model * vec4(inPosition, 1.0); // 将位置从模型空间转换到世界空间
    vec4 viewPosition = view * worldPosition; // 将位置从世界空间转换到视图空间
    vec4 clipPosition = projection * viewPosition; // 将位置从视图空间转换到裁剪空间

    // 将世界空间位置传递给片段着色器
    outWorldPos = worldPosition.xyz;

    // 设置裁剪空间位置
    gl_Position = clipPosition;

    // 计算点大小
    float nearHeight = 2.0 * zNear * tan(fovy / 2.0);
    float scale = 800.0 / nearHeight;
    float nearSize = particleRadius * zNear / (-viewPosition.z);
    gl_PointSize = 2.0 * scale * nearSize;
}