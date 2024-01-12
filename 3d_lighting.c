#ifndef LIGHTING_3D
#define LIGHTING_3D

#include "3d_drawing_globals.c"

point3 polygon_average(int o, int p) {

  //printf("Polygon average called.\n");
  
  if (polygons[o][p].points_per_polygon <= 0 ) {printf("AUGH! \n"); exit(0);}
  //print_object(o);
  
  point3 out;
  out.x = out.y = out.z = 0;
  
  for (int i = 0; i < polygons[o][p].points_per_polygon; i++) {
    out.x = out.x + points[o][polygons[o][p].which_points[i]].x;
    out.y = out.y + points[o][polygons[o][p].which_points[i]].y;
    out.z = out.z + points[o][polygons[o][p].which_points[i]].z;
    //printf("point added.\n");
  }
  
  out.x = out.x / polygons[o][p].points_per_polygon;
  out.y = out.y / polygons[o][p].points_per_polygon;
  out.z = out.z / polygons[o][p].points_per_polygon;
  
  //printf("polygon average computed!\n");
  //print_object(o);
  
  //printf("polygon_average()\n");
  
  return out;
  
}

vec unit_normal_vector(int o, int p) {
  
  vec a, b, norm;
  
  //printf("unit normal computations has begun.\n");
  
  a.dx = points[o][polygons[o][p].which_points[1]].x - points[o][polygons[o][p].which_points[0]].x;
  b.dx = points[o][polygons[o][p].which_points[2]].x - points[o][polygons[o][p].which_points[0]].x;
  a.dy = points[o][polygons[o][p].which_points[1]].y - points[o][polygons[o][p].which_points[0]].y;
  b.dy = points[o][polygons[o][p].which_points[2]].y - points[o][polygons[o][p].which_points[0]].y;
  a.dz = points[o][polygons[o][p].which_points[1]].z - points[o][polygons[o][p].which_points[0]].z;
  b.dz = points[o][polygons[o][p].which_points[2]].z - points[o][polygons[o][p].which_points[0]].z;
  
  norm = cross(a, b);
  
  normalize(&norm);
  
  //printf("unit_normal_vector()\n");
  
  return norm;
  
}

double compute_lighting_intensity(int o, int p) {
  
  double intensity;

  vec eye, norm, light, reflected;
  
  point3 base = polygon_average(o,p);
  //point3 base; base.x = base.y = 0;
  
  point3 origin; origin.x = origin.y = origin.z = 0;
  
  eye = unit_vector(base, origin);
  //printf("base: x = %lf, y = %lf, z = %lf\n", base.x, base.y, base.z);
  light = unit_vector(base, lighting.position);
  norm = unit_normal_vector(o,p);
  
  double ctheta = dot(light, norm);
  
  if (ctheta < 0) {
    norm.dx *= -1;
    norm.dy *= -1;
    norm.dz *= -1;
    ctheta *= -1;
  }
  
  if (dot(eye, norm) < 0) {intensity = lighting.ambient; return intensity;}
  
  double diffuse = lighting.diffuse * dot(light, norm);
  if (diffuse < 0) {diffuse = 0;}
  
  reflected.dx = 2*ctheta*norm.dx - light.dx;
  reflected.dy = 2*ctheta*norm.dy - light.dy;
  reflected.dz = 2*ctheta*norm.dz - light.dz;

  double specular = (1 - lighting.ambient - lighting.diffuse) * pow(dot(reflected, eye), lighting.spec_power);
  
  intensity = lighting.ambient + diffuse + specular;

  return intensity;

}

color apply_lighting_to_colors(color c, double intensity) {

  color output;
  
  //Adding this print statement makes it run smoother?
  //printf("Intensity: %lf \n", intensity); //Speeds up
  
  //double q = intensity; //Merely accessing the varibale doesn't speed anything up, either.
  
  //printf("Intensity: (no double access)\n"); //Does not speed up: related to accessing intensity.
  
  double cutoff = lighting.ambient + lighting.diffuse;

  if (intensity <= cutoff) {
    output.r = c.r * intensity;
    output.g = c.g * intensity;
    output.b = c.b * intensity;
  } else {
    //Is addition between Uint8 and double unconventional, or something?
    output.r = floor(c.r + (intensity - cutoff) * (255-c.r) / (1-cutoff));
    output.g = floor(c.g + (intensity - cutoff) * (255-c.g) / (1-cutoff));
    output.b = floor(c.b + (intensity - cutoff) * (255-c.b) / (1-cutoff));
  }
  
  //printf("New Color: \n r = %d, g = %d, b = %d \n", output.r, output.g, output.b);
  
  return output;

}

#endif

