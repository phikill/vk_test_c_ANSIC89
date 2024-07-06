#ifndef VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<limits.h>
#include<time.h>


#define VK_USE_PLATFORM_WIN32_KHR
#include<vulkan/vulkan.h>


typedef struct 
{
    VkSurfaceCapabilitiesKHR 	capabilities;
    VkSurfaceFormatKHR* 		formats;
    VkPresentModeKHR* 			presentModes;
} SwapChainSupportDetails;

extern VkSurfaceKHR                surface;

    uint32_t                formatCount;
    uint32_t                presentModeCount;


SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) 
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, NULL);
    if (formatCount != 0) 
    {
        details.formats = (VkSurfaceFormatKHR*) malloc(formatCount * sizeof(VkSurfaceFormatKHR));
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats);
    } 
    else 
    {
        details.formats = NULL;
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, NULL);
    if (presentModeCount != 0) 
    {
        details.presentModes = (VkPresentModeKHR*) malloc(presentModeCount * sizeof(VkPresentModeKHR));
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes);
    } 
    else 
    {
        details.presentModes = NULL;
    }

    return details;
}

void freeSwapChainSupportDetails(SwapChainSupportDetails* details) 
{
    if (details->formats) 
    {
        free(details->formats);
        details->formats = NULL;
    }
    if (details->presentModes) 
    {
        free(details->presentModes);
        details->presentModes = NULL;
    }
}


VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR* availableFormats) 
{
	uint32_t i;

    for (i = 0; i < formatCount; i++) 
    {
        if (availableFormats[i].format 		== VK_FORMAT_B8G8R8A8_SRGB && 
            availableFormats[i].colorSpace 	== VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
        {
            return availableFormats[i];
        }
    }

    // Se o formato preferido não estiver disponível, retorna o primeiro formato disponível
    return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR* 	availablePresentModes) 
{
	uint32_t i;

    for (i = 0; i < presentModeCount; i++) 
    {
        if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) 
        {
            return availablePresentModes[i];
        }
    }

    // Se o modo preferido não estiver disponível, retorna o modo FIFO, que é garantido estar disponível
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR* capabilities, int windowWidth, int windowHeight) 
{
    VkExtent2D actualExtent;
    
    if (capabilities->currentExtent.width != UINT32_MAX) 
    {
        return capabilities->currentExtent;
    } 
    else 
    {
        actualExtent.width 	= windowWidth;
        actualExtent.height = windowHeight;

        if (actualExtent.width < capabilities->minImageExtent.width) 
        {
            actualExtent.width = capabilities->minImageExtent.width;
        }
        if (actualExtent.width > capabilities->maxImageExtent.width) 
        {
            actualExtent.width = capabilities->maxImageExtent.width;
        }

        if (actualExtent.height < capabilities->minImageExtent.height) 
        {
            actualExtent.height = capabilities->minImageExtent.height;
        }
        if (actualExtent.height > capabilities->maxImageExtent.height) 
        {
            actualExtent.height = capabilities->maxImageExtent.height;
        }

        return actualExtent;
    }
}




























#endif /* VK_SWAPCHAIN_H */
