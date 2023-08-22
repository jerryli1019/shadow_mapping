/**************************************************
Scene.cpp contains the implementation of the draw command
*****************************************************/
#include "Scene.h"
#include "Cube.h"
#include "Obj.h"
#include "Scene.inl"


using namespace glm;
glm::mat4 depthMVP;
void Scene::draw(void){
    glm::vec3 lightInvDir = light["sun"]->position;
    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);
    glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0,0,0), glm::vec3(0,1,0));
    depthMVP = depthProjectionMatrix * depthViewMatrix;
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    const GLuint SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
    GLuint depthMap;

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT,
                GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0); 
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glUseProgram(shader2 -> program);
    glCullFace(GL_FRONT);
    draw(shader2);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, 800, 600);
    glUseProgram(shader -> program);
    glUniform1i(glGetUniformLocation(shader->program, "depthMap"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    draw(shader);
}

void Scene::draw(DepthShader* shader){
    camera -> computeMatrices();
    std::stack < Node* > dfs_stack;
    std::stack < mat4 >  matrix_stack;
    Node* cur = node["world"];
    mat4 cur_VM = camera -> view;
    dfs_stack.push(cur);
    matrix_stack.push(cur_VM);
    int total_number_of_edges = 0; 
    for ( const auto &n : node ) total_number_of_edges += n.second->childnodes.size();
    while( ! dfs_stack.empty() ){
        if ( dfs_stack.size() > total_number_of_edges ){
            std::cerr << "Error: The scene graph has a closed loop." << std::endl;
            exit(-1);
        }
        cur = dfs_stack.top();  dfs_stack.pop();
        cur_VM = matrix_stack.top();
        matrix_stack.pop();
        for ( size_t i = 0; i < cur -> models.size(); i++ ){
            shader -> modelview = cur_VM * cur->modeltransforms[i];
            shader -> depthMVP = depthMVP * (inverse(camera->view) * cur_VM * cur->modeltransforms[i]);
            shader -> setUniforms();
            ( cur -> models[i] ) -> geometry -> draw();
        }
        for ( size_t i = 0; i < cur -> childnodes.size(); i++ ){
            dfs_stack.push( cur -> childnodes[i] );
            matrix_stack.push( cur_VM * cur->childtransforms[i] );
        }
    }
}

void Scene::draw(SurfaceShader* shader){
    camera -> computeMatrices();
    shader -> view = camera -> view;
    shader -> projection = camera -> proj;
    shader -> nlights = light.size();
    shader -> lightpositions.resize( shader -> nlights );
    shader -> lightcolors.resize( shader -> nlights );
    int count = 0;
    for (std::pair<std::string, Light*> entry : light){
        shader -> lightpositions[ count ] = (entry.second) -> position;
        shader -> lightcolors[ count ] = (entry.second) -> color;
        count++;
    }
    std::stack < Node* > dfs_stack;
    std::stack < mat4 >  matrix_stack;
    Node* cur = node["world"];
    mat4 cur_VM = camera -> view; 
    dfs_stack.push(cur);
    matrix_stack.push(cur_VM);
    int total_number_of_edges = 0; 
    for ( const auto &n : node ) total_number_of_edges += n.second->childnodes.size();
    while( ! dfs_stack.empty() ){
        if ( dfs_stack.size() > total_number_of_edges ){
            std::cerr << "Error: The scene graph has a closed loop." << std::endl;
            exit(-1);
        }
        cur = dfs_stack.top();  dfs_stack.pop();
        cur_VM = matrix_stack.top();
        matrix_stack.pop();
        for ( size_t i = 0; i < cur -> models.size(); i++ ){
            shader -> modelview = cur_VM * cur->modeltransforms[i];
            shader -> depthMVP = depthMVP * (inverse(camera->view) * cur_VM * cur->modeltransforms[i]);
            shader -> material  = ( cur -> models[i] ) -> material;
            shader -> setUniforms();
            ( cur -> models[i] ) -> geometry -> draw();
        }
        for ( size_t i = 0; i < cur -> childnodes.size(); i++ ){
            dfs_stack.push( cur -> childnodes[i] );
            matrix_stack.push( cur_VM * cur->childtransforms[i] );
        }
    }
}
