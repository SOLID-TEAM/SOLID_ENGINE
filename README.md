# Solid Engine

**Solid Engine Project** is a project with the purpose of creating a fully functional video game engine with its own innovations and features implemented by **Solid Team**. We are 2  Video Game Design and Development Degree students from **CITM-UPC** Terrassa.  

- Alejandro's GitHub account: [AAurelioGN](https://github.com/alejandro61299)
- José Antonio's GitHub account: [peterMcP](https://github.com/peterMcP)
- Our GitHub organization: [Solid Engine](https://github.com/SOLID-TEAM/SOLID_ENGINE)

## About the engine

The code is written in C++.

- GitHub repository: [SolidEngine](https://github.com/SOLID-TEAM/SOLID_ENGINE)

## Instalation

- Download the latest version of the engine in the following [link](https://github.com/SOLID-TEAM/SOLID_ENGINE/releases)
- Unzip the **SolidEngine v.X.X** folder in some path of your pc 
- Start the **SolidEngine v.X.X/SolidEngine.exe** and enjoy! : D
## Innovations

- Delete GameObjects History (**Undo CTRL + Z**).
- Window Scene Viewport **FBO** with modifiable **MSAA**.
- ImGui modified as unity editor Gui.

### v2.0:
- KDTree for acceleration of mouse picking and frustum culling.
- Space partitioning window.
- Icons for layout.

## Features

### View port
- Selectable shading modes.
- Debug visualization for selected gameobject of vertex normals and face normals.
- Configurable grid
### v2.0:
- Game Viewport

### Game Objects
- Drag & Drop to Reorder GameObjects in the Hierarchy Window.
- Create Primitives GameObjects in the Primitives Window.
- Delete GameObjects by right-clicking them at Hierarchy.
- Ctrl + z with delete history (window).
### Components
- Added: Transform, Mesh Renderer , Mesh & Material Components
- Component material has one option to activate a procedurally checker texture.
 
### Debug
- Change ViewMode in Scene Window Left-Top Combo 
- Change Grid and Debug Settings in Scene Window Menus
- View all textures loaded in vram on Configuration->Loaded textures vram

### 2.0 added features:
- Correctly transformations on hierarchy moves.
- Textures, models, meshes, materials and components migrated to resources system.
- Models, materials and meshes on own binary file format.
- Resources loaded from .meta files generated on importing a asset.
- Added gizmos: when a gameobject selected press (or use icons on upper left to select one edit mode):
	- Translate : keyboard W
	- Rotate : keyboard E
	- Scale : keyboard R
	- Change local/world: number 1/2 respectively.
- View all loaded resources under panel configuration->loaded resources.
- View resources relative data under each component (material, mesh).
- Project panel, visualize all resources by type, and drag and drop models "prefabs" to scene window to instantiate one copy referenced to same resources.
- Gameobjects separateds by statics and dynamic to accelerate statics.
- Acceleration of static objects with KDTree.
- Time manager wich allows Play/Pause/Stop game logic and restore editor to last state before playing starts. (while playing camera culling is activated by default).
- Mouse picking with static KDTree.
- Scene serialization, save/load scenes.
- Visualize debug options under scene window (AABB, grid,... under eye/grid icon option).
- Configure editor camera options under scene window (camera icon option).

### Camera Controls

- **CTRL + Z** : Restore last deleted gameobject.
- **Mouse Right** Hold: 
- **Mouse Movement**: Look Around.
- **WASD**: Change local position on X-Z Plane.
- **QE**: Move up and down.
- **Mouse Wheel**: Zoom In/Out.
- **Shift** Hold : Movement Speed x2.
- **F**: Focus selected GameObject.
- **Alt** Hold :
- **Mouse Left** Hold : Orbit Last Selected GameObject.

## Third Party Tools

- IDE: Microsoft Visual Studio 2017
- External libraries: 
	- SDL 2.0.8
	- OpenGL 3.1
	- Glew 2.1
	- Assimp 4.1
	- DevIL 1.8
	- ImGui 1.8 ( Docking Branch)
	- ImGuizmo 
	- IconFontAwesome 5
	- MathGeoLib 1.5
	- PCG Random Number Generator 0.94
	- Parson
	- MMGR

## License

MIT License

Copyright (c) 2019 **Solid Engine** (Alejandro Aurelio Gamarra Niño & José Antonio Prieto García)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

**THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.**
