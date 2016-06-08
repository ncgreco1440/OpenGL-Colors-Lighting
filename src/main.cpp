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
#include <Objects/Light.h>
#include <Objects/Camera.h>
#include <Objects/Material.h>
#include <Objects/Shape.h>
// Include SOIL for loading images
#include <SOIL/SOIL.h>

bool masterKey = true;

/*************************************************************
 * Global Variables
 * -----------------
 * Set up global variables for this application
 ************************************************************/
GLFWwindow* window;
GLuint VBO, VAO, IBO, lightVAO, texture1;
const int WIDTH = 800;
const int HEIGHT = 600;
const char * TITLE = "OpenGL::Colors";
shaders::GLSL_SHADER shader;
shaders::GLSL_SHADER lightShader;
Light light;
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
void setMaterials();
void transformObject();
void drawObject();
void setCamera();
void createLamp();
void drawLamp();
void createMaterials();
void loadTexture();
graphics::Material Emerald;
graphics::Material Pearl;
graphics::Material Bronze;
graphics::Material Gold;
graphics::Material Cyan_Plastic;
graphics::Material Red_Plastic;
graphics::Material Green_Rubber;
graphics::Material Yellow_Rubber;
/*************************************************************
 * Camera SetUp
 ************************************************************/
Camera cam(WIDTH, HEIGHT, -90.0f, 0.0f);
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
    
    createMaterials();
    createObject();
    createLamp();
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
        
    // Draw the Cube
        shader.use();
        cam.move(keys);
        setCamera();
        drawObject();
        
    // Set/Draw the light
        lightShader.use();
        glm::mat4 model;
        model = glm::translate(model, light.getPos());
        model = glm::scale(model, glm::vec3(0.2f));
       
        lightShader.setUniform("model", model);
        lightShader.setUniform("view", cam.getView());
        lightShader.setUniform("projection", cam.getProj());
        light.draw();
        
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
    glClearColor((GLclampf)0.0, (GLclampf)0.0, (GLclampf)0.0, (GLclampf)1.0);
    
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
    cam.look(xpos, ypos);
}

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
    cam.zoom(yoffset);
}

void createMaterials()
{
// Emerald
    Emerald.ambient = glm::vec3(0.0, 0.1, 0.0);
    Emerald.diffuse = glm::vec3(0.01, 0.5, 0.01);
    Emerald.specular = glm::vec3(0.7, 0.7, 0.7);
    Emerald.shininess = 64.0f;
// Pearl
    Pearl.ambient = glm::vec3(0.075, 0.075, 0.075);
    Pearl.diffuse = glm::vec3(1.0, 1.0, 1.0);
    Pearl.specular = glm::vec3(0.3, 0.3, 0.3);
    Pearl.shininess = 2.0f;
// Bronze
    Bronze.ambient = glm::vec3(0.08, 0.0, 0.01);
    Bronze.diffuse = glm::vec3(0.804, 0.5, 0.197);
    Bronze.specular = glm::vec3(0.8, 0.8, 0.8);
    Bronze.shininess = 64.0f;
// Gold
    Gold.ambient = glm::vec3(0.1, 0.085, 0.0);
    Gold.diffuse = glm::vec3(0.8, 0.6, 0.0);
    Gold.specular = glm::vec3(0.8, 0.8, 0.8);
    Gold.shininess = 128.0f;
// Cyan Plastic
    Cyan_Plastic.ambient = glm::vec3(0.0f, 0.1f, 0.06f);
    Cyan_Plastic.diffuse = glm::vec3(0.0f, 0.50980392f, 0.50980392f);
    Cyan_Plastic.specular = glm::vec3(0.50196078f, 0.50196078f, 0.50196078f);
    Cyan_Plastic.shininess = 32.0f;
// Red Plastic
    Red_Plastic.ambient = glm::vec3(0.85, 0.0, 0.0);
    Red_Plastic.diffuse = glm::vec3(0.85, 0.0, 0.0);
    Red_Plastic.specular = glm::vec3(0.55, 0.55, 0.55);
    Red_Plastic.shininess = 64.0f;
// Green Rubber
    Green_Rubber.ambient = glm::vec3(0.2, 0.85, 0.2);
    Green_Rubber.diffuse = glm::vec3(0.0, 1.0, 0.0);
    Green_Rubber.specular = glm::vec3(0.2, 0.2, 0.2);
    Green_Rubber.shininess = 8.0f;
// Yellow Rubber
    Yellow_Rubber.ambient = glm::vec3(1.0, 0.91, 0.356);
    Yellow_Rubber.diffuse = glm::vec3(1.0, 0.91, 0.356);
    Yellow_Rubber.specular = glm::vec3(0.1, 0.1, 0.1);
    Yellow_Rubber.shininess = 8.0f;
}

/**************************************************************
 * loadTexture()
 * -------------
 * Loads a single texture using libSOIL.
 *************************************************************/
void loadTexture()
{
// Load Image using SOIL
    int w, h;
    unsigned char * image = SOIL_load_image("textures/container2.png", &w, &h, 0, SOIL_LOAD_RGB);
// Generate and Bind Texture
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
// Free up memory
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
}

/**************************************************************
 * createObject()
 * --------------
 * Creates an object to be rendered on screen. 
 * This can be a 2D or 3D object.
 *************************************************************/
void createObject()
{
    shader.loadShaders_File("shaders/shader.vert", "shaders/material.frag");
    
    loadTexture();
    
    GLfloat vertices[] = {
        // Positions           // Normals           // Texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 8, (GLvoid*)0);  //position
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 8, (GLvoid*)(sizeof(GLfloat) * 3)); //normal
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 8, (GLvoid*)(sizeof(GLfloat) * 6)); //texture
// Enable the locations
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void createLamp()
{
    light.init();
    light.assignBuffers(VBO, lightVAO);
    light.setColor(1.0f, 1.0f, 1.0f);
    lightShader.loadShaders_File("shaders/shader.vert", "shaders/light.frag");
}

void transformObject()
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
    glm::mat3 normal_model;
    
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
    normal_model = glm::transpose(glm::inverse(glm::mat3(model))); // To apply lighting in all correct areas when scaled/rotated
    
    // Apply the light to this object
    //shader.setUniform("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
    //shader.setUniform("lightColor", light.getColor());
    
    shader.setUniform("model", model);
    shader.setUniform("normal_model", normal_model);
}

void setMaterials()
{
    glm::vec3 lightColor;
    lightColor.x = sin(glfwGetTime() * 2.0f);
    lightColor.y = sin(glfwGetTime() * 0.7f);
    lightColor.z = sin(glfwGetTime() * 1.3f);
    
    //glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // Decrease the Influence
    //glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // Low Influence
    
    //shader.setUniform("material.ambient", currentMaterial.ambient);
    shader.setUniform("material.diffuse", 0);
    shader.setUniform("material.specular", glm::vec3(1.0f));
    shader.setUniform("material.shininess", 32.0f);

    
    shader.setUniform("light.position", light.getPos());
    shader.setUniform("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader.setUniform("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    shader.setUniform("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
}

/**************************************************************
 * drawObject()
 * ---------------
 * Draws the object that was created in createObject
 * function
 *************************************************************/
void drawObject()
{
    transformObject();
    setMaterials();
    
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, texture1);
    
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
// Perpective changes based on width and height
    cam.setCamera(WIDTH, HEIGHT);
// Set uniform values
    shader.setUniform("view", cam.getView());
    shader.setUniform("projection", cam.getProj());
    
    shader.setUniform("viewPos", cam.getPosition()); // For specular lighting
}