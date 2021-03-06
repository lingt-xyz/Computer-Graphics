#include <GL/glew.h>	    // include GL Extension Wrangler
#include <GLFW/glfw3.h>     // include GLFW helper library

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Angel.h"
#include <assert.h>
#include "MatrixStack.h"
#include "Node.h"
#include "Horse.h"

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

// 36 vertices for a cube: (6 faces)(2 triangles/face)(3 vertices/triangle)
const int NumVertices = 36;

point4 points[NumVertices];
vec3 normals[NumVertices];

// eight points of a base cube
point4 vertices[8] =
{
    point4(-0.5, -0.5, 0.5, 1.0),
    point4(-0.5, 0.5, 0.5, 1.0),
    point4(0.5, 0.5, 0.5, 1.0),
    point4(0.5, -0.5, 0.5, 1.0),
    point4(-0.5, -0.5, -0.5, 1.0),
    point4(-0.5, 0.5, -0.5, 1.0),
    point4(0.5, 0.5, -0.5, 1.0),
    point4(0.5, -0.5, -0.5, 1.0)
};

//----------------------------------------------------------------------------

// Set up menu item indices, which we can also use with the joint angles
enum
{
    Torso = 0,
    Head = 1,
    LeftUpperArm = 2,
    LeftLowerArm = 3,
    RightUpperArm = 4,
    RightLowerArm = 5,
    LeftUpperLeg = 6,
    LeftLowerLeg = 7,
    RightUpperLeg = 8,
    RightLowerLeg = 9,
    Neck = 10,
    NumNodes,
    Quit
};

MatrixStack  mvstack;
mat4         base_model;
GLuint       shader_model;

// Joint angles with initial values
GLfloat theta[NumNodes] =
{
    0.0,	// Torso
    -80.0,	// Head
    180.0,	// LeftUpperArm
    0.0,	// LeftLowerArm
    180.0,	// RightUpperArm
    0.0,	// RightLowerArm
    170.0,	// LeftUpperLeg
    10.0,	// LeftLowerLeg
    170.0,	// RightUpperLeg
    10.0,	// RightLowerLeg
    -45.0	// Neck
};

GLfloat target[NumNodes] =
{
    0.0,	// Torso
    0.0,	// Head
    0.0,	// LeftUpperArm
    0.0,	// LeftLowerArm
    0.0,	// RightUpperArm
    0.0,	// RightLowerArm
    0.0,	// LeftUpperLeg
    0.0,	// LeftLowerLeg
    0.0,	// RightUpperLeg
    0.0,	// RightLowerLeg
    0.0	    // Neck
};

GLint angle = Torso;

//----------------------------------------------------------------------------

Node nodes[NumNodes];

//----------------------------------------------------------------------------

int tmp_index = 0;
// given a face, initialize four points (two triangles) of it and their normals.
void quad(int a, int b, int c, int d)
{
    vec4 u = vertices[b] - vertices[a];
    vec4 v = vertices[c] - vertices[b];

    vec3 normal = normalize(cross(u, v));
    normals[tmp_index] = normal;
    points[tmp_index] = vertices[a];
    tmp_index++;
    normals[tmp_index] = normal;
    points[tmp_index] = vertices[b];
    tmp_index++;
    normals[tmp_index] = normal;
    points[tmp_index] = vertices[c];
    tmp_index++;
    normals[tmp_index] = normal;
    points[tmp_index] = vertices[a];
    tmp_index++;
    normals[tmp_index] = normal;
    points[tmp_index] = vertices[c];
    tmp_index++;
    normals[tmp_index] = normal;
    points[tmp_index] = vertices[d];
    tmp_index++;
}

// initialize the base cube and all the normals
void initBaseCube(void)
{
    quad(1, 0, 3, 2);
    quad(2, 3, 7, 6);
    quad(3, 0, 4, 7);
    quad(6, 5, 1, 2);
    quad(4, 5, 6, 7);
    quad(5, 4, 0, 1);
}

//----------------------------------------------------------------------------

void traverse(Node* node)
{
    if (node == NULL)
    {
        return;
    }

    mvstack.push(base_model);

    base_model *= node->transform;
    node->render();

    if (node->child)
    {
        traverse(node->child);
    }

    base_model = mvstack.pop();

    if (node->sibling)
    {
        traverse(node->sibling);
    }
}

//----------------------------------------------------------------------------

void torso()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * TORSO_HEIGHT, 0.0) * Scale(TORSO_WIDTH, TORSO_HEIGHT, TORSO_DEPTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void neck()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * NECK_HEIGHT, 0.0) * Scale(NECK_WIDTH, NECK_HEIGHT, NECK_DEPTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void head()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * HEAD_HEIGHT, 0.0) * Scale(HEAD_WIDTH, HEAD_HEIGHT, HEAD_DEPTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void left_upper_arm()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) * Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void left_lower_arm()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) * Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void right_upper_arm()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) * Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void right_lower_arm()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) * Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void left_upper_leg()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) * Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));

    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.65f, 0.65f, 0.65f));
    glm::mat4 translate = glm::translate(glm::mat4(0.1f), glm::vec3(0, -1, 0));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void left_lower_leg()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) * Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void right_upper_leg()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) * Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}

void right_lower_leg()
{
    mvstack.push(base_model);

    mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) * Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH));

    glUniformMatrix4fv(shader_model, 1, GL_TRUE, base_model * instance);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);

    base_model = mvstack.pop();
}


//----------------------------------------------------------------------------

void initNodes(void)
{
    mat4  m;

    m = RotateY(theta[Torso]);
    nodes[Torso] = Node(m, torso, NULL, &nodes[Neck]);

    m = Translate(TORSO_WIDTH / 2 - NECK_WIDTH / 2, TORSO_HEIGHT, 0.0) * RotateZ(theta[Neck]);
    nodes[Neck] = Node(m, neck, &nodes[LeftUpperArm], &nodes[Head]);

    m = Translate(0.0, NECK_HEIGHT, 0.0) * RotateZ(theta[Head]);
    nodes[Head] = Node(m, head, NULL, NULL);

    m = Translate(TORSO_WIDTH / 2 - UPPER_LEG_WIDTH / 2, 0.1*UPPER_LEG_HEIGHT, -1.5 + UPPER_LEG_WIDTH / 2) * RotateZ(theta[LeftUpperArm]);
    nodes[LeftUpperArm] = Node(m, left_upper_arm, &nodes[RightUpperArm], &nodes[LeftLowerArm]);

    m = Translate(TORSO_WIDTH / 2 - UPPER_LEG_WIDTH / 2, 0.1*UPPER_ARM_HEIGHT, 1.5 - UPPER_ARM_WIDTH / 2) * RotateZ(theta[RightUpperArm]);
    nodes[RightUpperArm] = Node(m, right_upper_arm, &nodes[LeftUpperLeg], &nodes[RightLowerArm]);

    m = Translate(-(TORSO_WIDTH / 2 - UPPER_ARM_WIDTH / 2), 0.1*UPPER_ARM_HEIGHT, -1.5 + UPPER_ARM_WIDTH / 2) * RotateZ(theta[LeftUpperLeg]);
    nodes[LeftUpperLeg] = Node(m, left_upper_leg, &nodes[RightUpperLeg], &nodes[LeftLowerLeg]);

    m = Translate(-(TORSO_WIDTH / 2 - UPPER_LEG_WIDTH / 2), 0.1*UPPER_LEG_HEIGHT, 1.5 - UPPER_LEG_WIDTH / 2) * RotateZ(theta[RightUpperLeg]);
    nodes[RightUpperLeg] = Node(m, right_upper_leg, NULL, &nodes[RightLowerLeg]);

    m = Translate(0.0, UPPER_ARM_HEIGHT, 0.0) * RotateZ(theta[LeftLowerArm]);
    nodes[LeftLowerArm] = Node(m, left_lower_arm, NULL, NULL);

    m = Translate(0.0, UPPER_ARM_HEIGHT, 0.0) * RotateZ(theta[RightLowerArm]);
    nodes[RightLowerArm] = Node(m, right_lower_arm, NULL, NULL);

    m = Translate(0.0, UPPER_LEG_HEIGHT, 0.0) * RotateZ(theta[LeftLowerLeg]);
    nodes[LeftLowerLeg] = Node(m, left_lower_leg, NULL, NULL);

    m = Translate(0.0, UPPER_LEG_HEIGHT, 0.0) * RotateZ(theta[RightLowerLeg]);
    nodes[RightLowerLeg] = Node(m, right_lower_leg, NULL, NULL);
}


GLFWwindow* window;
const std::string TITLE = "RobotHorse";
const unsigned int WIDTH=800, HEIGHT=600;

int init_window(int width, int height, const std::string title)
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    if (window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    // Needed for core profile
    glewExperimental = GL_TRUE;

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    return 0;
}
int main()
{

    if (init_window(WIDTH, HEIGHT, TITLE) != 0)
    {
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    initBaseCube();

    // Initialize tree
    initNodes();

    theta[Torso] = 50.0;
    nodes[Torso].transform = RotateY(theta[Torso]);

    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create and initialize a buffer object
    GLuint buffer[2];
    glGenBuffers(2, buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);// vertices shader, layout=0
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);// vertices shader, layout=1
    glEnableVertexAttribArray(1);

    // Load shaders and use the resulting shader program
    GLuint program = InitShader("vshader_a9.glsl", "fshader_a9.glsl");
    glUseProgram(program);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    shader_model = glGetUniformLocation(program, "Shader_Model");

    glm::vec3 c_pos = glm::vec3(0, 5, 20); // camera position
    glm::vec3 c_dir = glm::vec3(0.0f, 0.0f, 0.0f); // camera direction
    glm::vec3 c_up = glm::vec3(0, 1, 0); // tell the camera which way is 'up'
    glm::mat4 View = glm::lookAt(c_pos, c_dir, c_up);
    GLuint Shader_View = glGetUniformLocation(program, "Shader_View");
    glUniformMatrix4fv(Shader_View, 1, GL_TRUE, glm::value_ptr(View));

    float fov=45.0f;//perspective angle
    glm::mat4 Projection = glm::perspective(fov, (float)WIDTH/(float)HEIGHT, 0.1f, 300.0f);
    GLuint Shader_Projection = glGetUniformLocation(program, "Shader_Projection");
    glUniformMatrix4fv(Shader_Projection, 1, GL_TRUE, glm::value_ptr(Projection));

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        traverse(&nodes[Torso]);


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
