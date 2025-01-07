#include"renderer.h"
#include"renderer_types.h"
#include"glm/gtc/matrix_transform.hpp"

#include<iostream>
#include<exception>
#include<chrono>
#include<string>
#include<algorithm>

#undef APIENTRY
#define NOMINMAX

#define PI 3.1415926f


int main(int argc, char** argv) {

    try {
        float radius = 0.008;
        float restDesity = 1000.0f;
        float diam = 2 * radius;

        Renderer renderer = Renderer(800, 800, true);

        UniformRenderingObject renderingobj{};
        renderingobj.model = glm::mat4(1.0f);
        renderingobj.view = glm::lookAt(glm::vec3(1.5, 1.3, 1.5), glm::vec3(0, 0.3, 0), glm::vec3(0, 1, 0));
        renderingobj.projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        renderingobj.projection[1][1] *= -1;
        renderingobj.inv_projection = glm::inverse(renderingobj.projection);
        renderingobj.zNear = 0.1f;
        renderingobj.zFar = 10.0f;
        renderingobj.aspect = 1;
        renderingobj.fovy = glm::radians(90.0f);
        renderingobj.particleRadius = radius;
        renderer.SetRenderingObj(renderingobj);


        UniformSimulatingObject simulatingobj{};
        simulatingobj.dt = 1 / 900.0f;
        simulatingobj.restDensity = 1.0f / (diam * diam * diam);
        simulatingobj.sphRadius = 4 * radius;

        simulatingobj.coffPoly6 = 315.0f / (64 * PI * pow(simulatingobj.sphRadius, 3));
        simulatingobj.coffGradSpiky = -45 / (PI * pow(simulatingobj.sphRadius, 4));
        simulatingobj.coffSpiky = 15 / (PI * pow(simulatingobj.sphRadius, 3));

        simulatingobj.scorrK = 0.0001;
        simulatingobj.scorrQ = 0.1;
        simulatingobj.scorrN = 4;
        renderer.SetSimulatingObj(simulatingobj);

        UniformNSObject nsobj{};
        nsobj.sphRadius = 4 * radius;
        renderer.SetNSObj(nsobj);

        UniformBoxInfoObject boxinfoobj{};
        boxinfoobj.clampX = glm::vec2{ 0,1 };
        boxinfoobj.clampY = glm::vec2{ 0,1.5 };
        boxinfoobj.clampZ = glm::vec2{ 0,1 };
        boxinfoobj.clampX_still = glm::vec2{ 0,1 };
        boxinfoobj.clampY_still = glm::vec2{ 0,1.5 };
        boxinfoobj.clampZ_still = glm::vec2{ 0,1 };
        renderer.SetBoxinfoObj(boxinfoobj);

        std::vector<Particle> particles;
        for (float x = 0.0; x <= 1; x += diam) {
            for (float z = 0.0; z <= 1; z += diam) {
                for (float y = 0.02; y <= 0.15; y += diam) {
                    Particle particle{};
                    particle.Location = glm::vec3(x, y, z);
                    //particle.Velocity = glm::vec3(0, 5.0, 0);

                    particle.Mass = 1;
                    particle.NumNgbrs = 0;
                    particles.push_back(particle);
                }
            }
        }
        renderer.SetParticles(particles);

        float accumulated_time = 0.0f;
        renderer.Init();
        auto now = std::chrono::high_resolution_clock::now();
        float timer = 0.0f;
        float rorl = 0.0f;
        // 获取窗口指针
        GLFWwindow* window = renderer.GetWindow();
        // 记录上次点击的时间
        std::chrono::high_resolution_clock::time_point last_click_time = std::chrono::high_resolution_clock::now();
        const float click_cooldown = 0.5f;  // 0.5秒冷却时间
        bool leftButtonPressed = false;  // 用于跟踪鼠标左键是否被按下
        bool RightButtonPressed = false;  // 用于跟踪鼠标右键是否被按下
        std::chrono::high_resolution_clock::time_point press_start_time;  // 按下开始时间
        float velocity_multiplier = 1.0f;  // 速度乘数


        for (;;) {
            auto last = now;
            now = std::chrono::high_resolution_clock::now();
            float deltatime = std::chrono::duration<float, std::chrono::seconds::period>(now - last).count();

            float dt = std::clamp(deltatime, 1 / 900.0f, 1 / 300.0f);
            accumulated_time += dt;

            // 检查当前鼠标左键状态
            bool currentLeftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            // 检查当前鼠标右键状态
            bool currentRightButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

            if (currentLeftButtonState && !leftButtonPressed) {
                // 鼠标左键刚刚按下
                leftButtonPressed = true;
                press_start_time = now;
            }
            else if (!currentLeftButtonState && leftButtonPressed) {
                //double xpos, ypos;
                //glfwGetCursorPos(window, &xpos, &ypos);
                // 将鼠标位置转换为世界坐标，并传递给 Renderer 或其他负责处理逻辑的对象
                //renderer.HandleMouseClick(xpos, ypos,particles);

                // 将生成的粒子传递给渲染器
                //renderer.SetParticles(particles);
                //renderer.Updateinit(particles);


                {
                    //std::vector<Particle> PP;
                    particles = renderer.GetParticlesFromGPU();//取出GPU中瞬时粒子及其状态

                    // 鼠标左键刚刚释放
                    leftButtonPressed = false;

                    // 计算按压时间
                    auto press_duration = std::chrono::duration<float, std::chrono::seconds::period>(now - press_start_time).count();

                    // 根据按压时间计算速度乘数
                    velocity_multiplier = 1.0f + 5.0f * (std::sin(2 * PI * press_duration) + 1.0f) / 2.0f;

                    // 如果鼠标左键刚刚释放
                    //if (leftButtonPressed && !currentLeftButtonState) {
                    // 检查鼠标点击事件
                    //if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                    auto current_time = std::chrono::high_resolution_clock::now();
                    auto elapsed_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - last_click_time).count();
                    if (elapsed_time >= click_cooldown) {
                        std::cout << "Left mouse button clicked!" << std::endl;

                        if (particles.size() < 50000) {
                            if (particles.size() > 35722) {
                                // 删除从索引 0 到 30000 的粒子
                                particles.erase(particles.begin() + 35722, particles.end());
                            }
                            // 生成粒子

                            for (float x = 0.45; x <= 0.60; x += diam) {
                                for (float z = 0.45; z <= 0.60; z += diam) {
                                    for (float y = 0.05; y <= 0.8; y += diam) {
                                        Particle particle{};
                                        particle.Location = glm::vec3(x, y, z);
                                        particle.Velocity = glm::vec3(0, 5.0, 0);

                                        particle.Mass = 1;
                                        particle.NumNgbrs = 0;
                                        particles.push_back(particle);
                                    }
                                }
                            }
                        }

                        // 将生成的粒子添加到现有粒子列表中
                        //particles.insert(particles.end(), new_particles.begin(), new_particles.end());

                        // 将生成的粒子传递给渲染器
                        renderer.SetParticles(particles);
                        renderer.Updateinit(particles);
                        // 更新上次点击的时间
                        last_click_time = current_time;
                    }
                }
            }
            // 更新鼠标左键状态
            leftButtonPressed = currentLeftButtonState;
            // 如果鼠标左键正在按压
            /*if (leftButtonPressed) {
                // 计算按压时间
                auto press_duration = std::chrono::duration<float, std::chrono::seconds::period>(now - press_start_time).count();

                // 根据按压时间计算速度乘数
                velocity_multiplier = 1.0f + 5.0f * (std::sin(2 * PI * press_duration) + 1.0f) / 2.0f;          
            }*/


            /*// 每隔1秒执行一次的代码
            timer += dt;
            if (timer >= 2.0f && particles.size()<50000) {
                if (particles.size() > 35722) {
                    // 删除从索引 0 到 30000 的粒子
                    particles.erase(particles.begin()+35722, particles.end());
                }

                // 执行每隔1秒需要执行的代码
                std::cout << "每隔1秒执行一次的代码" << std::endl;
                for (float x = 0.45; x <= 0.60; x += diam) {
                    for (float z = 0.45; z <= 0.60; z += diam) {
                        for (float y = 0.1; y <= 0.8; y += diam) {
                            Particle particle{};
                            particle.Location = glm::vec3(x, y, z);
                            particle.Velocity = glm::vec3(0, 5.0, 0);

                            particle.Mass = 1;
                            particle.NumNgbrs = 0;
                            particles.push_back(particle);
                        }
                    }
                }
                renderer.SetParticles(particles);
                renderer.Updateinit(particles);
                

                // 重置计时器
                timer -= 2.0f;
            }*/
            simulatingobj.dt = dt;
            renderer.SetSimulatingObj(simulatingobj);

            // 如果鼠标右键刚刚释放
            if (RightButtonPressed && !currentRightButtonState && rorl == 0) {
            //if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && rorl==0) {
                std::cout << "Right mouse button clicked!" << std::endl;
                rorl = 1.0;
                boxinfoobj.clampX.y = 1 + 0.25 * (1 - glm::cos(5 * rorl));
            }
            // 更新鼠标右键状态
            RightButtonPressed = currentRightButtonState;
            if (rorl !=0) {
                if (boxinfoobj.clampX.y <=1.02) {
                    boxinfoobj.clampX.y = 1;
                    rorl = 0;
                }
                else {
                    rorl -= dt;
                    boxinfoobj.clampX.y = 1 + 0.25 * (1 - glm::cos(5 * rorl));
                }
            }
            //boxinfoobj.clampX.y = 0.5 + 0.25 * (1 - glm::cos(5 * accumulated_time));
            //boxinfoobj.clampY.x = 0 + 0.01 * (1 - glm::cos(1 * accumulated_time));
            renderer.SetBoxinfoObj(boxinfoobj);


     

            /*for (float x = 0.45; x <= 0.55; x += diam) {
                for (float z = 0.45; z <= 0.55; z += diam) {
                    for (float y = 0; y <= 0.8; y += diam) {
                        Particle particle{};
                        particle.Location = glm::vec3(x, y, z);
                        particle.Velocity = glm::vec3(0, 5.0, 0);

                        particle.Mass = 1;
                        particle.NumNgbrs = 0;
                        particles.push_back(particle);
                    }
                }
            }
            renderer.SetParticles(particles);*/

            renderer.Simulate();

            auto result = renderer.TickWindow(deltatime);

            if (result == TickWindowResult::EXIT) {
                break;
            }
            if (result != TickWindowResult::HIDE) {
                renderer.Draw();
            }

            printf("%f\n", 1 / deltatime);
        }

        renderer.Cleanup();
    }
    catch (std::runtime_error err) {
        std::cerr << err.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}