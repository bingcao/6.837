//
//  main.cpp
//  project
//
//  Created by Tony Cao on 12/4/16.
//  Copyright © 2016 Tony Cao. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include "MAC.h"
#include "vecmath.h"
#include <glfw3.h>
#include "gl.h"

void draw() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBegin(GL_QUADS);
  
  glVertex2f(-0.7f, -.7f);
  glVertex2f(0.7f, -.7f);
  glVertex2f(0.7f, .7f);
  glVertex2f(-0.7f, .7f);
  
  glEnd();
}

void initRendering() {
  glClearColor(0, 0, 0, 1);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char * argv[]) {
  // insert code here...
  GLFWwindow* window;
  /* Initialize the library */
  if (!glfwInit())
    return -1;
  
  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(1024, 1024, "SMOKE", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }
  
  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  
  initRendering();
  
  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
    
    /* Swap front and back buffers */
    glfwSwapBuffers(window);
    
    /* Poll for and process events */
    glfwPollEvents();
  }
  
  glfwTerminate();
  return 0;
}