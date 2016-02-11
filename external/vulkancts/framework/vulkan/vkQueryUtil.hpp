#ifndef _VKQUERYUTIL_HPP
#define _VKQUERYUTIL_HPP
/*-------------------------------------------------------------------------
 * Vulkan CTS Framework
 * --------------------
 *
 * Copyright (c) 2015 Google Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice(s) and this permission notice shall be
 * included in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 *//*!
 * \file
 * \brief Vulkan query utilities.
 *//*--------------------------------------------------------------------*/

#include "vkDefs.hpp"
#include "tcuMaybe.hpp"
#include "deMemory.h"

#include <vector>

namespace vk
{

// API queries

std::vector<VkPhysicalDevice>			enumeratePhysicalDevices				(const InstanceInterface& vk, VkInstance instance);
std::vector<VkQueueFamilyProperties>	getPhysicalDeviceQueueFamilyProperties	(const InstanceInterface& vk, VkPhysicalDevice physicalDevice);
VkPhysicalDeviceFeatures				getPhysicalDeviceFeatures				(const InstanceInterface& vk, VkPhysicalDevice physicalDevice);
VkPhysicalDeviceProperties				getPhysicalDeviceProperties				(const InstanceInterface& vk, VkPhysicalDevice physicalDevice);
VkPhysicalDeviceMemoryProperties		getPhysicalDeviceMemoryProperties		(const InstanceInterface& vk, VkPhysicalDevice physicalDevice);
VkFormatProperties						getPhysicalDeviceFormatProperties		(const InstanceInterface& vk, VkPhysicalDevice physicalDevice, VkFormat format);
VkImageFormatProperties					getPhysicalDeviceImageFormatProperties	(const InstanceInterface& vk, VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags);

VkMemoryRequirements					getBufferMemoryRequirements				(const DeviceInterface& vk, VkDevice device, VkBuffer buffer);
VkMemoryRequirements					getImageMemoryRequirements				(const DeviceInterface& vk, VkDevice device, VkImage image);

std::vector<VkLayerProperties>			enumerateInstanceLayerProperties		(const PlatformInterface& vkp);
std::vector<VkExtensionProperties>		enumerateInstanceExtensionProperties	(const PlatformInterface& vkp, const char* layerName);
std::vector<VkLayerProperties>			enumerateDeviceLayerProperties			(const InstanceInterface& vki, VkPhysicalDevice physicalDevice);
std::vector<VkExtensionProperties>		enumerateDeviceExtensionProperties		(const InstanceInterface& vki, VkPhysicalDevice physicalDevice, const char* layerName);

// Feature / extension support

bool									isShaderStageSupported					(const VkPhysicalDeviceFeatures& deviceFeatures, VkShaderStageFlagBits stage);

struct RequiredExtension
{
	std::string				name;
	tcu::Maybe<deUint32>	minVersion;
	tcu::Maybe<deUint32>	maxVersion;

	explicit RequiredExtension (const std::string&		name_,
								tcu::Maybe<deUint32>	minVersion_ = tcu::nothing<deUint32>(),
								tcu::Maybe<deUint32>	maxVersion_ = tcu::nothing<deUint32>())
		: name			(name_)
		, minVersion	(minVersion_)
		, maxVersion	(maxVersion_)
	{}
};

bool									isCompatible							(const VkExtensionProperties& extensionProperties, const RequiredExtension& required);

template<typename ExtensionIterator>
bool									isExtensionSupported					(ExtensionIterator begin, ExtensionIterator end, const RequiredExtension& required);
bool									isExtensionSupported					(const std::vector<VkExtensionProperties>& extensions, const RequiredExtension& required);

// Return variable initialization validation

typedef struct
{
	size_t		offset;
	size_t		size;
} QueryMemberTableEntry;
template <typename Context, typename Interface, typename Type>
bool validateInitComplete(Context context, void (Interface::*Function)(Context, Type*)const, const Interface& interface, const QueryMemberTableEntry* queryMemberTableEntry)
{
	const QueryMemberTableEntry	*iterator;
	Type vec[2];
	deMemset(&vec[0], 0x00, sizeof(Type));
	deMemset(&vec[1], 0xFF, sizeof(Type));

	(interface.*Function)(context, &vec[0]);
	(interface.*Function)(context, &vec[1]);

	for (iterator = queryMemberTableEntry; iterator->size != 0; iterator++)
	{
		if (deMemCmp(((deUint8*)(&vec[0]))+iterator->offset, ((deUint8*)(&vec[1]))+iterator->offset, iterator->size) != 0)
			return false;
	}

	return true;
}

// Template implementations

template<typename ExtensionIterator>
bool isExtensionSupported (ExtensionIterator begin, ExtensionIterator end, const RequiredExtension& required)
{
	for (ExtensionIterator cur = begin; cur != end; ++cur)
	{
		if (isCompatible(*cur, required))
			return true;
	}
	return false;
}

} // vk

#endif // _VKQUERYUTIL_HPP
