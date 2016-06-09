#ifndef GRAPHICS_LIGHT_H_
#define GRAPHICS_LIGHT_H_

#include "GL/glew.h"
#include "Shader/GLSL/glsl.h"
#include <glm/glm.hpp>
#include <string>

class Light
{
private:
    shaders::GLSL_SHADER m_shader;
    GLuint m_VAO;
    glm::vec3 m_color;
    glm::vec3 m_lightDir;
    glm::vec3 m_lightPos;
    int m_type;
    
    void defaultShaders()
    {
        string vertex = "#version 330 core\nlayout(location = 0) in vec3 pos;\nuniform mat4 model;\nuniform mat4 view\n;uniform mat4 projection;\n\nvoid main()\n{\ngl_Position = projection * view * model * vec4(pos, 1.0f);\n}";
        string fragment = "#version 330 core\nout vec4 color;\nuniform vec4 lightClr;\nvoid main()\n{\ncolor=vec4(0.6f);\ncolor=lightClr;\n}";
        m_shader.loadShaders_Str(vertex, fragment);
    }
public:
    enum {DIRECTION, POINT};
    Light():m_color(glm::vec3(1.0f)), m_lightDir(glm::vec3(0.0f)), m_lightPos(glm::vec3(0.0f)), m_shader()
    {}
    
    Light(GLuint vbo, GLuint vao):m_color(glm::vec3(1.0f)), m_lightDir(glm::vec3(0.0f)), m_lightPos(glm::vec3(0.0f)), m_shader()
    {
        assignBuffers(vbo, vao);
    }

    Light(const char * vertex, const char * fragment, GLuint vbo, GLuint vao):m_color(glm::vec3(1.0f)), m_lightDir(glm::vec3(0.0f)), m_lightPos(glm::vec3(0.0f)), m_shader()
    {
        assignBuffers(vbo, vao);
    }
    
    ~Light()
    {
        this->turnOff();
    }
    
    void init()
    {
        this->defaultShaders();
    }
    
    void init(const char * vertex, const char * fragment)
    {
        this->defaultShaders();
    }
    
    void assignBuffers(GLuint vbo, GLuint vao)
    {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
        
        m_VAO = vao;
    }
    
    void turnOn()
    {
        m_shader.use();
        
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
    
    void setType(int t)
    {
        m_type = t;
    }
    
    void setColor(float r, float g, float b)
    {
        m_color = glm::vec3(r,g,b);
    }
    
    glm::vec3 getColor()
    {
        return m_color;
    }
    
    void setPos(const glm::vec3& pos)
    {
        this->m_lightPos = pos;
    }

    
    glm::vec3 getPos()
    {
        return m_lightPos;
    }
    
    void setDir(const glm::vec3& dir)
    {
        this->m_lightDir = dir;
    }
    
    glm::vec3 getDir()
    {
        return m_lightDir;
    }
};

#endif