#pragma once
#include <map>

typedef unsigned char ubyte     ;
typedef unsigned int  tid       ;
typedef const char*   identifier;

class TextureBase
{
    static std::map<identifier, tid> registered_textures_;
    
public:
    
    static void load_texture(const char* file_path, identifier key);
    static tid  get_texture (const identifier key) { return registered_textures_.at(key); }
};
