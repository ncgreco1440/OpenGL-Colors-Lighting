#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
// Including the entire GLM library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// Included for GLM's to_string() function
#include <glm/gtx/string_cast.hpp>
#include <Shader/GLSL/glsl.h>

bool masterKey = true;

/*************************************************************
 * Global Variables
 * -----------------
 * Set up global variables for this application
 ************************************************************/
GLFWwindow* window;
GLuint VBO, VAO, IBO;
const int WIDTH = 800;
const int HEIGHT = 600;
const char * TITLE = "OpenGL::Colors";
shaders::GLSL_SHADER shader;
bool firstMouse = true; // prevents jumpyness of camera on first load
bool jumping = false;
/*************************************************************
 * Global Input
 * ------------
 * Array of bools to represent keys
 ************************************************************/
bool keys[1024];
/*************************************************************
 * Global GLFW and GL Functions
 * ----------------------------
 * Functions for this application
 ************************************************************/
void startApplication();
void runApplication();
void terminateApplication();
bool initGLFW();
bool initGLEW();
void checkForErrors();
void setWindowHints();
void setViewPort();
void error_callback(int error, const char * desc);
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow * window, double xpos, double ypos);
void scroll_callback(GLFWwindow * window, double xoffset, double yoffset);
/*************************************************************
 * Drawing Functions
*************************************************************/
void createObject();
void drawObject();
void setCamera();
/*************************************************************
 * Camera SetUp
 *************************************************************/
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);    // Where the camera starts at initially
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // Along what axis and direction is the front of the camera
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);     // Controls inverted vs normal y-axis/x-axis controls
GLfloat field_of_view = 45.0f;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;
void user_movement();

/**************************************************************
 * main()
 * -----
 * C++ starts up through main
 *************************************************************/
int main(int argc, const char * argv[]) {
    if(masterKey)
    {
        // Run gui application...
        startApplication();
        runApplication();
        terminateApplication();
    }
    else
    {
        // Run console application...
    }
    return 0;
}

/**************************************************************
 * startApplication()
 * -----------------
 * Helper function to run initGLFW and initGLEW, will 
 * terminate the application if any of these fail.
 *************************************************************/
void startApplication()
{
    if(!initGLFW())
        exit(-1);
    if(!initGLEW())
        exit(-1);
    
    createObject();
}

/**************************************************************
 * runApplication()
 * ---------------
 * Helper function to encase all logic behind updating
 * the main loop of the application.
 *************************************************************/
void runApplication()
{
    while (!glfwWindowShouldClose(window))
    {
    // Check for any OpenGL errors
        checkForErrors();
        
    // Poll for and process events
        glfwPollEvents();
        shader.use();
        
    // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        user_movement();
        setCamera();
        drawObject();
        
    // Swap front and back buffers
        glfwSwapBuffers(window);
    }
}

/**************************************************************
 * terminateApplication()
 * ---------------------
 * Helper function to encase all logic behind terminating
 * the application, will clean up any memory on the heap 
 * otherwise not cleaned up by anything else.
 *************************************************************/
void terminateApplication()
{
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
}

/**************************************************************
 * initGLFW()
 * ---------
 * Initialize GLFW, terminate the application if we 
 * fail to do so.
 *************************************************************/
bool initGLFW()
{
// Initialize GLFW
    if (!glfwInit())
        return false;
    
    setWindowHints();
    
// Create the window
    window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }
    
// Make window the current context and set all callback functions
    glfwMakeContextCurrent(window);
    glfwSetErrorCallback(error_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    return true;
}

/**************************************************************
 * initGLEW()
 * ---------
 * Initialize GLEW, terminate the application if we 
 * fail to do so.
 *************************************************************/
bool initGLEW()
{
// Due to a bug in GLEW, glewExperimental is REQUIRED to be set to GL_TRUE
    glewExperimental = GL_TRUE;

// Initialize GLEW
    if(glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW Initialization Failed" << std::endl;
        return false;
    }
    
// Long existing bug causes GL_INVALID_ENUM (1280) error after initialization.
// Clear the error with glGetError()
    glGetError();
    
// !IMPORTANT! see comments above setViewPort implementation for details
    setViewPort();
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE); // Uncomment to figure out if your model's normals are correct

// Set our initial color to grey when the window first opens
    glClearColor((GLclampf)0.2, (GLclampf)0.2, (GLclampf)0.2, (GLclampf)1.0);
    
    return true;
}

/**************************************************************
 * glViewPort()
 * ------------
 * Viewports are a little tricky to figure out due to 
 * differences in DPI of monitors. Because of this, it
 * is NOT appropriate to simply use our window's HEIGHT and WIDTH
 * values to specify our viewport.
 * Instead we need to figure our FrameBufferSize of our window
 * from there we use it's values as params for our 
 * glViewport.
 *************************************************************/
void setViewPort()
{
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);
}

/**************************************************************
 * setWindowHints()
 * ---------------
 * Sets up window hints for GLFW
 *************************************************************/
void setWindowHints()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
}

/**************************************************************
 * checkForErrors()
 * ---------------
 * Prints out any OpenGL errors and then terminates
 * the application.
 *************************************************************/
void checkForErrors()
{
    GLenum error = glGetError();
    if(error != GL_NO_ERROR)
    {
        std::cout << "OpenGL Error! " << error << std::endl;
        exit(error);
    }
}

/**************************************************************
 * error_callback()
 * ---------------
 * Callback function for GLFW errors
 *************************************************************/
void error_callback(int error, const char * desc)
{
    std::cerr << desc << std::endl;
}

/**************************************************************
 * key_callback()
 * ---------------
 * Callback function for key presses and releases. 
 * Sets the boolean value each time a key is 
 * pressed or released and other functions 
 * will run checks to see if the bools are 
 * true and will act accordingly
 *************************************************************/
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) //exit application with 'esc' key
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if(action == GLFW_PRESS)
        keys[key] = true;
    else if(action == GLFW_RELEASE)
        keys[key] = false;
}

void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
    GLfloat sensitivity = 0.15f;
    
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    GLfloat xOffset = (xpos - lastX);
    GLfloat yOffset = (lastY - ypos);
    
    lastX = xpos;
    lastY = ypos;
    
    xOffset *= sensitivity;
    yOffset *= sensitivity;
    
    yaw += xOffset;
    pitch += yOffset;
    
// Constraints for pitch angle
    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;
    
// Camera Variables
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
    if(field_of_view >= 1.0f && field_of_view <= 45.0f)
        field_of_view += yoffset;
    if(field_of_view <= 1.0f)
        field_of_view = 1.0f;
    if(field_of_view >= 45.0f)
        field_of_view = 45.0f;
}

/**************************************************************
 * createObject()
 * --------------
 * Creates an object to be rendered on screen. 
 * This can be a 2D or 3D object.
 *************************************************************/
void createObject()
{
    shader.loadShaders("shaders/shader.vert", "shaders/shader.frag");
    
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
/**************************************************************
 * Note that binding and unbinding VBOs and VAOs is expensive.
 * Therefore, in real applications, you need to figure better
 * practices so that you are more efficient.
 *************************************************************/
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(VAO);
    
// Buffer the vertices data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
/**************************************************************
 * That takes care of our buffers, now we set up our
 * attributes, so our Shaders know what do to with the 
 * data we are about to pass into them.
 *************************************************************/
//layout(location), size, type, should normalize, stride, offset
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, (GLvoid*)0);  //position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, (GLvoid*)(sizeof(GLfloat) * 3)); //color
// Enable the locations
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

/**************************************************************
 * drawObject()
 * ---------------
 * Draws the object that was created in createObject
 * function
 *************************************************************/
void drawObject()
{
    static float angle = 0.0f;
    static float jHeight = 0.0f;
    static bool maxHeightReached = false;
    /**************************************************************
     * model matrix
     * ---------------
     * Within the model's local space, it can be translated, 
     * rotated, and scaled to kingdom come. This particular 
     * matrix, however, has NO effect on our camera, 
     * nor should it. It effects this object and this object
     * ONLY.
     *************************************************************/
    glm::mat4 model, tran, rot;
    if(jumping)
    {
        if(jHeight <= 1.0f && !maxHeightReached)
        {
            jHeight+=0.05;
            if(jHeight >= 1.0f) { maxHeightReached = true; }
        }
        else
        {
            jHeight-=0.05;
        }
        if(jHeight <= 0.0 && jumping) { jumping = false; maxHeightReached = false; }
        tran = glm::translate(tran, glm::vec3(0.0f, jHeight, 0.0f));
    }
    rot = glm::rotate(rot, glm::radians(angle++), glm::vec3(0, 1, 0));
    model = rot * tran;
    if(angle == 360)
        angle = 0;
    shader.setUniform("model", model);
    
    glBindVertexArray(VAO);
    
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    glBindVertexArray(0);
}

/**************************************************************
 * setCamera()
 * ----------
 * Sets up the camera positions for the application
 * this is called every frame, and does in fact 
 * interact with the vertex shader.
 *************************************************************/
void setCamera()
{
// Coordinate Matrices
    glm::mat4 view, proj;
    // The view matrix is directly tied to the camera and thus is updated every frame
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    /**************************************************************
     * Perspective Matrix
     * ------------------
     * Realistically, this should not be set every frame. This
     * is one of those features that if you want to change 
     * you would force the player into a graphical settings
     * menu.
     *************************************************************/
    proj = glm::perspective(glm::radians(field_of_view), (GLfloat)WIDTH/(GLfloat)HEIGHT, 0.1f, 100.0f);
// Set uniform values
    shader.setUniform("view", view);
    shader.setUniform("projection", proj);
}

void user_movement()
{
// Camera Constant
    GLfloat cameraSpeed = 0.2f; //* deltaTime;
// Movement Keys
    if(keys[GLFW_KEY_W])
    {
        cameraPos += cameraSpeed * cameraFront;
    }
    if(keys[GLFW_KEY_A])
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if(keys[GLFW_KEY_S])
    {
        cameraPos -= cameraSpeed * cameraFront;
    }
    if(keys[GLFW_KEY_D])
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if(keys[GLFW_KEY_SPACE])
    {
        cameraPos.y += 0.5 * cameraSpeed;
    }
    if(keys[GLFW_KEY_H])
    {
        jumping = true;
    }
}