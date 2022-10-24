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

#include <iostream>
#include <thread>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void mouse_select(GLFWwindow* window, double xpos, double ypos);

bool intersectTriangle(const glm::vec3& orig, const glm::vec3& dir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t, float& u, float& v);
bool intersectTriangle2(const glm::vec3& orig, const glm::vec3& dir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t, float& u, float& v);

// settings
const unsigned int SCR_WIDTH = 500;
const unsigned int SCR_HEIGHT = 500;

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
Model bunnyOverPointModel;
bool bunnyOver = false;

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
MODE tmpMode = mode;
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
            bunnyOverPointModel = Model(path);
        }
        #pragma omp section
        {
            lightModel = Model("assets/model/sphere.ply");
        };
    };



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
        title += "LearnOpenGL -";
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

        if (bunnyOver)
        {
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);//设置绘制模型为绘制前面与背面模型，以填充的方式绘制
            // don't forget to enable shader before setting uniforms
            bunnyOverShader.use();

            bunnyOverShader.setValue("projection", projection);
            bunnyOverShader.setValue("view", view);
            // render the loaded model
            bunnyOverShader.setValue("model", model);
            bunnyOverLineModel.render(&bunnyOverShader);
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

        if (mode.line || mode.selected)
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
    // 2D屏幕坐标
    auto xpos = static_cast<float>(mouse_xpos);
    auto ypos = static_cast<float>(mouse_ypos);

    // NDC标准化坐标
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float x = (2.0f * xpos) / width - 1.0f;
    float y = 1.0f - (2.0f * ypos) / height;
    float z = 1.0f;  //  z = 1.0f 代表当前将鼠标的位置投影到远裁剪平面，如果设z的坐标为-1则代表将当前投影到近裁剪平面上
    auto ray_nds = glm::vec3(x, y, z);

    // 裁剪齐次坐标
    auto ray_clip = glm::vec4(ray_nds.x, ray_nds.y, ray_nds.z, 1.0f);

    // 视点坐标
    // 投影矩阵的逆矩阵左乘点的裁剪坐标
    auto ray_eye = glm::inverse(projection) * ray_clip;

    // 世界坐标
    auto ray_wor = glm::inverse(view) * ray_eye;

    if (ray_wor.w != 0)
    {
        ray_wor = ray_wor / ray_wor.w;
    }


    // 从摄像机位置发出一条射线
    auto ray_dir = glm::normalize(glm::vec3(ray_wor.x, ray_wor.y, ray_wor.z) - camera.Position);

    float minT = 1000.f;
    Face findFace;

    float u, v, t;

    // 遍历每个面
    for (auto& mesh : bunnyModel.getMeshes())
    {
        std::cout << "mesh size: " << mesh.getFaces().size() << std::endl;
        #pragma omp parallel
        #pragma omp for
        for (auto& face : mesh.getFaces())
        {
            auto v0 = mesh.getVertices()[face.v1];
            auto v1 = mesh.getVertices()[face.v2];
            auto v2 = mesh.getVertices()[face.v3];

            if (intersectTriangle2(camera.Position, ray_dir, v0.position, v1.position, v2.position, t, u, v))
            {
                if (t > 0 && t < minT)
                {
                    minT = t;
                    findFace = face;
                }
            }
        }

        if (minT < 1000.f)
        {
            if (!bunnyOver)
                bunnyOver = true;

            auto overMesh = bunnyOverLineModel.getMeshes()[0];
            overMesh.setIndices({findFace.v1, findFace.v2, findFace.v3});


            std::cout << "find face: " << findFace.v1 << " " << findFace.v2 << " " << findFace.v3 << std::endl;
            std::cout << "t: " << minT << std::endl;
        }
        else
        {
            std::cout << "not find face" << std::endl;
        }
    }

}

float getArea(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
    auto a = glm::length(v1 - v0);
    auto b = glm::length(v2 - v1);
    auto c = glm::length(v0 - v2);

    auto p = (a + b + c) / 2.0f;

    return sqrt(p * (p - a) * (p - b) * (p - c));
}

bool intersectTriangle(const glm::vec3& orig,
                        const glm::vec3& dir,
                        const glm::vec3& v0,
                        const glm::vec3& v1,
                        const glm::vec3& v2,
                        float& t,
                        float& u,
                        float& v)
{
    auto v0Model = glm::vec3(model * glm::vec4(v0, 1.0f));
    auto v1Model = glm::vec3(model * glm::vec4(v1, 1.0f));
    auto v2Model = glm::vec3(model * glm::vec4(v2, 1.0f));

    // Ax+By+Cz+D=0
    auto A = v0Model.y * (v2Model.z - v1Model.z) + v1Model.y * (v0Model.z - v2Model.z) + v2Model.y * (v1Model.z - v0Model.z);
    auto B = v0Model.z * (v2Model.x - v1Model.x) + v1Model.z * (v0Model.x - v2Model.x) + v2Model.z * (v1Model.x - v0Model.x);
    auto C = v0Model.x * (v2Model.y - v1Model.y) + v1Model.x * (v0Model.y - v2Model.y) + v2Model.x * (v1Model.y - v0Model.y);
    auto D = -(v0Model.x * (v2Model.y * v1Model.z - v1Model.y * v2Model.z) + v1Model.x * (v0Model.y * v2Model.z - v2Model.y * v0Model.z) + v2Model.x * (v1Model.y * v0Model.z - v0Model.y * v1Model.z));

    auto point1 = orig;
    auto point2 = orig + dir;

    // 线段与平面的交点
    t = -(A * point1.x + B * point1.y + C * point1.z + D) / (A * (point2.x - point1.x) + B * (point2.y - point1.y) + C * (point2.z - point1.z));
    auto point = point1 + t * (point2 - point1);

    if (t < 0.001f)
    {
        return false;
    }

    // 判断交点是否在三角形内
    auto s012 = getArea(v0Model, v1Model, v2Model);
    auto s01p = getArea(v0Model, v1Model, point);
    auto s02p = getArea(v0Model, v2Model, point);
    auto s12p = getArea(v1Model, v2Model, point);

    auto s = s01p + s02p + s12p;
    auto res = (s - s012) < 0.001f;

    return res;
}

/// 射线三角形检测
/// \param orig 射线起点
/// \param dir 射线方向
/// \param v0 三角形顶点0
/// \param v1 三角形顶点1
/// \param v2 三角形顶点2
/// \param t 交点距离起点的距离
/// \param u 交点距离v0的距离
/// \param v 交点距离v1的距离
/// \return 是否相交
/// 根据上面代码求出的t,u,v的值，交点的最终坐标可以用下面两种方法计算
/// orig + dir * t
/// (1 - u - v) * v0 + u * v1 + v * v2Model
bool intersectTriangle2(const glm::vec3& orig,
                       const glm::vec3& dir,
                       const glm::vec3& v0,
                       const glm::vec3& v1,
                       const glm::vec3& v2,
                       float& t,
                       float& u,
                       float& v)
{
    constexpr float EPSILON = 0.0000001f;

    auto v0Model = glm::vec3(model * glm::vec4(v0, 1.0f));
    auto v1Model = glm::vec3(model * glm::vec4(v1, 1.0f));
    auto v2Model = glm::vec3(model * glm::vec4(v2, 1.0f));

    // 计算三角形的法线
    auto E1 = v1Model - v0Model;  // 三角形的边
    auto E2 = v2Model - v0Model;
    auto P = glm::cross(dir, E2);

    auto det = glm::dot(E1, P);

    // 如果det=0，说明射线与三角形平行，不相交
    if (det > -EPSILON && det < EPSILON) {
        return false;
    }


    auto T = orig - v0Model;
    // 计算u
    u = glm::dot(T, P);
    if (u < EPSILON || u - det > -EPSILON)
    {
        return false;
    }

    auto Q = glm::cross(T, E1);

    // 计算v
    v = glm::dot(dir, Q);
    if (v < EPSILON || u + v - det > -EPSILON)
    {
        return false;
    }

    // 计算t
    t = glm::dot(E2, Q);

    auto fInvDet = 1.0f / det;
    t *= fInvDet;
    u *= fInvDet;
    v *= fInvDet;

    return true;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        if (!mouse_left_button && mode.selected)
            mouse_select(window, lastX, lastY);
        mouse_left_button = true;
        std::cout << "Left button pressed" << std::endl;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        mouse_right_button = true;
        std::cout << "Right button pressed" << std::endl;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
    {
        mouse_middle_button = true;
        std::cout << "Middle button pressed" << std::endl;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        mouse_left_button = false;
        std::cout << "Left button released" << std::endl;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        mouse_right_button = false;
        std::cout << "Right button released" << std::endl;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    {
        mouse_middle_button = false;
        std::cout << "Middle button released" << std::endl;
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
