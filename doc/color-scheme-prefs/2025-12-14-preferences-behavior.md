# Color Scheme Preferences – Behavior (2025-12-14)

## Scope
- Summarizes current behavior of Preferences interactions with editor color schemes and 3D render schemes.
- Captures live-update wiring, focus scoping, and preview sample handling as of Dec 14, 2025.
- No persistence of edited schemes is implemented; all overrides are session-only.

## Editor Schemes
- **Scope:** Only the last-focused editor receives scheme changes and per-color live edits.
- **Windows:** Focus tracking is global; when a specific editor gains focus, it becomes the sole target for later scheme or per-color updates. Editors in other windows are unaffected unless they gain focus.
- **Live per-color updates:** `Preferences::editorColorChanged` targets the last-focused editor and applies colors immediately via `ScintillaEditor::applyTemporaryColor` (paper/text and lexertl style keys such as operator, comment, number, string, variables, keywords, transformations, booleans, functions, models, special-variables, keyword-customN).
- **Scheme selection:** Changing the editor scheme in Preferences updates only the last-focused editor instance. Other editors keep their current scheme until they gain focus and a change is initiated.
- **Dirty markers:** Per-color edits mark the scheme name bold with a trailing " *" and mark the edited row. Save/Reset for editor schemes remain disabled (no persistence yet).

## 3D Render Schemes (F5/F6/ThrownTogether)
- **Scope:** Single global render settings instance; changes affect all 3D views across windows.
- **Live overrides:** Per-color changes are session-only; they mark the scheme dirty and trigger redraws for preview (F5), CGAL/Manifold render (F6), and ThrownTogether overlay.
- **Edge/crosshair behavior:** Edge uniforms are updated per draw; crosshair can be hidden via UI setting. Color resolution uses ColorMap with override-aware lookup.

## Layout and UI Behavior
- **Tables:** Both 3D and editor tables align on RGB column width (100 px) and picker width (60 px). Editor name column is wider (200 px) to fit longer keys. Button bars use fixed size policies.
- **Preview:** Editor preview uses the same live per-color updates as the main editor and shows dirty markers consistently.

## Preview Sample Loading
- **Search paths:** Tries `../scripts/prefsPreview.scad`, then `../../scripts/prefsPreview.scad`, then `./scripts/prefsPreview.scad` (installed layout). If all fail, logs the attempted paths and falls back to an inline sample.
- **Logging:** Kept intentionally; warning appears only if no candidate path succeeds.

## Design Intent (current)
- Editor scheme updates are intentionally scoped to a single, last-focused editor to avoid cross-window bleed.
- 3D render schemes remain global because there is one RenderSettings/ColorMap shared by all views.
- All edits are session-only; persistence and resource management are deferred.
- Live feedback is prioritized: per-color edits update both the preview widget and the targeted editor immediately.

## Known Limitations / Deferred Work
- No user-facing persistence for edited schemes (Save/Reset disabled for editor schemes; 3D Save is a placeholder message).
- Focus tracking is global; if multiple editors are in different windows, the last one to receive focus is the only target for subsequent editor-scheme changes until focus moves again.
- Preview sample warning will appear if none of the three paths exist; acceptable by current decision.

## Potential Next Steps (not implemented)
1) Add opt-in persistence/export for editor and render schemes (user config path under color-schemes/).
2) Optional toggle to broadcast editor scheme changes to all editors in the active window (still avoid cross-window updates).
3) Unify logging to the Error Log panel alongside qWarning for sample load failures.
4) Add a minimal 3D preview widget using the selected scheme to visualize edges/faces/background live.
