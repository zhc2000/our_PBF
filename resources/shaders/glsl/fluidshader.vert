#version 450
layout(location=0) in vec3 inPosition; // ����λ��

// �����Ƭ����ɫ���Ĳ�ֵ����
layout(location=0) out vec3 outWorldPos; // ����ռ�λ��

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
    // ��λ�ô�ģ�Ϳռ�ת�����ü��ռ�
    vec4 worldPosition = model * vec4(inPosition, 1.0); // ��λ�ô�ģ�Ϳռ�ת��������ռ�
    vec4 viewPosition = view * worldPosition; // ��λ�ô�����ռ�ת������ͼ�ռ�
    vec4 clipPosition = projection * viewPosition; // ��λ�ô���ͼ�ռ�ת�����ü��ռ�

    // ������ռ�λ�ô��ݸ�Ƭ����ɫ��
    outWorldPos = worldPosition.xyz;

    // ���òü��ռ�λ��
    gl_Position = clipPosition;

    // ������С
    float nearHeight = 2.0 * zNear * tan(fovy / 2.0);
    float scale = 800.0 / nearHeight;
    float nearSize = particleRadius * zNear / (-viewPosition.z);
    gl_PointSize = 2.0 * scale * nearSize;
}