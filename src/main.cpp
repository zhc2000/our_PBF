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
        // ��ȡ����ָ��
        GLFWwindow* window = renderer.GetWindow();
        // ��¼�ϴε����ʱ��
        std::chrono::high_resolution_clock::time_point last_click_time = std::chrono::high_resolution_clock::now();
        const float click_cooldown = 0.5f;  // 0.5����ȴʱ��
        bool leftButtonPressed = false;  // ���ڸ����������Ƿ񱻰���
        bool RightButtonPressed = false;  // ���ڸ�������Ҽ��Ƿ񱻰���
        std::chrono::high_resolution_clock::time_point press_start_time;  // ���¿�ʼʱ��
        float velocity_multiplier = 1.0f;  // �ٶȳ���


        for (;;) {
            auto last = now;
            now = std::chrono::high_resolution_clock::now();
            float deltatime = std::chrono::duration<float, std::chrono::seconds::period>(now - last).count();

            float dt = std::clamp(deltatime, 1 / 900.0f, 1 / 300.0f);
            accumulated_time += dt;

            // ��鵱ǰ������״̬
            bool currentLeftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
            // ��鵱ǰ����Ҽ�״̬
            bool currentRightButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

            if (currentLeftButtonState && !leftButtonPressed) {
                // �������ոհ���
                leftButtonPressed = true;
                press_start_time = now;
            }
            else if (!currentLeftButtonState && leftButtonPressed) {
                //double xpos, ypos;
                //glfwGetCursorPos(window, &xpos, &ypos);
                // �����λ��ת��Ϊ�������꣬�����ݸ� Renderer �������������߼��Ķ���
                //renderer.HandleMouseClick(xpos, ypos,particles);

                // �����ɵ����Ӵ��ݸ���Ⱦ��
                //renderer.SetParticles(particles);
                //renderer.Updateinit(particles);


                {
                    //std::vector<Particle> PP;
                    particles = renderer.GetParticlesFromGPU();//ȡ��GPU��˲ʱ���Ӽ���״̬

                    // �������ո��ͷ�
                    leftButtonPressed = false;

                    // ���㰴ѹʱ��
                    auto press_duration = std::chrono::duration<float, std::chrono::seconds::period>(now - press_start_time).count();

                    // ���ݰ�ѹʱ������ٶȳ���
                    velocity_multiplier = 1.0f + 5.0f * (std::sin(2 * PI * press_duration) + 1.0f) / 2.0f;

                    // ����������ո��ͷ�
                    //if (leftButtonPressed && !currentLeftButtonState) {
                    // ���������¼�
                    //if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                    auto current_time = std::chrono::high_resolution_clock::now();
                    auto elapsed_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - last_click_time).count();
                    if (elapsed_time >= click_cooldown) {
                        std::cout << "Left mouse button clicked!" << std::endl;

                        if (particles.size() < 50000) {
                            if (particles.size() > 35722) {
                                // ɾ�������� 0 �� 30000 ������
                                particles.erase(particles.begin() + 35722, particles.end());
                            }
                            // ��������

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

                        // �����ɵ�������ӵ����������б���
                        //particles.insert(particles.end(), new_particles.begin(), new_particles.end());

                        // �����ɵ����Ӵ��ݸ���Ⱦ��
                        renderer.SetParticles(particles);
                        renderer.Updateinit(particles);
                        // �����ϴε����ʱ��
                        last_click_time = current_time;
                    }
                }
            }
            // ����������״̬
            leftButtonPressed = currentLeftButtonState;
            // ������������ڰ�ѹ
            /*if (leftButtonPressed) {
                // ���㰴ѹʱ��
                auto press_duration = std::chrono::duration<float, std::chrono::seconds::period>(now - press_start_time).count();

                // ���ݰ�ѹʱ������ٶȳ���
                velocity_multiplier = 1.0f + 5.0f * (std::sin(2 * PI * press_duration) + 1.0f) / 2.0f;          
            }*/


            /*// ÿ��1��ִ��һ�εĴ���
            timer += dt;
            if (timer >= 2.0f && particles.size()<50000) {
                if (particles.size() > 35722) {
                    // ɾ�������� 0 �� 30000 ������
                    particles.erase(particles.begin()+35722, particles.end());
                }

                // ִ��ÿ��1����Ҫִ�еĴ���
                std::cout << "ÿ��1��ִ��һ�εĴ���" << std::endl;
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
                

                // ���ü�ʱ��
                timer -= 2.0f;
            }*/
            simulatingobj.dt = dt;
            renderer.SetSimulatingObj(simulatingobj);

            // �������Ҽ��ո��ͷ�
            if (RightButtonPressed && !currentRightButtonState && rorl == 0) {
            //if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && rorl==0) {
                std::cout << "Right mouse button clicked!" << std::endl;
                rorl = 1.0;
                boxinfoobj.clampX.y = 1 + 0.25 * (1 - glm::cos(5 * rorl));
            }
            // ��������Ҽ�״̬
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