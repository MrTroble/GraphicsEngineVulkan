#include "GLTFLoader.hpp"

#include <tiny_gltf.h>

#include <MemoryHelper.hpp>

constexpr auto POSITION = "POSITION";
constexpr auto NORMAL = "NORMAL";
constexpr auto COLOR = "COLOR";
constexpr auto TEXCOORD_0 = "TEXCOORD_0";
constexpr auto TEXCOORD_1 = "TEXCOORD_1";
constexpr auto TANGENT = "TANGENT";

constexpr auto enforceSemicolon = 0;

#ifdef DEBUG
#define DEBUG_CHECK(assertion, output, exitcon) \
  if (assertion) {                              \
    std::cout << output << std::endl;           \
    exitcon;                                    \
  }                                             \
  enforceSemicolon
#else
#define DEBUG_CHECK(assertion, output, exitcon) enforceSemicolon
#endif  // DEBUG

namespace cpr::loader {

std::shared_ptr<Model> GLTFLoader::loadModel(
    const std::string &modelFile) const {
  auto model = std::make_unique<Model>(this->device);
  std::vector<Vertex> vertex;
  std::vector<unsigned int> indicies;
  std::vector<unsigned int> materialIds;
  std::vector<ObjMaterial> material;

  tinygltf::TinyGLTF gltf;
  tinygltf::Model tglModel;
  std::string error;
  std::string warning;
  constexpr auto endOfBinary = ".glb";
  constexpr auto endSize = 4;
  bool check;
  if (modelFile.substr(modelFile.length() - endSize, endSize) == endOfBinary) {
    check = gltf.LoadBinaryFromFile(&tglModel, &error, &warning, modelFile);
  } else {
    check = gltf.LoadASCIIFromFile(&tglModel, &error, &warning, modelFile);
  }

  if (!error.empty()) {
    std::cerr << "TinyGLTF: " << error;
    exit(EXIT_FAILURE);  // IDK if this is any good
  }
  if (!warning.empty()) {
    std::cout << "TinyGLTF: " << warning << std::endl;
  }
  if (!check) {
    std::cerr << "TinyGLTF: Error!";
    return nullptr;
  }

  const auto sceneID = tglModel.defaultScene < 0 ? tglModel.scenes.size() - 1
                                                 : tglModel.defaultScene;
  DEBUG_CHECK(sceneID < 0, "TinyGLTF: No scenes in this model!",
              return nullptr);
  const auto scene = tglModel.scenes[sceneID];
  DEBUG_CHECK(scene.nodes.empty(),
              "TinyGLTF: No nodes in Scene[" << scene.name << "]!");

#pragma region buffer
  std::vector<VkBuffer> bufferCreateInfos;
  bufferCreateInfos.reserve(tglModel.buffers.size());

  VkMemoryAllocateInfo allocateInfo{};
  allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

  uint32_t typeBit{};
  std::vector<size_t> bufferOffsets;
  bufferOffsets.reserve(tglModel.buffers.size());
  for (const auto &buffer : tglModel.buffers) {
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferCreateInfo.size = buffer.data.size();

    auto vkdevice = device->getLogicalDevice();

    VkBuffer vkbuffer;
    vkCreateBuffer(vkdevice, &bufferCreateInfo, nullptr, &vkbuffer);
    bufferCreateInfos.push_back(vkbuffer);

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(vkdevice, vkbuffer, &requirements);
    typeBit = requirements.memoryTypeBits;
    bufferOffsets.push_back(allocateInfo.allocationSize); 
    requirements.size + requirements.size % requirements.alignment;
  }
  allocateInfo.memoryTypeIndex =
      find_memory_type_index(device->getPhysicalDevice(), typeBit,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

#pragma endregion

#pragma region node parsing
  for (const auto nodeID : scene.nodes) {
    DEBUG_CHECK(nodeID < 0,
                "TinyGLTF: Invalid node id in Scene[" << scene.name << "]!",
                continue);
    const auto &node = tglModel.nodes[nodeID];
    if (node.mesh < 0) {
      continue;
    }

    const auto &mesh = tglModel.meshes[node.mesh];
    for (const auto &primitiv : mesh.primitives) {
      const auto primitivEnd = primitiv.attributes.cend();
      const auto positionItr = primitiv.attributes.find(POSITION);
      if (positionItr != primitivEnd) {
        const auto accessorID = positionItr->second;
        DEBUG_CHECK(acessorID < 0, "TinyGLTF: Invalid accessor in primitv!",
                    return nullptr);
        const auto &accessor = tglModel.accessors[accessorID];
      }
      const auto colorItr = primitiv.attributes.find(COLOR);
      if (colorItr != primitivEnd) {
        const auto accessorID = colorItr->second;
        DEBUG_CHECK(acessorID < 0, "TinyGLTF: Invalid accessor in primitv!",
                    return nullptr);
        const auto &accessor = tglModel.accessors[accessorID];
      }
    }
  }
#pragma endregion

  model->add_new_mesh(device, transfer_queue, command_pool, vertex, indicies,
                      materialIds, material);
  return model;
}

}  // namespace cpr::loader
