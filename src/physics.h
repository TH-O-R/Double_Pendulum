#ifndef PHYSICS_H
#define PHYSICS_H


#include <stdbool.h>


typedef struct {
double m1, m2;
double l1, l2;
double theta1, theta2;
double omega1, omega2;
double damping; // small damping factor
} Pendulum;


void pendulum_init(Pendulum *p);
void pendulum_reset(Pendulum *p);
void pendulum_step(Pendulum *p, double dt);


double pendulum_total_energy(Pendulum *p);


#endif // PHYSICS_H
