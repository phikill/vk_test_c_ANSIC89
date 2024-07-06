#ifndef VK_DEVICE_H
#define VK_DEVICE_H


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<time.h>
#include<stdint.h>
#include<stdbool.h>


#define VK_USE_PLATFORM_WIN32_KHR
#include<vulkan/vulkan.h>

//#include"vk_dbg.h"


typedef struct 
{
    uint32_t graphicsFamily;
    uint32_t presentFamily;
} QueueFamilyIndices;

/*
struct QueueFamilyIndices 
{
    uint32_t graphicsFamily;
};
*/
extern VkDevice                    device;
extern VkQueue                     graphicsQueue;
extern VkPhysicalDevice            physicalDevice;
extern VkSurfaceKHR                surface;

extern const int enableValidationLayers;

uint32_t                           validationLayerCount = 0;

const char* deviceExtensions[] = 
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};



bool isDeviceSuitable(VkPhysicalDevice device);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);




void pickPhysicalDevice() 
{
    uint32_t            deviceCount = 0;
    uint32_t            i;
    VkPhysicalDevice*   devices;


    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

    if (deviceCount == 0) 
    {
        //throw std::runtime_error("failed to find GPUs with Vulkan support!");
        printf("\n  ~[ERROR!]: failed to find GPUs with Vulkan support! \n");
    }

    devices = (VkPhysicalDevice*) malloc(deviceCount * sizeof(VkPhysicalDevice));
    if (devices == NULL) 
    {
        printf("Failed to allocate memory for physical devices.\n");
        //exit(1);
    }

    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);


    // Verifica se algum dos dispositivos físicos atende aos requisitos
    for(i = 0; i < deviceCount; i++) 
    {
        if (isDeviceSuitable(devices[i])) 
        {
            physicalDevice = devices[i];
            break;
        }
    }

    // Verifica se encontrou um dispositivo adequado
    if (physicalDevice == VK_NULL_HANDLE) 
    {
        printf("\n  ~[ERROR!]: failed to find a suitable GPU! \n");
    }


};


void createLogicalDevice() 
{
    QueueFamilyIndices       indices = findQueueFamilies(physicalDevice);
    VkDeviceQueueCreateInfo  queueCreateInfo; //{}
    float                    queuePriority = 1.0f;
    VkPhysicalDeviceFeatures deviceFeatures; //{}
    VkDeviceCreateInfo       createInfo; //{}

    memset(&queueCreateInfo, 0, sizeof(VkDeviceQueueCreateInfo));
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    memset(&deviceFeatures, 0, sizeof(VkPhysicalDeviceFeatures));

    memset(&createInfo, 0, sizeof(VkDeviceCreateInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers) 
    {
        // Defina validationLayers como um array de strings contendo os nomes das camadas de validação desejadas
        createInfo.enabledLayerCount = validationLayerCount;
        createInfo.ppEnabledLayerNames = validationLayers;
    } 
    else 
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, NULL, &device) != VK_SUCCESS) 
    {
        //throw std::runtime_error("failed to create logical device!");
        printf("\n ~[!ERROR]: failed to create logical device! \n");
    }


    if (indices.graphicsFamily != UINT32_MAX) 
    {
        vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
    } 
    else 
    {
        printf("\n ~[!ERROR]: Failed to retrieve graphics queue from device! \n");
        // Trate o erro aqui, se necessário
    }
}


bool isComplete(QueueFamilyIndices indices)
{
    return indices.graphicsFamily != UINT32_MAX;
}

bool isDeviceSuitable(VkPhysicalDevice device) 
{
    QueueFamilyIndices indices = findQueueFamilies(device);

    return isComplete(indices);
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) 
{
    uint32_t                queueFamilyCount = 0;
    QueueFamilyIndices      indices;
    VkQueueFamilyProperties *queueFamilies;
    int                     i = 0;
    VkBool32                presentSupport = false;

    indices.graphicsFamily = UINT32_MAX;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    queueFamilies = calloc(queueFamilyCount, sizeof(VkQueueFamilyProperties));
    if(queueFamilies == NULL)
    {
        printf("\n ~[!ERROR]: findQueueFamilies(){} queueFamilies = NULL; \n");
    }

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (i = 0; i < queueFamilyCount; i++) 
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) 
        {
            indices.graphicsFamily = i;
        }
        
        //vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        
        //if (presentSupport) 
        //{
         //   indices.presentFamily = i;
        //}

        if (isComplete(indices)) 
        {
            break;
        }
    }

    free(queueFamilies);

    return indices;
}



/* =============================== SURFACE ========================================= */

void createSurface() 
{
    VkWin32SurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = hwnd;
    createInfo.hinstance = hInstance;
    createInfo.pNext = NULL;
    createInfo.flags = 0;


    if(vkCreateWin32SurfaceKHR(instance, &createInfo, NULL, &surface) != VK_SUCCESS) 
    {
        printf("\n ~[ERROR!]: failed to create window surface! \n");
    }


}


/* =============================== SURFACE ========================================= */




























#endif /* VK_DEVICE_H */
