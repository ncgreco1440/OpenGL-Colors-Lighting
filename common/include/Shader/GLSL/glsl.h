#ifndef SHADERPROG_H_
#define SHADERPROG_H_

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
using std::string;

namespace shaders
{
    
    class Shader_glsl
    {
    public:
        enum ShaderType
        {
            VERTEX,
            FRAGMENT,
            GEOMETRY,
            TESSILATION,
            PROGRAM
        };
        Shader_glsl();
        ~Shader_glsl();
        bool loadShaders_File(const char *, const char *);
        bool loadShaders_CStr(const char *, const char *);
        void use();
        void unuse();
        void setUniform(const GLchar *, const int i);
        void setUniform(const GLchar *, const float f);
        void setUniform(const GLchar *, const glm::vec2& v);
        void setUniform(const GLchar *, const glm::vec3& v);
        void setUniform(const GLchar *, const glm::vec4& v);
        void setUniform(const GLchar * name, const glm::mat4& m);
        void setUniform(const GLchar * name, const glm::mat3& m);
        GLuint reference();
    private:
        string fileToString(const string&);
        void checkCompileErrors(GLuint, ShaderType);
        GLint getUniformLocation(const GLchar *);
    
        GLuint m_Handle;
        std::map<string, GLint> mUniformLocation;
    };
    
    typedef Shader_glsl GLSL_SHADER;
}

#ifndef GLSL_EXT_TEMPLATE
#define GLSL_EXT_TEMPLATE
#include <Shader/GLSL/glsl.inl>
#endif

#endif
