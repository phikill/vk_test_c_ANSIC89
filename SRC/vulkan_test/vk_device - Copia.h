#ifndef VK_DEVICE_H
#define VK_DEVICE_H


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<time.h>


#define VK_USE_PLATFORM_WIN32_KHR
#include<vulkan/vulkan.h>

#include"vk_dbg.h"
#include"vk_swapchain.h"

typedef struct 
{
    int      graphicsFamily; // Índice da família de filas de gráficos
    uint32_t presentFamily;
} QueueFamilyIndices;

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

const uint32_t deviceExtensionCount = sizeof(deviceExtensions) / sizeof(deviceExtensions[0]);



void printPhysicalDeviceInfo(VkPhysicalDevice device);



VkPhysicalDevice listAndSelectFirstPhysicalDevice(VkInstance instance) 
{
    uint32_t deviceCount = 0;
    VkPhysicalDevice* physicalDevices = NULL;
    VkResult result;
    uint32_t i;
    VkPhysicalDevice selectedDevice;

    result = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    if (result != VK_SUCCESS || deviceCount == 0) 
    {
        fprintf(stderr, "\n~[ERROR!]: Failed to enumerate physical devices\n");
        return VK_NULL_HANDLE;
    }

    physicalDevices = (VkPhysicalDevice*)malloc(deviceCount * sizeof(VkPhysicalDevice));
    if (physicalDevices == NULL) 
    {
        fprintf(stderr, "\n~[ERROR!]: Failed to allocate memory for physical devices\n");
        return VK_NULL_HANDLE;
    }

    result = vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices);
    if (result != VK_SUCCESS) 
    {
        fprintf(stderr, "\n~[ERROR!]: Failed to enumerate physical devices\n");
        free(physicalDevices);
        return VK_NULL_HANDLE;
    }

    printf("\nFound %u Vulkan Physical Devices:\n\n", deviceCount);

    for (i = 0; i < deviceCount; ++i) 
    {
        printf("\nPhysical Device %u:\n", i + 1);
        printPhysicalDeviceInfo(physicalDevices[i]);

        // Selecionar o primeiro dispositivo disponível
        selectedDevice = physicalDevices[i];
        free(physicalDevices);
        return selectedDevice;
    }

    free(physicalDevices);
    fprintf(stderr, "\n~[ERROR!]: No suitable GPU found\n");
    return VK_NULL_HANDLE;
}

int isComplete(QueueFamilyIndices indices) 
{
    //return indices.graphicsFamily != -1; // -1 indica que o valor não foi inicializado
    return indices.graphicsFamily != -1 && indices.presentFamily != UINT32_MAX;
}


QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) 
{
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    VkQueueFamilyProperties* queueFamilies = NULL;
    int i;

    indices.graphicsFamily = -1; // Inicializa com um valor inválido


    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
    queueFamilies = (VkQueueFamilyProperties*) malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
    
    if (queueFamilies == NULL) 
    {
        fprintf(stderr, "Failed to allocate memory for queue families\n");
        return indices;
    }

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (i = 0; i < queueFamilyCount; ++i) 
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) 
        {
            indices.graphicsFamily = i;
        }

        if (isComplete(indices)) 
        {
            break;
        }
    }
    free(queueFamilies);

    return indices;
}

int checkDeviceExtensionSupport(VkPhysicalDevice device) 
{
    uint32_t                extensionCount;
    uint32_t                i,j;
    VkExtensionProperties*  availableExtensions;

    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);

    availableExtensions = (VkExtensionProperties*)malloc(extensionCount * sizeof(VkExtensionProperties));
    if (availableExtensions == NULL) 
    {
        fprintf(stderr, "Failed to allocate memory for extension properties\n");
        return 0;
    }

    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, availableExtensions);

    for (i = 0; i < deviceExtensionCount; i++) 
    {
        int extensionFound = 0;
        for (j = 0; j < extensionCount; j++) 
        {
            if (strcmp(deviceExtensions[i], availableExtensions[j].extensionName) == 0) 
            {
                extensionFound = 1;
                printf("Extension Found !");
                break;
            }
        }
        if (!extensionFound) 
        {
            free(availableExtensions);
            printf("Extension NOT Found !");
            return 0;
        }
    }

    free(availableExtensions);
    return 1;
}

int isDeviceSuitable(VkPhysicalDevice device) 
{
    QueueFamilyIndices indices = findQueueFamilies(device);
    int                extensionsSupported = 
                            checkDeviceExtensionSupport(device);
    int                swapChainAdequate = 0;
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);

    if (extensionsSupported) 
    {
        swapChainAdequate = formatCount > 0 && presentModeCount > 0;
        //freeSwapChainSupportDetails(&swapChainSupport);
    }

    //return isComplete(indices) && extensionsSupported;
    return isComplete(indices) && extensionsSupported && swapChainAdequate;
}


void createLogicalDevice() 
{
    VkPhysicalDevice physicalDevice = listAndSelectFirstPhysicalDevice(instance);
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    VkDeviceQueueCreateInfo queueCreateInfo;
    float queuePriority = 1.0f;
    VkPhysicalDeviceFeatures deviceFeatures;
    VkDeviceCreateInfo createInfo;

    if (physicalDevice == VK_NULL_HANDLE) 
    {
        fprintf(stderr, "\n~[ERROR!]: No suitable physical device found. Cannot create logical device.\n");
        return;
    }


    memset(&queueCreateInfo, 0, sizeof(queueCreateInfo));
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
    queueCreateInfo.queueCount = 1;


    queueCreateInfo.pQueuePriorities = &queuePriority;

    memset(&deviceFeatures, 0, sizeof(deviceFeatures));

    memset(&createInfo, 0, sizeof(createInfo));
    createInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos    = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures     = &deviceFeatures;
    createInfo.enabledExtensionCount = (uint32_t)deviceExtensionCount;
    createInfo.ppEnabledExtensionNames = deviceExtensions;

    if (enableValidationLayers) 
    {
        createInfo.enabledLayerCount = validationLayerCount;
        createInfo.ppEnabledLayerNames = validationLayers;
    } 
    else 
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, NULL, &device) != VK_SUCCESS) 
    {
        fprintf(stderr, "Failed to create logical device!\n");
        return;
    }

    vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
}


// Função para imprimir informações básicas do dispositivo físico
void printPhysicalDeviceInfo(VkPhysicalDevice device) 
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    printf("[Device Name: (%s)]\n", deviceProperties.deviceName);
    printf("   L [API Version: (%d.%d.%d)]\n",
           VK_VERSION_MAJOR(deviceProperties.apiVersion),
           VK_VERSION_MINOR(deviceProperties.apiVersion),
           VK_VERSION_PATCH(deviceProperties.apiVersion));
    printf("   L [Driver Version: (%d.%d.%d)]\n",
           VK_VERSION_MAJOR(deviceProperties.driverVersion),
           VK_VERSION_MINOR(deviceProperties.driverVersion),
           VK_VERSION_PATCH(deviceProperties.driverVersion));
    printf("   L [Vendor ID: (%u)]\n", deviceProperties.vendorID);
    printf("   L [Device ID: (%u)]\n", deviceProperties.deviceID);
    printf("   L [Device Type: (%u)]\n", deviceProperties.deviceType);

    // Exemplo de impressão de características do dispositivo
    printf("   L [Geometry Shader Support: (%s)]\n",
           deviceFeatures.geometryShader ? "(Supported!)" : "(!Not Supported)");

    printf("\n");
}

// Função para listar e imprimir informações de todos os dispositivos físicos Vulkan disponíveis
void listPhysicalDevices(VkInstance instance) 
{
    uint32_t deviceCount = 0;
    VkPhysicalDevice* physicalDevices = NULL;
    VkResult result;
    uint32_t i;

    result = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    if (result != VK_SUCCESS || deviceCount == 0) 
    {
        fprintf(stderr, "\n~[ERROR!]: Failed to enumerate physical devices\n");
        return;
    }

    physicalDevices = (VkPhysicalDevice*)malloc(deviceCount * sizeof(VkPhysicalDevice));
    if (physicalDevices == NULL) 
    {
        fprintf(stderr, "\n~[ERROR!]: Failed to allocate memory for physical devices\n");
        return;
    }

    result = vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices);
    if (result != VK_SUCCESS) 
    {
        fprintf(stderr, "\n~[ERROR!]: Failed to enumerate physical devices\n");
        free(physicalDevices);
        return;
    }

    printf("\n Found %u Vulkan Physical Devices:\n\n", deviceCount);

    for (i = 0; i < deviceCount; ++i) 
    {
        printf("\n Physical Device %u:\n", i + 1);
        printPhysicalDeviceInfo(physicalDevices[i]);
    }

    free(physicalDevices);
}





































#endif /* VK_DEVICE_H */
