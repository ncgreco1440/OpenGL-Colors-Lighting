#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>

/*************************************************************
 * Global Variables
 * -----------------
 * Set up global variables for this application
 ************************************************************/
GLFWwindow* window;
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
void error_callback(int error, const char * desc);

/**************************************************************
 * main()
 * -----
 * C++ starts up through main
 *************************************************************/
int main(int argc, const char * argv[]) {
    startApplication();
    runApplication();
    terminateApplication();
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
        
    // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
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
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return false;
    }
    
// Make window the current context
    glfwMakeContextCurrent(window);
    glfwSetErrorCallback(error_callback);
    
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

// Set our initial color to grey when the window first opens
    glClearColor((GLclampf)0.8, (GLclampf)0.8, (GLclampf)0.8, (GLclampf)1.0);
    
    return true;
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
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
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
