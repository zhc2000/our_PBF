#ifndef RENDER_H
#define RENDER_H
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include"GLFW/glfw3.h"
#include"glm/glm.hpp"
#include"renderer_types.h"

#include<vector>
#include<array>
#include<optional>
#include<string>

class Renderer{
public:
    Renderer(uint32_t w,uint32_t h,bool validation = false);
    virtual ~Renderer();
public:
    TickWindowResult TickWindow(float DeltaTime);
    void Init();
    void Cleanup();
    void Updateinit(std::vector<Particle>& particles);
public:
    void Simulate();
    void BoxRender(uint32_t dstimage);
    void FluidsRender(uint32_t dstimage);
    void Draw();
public:
    void WaitIdle();
private:
    void CreateInstance();
    void CreateDebugMessenger();
    void CreateSurface();
    void PickPhysicalDevice();
    void CreateLogicalDevice();

    void CreateSupportObjects();
    void CleanupSupportObjects();
    void CreateCommandPool();

    void CreateParticleBuffer();
    void CreateParticleNgbrBuffer();

    void CreateUniformRenderingBuffer();
    void CreateUniformSimulatingBuffer();
    void CreateUniformNSBuffer();
    void CreateUniformBoxInfoBuffer();

    void CreateRadixsortedIndexBuffer();
    void CreateRSGlobalBucketBuffer();
    void CreateCellinfoBuffer();
    void CreateLocalPrefixBuffer();

    void CreateDepthResources();
    void CreateThickResources();
    void CreateDefaultTextureResources();
    void CreateBackgroundResources();

    void CreateSwapChain();
    void CleanupSwapChain();
    void RecreateSwapChain();

    void CreateDescriptorSetLayout();
    void CreateDescriptorPool();
    void CreateDescriptorSet();
    void UpdateDescriptorSet();

    void CreateRenderPass();
    void CreateGraphicPipelineLayout();
    void CreateGraphicPipeline();

    void CreateComputePipelineLayout();
    void CreateComputePipeline();

    void CreateFramebuffers();

    void RecordSimulatingCommandBuffers();
    void RecordFluidsRenderingCommandBuffers();
    void RecordBoxRenderingCommandBuffers();

    void UpdateParticleBuffer(std::vector<Particle>& particles);


private:
    void GetRequestInstaceExts(std::vector<const char*>& exts);
    void GetRequestInstanceLayers(std::vector<const char*>& layers);
    void MakeMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& createinfo);
    QueuefamliyIndices GetPhysicalDeviceQueueFamilyIndices(VkPhysicalDevice pdevice);
    bool IsPhysicalDeviceSuitable(VkPhysicalDevice pdevice);
    void GetRequestDeviceExts(std::vector<const char*>& exts);
    void GetRequestDeviceFeature(VkPhysicalDeviceFeatures& features);
    SurfaceDetails GetSurfaceDetails();
    VkSurfaceFormatKHR ChooseSwapChainImageFormat(std::vector<VkSurfaceFormatKHR>& formats);
    VkPresentModeKHR ChooseSwapChainImagePresentMode(std::vector<VkPresentModeKHR>& presentmodes);
    VkExtent2D ChooseSwapChainImageExtents(VkSurfaceCapabilitiesKHR& capabilities);
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,void* pUserData);
    static void  WindowResizeCallback(GLFWwindow* window,int width,int height);
    VkShaderModule MakeShaderModule(const char* filename);

    VkCommandBuffer CreateCommandBuffer();
    void SubmitCommandBuffer(VkCommandBuffer& cb,VkSubmitInfo submitinfom,VkFence fence,VkQueue queue);

    void CreateBuffer(VkBuffer& buffer,VkDeviceMemory& memory,VkDeviceSize size,VkBufferUsageFlags usage,VkMemoryPropertyFlags memproperties);
    uint32_t ChooseMemoryType(uint32_t typefilter,VkMemoryPropertyFlags properties);
    void CleanupBuffer(VkBuffer& buffer,VkDeviceMemory& memory,bool mapped);
    void CreateImage(VkImage &image, VkDeviceMemory &memory,VkExtent3D extent,VkFormat format, VkImageUsageFlags usage,VkSampleCountFlagBits samplecount);
    void ImageLayoutTransition(VkImage& image,VkImageLayout oldlayout,VkImageLayout newlayout,VkImageAspectFlags asepct);
    //void UpdateParticleBuffer(const std::vector<Particle>& newParticles);

private:
    VkImageView CreateImageView(VkImage image,VkFormat format,VkImageAspectFlags aspectMask);
private:
    GLFWwindow* Window = nullptr;
    VkInstance Instance;
    VkDebugUtilsMessengerEXT Messenger;
    VkSurfaceKHR Surface;

    VkPhysicalDevice PDevice;
    VkDevice LDevice;
    VkQueue GraphicNComputeQueue;
    VkQueue PresentQueue;

    VkCommandPool CommandPool;
    VkSwapchainKHR SwapChain;
    VkFormat SwapChainImageFormat;
    VkExtent2D SwapChainImageExtent;
    std::vector<VkImage> SwapChainImages;
    std::vector<VkImageView> SwapChainImageViews;

    VkDescriptorPool DescriptorPool;

    VkDescriptorSetLayout NSDescriptorSetLayout;
   
    std::vector<VkDescriptorSet> NSDescriptorSets[2];

    VkPipelineLayout NSPipelineLayout;
    VkPipeline NSPipeline_CalcellHash;
    VkPipeline NSPipeline_Radixsort1;
    VkPipeline NSPipeline_Radixsort2;
    VkPipeline NSPipeline_Radixsort3;
    VkPipeline NSPipeline_FixcellBuffer;
    VkPipeline NSPipeline_GetNgbrs;

    VkRenderPass FluidGraphicRenderPass;
    VkDescriptorSetLayout FluidGraphicDescriptorSetLayout;
    VkDescriptorSet FluidGraphicDescriptorSet;
    VkPipelineLayout FluidGraphicPipelineLayout;
    VkPipeline FluidGraphicPipeline;

    VkRenderPass BoxGraphicRenderPass;
    VkDescriptorSetLayout BoxGraphicDescriptorSetLayout;
    VkDescriptorSet BoxGraphicDescriptorSet;
    VkPipelineLayout BoxGraphicPipelineLayout;
    VkPipeline BoxGraphicPipeline;

    VkDescriptorSetLayout SimulateDescriptorSetLayout;
    std::vector<VkDescriptorSet> SimulateDescriptorSet;
    VkPipelineLayout SimulatePipelineLayout;
    VkPipeline SimulatePipeline_Euler;
    VkPipeline SimulatePipeline_Lambda;
    VkPipeline SimulatePipeline_DeltaPosition;
    VkPipeline SimulatePipeline_PositionUpd;
    VkPipeline SimulatePipeline_VelocityUpd;
    VkPipeline SimulatePipeline_VelocityCache;
    VkPipeline SimulatePipeline_ViscosityCorr;
    VkPipeline SimulatePipeline_VorticityCorr;

    VkDescriptorSetLayout FilterDecsriptorSetLayout;
    VkDescriptorSet FilterDescriptorSet;
    VkPipelineLayout FilterPipelineLayout;
    VkPipeline FilterPipeline;

    VkDescriptorSetLayout PostprocessDescriptorSetLayout;
    std::vector<VkDescriptorSet> PostprocessDescriptorSets;
    VkPipelineLayout PostprocessPipelineLayout;
    VkPipeline PostprocessPipeline;

    VkFence DrawingFence;
    VkSemaphore ImageAvaliable;
    VkSemaphore FluidsRenderingFinish;
    VkSemaphore BoxRenderingFinish;
    VkSemaphore SimulatingFinish;

    VkBuffer UniformRenderingBuffer;
    VkDeviceMemory UniformRenderingBufferMemory;
    void* MappedRenderingBuffer;

    VkBuffer UniformSimulatingBuffer;
    VkDeviceMemory UniformSimulatingBufferMemory;
    void* MappedSimulatingBuffer;

    VkBuffer UniformNSBuffer;
    VkDeviceMemory UniformNSBufferMemory;
    void* MappedNSBuffer;

    VkBuffer UniformBoxInfoBuffer;
    VkDeviceMemory UniformBoxInfoBufferMemory;
    void* MappedBoxInfoBuffer;


    VkImage ThickImage;
    VkDeviceMemory ThickImageMemory;
    VkImageView ThickImageView;
    VkSampler ThickImageSampler;

    VkImage DepthImage;
    VkDeviceMemory DepthImageMemory;
    VkImageView DepthImageView;

    VkImage CustomDepthImage;
    VkDeviceMemory CustomDepthImageMemory;
    VkImageView CustomDepthImageView;
    VkSampler CustomDepthImageSampler;

    VkImage FilteredDepthImage;
    VkDeviceMemory FilteredDepthImageMemory;
    VkImageView FilteredDepthImageView;
    VkSampler FilteredDepthImageSampler;

    VkImage DefaultTextureImage;
    VkDeviceMemory DefaultTextureImageMemory;
    VkImageView DefaultTextureImageView;
    VkSampler DefaultTextureImageSampler;

    VkImage BackgroundImage;
    VkDeviceMemory BackgroundImageMemory;
    VkImageView BackgroundImageView;
    VkSampler BackgroundImageSampler;

    VkFramebuffer FluidsFramebuffer;
    VkFramebuffer BoxFramebuffer;

    std::vector<VkBuffer> ParticleBuffers;
    std::vector<VkDeviceMemory> ParticleBufferMemory;

    VkBuffer ParticleNgbrBuffer;
    VkDeviceMemory ParticleNgbrBufferMemory;

    VkBuffer RadixsortedIndexBuffer[2];
    VkDeviceMemory RadixsortedIndexBufferMemory[2];

    VkBuffer RSGlobalBucketBuffer;
    VkDeviceMemory RSGlobalBucketBufferMemory;

    VkBuffer LocalPrefixBuffer;
    VkDeviceMemory LocalPrefixBufferMemory;

    VkBuffer CellinfoBuffer;
    VkDeviceMemory CellinfoBufferMemory;

    VkBuffer BoxVertexBuffer;
    VkDeviceMemory BoxVertexBufferMemory;

    std::vector<VkCommandBuffer> SimulatingCommandBuffers;
    std::vector<VkCommandBuffer> FluidsRenderingCommandBuffers[2];
    VkCommandBuffer BoxRenderingCommandBuffer;
public:
    void SetRenderingObj(const UniformRenderingObject& robj);
    void SetSimulatingObj(const UniformSimulatingObject& sobj);
    void SetNSObj(const UniformNSObject& nobj);
    void SetBoxinfoObj(const UniformBoxInfoObject& bobj);
    void SetParticles(const std::vector<Particle>& ps);
    //void SetInnerBoxInfo(const UniformBoxInfoObject& boxInfo);
private:
    
    bool Initialized = false;
    uint32_t Width;
    uint32_t Height;
    std::vector<Particle> particles;

    UniformNSObject nsobject{};
    UniformRenderingObject renderingobj{};
    UniformSimulatingObject simulatingobj{};
    UniformBoxInfoObject boxinfobj{};

    bool bEnableValidation = false;
    uint32_t CurrentFlight = 0;
    uint32_t MAXInFlightRendering = 2;

    uint32_t ONE_GROUP_INVOCATION_COUNT = 512;
    uint32_t WORK_GROUP_COUNT;

    uint32_t MAX_NGBR_NUM = 128;
    bool bFramebufferResized = false;

    //std::vector<Particle> particles; // 存储所有粒子
    //int maxParticles = 50000; // 最大粒子数限制
    //float accumulated_time = 0.0f; // 累积时间，用于控制粒子生成
    //UniformBoxInfoObject innerBoxInfo; // 用于存储新box的信息

public:
    GLFWwindow* GetWindow() const {
        return Window;
    }
    std::vector<Particle> GetParticlesFromGPU();
    void HandleMouseClick(double screenX, double screenY, std::vector<Particle>& particles);
    void ApplyForceAroundPoint(const glm::vec3& point, float radius, float forceMagnitude, std::vector<Particle>& particles);


    //void UpdateParticleBuffer(const std::vector<Particle>& newParticles);
    //void GenerateParticles(float deltaTime);
    //void AddParticles(const std::vector<Particle>& newParticles);
    //void UpdateParticleBuffers();

};
#endif