// This file was generated by generate-classes.
// DO NOT EDIT THIS FILE!
#pragma once

#include "CesiumGltf/ExtensionExtFeatureMetadataTextureAccessor.h"
#include "CesiumGltf/Library.h"

#include <CesiumUtility/ExtensibleObject.h>

#include <string>

namespace CesiumGltf {
/**
 * @brief An object describing a texture used for storing per-texel feature IDs.
 */
struct CESIUMGLTF_API ExtensionExtFeatureMetadataFeatureIDTexture final
    : public CesiumUtility::ExtensibleObject {
  static inline constexpr const char* TypeName =
      "ExtensionExtFeatureMetadataFeatureIDTexture";

  /**
   * @brief The ID of the feature table in the model's root
   * `EXT_feature_metadata.featureTables` dictionary.
   */
  std::string featureTable;

  /**
   * @brief A description of the texture and channel to use for feature IDs. The
   * `channels` property must have a single channel. Furthermore, feature IDs
   * must be whole numbers in the range `[0, count - 1]` (inclusive), where
   * `count` is the total number of features in the feature table. Texel values
   * must be read as integers. Texture filtering should be disabled when
   * fetching feature IDs.
   */
  CesiumGltf::ExtensionExtFeatureMetadataTextureAccessor featureIds;
};
} // namespace CesiumGltf
