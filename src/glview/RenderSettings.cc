#include "glview/RenderSettings.h"
#include <stdexcept>
#include <string>
#include "utils/printutils.h"

std::string renderBackend3DToString(RenderBackend3D backend)
{
  switch (backend) {
  case RenderBackend3D::CGALBackend:     return "CGAL";
  case RenderBackend3D::ManifoldBackend: return "Manifold";
  default:                               throw std::runtime_error("Unknown rendering backend");
  }
}

std::optional<RenderBackend3D> renderBackend3DFromString(std::string backend)
{
  boost::algorithm::to_lower(backend);
  if (backend == "cgal") {
    return RenderBackend3D::CGALBackend;
  } else if (backend == "manifold") {
    return RenderBackend3D::ManifoldBackend;
  } else {
    return {};
  }
}

RenderSettings *RenderSettings::inst(bool erase)
{
  static auto instance = new RenderSettings;
  if (erase) {
    delete instance;
    instance = nullptr;
  }
  return instance;
}

RenderSettings::RenderSettings()
{
  backend3D = DEFAULT_RENDERING_BACKEND_3D;
  openCSGTermLimit = 100000;
  far_gl_clip_limit = 100000.0;
  colorscheme = "Cornfield";
}

void RenderSettings::setColorOverride(RenderColor rc, const Color4f& color)
{
  color_overrides_[rc] = color;
  Color4f c = color;
  if (!c.hasAlpha()) c.setAlpha(1.0f);  // Ensure fully opaque when user picks RGB
  color_overrides_[rc] = c;
  color_override_revision_++;
}

void RenderSettings::clearColorOverrides()
{
  color_overrides_.clear();
  color_override_revision_++;
}

bool RenderSettings::hasColorOverride(RenderColor rc) const
{
  return color_overrides_.find(rc) != color_overrides_.end();
}

std::optional<Color4f> RenderSettings::getColorOverride(RenderColor rc) const
{
  if (const auto it = color_overrides_.find(rc); it != color_overrides_.end()) {
    return it->second;
  }
  return std::nullopt;
}
