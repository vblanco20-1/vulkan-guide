
#
# generate_features_chain.py
#
# Copyright © 2025 Charles Giessen (charles@lunarg.com)
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
# documentation files (the “Software”), to deal in the Software without restriction, including without
# limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
# LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# This file is a part of VkBootstrap
# https://github.com/charles-lunarg/vk-bootstrap

# This script requires the vulkan_object package to be installed
# https://github.com/KhronosGroup/vulkan-object

import os
import codecs
from vulkan_object import get_vulkan_object

vk = get_vulkan_object()

header = '''/*
 * Copyright © 2025 Charles Giessen (charles@lunarg.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the “Software”), to deal in the Software without restriction, including without
 * limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

// This file is a part of VkBootstrap
// https://github.com/charles-lunarg/vk-bootstrap

#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>
'''

namespace_decl = '''
namespace vkb::detail {

'''

# Structures that used to have a protect (like beta ext being moved to non-beta
HEADER_VERSION_MISSING_DEFINES = {
    'VkPhysicalDevicePresentMeteringFeaturesNV': {'promoted_version' : '345', 'protect' : 'VK_ENABLE_BETA_EXTENSIONS'},
}

feature_struct_ext_map = {}
feature_struct_ext_stype_map = {}

def setup_feature_struct_aliases():
    for feature in [x for x in vk.structs.values() if x.extends is not None and 'VkPhysicalDeviceFeatures2' in x.extends and len(x.aliases) >0]:
        type_list = [feature.name] + feature.aliases
        for type_name in type_list:
            for ext in feature.extensions:
                if vk.extensions[ext].vendorTag is not None and type_name.endswith(vk.extensions[ext].vendorTag):
                    feature_struct_ext_map[type_name] = ext
                    for sTypes in vk.extensions[ext].enumFields['VkStructureType']:
                        if sTypes.name == feature.sType and type_name is not feature.name:
                            feature_struct_ext_stype_map[type_name] = sTypes.aliases[0]
                            break
                    break

def get_struct_guards(struct, struct_name):
    reqs = []
    if struct.protect is not None:
        reqs.append( f'defined({struct.protect})')
    if struct_name in HEADER_VERSION_MISSING_DEFINES:
        version = HEADER_VERSION_MISSING_DEFINES[struct_name]['promoted_version']
        protect = HEADER_VERSION_MISSING_DEFINES[struct_name]['protect']
        reqs.append(f'(VK_HEADER_VERSION >= {version} || defined({protect}))')
    enablement = []
    if struct.version is not None:
        enablement.append(f'defined({struct.version.name})')
    if struct_name in feature_struct_ext_map:
        enablement.append(f'defined({feature_struct_ext_map[struct_name]})')
    else:
        for ext in struct.extensions:
            enablement.append(f'defined({ext})')
    reqs.append('(' + ' || '.join(enablement) + ')')
    return ' && '.join(reqs)

def get_struct_guards_start(struct, struct_name):
    guard = get_struct_guards(struct, struct_name)
    return f'#if {guard}\n' if guard else '\n'

def get_struct_guards_end(struct, struct_name):
    guard = get_struct_guards(struct, struct_name)
    return f'#endif //{guard}\n' if guard else '\n'

def print_required_platform_defines():
    required_platforms = set()
    for feature in [x for x in vk.structs.values() if x.extends is not None and 'VkPhysicalDeviceFeatures2' in x.extends]:
        if feature.protect:
            required_platforms.add(feature.protect)

    required_platforms = sorted(required_platforms)
    out = ''
    for required_platform in required_platforms:
        out += f'#if defined({required_platform})\n'
        if required_platform == 'VK_ENABLE_BETA_EXTENSIONS':
            out += '#include <vulkan/vulkan_beta.h>\n'
        elif required_platform == 'VK_USE_PLATFORM_SCREEN_QNX':
            out += '#include <screen/screen.h>\n'
            out += '#include <vulkan/vulkan_screen.h>\n'
        elif required_platform == 'VK_USE_PLATFORM_ANDROID_KHR':
            out += '#include <vulkan/vulkan_android.h>\n'
        else:
            raise RuntimeError(f'Unhandled required platform {required_platform}!')
        out += f'#endif // defined({required_platform})\n'
    return out

def create_feature_chain_declarations():
    out = ''
    out += 'void compare_VkPhysicalDeviceFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceFeatures const& supported, VkPhysicalDeviceFeatures const& requested);\n'
    out += 'void merge_VkPhysicalDeviceFeatures(VkPhysicalDeviceFeatures & current, VkPhysicalDeviceFeatures const& merge_in);\n'

    for feature in [x for x in vk.structs.values() if x.extends is not None and 'VkPhysicalDeviceFeatures2' in x.extends]:
        type_list = [feature.name] + feature.aliases
        for type_name in type_list:
            out += get_struct_guards_start(feature, type_name)
            out += f'void compare_{type_name}(std::vector<std::string> & error_list, {type_name} const& supported, {type_name} const& requested);\n'
            out += f'void merge_{type_name}({type_name} & current, {type_name} const& merge_in);\n'
            out += get_struct_guards_end(feature, type_name)

    out += 'void compare_feature_struct(VkStructureType sType, std::vector<std::string> & error_list, const void* supported, const void* requested);\n'
    out += 'void merge_feature_struct(VkStructureType sType, void* current, const void* merge_in);\n'
    return out

def create_feature_chain_definitions():
    out = ''

    out += 'void compare_VkPhysicalDeviceFeatures(std::vector<std::string> & error_list, VkPhysicalDeviceFeatures const& supported, VkPhysicalDeviceFeatures const& requested) {\n'
    for member in vk.structs['VkPhysicalDeviceFeatures'].members:
        if member.name in ['sType', 'pNext']:
            continue
        out += f'    if (requested.{member.name} && !supported.{member.name}) {{\n        error_list.push_back("Missing feature VkPhysicalDeviceFeatures::{member.name}");\n    }}\n'
    out += '}\n'
    out += 'void merge_VkPhysicalDeviceFeatures(VkPhysicalDeviceFeatures & current, VkPhysicalDeviceFeatures const& merge_in) {\n'
    for member in vk.structs['VkPhysicalDeviceFeatures'].members:
        if member.name in ['sType', 'pNext']:
            continue
        out += f'    current.{member.name} = current.{member.name} || merge_in.{member.name};\n'
    out += '}\n'

    for feature in [x for x in vk.structs.values() if x.extends is not None and 'VkPhysicalDeviceFeatures2' in x.extends]:
        type_list = [feature.name] + feature.aliases
        for type_name in type_list:
            out += get_struct_guards_start(feature, type_name)

            out += f'void compare_{type_name}(std::vector<std::string> & error_list, {type_name} const& supported, {type_name} const& requested) {{\n'
            for member in feature.members:
                if member.name in ['sType', 'pNext']:
                    continue
                out += f'    if (requested.{member.name} && !supported.{member.name}) {{\n        error_list.push_back("Missing feature {type_name}::{member.name}");\n    }}\n'

            out += '}\n'
            out += f'void merge_{type_name}({type_name} & current, {type_name} const& merge_in) {{\n'
            for member in feature.members:
                if member.name in ['sType', 'pNext']:
                    continue
                out += f'    current.{member.name} = current.{member.name} || merge_in.{member.name};\n'

            out += '}\n'
            out += get_struct_guards_end(feature, type_name)

    out += 'void compare_feature_struct(VkStructureType sType, std::vector<std::string> & error_list, const void* supported, const void* requested) {\n'
    out += '    switch (sType) {\n'
    for feature in [x for x in vk.structs.values() if x.extends is not None and 'VkPhysicalDeviceFeatures2' in x.extends]:
        type_list = [feature.name] + feature.aliases
        printed_already = False
        guard = None
        for type_name in type_list:
            guard = get_struct_guards(feature, type_name)
            out += f'#{"el" if printed_already else ""}if {guard}\n'
            printed_already = True
            out += f'        case({feature_struct_ext_stype_map[type_name] if type_name in feature_struct_ext_stype_map else feature.sType}):\n'
            out += f'            compare_{type_name}(error_list, *reinterpret_cast<const {type_name}*>(supported), *reinterpret_cast<const {type_name}*>(requested));\n'
            out += '            break;\n'
        if not guard is None:
            out += '#endif\n'
    out += '        default:\n'
    out += '            break;\n'
    out += '    }\n'
    out += '}\n'

    out += 'void merge_feature_struct(VkStructureType sType, void* current, const void* merge_in) {\n'
    out += '    switch (sType) {\n'
    for feature in [x for x in vk.structs.values() if x.extends is not None and 'VkPhysicalDeviceFeatures2' in x.extends]:
        type_list = [feature.name] + feature.aliases
        printed_already = False
        guard = None
        for type_name in type_list:
            guard = get_struct_guards(feature, type_name)
            out += f'#{"el" if printed_already else ""}if {guard}\n'
            printed_already = True
            out += f'        case({feature_struct_ext_stype_map[type_name] if type_name in feature_struct_ext_stype_map else feature.sType}):\n'
            out += f'            merge_{type_name}(*reinterpret_cast<{type_name}*>(current), *reinterpret_cast<const {type_name}*>(merge_in));\n'
            out += '            break;\n'
        if not guard is None:
            out += '#endif\n'
    out += '        default:\n'
    out += '            break;\n'
    out += '    }\n'
    out += '}\n'

    return out

tail = '} // namespace vkb'

# find the version used to generate the code
path_to_src = os.path.join('src')
if not os.path.exists(path_to_src):
    path_to_src = os.path.join('..', 'src')
if not os.path.exists(path_to_src):
    print('Could not find source folder. Is the current directory wrong?')
    exit(-1)

setup_feature_struct_aliases()

features_chain = codecs.open(os.path.join(path_to_src,'VkBootstrapFeatureChain.h'), 'w', 'utf-8')
features_chain.write(header + print_required_platform_defines() + namespace_decl + create_feature_chain_declarations() + tail)
features_chain.close()

features_chain_inl = codecs.open(os.path.join(path_to_src,'VkBootstrapFeatureChain.inl'), 'w', 'utf-8')
features_chain_inl.write(header + namespace_decl + create_feature_chain_definitions() + tail)
features_chain_inl.close()

path_to_gen = os.path.join('gen')
if not os.path.exists(path_to_gen):
    path_to_gen = os.path.join('..', 'gen')
if not os.path.exists(path_to_gen):
    print('Could not find gen folder. Is the current directory wrong?')
    exit(-1)

print('Generation finished.')
