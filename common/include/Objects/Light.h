#ifndef LIGHT_H_
#define LIGHT_H_

#include "GL/glew.h"
#include "Shader/GLSL/glsl.h"
#include <glm/glm.hpp>

class Light
{
private:
    GLuint m_VAO;
    glm::vec3 m_lightPos;
    glm::vec3 m_color;
    
    void defaultShaders()
    {
//        const char * vertex = "#version 330 core\nlayout(location = 0) in vec3 pos;\nuniform mat4 model;\nuniform mat4 view\n;uniform mat4 projection;\n\nvoid main()\n{\ngl_Position = projection * view * model * vec4(pos, 1.0f);\n}";
//        const char * fragment = "#version 330 core\nout vec4 color;\nvoid main()\n{\ncolor=vec4(1.0f);}";
//        m_shader.loadShaders_CStr(vertex, fragment);
        m_shader.loadShaders_File("shaders/shader.vert", "shaders/light.frag");
    }
public:
    shaders::GLSL_SHADER m_shader;
    Light()
    {
        m_color = glm::vec3(1.0f);
    }
    
    Light(GLuint vbo, GLuint vao)
    {
        this->defaultShaders();
        
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
        
        m_VAO = vao;
    }

    Light(const char * vertex, const char * fragment, GLuint vbo, GLuint vao)
    {
        m_shader.loadShaders_File(vertex, fragment);
        
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
        
        m_VAO = vao;
    }
    
    ~Light()
    {
        this->turnOff();
    }
    
    void init()
    {
        this->m_lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
    }
    
    void assignBuffers(GLuint vbo, GLuint vao)
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
        
        m_VAO = vao;
    }
    
    void turnOn()
    {
        this->draw();
    }
    
    void turnOff()
    {
        m_shader.unuse();
    }
    
    void draw()
    {        
        glBindVertexArray(m_VAO);
        
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glBindVertexArray(0);
    }
    
    void setColor(float r, float g, float b)
    {
        m_color = glm::vec3(r,g,b);
    }
    
    glm::vec3 getColor()
    {
        return m_color;
    }
    
    glm::vec3 getPos()
    {
        return m_lightPos;
    }
};

#endif