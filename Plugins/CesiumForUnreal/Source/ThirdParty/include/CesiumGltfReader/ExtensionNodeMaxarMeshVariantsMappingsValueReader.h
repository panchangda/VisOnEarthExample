// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include <CesiumGltf/ExtensionNodeMaxarMeshVariantsMappingsValue.h>
#include <CesiumGltfReader/Library.h>
#include <CesiumJsonReader/JsonReader.h>
#include <CesiumJsonReader/JsonReaderOptions.h>

#include <gsl/span>
#include <rapidjson/fwd.h>

#include <vector>

namespace CesiumGltf {
struct ExtensionNodeMaxarMeshVariantsMappingsValue;
}

namespace CesiumGltfReader {

/**
 * @brief Reads {@link ExtensionNodeMaxarMeshVariantsMappingsValue} instances from JSON.
 */
class CESIUMGLTFREADER_API ExtensionNodeMaxarMeshVariantsMappingsValueReader {
public:
  /**
   * @brief Constructs a new instance.
   */
  ExtensionNodeMaxarMeshVariantsMappingsValueReader();

  /**
   * @brief Gets the options controlling how the JSON is read.
   */
  CesiumJsonReader::JsonReaderOptions& getOptions();

  /**
   * @brief Gets the options controlling how the JSON is read.
   */
  const CesiumJsonReader::JsonReaderOptions& getOptions() const;

  /**
   * @brief Reads an instance of ExtensionNodeMaxarMeshVariantsMappingsValue
   * from a byte buffer.
   *
   * @param data The buffer from which to read the instance.
   * @return The result of reading the instance.
   */
  CesiumJsonReader::ReadJsonResult<
      CesiumGltf::ExtensionNodeMaxarMeshVariantsMappingsValue>
  readFromJson(const gsl::span<const std::byte>& data) const;

  /**
   * @brief Reads an instance of ExtensionNodeMaxarMeshVariantsMappingsValue
   * from a rapidJson::Value.
   *
   * @param data The buffer from which to read the instance.
   * @return The result of reading the instance.
   */
  CesiumJsonReader::ReadJsonResult<
      CesiumGltf::ExtensionNodeMaxarMeshVariantsMappingsValue>
  readFromJson(const rapidjson::Value& value) const;

  /**
   * @brief Reads an array of instances of
   * ExtensionNodeMaxarMeshVariantsMappingsValue from a rapidJson::Value.
   *
   * @param data The buffer from which to read the array of instances.
   * @return The result of reading the array of instances.
   */
  CesiumJsonReader::ReadJsonResult<
      std::vector<CesiumGltf::ExtensionNodeMaxarMeshVariantsMappingsValue>>
  readArrayFromJson(const rapidjson::Value& value) const;

private:
  CesiumJsonReader::JsonReaderOptions _options;
};

} // namespace CesiumGltfReader