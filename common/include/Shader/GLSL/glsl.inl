namespace shaders
{
    Shader_glsl::Shader_glsl():m_Handle(0){}
    Shader_glsl::~Shader_glsl()
    {
        if(m_Handle > 0)
            glDeleteProgram(m_Handle);
    }
    
    bool Shader_glsl::loadShaders_File(const char * vertShader, const char * fragShader)
    {
        string vsStr = fileToString(vertShader);
        string fsStr = fileToString(fragShader);
        const GLchar * vsStrPtr = vsStr.c_str();
        const GLchar * fsStrPtr = fsStr.c_str();
        
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        
    // Compile Shaders
        glShaderSource(vertexShader, 1, &vsStrPtr, nullptr);
        glCompileShader(vertexShader);
        checkCompileErrors(vertexShader, VERTEX);
        glShaderSource(fragmentShader, 1, &fsStrPtr, nullptr);
        glCompileShader(fragmentShader);
        checkCompileErrors(fragmentShader, FRAGMENT);
        
    // Create Shader Program
        m_Handle = glCreateProgram();
        
        glAttachShader(m_Handle, vertexShader);
        glAttachShader(m_Handle, fragmentShader);
        glLinkProgram(m_Handle);
        checkCompileErrors(m_Handle, PROGRAM);
        
    // After shaders are linked, delete them to free up the memory
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        mUniformLocation.clear();
        
        return true;
    }
    
    bool Shader_glsl::loadShaders_CStr(const char * vector, const char * fragment)
    {
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        
        // Compile Shaders
        glShaderSource(vertexShader, 1, &vector, nullptr);
        glCompileShader(vertexShader);
        checkCompileErrors(vertexShader, VERTEX);
        glShaderSource(fragmentShader, 1, &fragment, nullptr);
        glCompileShader(fragmentShader);
        checkCompileErrors(fragmentShader, FRAGMENT);
        
        // Create Shader Program
        m_Handle = glCreateProgram();
        
        glAttachShader(m_Handle, vertexShader);
        glAttachShader(m_Handle, fragmentShader);
        glLinkProgram(m_Handle);
        checkCompileErrors(m_Handle, PROGRAM);
        
        // After shaders are linked, delete them to free up the memory
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        mUniformLocation.clear();
        
        return true;
    }
    
    void Shader_glsl::use()
    {
        if(m_Handle > 0)
            glUseProgram(m_Handle);
    }
    
    void Shader_glsl::unuse()
    {
        if(m_Handle != 0)
            glUseProgram(0);
    }

    string Shader_glsl::fileToString(const string& filename)
    {
        string str;
        std::ifstream file;
        file.open(filename, std::ios_base::in);
        
        if(!file.is_open())
        {
            std::cerr << "ERROR: cannot open open/read shader file \"" << filename << "\"" << std::endl;
            exit(-1);
        }
        string line;
        while(std::getline(file, line))
        {
            str += line + '\n';
        }
        
        file.close();
        return str;
    }
    
    void Shader_glsl::checkCompileErrors(GLuint shader, ShaderType type)
    {
        int status = 0;
        if(type == PROGRAM)
        {
            glGetProgramiv(m_Handle, GL_LINK_STATUS, &status);
            if(status == GL_FALSE)
            {
                GLint length = 0;
                glGetProgramiv(m_Handle, GL_INFO_LOG_LENGTH, &length);
                string errorLog(length, ' ');
                glGetProgramInfoLog(m_Handle, length, &length, &errorLog[0]);
                std::cerr << "ERROR: Shader Program failed to link! " << errorLog << std::endl;
                exit(-1);
            }
        }
        else    // VERTEX or FRAGMENT
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
            if(status == GL_FALSE)
            {
                GLint length = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
                string errorLog(length, ' ');
                glGetShaderInfoLog(shader, length, &length, &errorLog[0]);
                std::cerr << "ERROR: " << (type == VERTEX ? "Vertex" : "Fragment") << " Shader failed to compile! " << errorLog << std::endl;
                exit(-1);
            }
        }
    }
    
    GLint Shader_glsl::getUniformLocation(const GLchar * name)
    {
        std::map<string, GLint>::iterator it = mUniformLocation.find(name);
        if(it == mUniformLocation.end())
        {
            mUniformLocation[name] = glGetUniformLocation(m_Handle, name);
        }
        return mUniformLocation[name];
    }
    
    void Shader_glsl::setUniform(const GLchar * name, const int i)
    {
        GLint loc = getUniformLocation(name);
        glUniform1i(loc, i);
    }
    
    void Shader_glsl::setUniform(const GLchar * name, const float f)
    {
        GLint loc = getUniformLocation(name);
        glUniform1f(loc, f);
    }
    
    void Shader_glsl::setUniform(const GLchar * name, const glm::vec2& v)
    {
        GLint loc = getUniformLocation(name);
        glUniform2f(loc, v.x, v.y);
    }
    
    void Shader_glsl::setUniform(const GLchar * name, const glm::vec3& v)
    {
        GLint loc = getUniformLocation(name);
        glUniform3f(loc, v.x, v.y, v.z);
    }
    
    void Shader_glsl::setUniform(const GLchar * name, const glm::vec4& v)
    {
        GLint loc = getUniformLocation(name);
        glUniform4f(loc, v.x, v.y, v.z, v.w);
    }
    
    void Shader_glsl::setUniform(const GLchar * name, const glm::mat4& m)
    {
        GLint loc = getUniformLocation(name);
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
    }
    
    void Shader_glsl::setUniform(const GLchar * name, const glm::mat3& m)
    {
        GLint loc = getUniformLocation(name);
        glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(m));
    }
    
    GLuint Shader_glsl::reference()
    {
        return m_Handle;
    }
}