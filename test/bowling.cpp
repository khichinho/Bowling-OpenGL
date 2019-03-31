#include <iostream>

#include <glad/glad.h>
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"

#include <SOIL/SOIL.h>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 2.5f, 7.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
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

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    Shader pinShader("pin.vs", "pin.fs");

    // load models
    vector<Model> pinModels;
    for(int i=0; i<10; i++){
        Model pinModel("../resources/Bowling/BowlingPins/BowlingPins.obj");
        pinModels.push_back(pinModel);
    }

    // build and compile shaders
    Shader ballShader("ball.vs", "ball.fs");

    // load models
    Model ballModel("../resources/Bowling/Ball/Ball.obj");










    // build and compile our shader zprogram
    Shader floorShader("floor_camera.vs", "floor_camera.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] = {
        // positions          // texture coords
         100.0f, 0.0f,  0.0f,   1.0f, 1.0f, // top right
         100.0f, 0.0f, -1500.0f,   1.0f, 0.0f, // bottom right
        -100.0f, 0.0f, -1500.0f,   0.0f, 0.0f, // bottom left
        -100.0f, 0.0f,  0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

   // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // load and create a texture 
    // -------------------------
    unsigned int texture1;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); 
     // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height;
    unsigned char *data = SOIL_load_image("wooden_floor.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    SOIL_free_image_data(data);

    // activate shader
    floorShader.use();
    floorShader.setInt("texture1", 0);


    // ? draw in wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);









    int pinsDistance = 30.0f;
    int omega = 0.0f;
    int speed = -3.0f;
    int distance = 0.0f;
    // pins data
    glm::vec3 pinLocation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 ballLocation = glm::vec3(0.0f, 0.0f, 0.0f);

    vector<glm::vec3> pinLocationVector;

    int isBallonFloor = 1;
    vector<int> isPinHit;
    for(int i=0; i<10; i++) isPinHit.push_back(0);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);





        ///////////// PINS ////////////////
        // don't forget to enable shader before setting uniforms
        pinShader.use();

        // view/projection transformations
        glm::mat4 pinProjection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        pinShader.setMat4("projection", pinProjection);

        glm::mat4 pinView = camera.GetViewMatrix();
        pinShader.setMat4("view", pinView);
        
        glm::mat4 pinModel = glm::mat4(1.0f);
        pinShader.setMat4("model", pinModel);
        pinModel = glm::scale(pinModel, glm::vec3(0.02f, 0.02f, 0.02f));	// it's a bit too big for our scene, so scale it down

        for(int i=0; i<10; i++)
        {
            // render the loaded model
            if(i==0){
                pinModel = glm::translate(pinModel, glm::vec3(1.5*pinsDistance, -10.9f, -1450.0f));
                pinLocation += glm::vec3(1.5*pinsDistance, 0.0f, -1450.0f);
            }
            pinShader.setMat4("model", pinModel);

            if(isPinHit.at(i) == 0){
                pinModels[i].Draw(pinShader);
            }

            // arrangment for pin structure
            if(i==3){
                pinModel = glm::translate(pinModel, glm::vec3(2.5*pinsDistance, 0.0f, pinsDistance));
                pinLocation += glm::vec3(2.5*pinsDistance, 0.0f, pinsDistance);
            }
            else if(i==6){
                pinModel = glm::translate(pinModel, glm::vec3(1.5*pinsDistance, 0.0f, pinsDistance));
                pinLocation += glm::vec3(1.5*pinsDistance, 0.0f, pinsDistance);
            }
            else if(i==8){
                pinModel = glm::translate(pinModel, glm::vec3(0.5*pinsDistance, 0.0f, pinsDistance));
                pinLocation += glm::vec3(0.5*pinsDistance, 0.0f, pinsDistance);
            }
            else{
                pinModel = glm::translate(pinModel, glm::vec3(-pinsDistance, 0.0f, 0.0f));
                pinLocation += glm::vec3(-pinsDistance, 0.0f, 0.0f);
            }
            pinLocationVector.push_back(pinLocation);
        }








        ///////////// BALL ////////////////
        // view/projection transformations
        glm::mat4 ballProjection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        pinShader.setMat4("projection", ballProjection);

        glm::mat4 ballView = camera.GetViewMatrix();
        pinShader.setMat4("view", ballView);
        
        glm::mat4 ball_Model = glm::mat4(1.0f);
        pinShader.setMat4("model", ball_Model);

        ball_Model = glm::scale(ball_Model, glm::vec3(0.02f, 0.02f, 0.02f));	// it's a bit too big for our scene, so scale it down

        distance += speed;
        ball_Model = glm::translate(ball_Model, glm::vec3(0.0f, 0.0f, distance));
        ballLocation += glm::vec3(0.0f, 0.0f, distance);

        // don't forget to enable shader before setting uniforms
        ballShader.use();

        // view/projection transformations
        ballShader.setMat4("projection", ballProjection);
        ballShader.setMat4("view", ballView);

        // render the loaded model
        ballShader.setMat4("model", ball_Model);
        ball_Model = glm::translate(ball_Model, glm::vec3(0.0f, -11.0f, 0.0f));

        if(isBallonFloor == 1 ){ ballModel.Draw(ballShader); }




        // intersection loop here
        for(int i=0; i<10; i++){

        }
        if(ballLocation.z < -5050/0.02 ){
            cout<<ballLocation.z;
            isBallonFloor = 0;
        }





        // view/projection transformations
        glm::mat4 floorProjection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        pinShader.setMat4("projection", floorProjection);

        glm::mat4 floorView = camera.GetViewMatrix();
        pinShader.setMat4("view", floorView);
        
        glm::mat4 floorModel = glm::mat4(1.0f);
        pinShader.setMat4("model", floorModel);

        floorModel = glm::scale(floorModel, glm::vec3(0.02f, 0.02f, 0.02f)); // it's a bit too big for our scene, so scale it down
        floorModel = glm::translate(floorModel, glm::vec3(0.0f, -11.0f, 0.0f));

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        floorShader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        floorShader.setMat4("projection", floorProjection);

        // camera/view transformation
        floorShader.setMat4("view", floorView);

        // render boxes
        glBindVertexArray(VAO);
        floorShader.setMat4("model", floorModel);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);





        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, 5*deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, 5*deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, 5*deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, 5*deltaTime);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}