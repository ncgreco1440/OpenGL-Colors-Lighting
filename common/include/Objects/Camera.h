#ifndef GRAPHICS_CAMERA_H_
#define GRAPHICS_CAMERA_H_

#include <GL/glew.h>
#include <GL/glfw3.h>
#include <glm/glm.hpp>

class Camera
{
public:
    Camera(int x, int y, float yw, float p): firstlooking(true), cameraPos(glm::vec3(0.0f, 0.0f, 5.0f)),
        cameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), cameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
        field_of_view(45.0f), lastX(x / 2), lastY(y / 2), yaw(yw), pitch(p), cameraSpeed(0.2f),
        view(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)),
        proj(glm::perspective(glm::radians(field_of_view), (GLfloat)x/(GLfloat)x, 0.1f, 100.0f)){}
    
    ~Camera() {};
    
    void setCamera(int w, int h)
    {
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
        proj = glm::perspective(glm::radians(field_of_view), (GLfloat)w/(GLfloat)h, 0.1f, 100.0f);
    }
    
    void move(bool input[])
    {
        if(input[GLFW_KEY_W])
        {
            cameraPos += cameraSpeed * cameraFront;
        }
        if(input[GLFW_KEY_A])
        {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if(input[GLFW_KEY_S])
        {
            cameraPos -= cameraSpeed * cameraFront;
        }
        if(input[GLFW_KEY_D])
        {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        if(input[GLFW_KEY_SPACE])
        {
            cameraPos.y += 0.5 * cameraSpeed;
        }
        if(input[GLFW_KEY_H])
        {
            //jumping = true;
        }
    }
    
    void look(double xpos, double ypos)
    {
        GLfloat sensitivity = 0.15f;
        
        if(firstlooking)
        {
            lastX = xpos;
            lastY = ypos;
            firstlooking = false;
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
    
    void zoom(double yoffset)
    {
        if(field_of_view >= 1.0f && field_of_view <= 45.0f)
            field_of_view += yoffset;
        if(field_of_view <= 1.0f)
            field_of_view = 1.0f;
        if(field_of_view >= 45.0f)
            field_of_view = 45.0f;
    }
    
// Getters /////////////////////////////////////////////////////////////////////////////////
    glm::mat4 getView()
    {
        return view;
    }
    
    glm::mat4 getProj()
    {
        return proj;
    }
    
    glm::vec3 getPosition()
    {
        return cameraPos;
    }
    
    glm::vec3 getFront()
    {
        return cameraFront;
    }
    
    glm::vec3 getUp()
    {
        return cameraUp;
    }
    
private:
    bool firstlooking;
    glm::vec3 cameraPos;            // Where the camera starts at initially
    glm::vec3 cameraFront;          // Along what axis and direction is the front of the camera
    glm::vec3 cameraUp;             // Controls inverted vs normal y-axis/x-axis controls
    GLfloat field_of_view;
    GLfloat lastX;
    GLfloat lastY;
    GLfloat yaw;
    GLfloat pitch;
    GLfloat cameraSpeed;
    glm::mat4 view, proj;           // View and Projection Matrices
};

#endif