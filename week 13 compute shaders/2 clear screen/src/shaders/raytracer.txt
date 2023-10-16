#version 430

layout(local_size_x = 8, local_size_y = 8) in;
layout(rgba32f, binding = 0) uniform image2D colorbuffer;

void main() {

    ivec2 pixelPos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 screen_size = imageSize(colorbuffer);
    if (pixelPos.x >= screen_size.x || pixelPos.y >= screen_size.y) {
        return;
    }
    
    imageStore(colorbuffer, pixelPos, vec4(1.0, 0.75, 0.5, 1.0));
}