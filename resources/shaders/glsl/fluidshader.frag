#version 450
layout(location=0) out vec4 outColor; // 输出颜色

layout(location=0) in vec3 inWorldPos; // 世界空间位置

void main(){
    // 红色流体效果
    vec3 fluidColor = vec3(1.0, 0.0, 0.0); // 红色

    // 输出颜色
    outColor = vec4(fluidColor, 1.0);
}