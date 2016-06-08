#include <Objects/Shape.h>

namespace graphics
{
    float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

// Shape
    Shape::Shape()
    {
        //Constructor...
    };
    Shape::~Shape()
    {
        //Destructor...
    };

// Cube
    Cube::Cube(GLuint vbo, GLuint vao)
    {
        glGenBuffers(1, &vbo);
        glGenVertexArrays(1, &vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindVertexArray(vao);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6, (GLvoid*)0);  //position
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 6, (GLvoid*)(sizeof(GLfloat) * 3));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        m_vbo = vbo;
        m_vao = vao;
    };
    
    Cube::~Cube()
    {
        //Destructor...
    };
    
    void Cube::draw()
    {
        glBindVertexArray(m_vao);
        
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glBindVertexArray(0);
    }
    
    void Cube::toon()
    {
        m_model = m_rotation * m_translate * m_scale;
        m_normal_model = glm::transpose(glm::inverse(glm::mat3(m_model)));
    }
    
    void Cube::setMaterial(const graphics::Material &m)
    {
        m_material = m;
    }
    
    Material Cube::getMaterial() const
    {
        return m_material;
    }
    
    glm::mat4 Cube::getModel() const
    {
        return m_model;
    }
    
    glm::mat4 Cube::getScale() const
    {
        return m_scale;
    }
    
    glm::mat4 Cube::getTranslate() const
    {
        return m_translate;
    }
    
    glm::mat4 Cube::getRotate() const
    {
        return m_rotation;
    }
    
    glm::mat3 Cube::getNormalModel() const
    {
        return m_normal_model;
    }
    
    void Cube::rotate(float angle, const glm::vec3& v)
    {
        m_rotation = glm::rotate(m_rotation, glm::radians(angle), v);
    }
    
    void Cube::translate(const glm::vec3& v)
    {
        m_translate = glm::translate(m_translate, v);
    }
    
    void Cube::translate(float x, float y, float z)
    {
        m_translate = glm::translate(m_translate, glm::vec3(x, y, z));
    }
    
    void Cube::scale(const glm::vec3& v)
    {
        m_scale = glm::scale(m_scale, v);
    }
    
    void Cube::scale(float x, float y, float z)
    {
        m_scale = glm::scale(m_scale, glm::vec3(x, y, z));
    }
}
