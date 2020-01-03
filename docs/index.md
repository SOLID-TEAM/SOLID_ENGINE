# Description

**Solid Engine** is a project of the video games engine subject with the purpose of creating a fully functional engine with its own innovations and features implemented by **Solid Team**. We are 2  Video Game Design and Development Degree students from **CITM-UPC** Terrassa.  

-  [Code](https://github.com/SOLID-TEAM/SOLID_ENGINE)
- [Release](https://github.com/SOLID-TEAM/SOLID_ENGINE/releases/tag/Assignment3.0)
# Downloads

 Zip: <a class="github-button" href="https://github.com/SOLID-TEAM/SOLID_ENGINE/releases/download/Assignment3.0/Assigment3.zip" data-icon="octicon-cloud-download" aria-label="Download ntkme/github-buttons on GitHub">Solid Engine 3.0</a>


# How to Play Demo

- Click on editor File->Load Scene->assignament3.solidscene and enjoy the physics playground :D

# Main Sub-Systems

### GameObjects & Components
The engine uses a structure of gameobjects and components ordered by hierarchy. You can add them, remove them and reorder them as you want through the inspector and hierarchy window.

### Scenes
The user has the possibility to save their current hierarchy of game objects and components by saving them in scenes. Later they can be loaded in a few simple clicks.

### Game Controls 
With the controls at the top you can start, stop, pause, resume and go frame by frame the game. When the game stops, the scene will return to the way it was when you press start.

### Resources & Library
The resources system allows you to optimize the memory using the same resource by instantiating it. For each resource in the assets folder an own document will be generated in the library of the hidden project for the user. These documents are a copy in the engine format that allows you to access the data faster saving loading time.

### Editor
The unity-style editor uses the ImGui library. It has the necessary windows to control the entire scene. With the scene window you can edit the game objects with the mouse picking and the gizmo and with the game window view the scene from a camera view .

# Phyisics Sub-System

## Module
The physics module integrates the [Bullet Physics 2.89](https://github.com/bulletphysics/bullet3/releases/tag/2.89) library. Our engine has a simulation of a dynamic world that is updated in each preupdate with the game time. It is also responsible for distributing collision callbacks (OnCollision) to game objects.

## Components

### Colliders
The colliders contain the shape of the Rigid Body. You can configure the center of mass, the configurable values of the shape and its physical properties when interacting with other colliders.

 - **Bouncing :**  Control the restitution value. The higher, the crashes retain more momentum and the rigid body will bounce.
 - **Linear Friction:**  Reference to the coefficient of friction. It only affects colliding with other colliders and specifically the linear movement.
 - **Angular Friction:** Same as the previous value but only modifies the angular movement.

#### Types

 - **Box Collider :** Center and Size
 - **Sphere Collider:** Center and Radius
 - **Capsule Collider:** Center, Radius and Height
 - **ConvexHull Collider:** Center
 
 <p align="center"></iframe></p>
<p align="center"> <iframe src="https://drive.google.com/file/d/1BkA8W8bRJ4tL1elrGe19_LmUl5RgnxaI/preview" width="600" height="320"></iframe></p>

### Rigid Body
As the name implies, this component converts the gameobject into a rigid body that will be used in physics simulation and will control the transformation in position and rotation. This body can be **dynamic, static or kinematic**. If the gameobject itself has a collider component, it will be used as a shape for the rigid body. It affects gravity and has configurable values:

 - **Mass**  
 - **Drag :** Coefficient of aerodynamic friction. It only affects linear motion. 
 - **Angular Drag** Coefficient of aerodynamic friction. It only affects angular motion.
 - **Freeze Constraint** You can block the position and rotation components of the rigid body.

 <p align="center"></iframe></p>
<p align="center"><iframe src="https://drive.google.com/file/d/1kANmd4taEhykg5K6mUabCKk5O6_Infzk/preview" width="600" height="320"></iframe> </p>


### Joint P2P
With this component you can apply a constraint of position to two gameobjects with rigid body. You can drag the gameobject from the hierarchy to the **connected body** value.

### Character Controller
The component needs a collider and a rigid body. Turn any game object into a character. Some keys have been added as a demo to test the physics.

 - **U,H,J,K :** movement
 - **Y :** turn left 
 - **I :** turn right
 - **P :** shoot balls

### Vehicle 
The component needs a collider and a rigid body. Turn any game object into a vehicle . Some keys have been added as a demo to test the physics.

-   **Arrows :**  movement

# Videos

<iframe width="100%" height="500" src="https://www.youtube.com/embed/I8HB0hHQbtI" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

# Team

## Alejandro Gamarra Niño

<iframe src="https://drive.google.com/file/d/1IARgoZQPvskI3M-0ZzTupzNw1lhMU0Af/preview" width="300" height="400"></iframe>

###  Member implementations
- Game Controls (Start/Pause/...)
 - Editor Camera 
 - Viewports 
 - Module Time
 - Module Physics, bullet implementation
 - Module Editor, ImGui implementation , some windows and modification, 
 - Components :  Transform, Colliders, Rigid Body, Camera. Character Controller, Mesh Renderer
 

## José Antonio Prieto García 
<iframe src="" width="300" height="400"></iframe>

###  Member implementations


# License

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

**THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.**
