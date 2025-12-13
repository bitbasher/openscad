#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include "glview/ColorMap.h"

enum class RenderBackend3D {
  UnknownBackend,
  CGALBackend,
  ManifoldBackend,
};

inline constexpr RenderBackend3D DEFAULT_RENDERING_BACKEND_3D = RenderBackend3D::ManifoldBackend;

std::string renderBackend3DToString(RenderBackend3D backend);
std::optional<RenderBackend3D> renderBackend3DFromString(std::string backend);

class RenderSettings
{
public:
  static RenderSettings *inst(bool erase = false);

  RenderBackend3D backend3D;
  unsigned int openCSGTermLimit;
  double far_gl_clip_limit;
  std::string colorscheme;

  // Per-session color overrides applied on top of the active scheme
  void setColorOverride(RenderColor rc, const Color4f& color);
  void clearColorOverrides();
  bool hasColorOverride(RenderColor rc) const;
  std::optional<Color4f> getColorOverride(RenderColor rc) const;
  std::uint64_t colorOverrideRevision() const { return color_override_revision_; }

private:
  RenderSettings();

  std::map<RenderColor, Color4f> color_overrides_;
  std::uint64_t color_override_revision_{0};
};
