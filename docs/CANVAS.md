# Canvas

Canvas is an optional host-owned drawing surface. Check HasCanvas before
creating one. Create and draw on the host owner thread, bracket changes with
Begin and End, then use TextureId or DrawImGui during the current frame.

fwgui::Canvas and fwgui::CanvasImage are move-only RAII wrappers. They destroy
their host resources when the wrapper is reset or destroyed. The host also
reclaims resources still owned by a plugin during teardown.

Coordinates are pixels with the origin at the upper left. Colors are packed
0xAARRGGBB. The API covers state, fill and stroke styles, paths, rectangles,
text metrics, images, readback, line dashes, and transforms. Guard newer
callbacks with FWGUI_HAS when using the C layer directly.

Panel controls draw on a borrowed canvas; see [PANEL_CONTROLS.md](PANEL_CONTROLS.md).
