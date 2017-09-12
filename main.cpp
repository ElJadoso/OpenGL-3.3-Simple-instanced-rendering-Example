#include <SDL2/SDL.h>
#define GL3_PROTOTYPES 1
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

GLuint rendering_program;


void CompileProgram();

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window;
    SDL_GLContext opengl_context;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow("Title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
    opengl_context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    glewExperimental = GL_TRUE;
    glewInit();

    CompileProgram();
    glUseProgram(rendering_program);
    glPointSize(15);

    GLuint vertex_array_object;    // VAO
    GLuint vertex_buffer_coord;    // VBO
    GLuint vertex_buffer_position; // VBO
    GLfloat vertex_coord_data[] = {       // Triangle coords
        -0.3, -0.3, 0.0,
         0.3, -0.3, 0.0,
         0.0,  0.3, 0.0
    };
    GLfloat vertex_position_data[] = {   // Triangle object position
        -0.5,  0.5,
         0.5, -0.5
    };

    glCreateVertexArrays(1, &vertex_array_object);
    glBindVertexArray(vertex_array_object);
    // This buffer contains the vertex coords of the triangle.
    glCreateBuffers(1, &vertex_buffer_coord);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_coord);
    glBufferData(GL_ARRAY_BUFFER, 3*3*sizeof(GLfloat), vertex_coord_data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    // This buffer contains the instanced triangles position.
    glCreateBuffers(1, &vertex_buffer_position);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_position);
    glBufferData(GL_ARRAY_BUFFER, 2*2*sizeof(GLfloat), vertex_position_data, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);

    // This means: the vertex attrib #1 (vec2 instance_position) will be splited one time for each triangle object.
    glVertexAttribDivisor(1, 1);
    // After this, the triangle object 1, at vertex attrib #1 will have the value -.5 and .5 (x,y).
    // Triangle object 2 will have .5 and -.5 (x,y).

    bool running = true;
    while(running)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                running = false;
        }
        glClear(GL_COLOR_BUFFER_BIT);

        //glDrawArrays(GL_POINTS, 0, 1);
        // Draw
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 2);

        SDL_GL_SwapWindow(window);
    }

    glDeleteBuffers(1, &vertex_buffer_position);
    glDeleteBuffers(1, &vertex_buffer_coord);

    glDeleteVertexArrays(1, &vertex_array_object);

    glDeleteProgram(rendering_program);

    SDL_GL_DeleteContext(opengl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void CompileProgram()
{
    GLchar* vertex_shader_source[] = {
        "#version 330 core                                  \n"
        "layout(location=0) in vec3 vertex_coord;           \n"
        "layout(location=1) in vec2 instance_position;      \n"
        "void main() {                                      \n"
        "gl_Position = vec4(vertex_coord + vec3(instance_position,0.0), 1.0);            \n"
        "}                                                  \n"
    };
    GLchar* fragment_shader_source[] = {
        "#version 330 core                                  \n"
        "out vec3 color;                                    \n"
        "void main() {                                      \n"
        "color = vec3(0.2, 0.8, 0.1);                       \n"
        "}                                                  \n"
    };

    GLuint vertex_shader;
    GLuint fragment_shader;

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    rendering_program = glCreateProgram();
    glAttachShader(rendering_program, vertex_shader);
    glAttachShader(rendering_program, fragment_shader);
    glLinkProgram(rendering_program);

    glDetachShader(rendering_program, vertex_shader);
    glDetachShader(rendering_program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}











