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
- [ ] Easily bind multiple descriptor sets.
- [ ] Draw multiple entities.

## ENGINE
- [x] Load .obj files.
- [ ] Load .gltf files.
- [ ] Create transform component.
- [ ] Create camera component.
- [ ] Create render component.
    - [ ] Create mesh data.
    - [ ] Create material data.
- [ ] Create light component.
- [ ] Parse json scene file.
