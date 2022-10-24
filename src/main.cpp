#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util/ShaderProgram.h"
#include "util/Camera.h"
#include "util/Model.h"
#include "util/RayPicker.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void mouse_select(GLFWwindow* window, double xpos, double ypos);

// settings
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// mouse
auto mouse_left_button = false;
auto mouse_middle_button = false;
auto mouse_right_button = false;
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(0.0f, 2.0f, 1.0f);

// matrix
glm::mat4 projection;
glm::mat4 view;
glm::mat4 model;

// model
Model bunnyModel;
Model bunnyOverLineModel;

RayPicker rayPicker;

struct MODE
{
    bool light = true;
    bool fill = true;
    bool line = false;
    bool point = false;
    bool selected = false;
};

struct KeyLock
{
    bool light = false;
    bool fill = false;
    bool line = false;
    bool point = false;
    bool selected = false;
};

MODE mode;
KeyLock keyLock;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    ShaderProgram bunnyShader;
    ShaderProgram bunnyLineShader;
    ShaderProgram bunnyOverShader;
    ShaderProgram bunnyOverPointShader;
    ShaderProgram bunnyPointShader;
    ShaderProgram lightShader;

    // build and compile shaders
    // -------------------------
    #pragma omp parallel
    #pragma omp sections
    {
        #pragma omp section
        {
            bunnyShader.load("assets/shader/model_vertex.glsl", "assets/shader/model_fragment.glsl");
        }
        #pragma omp section
        {
            bunnyLineShader.load("assets/shader/model_vertex.glsl", "assets/shader/model_line_fragment.glsl");
        }
        #pragma omp section
        {
            bunnyOverShader.load("assets/shader/model_vertex.glsl", "assets/shader/model_over_fragment.glsl");
        }
        #pragma omp section
        {
            bunnyOverPointShader.load("assets/shader/model_vertex.glsl", "assets/shader/model_over_point_fragment.glsl");
        }
        #pragma omp section
        {
            bunnyPointShader.load("assets/shader/model_vertex.glsl","assets/shader/model_point_fragment.glsl");
        }
        #pragma omp section
        {
            lightShader.load("assets/shader/light_vertex.glsl", "assets/shader/light_fragment.glsl");
        }
    };
    // load models
    // -----------
    //bunnyModel = Model("assets/model/bunny_texture/bunny.obj");

    Model lightModel;

    auto path = "assets/model/bunny_texture/bunny.obj";
    //auto path = "assets/model/plane.ply";
    #pragma omp parallel
    #pragma omp sections
    {
        #pragma omp section
        {
            bunnyModel = Model(path);
        }
        #pragma omp section
        {
            bunnyOverLineModel = Model(path);
        }
        #pragma omp section
        {
            lightModel = Model("assets/model/sphere.ply");
        };
    };

    // super scale
    glfwWindowHint(GLFW_SAMPLES, 8);
    glEnable(GL_MULTISAMPLE);

    float selectPointVertices[] = {
            -0.5f, -0.5f, 0.0f, // left
            0.5f, -0.5f, 0.0f, // right
            0.0f,  0.5f, 0.0f  // top
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(selectPointVertices), selectPointVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // title
        // -----
        std::string title;
        if (mode.selected) title += "[Select] ";
        title += "LearnOpenGL - ";
        if (mode.fill) title += "Fill ";
        if (mode.line) title += "Line ";
        if (mode.point) title += "Point ";


        glfwSetWindowTitle(window, title.c_str());

        // view/projection transformations
        projection = glm::perspective(
                glm::radians(camera.Zoom),
                (float)SCR_WIDTH / (float)SCR_HEIGHT,
                0.1f,
                100.0f);
        view = camera.GetViewMatrix();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
        model = glm::scale(model, glm::vec3(10.5f, 10.5f, 10.5f));

        if (mode.selected)
        {
            if (rayPicker.selectPointValid)
            {
                auto point = rayPicker.selectPoint.position;
                selectPointVertices[0] = point.x;
                selectPointVertices[1] = point.y;
                selectPointVertices[2] = point.z;

                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(selectPointVertices), selectPointVertices);


                glPointSize(5.0f);
                bunnyOverPointShader.use();
                bunnyOverPointShader.setValue("projection", projection);
                bunnyOverPointShader.setValue("view", view);
                bunnyOverPointShader.setValue("model", model);

                glBindVertexArray(VAO);
                glDrawArrays(GL_POINTS, 0, 1);
                glBindVertexArray(0);
            }
            else if (rayPicker.selectFaceValid)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//设置绘制模型为绘制前面与背面模型，以填充的方式绘制
                glEnable(GL_POLYGON_OFFSET_LINE);//开启多边形偏移
                glPolygonOffset(-1.6f,-1.6f);//设置多边形偏移量
                // don't forget to enable shader before setting uniforms
                bunnyOverShader.use();

                bunnyOverShader.setValue("projection", projection);
                bunnyOverShader.setValue("view", view);
                // render the loaded model
                bunnyOverShader.setValue("model", model);
                bunnyOverLineModel.render(&bunnyOverShader);
            }
        }

        if (mode.fill)
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);//设置绘制模型为绘制前面与背面模型，以填充的方式绘制
            // don't forget to enable shader before setting uniforms
            bunnyShader.use();
            bunnyShader.setValue("lightPos", lightPos);
            bunnyShader.setValue("viewPos", camera.Position);

            bunnyShader.setValue("light.ambient", 0.3f, 0.3f, 0.3f);
            bunnyShader.setValue("light.diffuse", 0.5f, 0.5f, 0.5f);
            bunnyShader.setValue("light.specular", 1.0f, 1.0f, 1.0f);

            bunnyShader.setValue("light.constant", 1.0f);
            bunnyShader.setValue("light.linear", 0.09f);
            bunnyShader.setValue("light.quadratic", 0.032f);

            bunnyShader.setValue("material.shininess", 64.0f);

            bunnyShader.setValue("projection", projection);
            bunnyShader.setValue("view", view);
            // render the loaded model
            bunnyShader.setValue("model", model);

            bunnyModel.render(&bunnyShader, false);
        }

        if (mode.light)
        {
            glm::mat4 lightModelMatrix = glm::mat4(1.0f);
            lightModelMatrix = glm::translate(lightModelMatrix, lightPos);
            lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(.3f, .3f, .3f));

            lightShader.use();
            lightShader.setValue("projection", projection);
            lightShader.setValue("view", view);
            lightShader.setValue("model", lightModelMatrix);
            lightModel.render(&lightShader);
        }

        if (mode.line)
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);//将绘制模式改为线
            glEnable(GL_POLYGON_OFFSET_LINE);//开启多边形偏移
            glLineWidth(1.0f);
            glPolygonOffset(-1.0f,-1.0f);//设置多边形偏移量
            bunnyLineShader.use();
            bunnyLineShader.setValue("model", model);
            bunnyLineShader.setValue("projection", projection);
            bunnyLineShader.setValue("view", view);
            bunnyModel.render(&bunnyLineShader);
            glDisable(GL_POLYGON_OFFSET_LINE);//关闭多边形偏移
        }

        if (mode.point)
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);//将绘制模式改为点
            glEnable(GL_POLYGON_OFFSET_POINT);//开启多边形偏移
            glPolygonOffset(-1.5f,-1.5f);//设置多边形偏移量
            glPointSize(2.5f);
            bunnyPointShader.use();
            bunnyPointShader.setValue("model", model);
            bunnyPointShader.setValue("projection", projection);
            bunnyPointShader.setValue("view", view);
            bunnyModel.render(&bunnyPointShader);
            glDisable(GL_POLYGON_OFFSET_POINT);//关闭多边形偏移
        }


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        if (!keyLock.line)
        {
            keyLock.line = true;
            mode.line = !mode.line;
        }
    }
    else
    {
        keyLock.line = false;
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        if (!keyLock.fill)
        {
            keyLock.fill = true;
            mode.fill = !mode.fill;
        }
    }
    else
    {
        keyLock.fill = false;
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        if (!keyLock.point)
        {
            keyLock.point = true;
            mode.point = !mode.point;
        }
    }
    else
    {
        keyLock.point = false;
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        if (!keyLock.line)
        {
            keyLock.line = true;
            mode.line = !mode.line;
        }
    }
    else
    {
        keyLock.line = false;
    }

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        if (!keyLock.light)
        {
            keyLock.light = true;
            mode.light = !mode.light;
        }
    }
    else
    {
        keyLock.light = false;
    }


    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        if (!keyLock.selected)
        {
            keyLock.selected = true;
            mode.selected = true;
        }
        mouse_select(window, lastX, lastY);
    }
    else
    {
        keyLock.selected = false;
        mode.selected = false;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessKeyboard(Camera::RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (mouse_left_button)
    {
        float sensitivity = 0.01f * camera.Zoom; // change this value to your liking
        camera.ProcessMouseMovement(-xoffset * sensitivity, -yoffset * sensitivity);
    }
}

void mouse_select(GLFWwindow* window, double mouse_xpos, double mouse_ypos)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    rayPicker.rayPick(
            bunnyModel.getMeshes()[0], camera.Position,
            model, view, projection, (float)mouse_xpos, (float)mouse_ypos, width, height);

    if (rayPicker.selectFaceValid)
        bunnyOverLineModel.getMeshes()[0].setIndices({rayPicker.selectFaceIndex[0], rayPicker.selectFaceIndex[1], rayPicker.selectFaceIndex[2]});
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            if (rayPicker.selectPointValid)
                std::cout << "select point (" << rayPicker.selectPointIndex << "): " << rayPicker.selectPoint.position.x << ", " << rayPicker.selectPoint.position.y << ", " << rayPicker.selectPoint.position.z << std::endl;
            else if(rayPicker.selectFaceValid)
            {
                std::cout << "select face: " << std::endl;
                for (int i = 0; i < 3; i++)
                {
                    std::cout << "\t point (" << rayPicker.selectFaceIndex[i] << "): " << rayPicker.selectFace[i].position.x << ", " << rayPicker.selectFace[i].position.y << ", " << rayPicker.selectFace[i].position.z << std::endl;
                }

            }
            else
            {
                std::cout << "select nothing" << std::endl;
            }

        }
        mouse_left_button = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        mouse_right_button = true;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        mouse_middle_button = true;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mouse_left_button = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        mouse_right_button = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
        mouse_middle_button = false;
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    float sensitivity = 1.5f; // change this value to your liking
    camera.ProcessMouseScroll(static_cast<float>(yoffset * sensitivity));
    std::cout << "Zoom: " << camera.Zoom << std::endl;
}
