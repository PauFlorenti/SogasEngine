# SOGAS ENGINE

Sogas engine is a custom engine made with learning purposes.

## PLATFORM ROADMAP

So far, platform should allow the user to create multiple windows. Each window should receive
a callback function with its own behaviour. 
Some behaviour should be the same for all windows by default.

    - [ ] Resize
    - [ ] Fullscreen

- [ ] Program should only close when all windows are closed.
- [ ] There should only be one main window *Main Editor*, all other windows should be childs from
this main one.

- [ ] Platform window creation should only return a window ID.
- [ ] All window info is stored in a fixed array. At the moment no more than 4 windows are allowed.

## PINUT
- [x] Wired pipeline.
- [ ] Easily bind multiple descriptor sets.
- [ ] Draw multiple entities.
- [ ] Frustrum culling.
- [ ] Shadows from spot lights.
- [ ] Multiple passes.
- [ ] Pipeline read and created from json file specs.
- [ ] Render graph.

## ENGINE
- [x] Load .obj files.
- [ ] Load .gltf files.
- [x] Create transform component.
- [x] Create camera component.
- [ ] Create render component.
    - [x] Create mesh data.
    - [ ] Create material data.
- [ ] Create point light component.
- [ ] Create spot light component.
- [ ] Add directional light to the scene.
- [x] Parse json scene file.
- [x] Add imgui.
- [ ] ImGui rendered only on debug mode.
- [ ] Add bounding sphere to entities.
- [ ] Add bounding boxes to entities.
- [ ] Create and allocator for the engine. Use that.
    - [ ] Maybe use allocator and get rid of smart pointers?
- [ ] Resource module.
- [ ] Animations

## CODEGEN
- [ ] ImGui should only be built and linked in debug mode.
