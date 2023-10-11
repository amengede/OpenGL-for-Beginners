#include "post_system.h"

PostSystem::PostSystem(unsigned int shader, unsigned int colorbuffer):
shader(shader),
colorbuffer(colorbuffer) {
    
}
    
void PostSystem::update() {

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    //Screen
    glUseProgram(shader); 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorbuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}