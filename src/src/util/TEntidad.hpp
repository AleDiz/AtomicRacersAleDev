#include <glm/mat4x4.hpp> 
#include <glm/vec3.hpp> 

// clase abstracta de la que heredan el resto de clases que sean tipo entidad que se vayan a dibujar
class TEntidad
{
   virtual void dibujar(glm::mat4 transformacion) = 0;
};