#ifndef GRAPHICS_MATERIAL_H_
#define GRAPHICS_MATERIAL_H_

#include <GL/glew.h>
#include <glm/glm.hpp>


/***********************************************************
    1) Set the ambient and diffuse component to the color 
    you’d like the object to have
 
    2) Set the specular component of the object to a 
    medium-bright color
 
    3) Adjust shininess as you will
 **********************************************************/
struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

/**********************************************************
 Predefined Materials
 *********************************************************/
Material material_Emerald;
Material material_Pearl;
Material material_Bronze;
Material material_Gold;
Material material_Cyan_Plastic;
Material material_Red_Plastic;
Material material_Green_Rubber;
Material material_Yellow_Rubber;

#endif
