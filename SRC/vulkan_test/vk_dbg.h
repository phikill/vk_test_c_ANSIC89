#ifndef VK_DBG_H
#define VK_DBG_H

/* Vulkan API Debugger Functions */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<time.h>


#define VK_USE_PLATFORM_WIN32_KHR
#include<vulkan/vulkan.h>

/*
#define VK_MAX_VALIDATION_LAYERS 1

const char* validationLayers[VK_MAX_VALIDATION_LAYERS] = 
{
    "VK_LAYER_KHRONOS_validation"
};
*/

const char* validationLayers[] = 
{
    "VK_LAYER_KHRONOS_validation"
};

const char* extensions[] = 
{
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    "VK_EXT_DEBUG_UTILS_EXTENSION_NAME"
};


#ifdef NDEBUG
    const int enableValidationLayers = 0;
#else
    const int enableValidationLayers = 1;
#endif


extern VkInstance instance;
extern VkDebugUtilsMessengerEXT debugMessenger;


// Function to get required Vulkan instance extensions
static 
const char** getRequiredExtensions(uint32_t* extensionCount) 
{
    static const char* extensions[] = 
    {
        "VK_KHR_surface",
        "VK_KHR_win32_surface",
        "VK_KHR_device_group_creation",
        "VK_KHR_display",
        "VK_KHR_external_fence_capabilities",
        "VK_KHR_external_memory_capabilities",
        "VK_KHR_external_semaphore_capabilities",
        "VK_KHR_get_display_properties2",
        "VK_KHR_get_physical_device_properties2",
        "VK_KHR_get_surface_capabilities2",
        "VK_EXT_debug_report",
        "VK_EXT_debug_utils",
        "VK_EXT_direct_mode_display",
        "VK_EXT_swapchain_colorspace",
    };

    *extensionCount = sizeof(extensions) / sizeof(extensions[0]);

    if (enableValidationLayers) 
    {
        static const char* validationExtensions[] = 
        {
            "VK_KHR_surface",
            "VK_KHR_win32_surface",
            "VK_KHR_device_group_creation",
            "VK_KHR_display",
            "VK_KHR_external_fence_capabilities",
            "VK_KHR_external_memory_capabilities",
            "VK_KHR_external_semaphore_capabilities",
            "VK_KHR_get_display_properties2",
            "VK_KHR_get_physical_device_properties2",
            "VK_KHR_get_surface_capabilities2",
            "VK_EXT_debug_report",
            "VK_EXT_debug_utils",
            "VK_EXT_direct_mode_display",
            "VK_EXT_swapchain_colorspace",
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME
        };
        *extensionCount = sizeof(validationExtensions) / sizeof(validationExtensions[0]);
        return validationExtensions;
    } 
    else 
    {
        return extensions;
    }
}




static 
int check_validation_layer_support() 
{
    uint32_t layerCount;
    size_t   i = 0;
    uint32_t j = 0;
    int      layerFound = 0;
    const char *layerName = validationLayers[i];
    VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    VkLayerProperties *availableLayers = (VkLayerProperties *)malloc(sizeof(VkLayerProperties) * layerCount);


    if (result != VK_SUCCESS || layerCount == 0) 
    {
        return 0;
    };
    if (availableLayers == NULL) 
    {
        printf("Failed to allocate memory for layer properties.\n");
        return 0;
    };
    result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
    if (result != VK_SUCCESS) 
    {
        free(availableLayers);
        return 0;
    };

    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    for (i = 0; i < sizeof(validationLayers) / sizeof(validationLayers[0]); i++) 
    {

        for (j = 0; j < layerCount; j++) 
        {
            if (strcmp(layerName, availableLayers[j].layerName) == 0) 
            {
                layerFound = 1;
                break;
            };
        };

        if (!layerFound) 
        {
            free(availableLayers);
            return 0;
        };
    };

    free(availableLayers);
    return 1;
};

// Função de callback para debug
static 
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT        messageType,
                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                            void*                                   pUserData) 
{
    // Implementação do callback (para simplificar, apenas imprimimos a mensagem)
    printf("\n [!validation layer]: %s\n", pCallbackData->pMessage);
    return VK_FALSE;
};

static 
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT *createInfo) 
{

    memset(createInfo, 0, sizeof(VkDebugUtilsMessengerCreateInfoEXT));
    
    createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT       |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT    |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo->pfnUserCallback = debugCallback;
};

static
VkResult CreateDebugUtilsMessengerEXT(VkInstance                                    instance, 
                                        const VkDebugUtilsMessengerCreateInfoEXT    *pCreateInfo, 
                                        const VkAllocationCallbacks                 *pAllocator, 
                                        VkDebugUtilsMessengerEXT                    *pDebugMessenger) 
{
    // Recupera a função vkCreateDebugUtilsMessengerEXT da instância Vulkan
    PFN_vkCreateDebugUtilsMessengerEXT func = 
        (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, 
                                                                    "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) 
    {
        //printf("Failed to get function address for vkCreateDebugUtilsMessengerEXT.\n");
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else 
    {
        printf("\n ~[ERROR!]: VK_ERROR_EXTENSION_NOT_PRESENT");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    };
};


static 
void DestroyDebugUtilsMessengerEXT(VkInstance                       instance, 
                                    VkDebugUtilsMessengerEXT        debugMessenger, 
                                    const VkAllocationCallbacks*    pAllocator) 
{
    PFN_vkDestroyDebugUtilsMessengerEXT func = 
        (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, 
                                                "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) 
    {
        func(instance, debugMessenger, pAllocator);
    };
};

void setupDebugMessenger() 
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo;

    if (!enableValidationLayers) return;

    populateDebugMessengerCreateInfo(&createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, 
                                     &createInfo, 
                                     NULL, 
                                     &debugMessenger) != VK_SUCCESS) 
    {
        printf("\n ~[ERROR!]: failed to set up debug messenger!\n");
        //exit(EXIT_FAILURE);
    };
};































#endif /* VK_DBG_H */
