#version 450
layout(location=0) out vec4 outColor; // �����ɫ

layout(location=0) in vec3 inWorldPos; // ����ռ�λ��

void main(){
    // ��ɫ����Ч��
    vec3 fluidColor = vec3(1.0, 0.0, 0.0); // ��ɫ

    // �����ɫ
    outColor = vec4(fluidColor, 1.0);
}