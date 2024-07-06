#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<time.h>
#include<stdbool.h>


//#include<Windows.h>
#include"win32_config.h"    /* WIN32 API Window Config */


#define VK_USE_PLATFORM_WIN32_KHR  
#include<vulkan/vulkan.h>   /* Vulkan Header */

//#include"vk_dbg.h" /* Vulkan Validation Debbugger Header */
//#include"vk_device.h"
//#include"vk_swapchain.h"




/* ============================== (global statements) ============================================== */

int loopStatus = 1; /* Program Main LOOP ! */

int wWidth, /* Window Width */
    wHeight;/* Window Height */

#ifdef NDEBUG
    const int enableValidationLayers = 0;
#else
    const int enableValidationLayers = 1;
#endif

VkInstance                  instance;
VkDevice                    device;
VkDebugUtilsMessengerEXT    debugMessenger;
VkPhysicalDevice            physicalDevice = VK_NULL_HANDLE;
VkQueue                     graphicsQueue;
VkSurfaceKHR                surface;
VkQueue                     presentQueue;

const char* validationLayers[] = 
{
    "VK_LAYER_KHRONOS_validation"
};

const char* extensions[] = 
{
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    "VK_EXT_DEBUG_UTILS_EXTENSION_NAME"
};

const char* deviceExtensions[] = 
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


typedef struct 
{
    uint32_t graphicsFamily;
    uint32_t presentFamily;
} QueueFamilyIndices;

uint32_t  validationLayerCount = 0;



HWND hwnd;

/* ============================== (global statements) ============================================== */


/*_______GLOBAL FUNCTIONS DECLARATION___________*/

bool isDeviceSuitable(VkPhysicalDevice device);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

/*====================================(Vulkan Debug Code)=============================================*/



// Function to get required Vulkan instance extensions
const char** getRequiredExtensions(uint32_t* extensionCount) 
{
    static const char* extensions[] = 
    {
        "VK_KHR_surface",
        "VK_KHR_win32_surface",
        "VK_KHR_device_group_creation",
        "VK_KHR_external_fence_capabilities",
        "VK_KHR_external_memory_capabilities",
        "VK_KHR_external_semaphore_capabilities",
        "VK_KHR_get_physical_device_properties2",
        "VK_KHR_get_surface_capabilities2",
        "VK_EXT_debug_report",
        "VK_EXT_debug_utils",
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
            "VK_KHR_external_fence_capabilities",
            "VK_KHR_external_memory_capabilities",
            "VK_KHR_external_semaphore_capabilities",
            "VK_KHR_get_physical_device_properties2",
            "VK_KHR_get_surface_capabilities2",
            "VK_EXT_debug_report",
            "VK_EXT_debug_utils",
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
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT        messageType,
                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                            void*                                   pUserData) 
{
    // Implementação do callback (para simplificar, apenas imprimimos a mensagem)
    printf("\n [!validation layer]: %s\n", pCallbackData->pMessage);
    return VK_FALSE;
};

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


/*====================================(Vulkan Debug Code)=============================================*/



/*==================================Vulkan Device Code ===============================================*/






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
    QueueFamilyIndices       indices = findQueueFamilies(physicalDevice, surface);
    VkDeviceQueueCreateInfo  queueCreateInfo; //{}
    float                    queuePriority = 1.0f;
    VkPhysicalDeviceFeatures deviceFeatures; //{}
    VkDeviceCreateInfo       createInfo; //{}

    VkDeviceQueueCreateInfo queueCreateInfos[2];
    uint32_t uniqueQueueFamilies[2];
    uint32_t uniqueQueueFamilyCount = 0;
    uint32_t i;

/*
    memset(&queueCreateInfo, 0, sizeof(VkDeviceQueueCreateInfo));
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
*/
    memset(queueCreateInfos, 0, sizeof(queueCreateInfos));
    memset(&queueCreateInfo, 0, sizeof(VkDeviceQueueCreateInfo));

    if (indices.graphicsFamily != UINT32_MAX) 
    {
        uniqueQueueFamilies[uniqueQueueFamilyCount++] = indices.graphicsFamily;
    }

    if (indices.presentFamily != UINT32_MAX && indices.presentFamily != indices.graphicsFamily) 
    {
        uniqueQueueFamilies[uniqueQueueFamilyCount++] = indices.presentFamily;
    }

    for(i = 0; i < uniqueQueueFamilyCount; ++i) 
    {
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        //queueCreateInfo.queueFamilyIndex = uniqueQueueFamilies[i];
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos[i] = queueCreateInfo;
    }


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
        printf("\n ~[!ERROR]: failed to create logical device! \n");
    }


    if (indices.graphicsFamily != UINT32_MAX) 
    {
        vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
    } 
    else 
    {
        printf("\n ~[!ERROR]: Failed to retrieve graphics queue from device! \n");
    }


};


bool isComplete(QueueFamilyIndices indices)
{
    return indices.graphicsFamily != UINT32_MAX && indices.presentFamily;
}

bool isDeviceSuitable(VkPhysicalDevice device) 
{
    QueueFamilyIndices indices = findQueueFamilies(device, surface);

    return isComplete(indices);
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) 
{
    uint32_t                queueFamilyCount = 0;
    QueueFamilyIndices      indices;
    VkQueueFamilyProperties *queueFamilies;
    int                     i = 0;
    VkBool32                presentSupport = false;

    indices.graphicsFamily = UINT32_MAX;
    indices.presentFamily = UINT32_MAX;

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
        
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        
        if (presentSupport) 
        {
            indices.presentFamily = i;
        }

        if (isComplete(indices)) 
        {
            break;
        }

        i++;
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

/*==================================Vulkan Device Code ===============================================*/

void createInstance() 
{
  
    VkApplicationInfo                   appInfo;
    VkInstanceCreateInfo                createInfo;
    uint32_t                            extensionCount = 0;
    VkResult                            result;
    const char**                        extensions = NULL;
    VkExtensionProperties*              extensionProperties;
    uint32_t                            i;
    VkDebugUtilsMessengerCreateInfoEXT  debugCreateInfo;

    if (enableValidationLayers && !check_validation_layer_support()) 
    {
        printf("\n~[ERROR!]: Validation layers requested, but not available!\n");
        exit(EXIT_FAILURE);
    };

    // Contar o número de extensões de instância disponíveis
    result = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
    if (result != VK_SUCCESS) 
    {
        printf("\n~[ERROR!]: Failed to enumerate instance extension properties\n");
        return;
    };
    printf("%u extensions supported\n", extensionCount);

    // Alocar memória para as extensões e obtê-las
    extensions = (const char**) malloc(extensionCount * sizeof(const char*));
    if (extensions == NULL) 
    {
        printf("\n~[ERROR!]: Failed to allocate memory for extensions\n");
        return;
    };

    extensionProperties = (VkExtensionProperties*) malloc(extensionCount * sizeof(VkExtensionProperties));
    if (extensionProperties == NULL) 
    {
        printf("\n~[ERROR!]: Failed to allocate memory for extension properties\n");
        free(extensions);
        return;
    };

    result = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensionProperties);
    if (result != VK_SUCCESS) 
    {
        printf("\n~[ERROR!]: Failed to enumerate instance extension properties\n");
        free(extensionProperties);
        free(extensions);
        return;
    };

    printf("\n\n==========[Available extensions]==========\n");
    for (i = 0; i < extensionCount; i++) 
    {
        extensions[i] = extensionProperties[i].extensionName;
        printf("\t%s\n", extensionProperties[i].extensionName);
    };
    printf("==========[Available extensions]==========\n\n\n");

    // Obter extensões necessárias
    extensions = getRequiredExtensions(&extensionCount);

    // Zerando a estrutura para evitar lixo de memória
    memset(&appInfo,    0, sizeof(appInfo)); 
    memset(&createInfo, 0, sizeof(createInfo));

    // Inicializando VkApplicationInfo
    appInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName    = "Hello Triangle";
    appInfo.applicationVersion  = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName         = "No Engine";
    appInfo.engineVersion       = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion          = VK_API_VERSION_1_0;

    // Inicializando VkInstanceCreateInfo
    createInfo.sType                    = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo         = &appInfo;
    createInfo.enabledExtensionCount    = extensionCount;
    createInfo.ppEnabledExtensionNames  = extensions;

    memset(&debugCreateInfo, 0, sizeof(VkDebugUtilsMessengerCreateInfoEXT));
    if (enableValidationLayers) 
    {
        createInfo.enabledLayerCount = sizeof(validationLayers) / sizeof(validationLayers[0]);
        createInfo.ppEnabledLayerNames = validationLayers;

        populateDebugMessengerCreateInfo(&debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } 
    else 
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = NULL;
    }

    // Criando a instância Vulkan
    //result = vkCreateInstance(&createInfo, NULL, &instance);

    if (vkCreateInstance(&createInfo, NULL, &instance) != VK_SUCCESS) 
    {
        fprintf(stderr, "\n ~[ERROR!]: failed to create instance!\n");
        free(extensionProperties);
        free(extensions);
        //exit(EXIT_FAILURE);
    }
    else
    {
        printf("\n Vulkan instance created successfully!.\n");
    };

// Liberar memória alocada
    free(extensionProperties);
    free(extensions);
}; 
/* END: createInstance()  */






void initVulkan() 
{
    createInstance();
    
    if(enableValidationLayers) 
    {
        printf("\n Setting up (debug messenger) ...\n");
        setupDebugMessenger();
    };

    pickPhysicalDevice();

    createSurface();
    createLogicalDevice();
};

void mainLoop() 
{

};

void cleanup() 
{
    vkDestroyDevice(device, NULL);
    
    if(enableValidationLayers) 
    {
        printf("\n Destroying (debug messenger) ...\n");
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, NULL);
    }

    vkDestroySurfaceKHR(instance, surface, NULL);
        printf("\n Destroying (Vulkan Instance) ...\n");
    vkDestroyInstance(instance, NULL);

};

void run() 
{
    initVulkan();
    mainLoop();
    cleanup();
};


 
int main(int argc, char** argv) /* =============== MAIN FUNC================ */ 
{
    wWidth  = 800,
    wHeight = 600;

    initVulkan();
    //listPhysicalDevices(instance);


    engineCreateDisplay(hwnd, wWidth, wHeight);
    engineSetWindowTitle(hwnd, "Vulkan TEST !");

    runEngineLoop(hwnd, mainLoop, loopStatus);


    cleanup();
    return 0;
};


