#include "Shader.h"
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#ifndef __DEPTHSHADER_H__
#define __DEPTHSHADER_H__

struct DepthShader : Shader {
    glm::mat4 modelview = glm::mat4(1.0f); GLuint modelview_loc;
    glm::mat4 projection = glm::mat4(1.0f); GLuint projection_loc;
    glm::mat4 depthMVP = glm::mat4(1.0f);
    GLuint depthMVP_loc;
    
    
    void initUniforms(){
        modelview_loc  = glGetUniformLocation( program, "modelview" );
        projection_loc = glGetUniformLocation( program, "projection" );
        depthMVP_loc = glGetUniformLocation( program, "depthMVP" );
    }
    void setUniforms(){
        glUniformMatrix4fv(modelview_loc, 1, GL_FALSE, &modelview[0][0]);
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(depthMVP_loc, 1, GL_FALSE, &depthMVP[0][0]);
    }
};

#endif 
