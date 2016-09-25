# My Racing

![screenshot](https://cloud.githubusercontent.com/assets/11234396/18818325/cfe118fe-838f-11e6-8f75-32196fefef1e.png)

This is a racing game written in OpenGL. More needs to be added, additional information about the TODO below.

##TODO
  * Implement a menu system
  * Add music/sound effects to the menus and game
  * Implement better AI
  * Implement environment collisions
  * Add more car models
  * Add more racing levels

##How to run:

###Linux:
 1. Install opengl, glut and soil libraries
 ```bash
  sudo apt-get install freeglut3-dev 
 ```

 2. Clone the repository
 
 ```bash
  git clone https://github.com/MrPlumbum82/my_racing
 ```

 3. Compile
 
 ```bash
  g++ -std=c++11 asteroids.cpp -lGL -lGLU -lglut 
 ```
 
 4. Execute
 
 ```bash
  ./a.out
 ```
