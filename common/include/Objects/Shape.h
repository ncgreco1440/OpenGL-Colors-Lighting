#ifndef GRAPHICS_SHAPES_H_
#define GRAPHICS_SHAPES_H_

#include <GL/glew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Objects/Material.h>

namespace graphics
{
    class Shape
    {
    public:
        Shape();
        virtual ~Shape();
    };
    
    class Cube : public Shape
    {
    public:
        Cube(GLuint vbo, GLuint vao);
        ~Cube();
        
        void draw();
        void toon();
        void setMaterial(const Material& m);
        Material getMaterial() const;
        
        void rotate(float angle, const glm::vec3& v);
        //void rotate(float x, float y, float z, float w = 0);
        glm::mat4 getRotate() const;
        
        void translate(const glm::vec3& v);
        void translate(float x, float y, float z);
        glm::mat4 getTranslate() const;
        
        void scale(const glm::vec3& v);
        void scale(float x, float y, float z);
        glm::mat4 getScale() const;
        
        glm::mat3 getNormalModel() const;
        glm::mat4 getModel() const;
    private:
        GLuint m_vbo, m_vao;
        Material m_material;
        glm::mat4 m_model, m_translate, m_scale, m_rotation;
        glm::mat3 m_normal_model;
        Cube(); // Set default constructor to private to prevent usage
    };
}

#endif