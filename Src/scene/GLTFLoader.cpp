#include "GLTFLoader.hpp"

#include <tiny_gltf.h>

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
#define DEBUG_CHECK(assertion, output, exitcon)
#endif  // DEBUG

namespace cpr::loader {

std::shared_ptr<Model> GLTFLoader::loadModel(
    const std::string& modelFile) const {
  const auto model = std::make_unique<Model>(this->device);
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

  for (const auto nodeID : scene.nodes) {
    DEBUG_CHECK(nodeID < 0,
                "TinyGLTF: Invalid node id in Scene[" << scene.name << "]!",
                continue);
    const auto node = tglModel.nodes[nodeID];
    if (node.mesh < 0) {
      continue;
    }
    const auto mesh = tglModel.meshes[node.mesh];
    for (const auto primitiv : mesh.primitives) {
      const auto primitivEnd = primitiv.attributes.cend();
      const auto iterator = primitiv.attributes.find(POSITION);
      if (iterator != primitivEnd) {
        
      }
    }
  }

  model->add_new_mesh(device, transfer_queue, command_pool, vertex, indicies,
                      materialIds, material);
  return model;
}

}  // namespace cpr::loader
