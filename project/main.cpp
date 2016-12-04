//
//  main.cpp
//  project
//
//  Created by Tony Cao on 12/4/16.
//  Copyright © 2016 Tony Cao. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include "vecmath.h"
#include <glfw3.h>
#include "gl.h"

#define IX(i,j) ((i)+(N+2)*(j))

extern void density_step ( int N, float * density, float * density_old, float * v_x, float * v_y, float diff, float dt );
extern void velocity_step ( int N, float * v_x, float * v_y, float * v_x_old, float * v_y_old, float visc, float dt );

static int N;
static float dt, diff, visc;
static float force, source;
static int dvel;

static float *v_x, *v_y, *v_x_old, *v_y_old;
static float *density, *density_old;

//static int mouse_down[3];
//static int omx, omy, mx, my;

void draw() {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBegin(GL_QUADS);
  
  float x, y, h, d00, d01, d10, d11;
  
  h = 1.0f/N;
  
  for ( int i=0 ; i<=N ; i++ ) {
    x = (i-0.5f)*h;
    for ( int j=0 ; j<=N ; j++ ) {
      y = (j-0.5f)*h;
      
      d00 = density[IX(i,j)];
      d01 = density[IX(i,j+1)];
      d10 = density[IX(i+1,j)];
      d11 = density[IX(i+1,j+1)];
      
      glColor3f ( d00, d00, d00 ); glVertex2f ( x, y );
      glColor3f ( d10, d10, d10 ); glVertex2f ( x+h, y );
      glColor3f ( d11, d11, d11 ); glVertex2f ( x+h, y+h );
      glColor3f ( d01, d01, d01 ); glVertex2f ( x, y+h );
    }
  }
  
  glEnd();
}

void freeData() {
  if (v_x) free(v_x);
  if (v_y) free(v_y);
  if (v_x_old) free(v_x_old);
  if (v_y_old) free(v_y_old);
  if (density) free(density);
  if (density_old) free(density_old);
}

void initData() {
  int size = (N + 2) * (N + 2);
  
  v_x         = (float *) malloc ( size*sizeof(float) );
  v_y         = (float *) malloc ( size*sizeof(float) );
  v_x_old     = (float *) malloc ( size*sizeof(float) );
  v_y_old     = (float *) malloc ( size*sizeof(float) );
  density     = (float *) malloc ( size*sizeof(float) );
  density_old = (float *) malloc ( size*sizeof(float) );
}

void initRendering() {
  glClearColor(0, 0, 0, 1);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char * argv[]) {
  if (argc > 1) {
    N = atoi(argv[1]);
  } else {
    N = 10;
  }
  dt = 0.1f;
  diff = 0.01f;
  visc =0.0f;
  
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
  initData();
  initRendering();
  
  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    velocity_step(N, v_x, v_y, v_x_old, v_y_old, visc, dt);
    density_step(N, density, density_old, v_x, v_y, diff, dt);
    draw();
    
    /* Swap front and back buffers */
    glfwSwapBuffers(window);
    
    /* Poll for and process events */
    glfwPollEvents();
  }
  
  glfwTerminate();
  return 0;
}