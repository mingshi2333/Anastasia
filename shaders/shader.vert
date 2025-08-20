// #version 450

// layout(binding=0)uniform UniformBufferObject{
//     mat4 model;
//     mat4 view;
//     mat4 proj;
// }ubo;

// layout(location=0)in vec3 inPosition;
// layout(location=1)in vec3 inColor;
// layout(location=2) in vec2 inTexCoord;

// layout(location=0)out vec3 fragColor;
// layout(location=1) out vec2 fragTexCoord;
// void main(){
//     gl_Position=ubo.proj*ubo.view*ubo.model*vec4(inPosition,1.);
//     fragColor=inColor;
//     fragTexCoord = inTexCoord;
// }
#version 450


layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

layout(push_constant) uniform Push {
  mat2 transform;
  vec2 offset;
  vec3 color;
} push;

void main() {
gl_Position = vec4(push.transform*position+ push.offset, 0.0, 1.0);
  // fragColor = color;
}