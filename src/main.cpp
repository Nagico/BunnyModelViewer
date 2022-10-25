#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util/opengl/ShaderProgram.h"
#include "util/opengl/Camera.h"
#include "util/opengl/Model.h"
#include "util/RayPicker.h"
#include "util/event/Mouse.h"
#include "util/opengl/PolygonPoint.h"
#include "util/opengl/PolygonTriangle.h"

#include <iostream>

void processInput(GLFWwindow *window);
void mouse_select(GLFWwindow* window, double xpos, double ypos);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

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
Model lightModel;

// polygon mode
PolygonPoint highlightPoint;
PolygonPoint selectPoint;
PolygonTriangle highlightTriangle;
PolygonTriangle selectTriangle;

glm::vec4 lineColor(0.12156863f, 0.8862745f, 0.6039216f, 1.f);
glm::vec4 pointColor(0.35686275f, 0.078431375f, 0.83137256f, 0.01f);
glm::vec4 selectPointColor(0.05882353f, 0.73333335f, 0.8352941f, 1.f);
glm::vec4 selectTriangleColor(0.82156863f, 0.0862745f, 0.2039216f, 1.f);
glm::vec4 highlightPointColor(0.02156863f, 0.9862745f, 0.9039216f, 1.f);
glm::vec4 highlightTriangleColor(0.6901961f, 0.14117648f, 0.8980392f, 1.f);

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

GLFWwindow* window;

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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); });
    Mouse::init(window);

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

    ShaderProgram modelShader;
    ShaderProgram modelColorShader;
    ShaderProgram lightShader;

    // build and compile shaders
    // -------------------------
    #pragma omp parallel
    #pragma omp sections
    {
        #pragma omp section
        {
            modelShader.load("assets/shader/model_vertex.glsl", "assets/shader/model_fragment.glsl");
        }
        #pragma omp section
        {
            modelColorShader.load("assets/shader/model_vertex.glsl", "assets/shader/model_color_fragment.glsl");
        }
        #pragma omp section
        {
            lightShader.load("assets/shader/light_vertex.glsl", "assets/shader/light_fragment.glsl");
        }
    };
    // load models
    // -----------
    auto path = "assets/model/bunny_texture/bunny.obj";
    //auto path = "assets/model/bunny_iH.ply2";
    #pragma omp parallel
    #pragma omp sections
    {
        #pragma omp section
        {
            bunnyModel = Model(path);
        }
        #pragma omp section
        {
            lightModel = Model("assets/model/sphere.ply");
        };
    };

    // super scale
    glfwWindowHint(GLFW_SAMPLES, 8);
    glEnable(GL_MULTISAMPLE);

    selectPoint.setVao(bunnyModel.getMeshes()[0].getVao());
    selectPoint.setVertex(bunnyModel.getMeshes()[0].getVertices());
    highlightPoint.setVao(bunnyModel.getMeshes()[0].getVao());
    highlightPoint.setVertex(bunnyModel.getMeshes()[0].getVertices());
    selectTriangle.setVao(bunnyModel.getMeshes()[0].getVao());
    highlightTriangle.setVao(bunnyModel.getMeshes()[0].getVao());

    EventHandler::get().addListener([](const event::Mouse::ClickEvent<MouseButton::LEFT>& event) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            if (rayPicker.selectPointValid) {
                std::cout << "select point (" << rayPicker.selectPointIndex << "): " << rayPicker.selectPoint.position.x
                          << ", " << rayPicker.selectPoint.position.y << ", " << rayPicker.selectPoint.position.z
                          << std::endl;
            }
            else if (rayPicker.selectFaceValid) {
                std::cout << "select face: " << std::endl;
                for (int i = 0; i < 3; i++) {
                    std::cout << "\t point (" << rayPicker.selectFaceIndex[i] << "): "
                              << rayPicker.selectFace[i].position.x << ", " << rayPicker.selectFace[i].position.y
                              << ", " << rayPicker.selectFace[i].position.z << std::endl;
                }

            } else {
                std::cout << "select nothing" << std::endl;
            }

        }
    });

    EventHandler::get().addListener([](const event::Mouse::ClickHoldEvent<MouseButton::RIGHT>& event) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            if (rayPicker.selectPointValid) {
                auto status = highlightPoint.modifyIndices(rayPicker.selectPointIndex);
                std::cout << (status ? "" : "un") << "highlight point (" << rayPicker.selectPointIndex << "): " << rayPicker.selectPoint.position.x
                          << ", " << rayPicker.selectPoint.position.y << ", " << rayPicker.selectPoint.position.z
                          << std::endl;
            }
            else if (rayPicker.selectFaceValid) {
                auto status = highlightTriangle.modifyIndices(rayPicker.selectFaceIndex[0], rayPicker.selectFaceIndex[1], rayPicker.selectFaceIndex[2]);
                std::cout << (status ? "" : "un") << "highlight face: " << std::endl;
                for (int i = 0; i < 3; i++) {
                    std::cout << "\t point (" << rayPicker.selectFaceIndex[i] << "): "
                              << rayPicker.selectFace[i].position.x << ", " << rayPicker.selectFace[i].position.y
                              << ", " << rayPicker.selectFace[i].position.z << std::endl;
                }

            } else {
                std::cout << "highlight nothing" << std::endl;
            }

        }
    });

    EventHandler::get().addListener([](const event::Mouse::MoveEvent &event) {
        if (Mouse::state[MouseButton::LEFT]){
            float sensitivity = 0.01f * camera.Zoom; // change this value to your liking
            auto pos = event.offset * sensitivity;
            camera.ProcessMouseMovement(-pos.x, -pos.y);
        }
    });

    EventHandler::get().addListener([](const event::Mouse::ScrollEvent &event) {
        float sensitivity = 1.5f; // change this value to your liking
        camera.ProcessMouseScroll(static_cast<float>(event.offset.y * sensitivity));
    });

    auto baseModelScale = std::min(SCR_WIDTH, SCR_HEIGHT) * 0.002f / pow(bunnyModel.getMeshes()[0].getMeshInfo().maxDis,1.15);
    std::cout << "baseModelScale: " << baseModelScale << std::endl;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        EventHandler::get().getEventBus()->process();
        // per-frame time logic
        // --------------------
        auto currentFrame = static_cast<float>(glfwGetTime());
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
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(baseModelScale));

        modelColorShader.use(model, view, projection);
        modelColorShader.setValue("pureColor", highlightPointColor);
        highlightPoint.render(-1.15f, 5.0f);

        modelColorShader.use(model, view, projection);
        modelColorShader.setValue("pureColor", highlightTriangleColor);
        highlightTriangle.render(-1.25f);

        if (mode.selected)
        {
            if (rayPicker.selectPointValid)
            {
                selectPoint.resetIndices(rayPicker.selectPointIndex);
                modelColorShader.use(model, view, projection);
                modelColorShader.setValue("pureColor", selectPointColor);
                selectPoint.render(-1.1f, 5.0f);

            }
            else if (rayPicker.selectFaceValid)
            {
                selectTriangle.resetIndices(rayPicker.selectFaceIndex[0], rayPicker.selectFaceIndex[1], rayPicker.selectFaceIndex[2]);
                modelColorShader.use(model, view, projection);
                modelColorShader.setValue("pureColor", selectTriangleColor);
                selectTriangle.render(-1.1f);
            }
        }

        if (mode.fill)
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);//设置绘制模型为绘制前面与背面模型，以填充的方式绘制
            // don't forget to enable shader before setting uniforms
            modelShader.use(model, view, projection);
            modelShader.setValue("lightPos", lightPos);
            modelShader.setValue("viewPos", camera.Position);

            modelShader.setValue("light.ambient", 0.3f, 0.3f, 0.3f);
            modelShader.setValue("light.diffuse", 0.5f, 0.5f, 0.5f);
            modelShader.setValue("light.specular", 1.0f, 1.0f, 1.0f);

            modelShader.setValue("light.constant", 1.0f);
            modelShader.setValue("light.linear", 0.09f);
            modelShader.setValue("light.quadratic", 0.032f);

            modelShader.setValue("material.shininess", 64.0f);

            bunnyModel.render(&modelShader, false);
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
            modelColorShader.use(model, view, projection);
            modelColorShader.setValue("pureColor", lineColor);
            bunnyModel.render(&modelColorShader);
            glDisable(GL_POLYGON_OFFSET_LINE);//关闭多边形偏移
        }

        if (mode.point)
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);//将绘制模式改为点
            glEnable(GL_POLYGON_OFFSET_POINT);//开启多边形偏移
            glPolygonOffset(-1.5f,-1.5f);//设置多边形偏移量
            glPointSize(2.5f);
            modelColorShader.use(model, view, projection);
            modelColorShader.setValue("pureColor", pointColor);
            bunnyModel.render(&modelColorShader);
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
        mouse_select(window, Mouse::position.x, Mouse::position.y);
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

void mouse_select(GLFWwindow* window, double mouse_xpos, double mouse_ypos)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    rayPicker.rayPick(
            bunnyModel.getMeshes()[0], camera.Position,
            model, view, projection, (float)mouse_xpos, (float)mouse_ypos, width, height);
}
