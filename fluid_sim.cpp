//
//  fluid_sim.cpp
//  project
//
//  Created by Tony Cao on 12/4/16.
//  Copyright © 2016 Tony Cao. All rights reserved.
//

#include "fluid_sim.hpp"

#define INDEX(i,j) ((i)+(N+2)*(j))
#define SWAP_WITH(x0,x) {float * tmp=x0;x0=x;x=tmp;}

void copy_values ( int N, float * new_x, float * old_x, float dt )
{
  int size=(N+2)*(N+2);
  
  for ( int i=0 ; i<size ; i++ ) {
    new_x[i] += dt*old_x[i];
  }
}

void set_bnd ( int N, int b, float * x )
{
  for ( int i=1 ; i<=N ; i++ ) {
    x[INDEX(0  ,i)] = b==1 ? -x[INDEX(1,i)] : x[INDEX(1,i)];
    x[INDEX(N+1,i)] = b==1 ? -x[INDEX(N,i)] : x[INDEX(N,i)];
    x[INDEX(i,0  )] = b==2 ? -x[INDEX(i,1)] : x[INDEX(i,1)];
    x[INDEX(i,N+1)] = b==2 ? -x[INDEX(i,N)] : x[INDEX(i,N)];
  }
  
  x[INDEX(0  ,0  )] = 0.5f*(x[INDEX(1,0  )]+x[INDEX(0  ,1)]);
  x[INDEX(0  ,N+1)] = 0.5f*(x[INDEX(1,N+1)]+x[INDEX(0  ,N)]);
  x[INDEX(N+1,0  )] = 0.5f*(x[INDEX(N,0  )]+x[INDEX(N+1,1)]);
  x[INDEX(N+1,N+1)] = 0.5f*(x[INDEX(N,N+1)]+x[INDEX(N+1,N)]);
}

void lin_solve ( int N, int b, float * x, float * x0, float a, float c )
{
  for (int k=0 ; k<20 ; k++ ) {
    for (int i = 0; i <= N; i++) {
      for (int j = 0; j <= N; j++) {
        x[INDEX(i,j)] = (x0[INDEX(i,j)] + a * (x[INDEX(i-1,j)] + x[INDEX(i+1,j)] + x[INDEX(i,j-1)] + x[INDEX(i,j+1)])) / c;
      }
    }
    set_bnd ( N, b, x );
  }
}

void diffuse ( int N, int b, float * x, float * x0, float diff, float dt )
{
  float a = dt * diff * N * N;
  if (a == 0) {
    return;
  }
  lin_solve ( N, b, x, x0, a, 1 + 4 * a );
}

void advect ( int N, int b, float * d, float * d0, float * u, float * v, float dt )
{
  int i, j, i0, j0, i1, j1;
  float x, y, s0, t0, s1, t1, dt0;
  
  dt0 = dt*N;
  for (int i = 0; i <= N; i++) {
    for (int j = 0; j <= N; j++) {
      x = i-dt0*u[INDEX(i,j)]; y = j-dt0*v[INDEX(i,j)];
      if (x<0.5f) x=0.5f; if (x>N+0.5f) x=N+0.5f; i0=(int)x; i1=i0+1;
      if (y<0.5f) y=0.5f; if (y>N+0.5f) y=N+0.5f; j0=(int)y; j1=j0+1;
      s1 = x-i0; s0 = 1-s1; t1 = y-j0; t0 = 1-t1;
      d[INDEX(i,j)] = s0 * (t0 * d0[INDEX(i0,j0)] + t1 * d0[INDEX(i0,j1)]) + s1 * (t0 * d0[INDEX(i1,j0)] + t1 * d0[INDEX(i1,j1)]);
    }
  }
  set_bnd ( N, b, d );
}

void project ( int N, float * v_x, float * v_y, float * p, float * div )
{
  for (int i = 0; i <= N; i++) {
    for (int j = 0; j <= N; j++) {
        div[INDEX(i,j)] = -0.5f * (v_x[INDEX(i+1,j)] - v_x[INDEX(i-1,j)] + v_y[INDEX(i,j+1)] - v_y[INDEX(i,j-1)]) / N;
        p[INDEX(i,j)] = 0;
    }
  }
  set_bnd ( N, 0, div );
  set_bnd ( N, 0, p );
  
  lin_solve ( N, 0, p, div, 1, 4 );
  
  for (int i = 0; i <= N; i++) {
    for (int j = 0; j <= N; j++) {
      v_x[INDEX(i,j)] -= 0.5f*N*(p[INDEX(i+1,j)]-p[INDEX(i-1,j)]);
      v_y[INDEX(i,j)] -= 0.5f*N*(p[INDEX(i,j+1)]-p[INDEX(i,j-1)]);
    }
  }
  
  set_bnd ( N, 1, v_x );
  set_bnd ( N, 2, v_y );
}

void density_step ( int N, float * density, float * density_old, float * v_x, float * v_y, float diff, float dt )
{
  copy_values ( N, density, density_old, dt );
  SWAP_WITH ( density_old, density );
  diffuse ( N, 0, density, density_old, diff, dt );
  
  SWAP_WITH ( density_old, density );
  advect ( N, 0, density, density_old, v_x, v_y, dt );
}

void velocity_step ( int N, float * v_x, float * v_y, float * v_x_old, float * v_y_old, float visc, float dt )
{
  copy_values ( N, v_x, v_x_old, dt );
  copy_values ( N, v_y, v_y_old, dt );
  
  SWAP_WITH ( v_x_old, v_x );
  diffuse ( N, 1, v_x, v_x_old, visc, dt );
  SWAP_WITH ( v_y_old, v_y );
  diffuse ( N, 2, v_y, v_y_old, visc, dt );
  
  project ( N, v_x, v_y, v_x_old, v_y_old );
  SWAP_WITH ( v_x_old, v_x );
  SWAP_WITH ( v_y_old, v_y );
  
  advect ( N, 1, v_x, v_x_old, v_x_old, v_y_old, dt );
  advect ( N, 2, v_y, v_y_old, v_x_old, v_y_old, dt );
  project ( N, v_x, v_y, v_x_old, v_y_old );
}

