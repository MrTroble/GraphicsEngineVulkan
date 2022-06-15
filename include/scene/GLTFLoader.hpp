#pragma once

#include <vulkan/vulkan_core.h>

#include <memory>

#include "Model.hpp"
#include "ObjMaterial.hpp"
#include "Vertex.hpp"

namespace cpr::loader {

class GLTFLoader {
 public:
  GLTFLoader(VulkanDevice* device, VkQueue transfer_queue,
             VkCommandPool command_pool)
      : device(device),
        transfer_queue(transfer_queue),
        command_pool(command_pool) {}

  std::shared_ptr<Model> loadModel(const std::string& modelFile) const;

 private:
  VulkanDevice* device;
  VkQueue transfer_queue;
  VkCommandPool command_pool;
};

}  // namespace cpr::loader