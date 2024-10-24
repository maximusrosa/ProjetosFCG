//     Universidade Federal do Rio Grande do Sul
//             Instituto de Informática
//       Departamento de Informática Aplicada
//
//    INF01047 Fundamentos de Computação Gráfica
//               Prof. Eduardo Gastal
//
//                   LABORATÓRIO 1
//

// Arquivos "headers" padrões de C podem ser incluídos em um
// programa C++, sendo necessário somente adicionar o caractere
// "c" antes de seu nome, e remover o sufixo ".h". Exemplo:
//    #include <stdio.h> // Em C
//  vira
//    #include <cstdio> // Em C++
//
#include <cmath>
#include <cstdio>
#include <cstdlib>

// Headers abaixo são específicos de C++
#include <string>
#include <fstream>
#include <sstream>

// Headers das bibliotecas OpenGL
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional

// Headers locais, definidos na pasta "include/"
#include "utils.h"

// Definição de constantes
#define M_PI 3.14159265358979323846
#define NUM_VERTICES 16

// Declaração de várias funções utilizadas em main().  Essas estão definidas
// logo após a definição de main() neste arquivo.
void LoadShadersFromFiles(); // Carrega os shaders de vértice e fragmento, criando um programa de GPU
GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
void LoadShader(const char* filename, GLuint shader_id); // Função utilizada pelas duas acima
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU
GLuint BuildDigit(int digit, GLfloat deslocamento);

// Funções callback para comunicação com o sistema operacional e interação do
// usuário. Veja mais comentários nas definições das mesmas, abaixo.
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod);

// Variáveis que definem um programa de GPU (shaders). Veja função LoadShadersFromFiles().
GLuint g_GpuProgramID = 0;

int main()
{
    // Inicializamos a biblioteca GLFW
    int success = glfwInit();
    if (!success)
    {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definimos o callback para erros e os parâmetros da janela
    glfwSetErrorCallback(ErrorCallback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criamos uma janela
    GLFWwindow* window = glfwCreateWindow(500, 500, "INF01047 - 342337 - Maximus Borges da Rosa", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Configurações de contexto
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Informações da GPU
    const GLubyte *vendor = glGetString(GL_VENDOR);
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *glversion = glGetString(GL_VERSION);
    const GLubyte *glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    printf("GPU: %s, %s, OpenGL %s, GLSL %s\n", vendor, renderer, glversion, glslversion);

    // Carregamento dos shaders
    LoadShadersFromFiles();

    // Construímos a representação de um dígito
    GLuint VAOs[4];

    // "Pesos":            2^3,     2^2,        2^1,      2^0
    GLfloat posicao[4] = {1.85f, 1.85f / 3, -1.85f / 3, -1.85f};

    // Loop de renderização
    while (!glfwWindowShouldClose(window))
    {
        // Definimos a cor do fundo como branco
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Ativamos o programa da GPU
        glUseProgram(g_GpuProgramID);

        // Obtemos o tempo atual em segundos e convertemos para um contador de 4 bits
        int time = static_cast<int>(glfwGetTime()) % 16; // 16 = 2^4 (número de valores possíveis com 4 bits)

        for (int i = 0; i < 4; ++i)
        {
            // Calcula o bit correspondente para a posição `i`
            int digit = (time >> i) & 1;

            // Constrói o dígito e armazena o VAO correspondente
            VAOs[i] = BuildDigit(digit, posicao[i]);

            // Associa o VAO e desenha os elementos
            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLE_STRIP, 32 + 2, GL_UNSIGNED_BYTE, 0);
        }

        // Desassocia o VAO
        glBindVertexArray(0);

        // Troca os buffers e processa os eventos de entrada
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Finalizamos os recursos do sistema operacional
    glfwTerminate();

    return 0;
}


GLuint BuildDigit(int digit, GLfloat deslocamento_x){
    if (digit == 1){
        GLfloat encolhimento = 0.4f;

        // A posição de cada vértice é definida por coeficientes em "normalized
        // device coordinates" (NDC), onde cada coeficiente está entre -1 e 1.
        // Este vetor "NDC_coefficients" define a GEOMETRIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
        //

        GLfloat NDC_coefficients[] = {
                (-0.15f + deslocamento_x) * encolhimento, 0.77f * encolhimento, 0.0f, 1.0f, // Vértice 0
                (-0.15f + deslocamento_x) * encolhimento, -0.77f * encolhimento, 0.0f, 1.0f,  // Vértice 1
                (0.15f + deslocamento_x) * encolhimento, 0.77f * encolhimento, 0.0f, 1.0f,   // Vértice 2
                (0.15f + deslocamento_x) * encolhimento, -0.77f * encolhimento, 0.0f, 1.0f,   // Vértice 3

                (-0.4f + deslocamento_x) * encolhimento, 0.35f * encolhimento, 0.0f, 1.0f, // Vértice 4

        };

        GLuint VBO_NDC_coefficients_id;
        glGenBuffers(1, &VBO_NDC_coefficients_id);

        GLuint vertex_array_object_id;
        glGenVertexArrays(1, &vertex_array_object_id);

        glBindVertexArray(vertex_array_object_id);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_NDC_coefficients_id);

        glBufferData(GL_ARRAY_BUFFER, sizeof(NDC_coefficients), NULL, GL_STATIC_DRAW);

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(NDC_coefficients), NDC_coefficients);

        GLuint location = 0;
        GLint  number_of_dimensions = 4;
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(location);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Agora repetimos todos os passos acima para atribuir um novo atributo a
        // cada vértice: uma cor (veja slides 109-112 do documento Aula_03_Rendering_Pipeline_Grafico.pdf e slide 111 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
        // Tal cor é definida como coeficientes RGBA: Red, Green, Blue, Alpha;
        // isto é: Vermelho, Verde, Azul, Alpha (valor de transparência).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.

        GLfloat color_coefficients[] = {
                //  R     G     B     A
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
                0.0f, 0.0f, 1.0f, 1.0f,
        };

        GLuint VBO_color_coefficients_id;
        glGenBuffers(1, &VBO_color_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(color_coefficients), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color_coefficients), color_coefficients);
        location = 1;
        number_of_dimensions = 4;
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Vamos então definir dois triângulos utilizando os vértices do array
        // NDC_coefficients. O primeiro triângulo é formado pelos vértices 0,1,2;
        // e o segundo pelos vértices 2,1,3. Note que usaremos o modo de renderização
        // GL_TRIANGLES na chamada glDrawElements() dentro de main(). Veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf.
        //
        // Este vetor "indices" define a TOPOLOGIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
        //

        GLubyte indices[] = {0, 1, 2,  1, 3, 2,  0, 2, 4};
        /* GLubyte indices[] = {0, 1, 2, 3, 0, 0, 4, 2}; */

        GLuint indices_id;
        glGenBuffers(1, &indices_id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);

        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);

        glBindVertexArray(0);

        // Retornamos o ID do VAO. Isso é tudo que será necessário para renderizar
        // os triângulos definidos acima.
        return vertex_array_object_id;
    }

    else{
        // A posição de cada vértice é definida por coeficientes em "normalized
        // device coordinates" (NDC), onde cada coeficiente está entre -1 e 1.
        // Este vetor "NDC_coefficients" define a GEOMETRIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
        //

        GLfloat encolhimento = 0.4f;      // Fator de encolhimento geral

        GLfloat innerRadius = 0.4f;  // Raio do círculo interno
        GLfloat outerRadius = 0.6f;  // Raio do círculo externo

        // Defina os fatores de escala para os eixos X e Y
        float scaleX = 0.9f; // Escala para o eixo X (alonga ou comprime a elipse horizontalmente)
        float scaleY = 1.3f; // Escala para o eixo Y (alonga ou comprime a elipse verticalmente)

        GLfloat NDC_coefficients[4 * (NUM_VERTICES * 2)];

        // Ângulo dos triângulos
        float angle = 2 * M_PI / NUM_VERTICES; // 360 graus (em radianos) dividido pelo número de vértices do "círculo"

        for (int i = 0; i < NUM_VERTICES; i++)
        {
            // Vértices externos (Elipse)
            NDC_coefficients[i * 4 + 0] = (outerRadius * cos(i * angle) * scaleX + deslocamento_x) * encolhimento;  // X
            NDC_coefficients[i * 4 + 1] = (outerRadius * sin(i * angle) * scaleY) * encolhimento;                    // Y

            NDC_coefficients[i * 4 + 2] = 0.0f;                                                                      // Z
            NDC_coefficients[i * 4 + 3] = 1.0f;                                                                      // W

            // Vértices internos (Elipse)
            NDC_coefficients[(i + NUM_VERTICES) * 4 + 0] = (innerRadius * cos(i * angle) * scaleX + deslocamento_x) * encolhimento;  // X
            NDC_coefficients[(i + NUM_VERTICES) * 4 + 1] = (innerRadius * sin(i * angle) * scaleY) * encolhimento;                    // Y

            NDC_coefficients[(i + NUM_VERTICES) * 4 + 2] = 0.0f;                                                                      // Z
            NDC_coefficients[(i + NUM_VERTICES) * 4 + 3] = 1.0f;                                                                      // W
        }

        GLuint VBO_NDC_coefficients_id;
        glGenBuffers(1, &VBO_NDC_coefficients_id);

        GLuint vertex_array_object_id;
        glGenVertexArrays(1, &vertex_array_object_id);

        glBindVertexArray(vertex_array_object_id);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_NDC_coefficients_id);

        glBufferData(GL_ARRAY_BUFFER, sizeof(NDC_coefficients), NULL, GL_STATIC_DRAW);

        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(NDC_coefficients), NDC_coefficients);

        GLuint location = 0;
        GLint  number_of_dimensions = 4;
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(location);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Agora repetimos todos os passos acima para atribuir um novo atributo a
        // cada vértice: uma cor (veja slides 109-112 do documento Aula_03_Rendering_Pipeline_Grafico.pdf e slide 111 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
        // Tal cor é definida como coeficientes RGBA: Red, Green, Blue, Alpha;
        // isto é: Vermelho, Verde, Azul, Alpha (valor de transparência).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.

        GLfloat color_coefficients[4 * (NUM_VERTICES * 2)];

        // Cor dos vértices (vermelho)
        for (int i = 0; i < 2 * NUM_VERTICES; i++)
        {
            color_coefficients[i * 4 + 0] = 1.0f;  // R
            color_coefficients[i * 4 + 1] = 0.0f;  // G
            color_coefficients[i * 4 + 2] = 0.0f;  // B
            color_coefficients[i * 4 + 3] = 1.0f;  // A
        }

        GLuint VBO_color_coefficients_id;
        glGenBuffers(1, &VBO_color_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(color_coefficients), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color_coefficients), color_coefficients);
        location = 1; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Vamos então definir os triângulos utilizando os vértices do array
        // NDC_coefficients. O primeiro triângulo é formado pelos vértices 0,1,2;
        // o segundo pelos vértices 0,2,3; e assim sucessivamente. Note que usaremos o modo de renderização
        // GL_TRIANGLE_STRIP na chamada glDrawElements() dentro de main(). Veja slides 182-188 do documento Aula_04_Modelagem_Geometrica_3D.pdf.
        //
        // Este vetor "indices" define a TOPOLOGIA (veja slides 103-110 do documento Aula_04_Modelagem_Geometrica_3D.pdf).
        //

        GLubyte indices[(NUM_VERTICES * 2) + 2]; // GLubyte: valores entre 0 e 255 (8 bits sem sinal).

        //   0   1   2    3    4    5           31    32   33
        // [v0, vn, v1, vn+1, v2, vn+2, ... ,  v2n-1, vn,  v0]
        // [ 0, 16,  1,  17,  2,  18,   ... ,  v31,   16,  0]

        // Indices dos vértices ao redor do círculo
        for (int i = 0; i < NUM_VERTICES; i++)
        {
            indices[i * 2] = i;                                // vertices externos
            indices[i * 2 + 1] = i + NUM_VERTICES;             // vertices internos
        }

        indices[NUM_VERTICES * 2] = 0;                         // Conectando ultimo vertice externo ao primeiro
        indices[NUM_VERTICES * 2 + 1] = NUM_VERTICES;          // Conectando ultimo vertice interno ao primeiro

        GLuint indices_id;
        glGenBuffers(1, &indices_id);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);

        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);

        glBindVertexArray(0);

        // Retornamos o ID do VAO. Isso é tudo que será necessário para renderizar
        // os triângulos definidos acima.
        return vertex_array_object_id;
    }
}

// Carrega um Vertex Shader de um arquivo GLSL. Veja definição de LoadShader() abaixo.
GLuint LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos vértices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo GLSL . Veja definição de LoadShader() abaixo.
GLuint LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo
    // será aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Função auxilar, utilizada pelas duas funções acima. Carrega código de GPU de
// um arquivo GLSL e faz sua compilação.
void LoadShader(const char* filename, GLuint shader_id)
{
    // Lemos o arquivo de texto indicado pela variável "filename"
    // e colocamos seu conteúdo em memória, apontado pela variável
    // "shader_string".
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o código do shader GLSL, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o código do shader GLSL (em tempo de execução)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compilação
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    // Alocamos memória para guardar o log de compilação.
    // A chamada "new" em C++ é equivalente ao "malloc()" do C.
    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    // Imprime no terminal qualquer erro ou "warning" de compilação
    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    // A chamada "delete" em C++ é equivalente ao "free()" do C
    delete [] log;
}

// Função que carrega os shaders de vértices e de fragmentos que serão
// utilizados para renderização. Veja slides 180-200 do documento Aula_03_Rendering_Pipeline_Grafico.pdf.
//
void LoadShadersFromFiles()
{
    // Note que o caminho para os arquivos "shader_vertex.glsl" e
    // "shader_fragment.glsl" estão fixados, sendo que assumimos a existência
    // da seguinte estrutura no sistema de arquivos:
    //
    //    + FCG_Lab_01/
    //    |
    //    +--+ bin/
    //    |  |
    //    |  +--+ Release/  (ou Debug/ ou Linux/)
    //    |     |
    //    |     o-- main.exe
    //    |
    //    +--+ src/
    //       |
    //       o-- shader_vertex.glsl
    //       |
    //       o-- shader_fragment.glsl
    //
    GLuint vertex_shader_id = LoadShader_Vertex("../../src/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../../src/shader_fragment.glsl");

    // Deletamos o programa de GPU anterior, caso ele exista.
    if ( g_GpuProgramID != 0 )
        glDeleteProgram(g_GpuProgramID);

    // Criamos um programa de GPU utilizando os shaders carregados acima.
    g_GpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);
}

// Esta função cria um programa de GPU, o qual contém obrigatoriamente um
// Vertex Shader e um Fragment Shader.
GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    // Criamos um identificador (ID) para este programa de GPU
    GLuint program_id = glCreateProgram();

    // Definição dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(program_id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memória para guardar o log de compilação.
        // A chamada "new" em C++ é equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "== End of link log\n";

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Retorna o ID gerado acima
    return program_id;
}

// Definição da função que será chamada sempre que a janela do sistema
// operacional for redimensionada, por consequência alterando o tamanho do
// "framebuffer" (região de memória onde são armazenados os pixels da imagem).
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // Indicamos que queremos renderizar em toda região do framebuffer. A
    // função "glViewport" define o mapeamento das "normalized device
    // coordinates" (NDC) para "pixel coordinates".  Essa é a operação de
    // "Screen Mapping" ou "Viewport Mapping" vista em aula (slides 141-148 do documento Aula_03_Rendering_Pipeline_Grafico.pdf).
    glViewport(0, 0, width, height);
}

// Definição da função que será chamada sempre que o usuário pressionar alguma
// tecla do teclado. Veja http://www.glfw.org/docs/latest/input_guide.html#input_key
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
    // =====================
    // Não modifique este loop! Ele é utilizando para correção automatizada dos
    // laboratórios. Deve ser sempre o primeiro comando desta função KeyCallback().
    for (int i = 0; i < 10; ++i)
        if (key == GLFW_KEY_0 + i && action == GLFW_PRESS && mod == GLFW_MOD_SHIFT)
            std::exit(100 + i);
    // =====================

    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// Definimos o callback para impressão de erros da GLFW no terminal
void ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// vim: set spell spelllang=pt_br :

