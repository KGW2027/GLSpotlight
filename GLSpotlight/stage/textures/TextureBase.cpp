#include "TextureBase.h"

#include <fstream>
#include <gl/glew.h>
#include <soil.h>

std::map<identifier, tid> TextureBase::registered_textures_;

void TextureBase::load_texture(const char* file_path, identifier key)
{
    int width, height;
    unsigned char* image = SOIL_load_image(file_path, &width, &height, 0, SOIL_LOAD_RGBA);

    if(!image)
    {
        printf("Failed load to image %s.\n", file_path);
        exit(EXIT_FAILURE);
    }

    tid t_id;

    // Generate OpenGL texture
    glGenTextures(1, &t_id);
    glBindTexture(GL_TEXTURE_2D, t_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);

    registered_textures_.insert_or_assign(key, t_id);
    printf("Complate Texture Load %s to %d\n", key, t_id);
}
