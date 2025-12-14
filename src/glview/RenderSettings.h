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
  void clearSchemeOverrides(const std::string& scheme);
  bool hasColorOverride(RenderColor rc) const;
  std::optional<Color4f> getColorOverride(RenderColor rc) const;
  std::uint64_t colorOverrideRevision() const { return color_override_revision_; }
  bool hasColorOverrides() const { return !color_overrides_.empty(); }
  bool hasSchemeOverrides(const std::string& scheme) const;
  std::map<RenderColor, Color4f> getSchemeOverrides(const std::string& scheme) const;
  void setCurrentScheme(const std::string& scheme) { current_scheme_ = scheme; }

private:
  RenderSettings();

  std::map<RenderColor, Color4f> color_overrides_;
  std::uint64_t color_override_revision_{0};
  std::string current_scheme_;
  // Per-scheme overrides: scheme_name -> map of color overrides
  std::map<std::string, std::map<RenderColor, Color4f>> scheme_overrides_;
};
