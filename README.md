# Special-Relativity-OpenGL
A simple graphics engine to simulate visual effect of special relativity - including Lorentz transform, Doppler effect and finite speed of propagation of light.

This is a simple graphics engine to simulate visual effect of special relativity.
OpenGL 4.1 is used to handle the graphics. Window is created with GLFW and GLAD libraries; GLM library is used in
the vector calculations as it is compatible with OpenGL. In the simulation it is assumed that the other frames
are moving at a constant velocities relative to the observer (the objects in those frame can perform any
transformation). The effects of special relativity (Lorentz transformation and Doppler shift for light) and
finite speed of propagation of light are taken into account. The physical theory is derived in the presentation -
the code uses the same notation. To change the the scenario visible on the scene, modify "setUpScene" function in
"scene.h".

Example models and textures included. Libraries not included.

The code was written with the help of the following tutorials and websites:
https://learnopengl.com
The source code for "mesh.h", "model.h" was taken directly from the https://learnopengl.com tutorial. The code
for "shader.h" was also taken from the website, but was slightly modified (added an option to modify the code for
the shader on the go). 
Those scripts were created by Joey de Vries, used under the CC BY-NC 4.0 license: https://creativecommons.org/licenses/by-nc/4.0/legalcode.
Joey de Vries's Twitter: https://twitter.com/JoeyDeVriez.More details in the file headers.

Controls:
A,W,S,D       to move the camera
SHIFT         to speed up the camera
MOUSE         to rotate the camera
MOUSE SCROLL  to zoom the camera
MOUSE CLICK   to release/show and lock/hide cursor
ENTER         to take a screenshot - the result: "screenshot.tga" will appear in the screenshots folder
1             to show/hide coordinate system
2             to show/hide GUI
3             to set the speed of propagation of light to infinity/back to normal - to show where the objects
actually are at a given time
4             to turn off/on doppler effect

THIS PROGRAM HAS ONLY BEEN TESTED ON MAC OS 10.15.2
