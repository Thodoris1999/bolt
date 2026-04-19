#include "gfx/opengl/OpenglProgram.hpp"
#include "gfx/opengl/GlUtils.hpp"

#include <vector>

namespace bolt {
namespace gfx {

static GLenum csp2oglStageFlag(csp::ShaderStage stage) {
    switch (stage) {
    case csp::ShaderStage::Vertex:
        return GL_VERTEX_SHADER;
    
    case csp::ShaderStage::TessControl:
        return GL_TESS_CONTROL_SHADER;
    
    case csp::ShaderStage::TessEval:
        return GL_TESS_EVALUATION_SHADER;
    
    case csp::ShaderStage::Geometry:
        return GL_GEOMETRY_SHADER;
    
    case csp::ShaderStage::Fragment:
        return GL_FRAGMENT_SHADER;
    
    case csp::ShaderStage::Compute:
        return GL_COMPUTE_SHADER;
    
    default:
        PANIC("Unkown shader stage");
        return GL_COMPUTE_SHADER;
    }
}

static void loadShader(std::string_view name, GLuint& shader, GLenum type) {
    unsigned int size;
    void* source = util::Filesystem::loadFile(name, size);

    shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar* const*)&source, (const GLint*)&size);
    glCompileShader(shader);

#ifndef NDEBUG
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        printf("Shader Source (%s):\n%s\n", name, static_cast<const char*>(source));

        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        RUNTIME_ASSERT(success, infoLog);
    }
#endif

    free(source);
}

static void assertProgramOk(unsigned int program, const csp::ProgramDescriptor* pd) {
#ifndef NDEBUG
    int  success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("Failed to load program with sources:\n");
        for (uint32_t i = 0; i < pd->source_count; ++i) {
            printf("%s\n", pd->ogl_sources[i].filename.data());
        }
        RUNTIME_ASSERT(success, infoLog);
    }
#endif
}

OpenglProgram::OpenglProgram(const csp::ProgramDescriptor* pd) : RenderProgram(pd) {
    std::vector<GLuint> shaders;
    shaders.reserve(pd->source_count);

    mId = glCreateProgram();

    for (uint32_t i = 0; i < pd->source_count; ++i) {
        const auto& src = pd->ogl_sources[i];

        GLuint shader;
        loadShader(src.filename, shader, csp2oglStageFlag(src.stage));

        glAttachShader(mId, shader);
        shaders.push_back(shader);
    }

    glLinkProgram(mId);
    assertProgramOk(mId, pd);
    glCheckError();

    for (GLuint shader : shaders) {
        glDeleteShader(shader);
    }
}

} // gfx
} // bolt
