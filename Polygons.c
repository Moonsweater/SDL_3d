#ifndef POLYGONS
#define POLYGONS

#include <SDL2/SDL.h>

//WE ARE GAMING! IT WORKS!
//The problem could be here: are we sure these are memory safe? (What safety issues could even exist for it, though? It's not like we're doing concurrency. :p

int double_compare(const void *a,const void *b) {
  double *A; double *B;
  
  A = (double*)a; B = (double*)b;
  
  if (*A < *B) {return -1;}
  else if (*A > *B) {return 1;}
  else {return 0;}
  
}

void horizontalFill(SDL_Renderer *r, double x[], double y[], int lineHeight, int numPoints) {

  double intersection_x_vals[numPoints];
  int numberOfIntersections = 0;
  double reverse_slope;
  
  //Find all the intercepts of the line x = lineHeight with the polygon.
  int next, higher, lower;
  for (int i=0; i < numPoints; i++) {
    next = (i+1)%numPoints;
    if(y[next] >= y[i]) {higher = next; lower = i;} else {lower = next; higher = i;}
    if (y[lower] <= lineHeight && lineHeight < y[higher]) {      
      if (y[higher] == y[lower]) {
        //Horizontal case (reverse slope is infinite):
        intersection_x_vals[numberOfIntersections] = fmin(x[higher], x[lower]);
        numberOfIntersections++;
        intersection_x_vals[numberOfIntersections] = fmax(x[higher], x[lower]);
        numberOfIntersections++;       
      } else {	
        reverse_slope = (x[higher] - x[lower]) / (y[higher] - y[lower]);
        //point-slope, centered at x[lower], y[lower].
        //LH = m(x0 - x) + y0
        //(LH - y0) / m = x0 - x
        //x = x0 - (LH - y0)/m
        //For some reason, this basic-ass algebra is wrong, and we need x = x0 + (LH-y0)/m instead. This is absolutely baffling.
        intersection_x_vals[numberOfIntersections] = x[lower] + reverse_slope * (lineHeight - y[lower]);
        numberOfIntersections++;
      }
    }
  }
  
  qsort(intersection_x_vals, numberOfIntersections, sizeof(double), double_compare);
  
  for (int i=0; i < numberOfIntersections; i+=2) {
    SDL_RenderDrawLine(r,intersection_x_vals[i], lineHeight, intersection_x_vals[i+1], lineHeight);
    i++;
  }
}

void fill_polygon(SDL_Renderer *r, double x[], double y[], int numPoints) {
  int ymin, ymax;
  ymin = y[0];
  for (int i=0; i < numPoints; i++) {
    if (y[i] < ymin) {ymin = y[i];}
    if (y[i] > ymax) {ymax = y[i];}
  }  
    
  for (int i=ymin; i <= ymax; i++) {
    horizontalFill(r, x, y, i, numPoints);
  }
  
  return;
  
}

#endif
