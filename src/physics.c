#include "physics.h"
#include <math.h>
#include <stdlib.h>

static const double G = 9.81; /* gravity constant */

/* Initialize pendulum with sane defaults */
void pendulum_init(Pendulum *p) {
    if (!p) return;
    p->m1 = 1.0;
    p->m2 = 1.0;
    p->l1 = 170.0;
    p->l2 = 170.0;
    p->theta1 = M_PI * 0.6;
    p->theta2 = M_PI * 0.4;
    p->omega1 = 0.0;
    p->omega2 = 0.0;
    p->damping = 0.0; /* numeric damping, small positive reduces energy drift */
}

/* Reset to initial state (same as init for now) */
void pendulum_reset(Pendulum *p) {
    pendulum_init(p);
}

/* Internal: compute angular accelerations alpha1 and alpha2 for given state.
   Uses standard double-pendulum algebraic form. */
static void compute_acc(const Pendulum *p,
                        double th1, double th2, double w1, double w2,
                        double *alpha1, double *alpha2)
{
    /* local aliases */
    double m1 = p->m1;
    double m2 = p->m2;
    double l1 = p->l1;
    double l2 = p->l2;

    double delta = th2 - th1;
    double sin_d = sin(delta);
    double cos_d = cos(delta);

    /* Denominators (avoid near-zero) */
    double den1 = (m1 + m2) * l1 - m2 * l1 * cos_d * cos_d;
    if (fabs(den1) < 1e-12) den1 = (den1 < 0) ? -1e-12 : 1e-12;
    double den2 = (l2 / l1) * den1;
    if (fabs(den2) < 1e-12) den2 = (den2 < 0) ? -1e-12 : 1e-12;

    /* Numerators derived from Lagrange equations */
    double num1 =
        m2 * l1 * w1 * w1 * sin_d * cos_d
        + m2 * G * sin(th2) * cos_d
        + m2 * l2 * w2 * w2 * sin_d
        - (m1 + m2) * G * sin(th1);

    double num2 =
        -m2 * l2 * w2 * w2 * sin_d * cos_d
        + (m1 + m2) * G * sin(th1) * cos_d
        - (m1 + m2) * l1 * w1 * w1 * sin_d
        - (m1 + m2) * G * sin(th2);

    *alpha1 = num1 / den1;
    *alpha2 = num2 / den2;
}

/* RK4 integration step */
void pendulum_step(Pendulum *p, double dt)
{
    if (!p) return;

    double t1 = p->theta1, t2 = p->theta2;
    double w1 = p->omega1, w2 = p->omega2;

    double k1_t1, k1_t2, k1_w1, k1_w2;
    double k2_t1, k2_t2, k2_w1, k2_w2;
    double k3_t1, k3_t2, k3_w1, k3_w2;
    double k4_t1, k4_t2, k4_w1, k4_w2;

    /* k1 */
    compute_acc(p, t1, t2, w1, w2, &k1_w1, &k1_w2);
    k1_t1 = w1;
    k1_t2 = w2;

    /* k2 */
    compute_acc(p,
                t1 + 0.5 * dt * k1_t1,
                t2 + 0.5 * dt * k1_t2,
                w1 + 0.5 * dt * k1_w1,
                w2 + 0.5 * dt * k1_w2,
                &k2_w1, &k2_w2);
    k2_t1 = w1 + 0.5 * dt * k1_w1;
    k2_t2 = w2 + 0.5 * dt * k1_w2;

    /* k3 */
    compute_acc(p,
                t1 + 0.5 * dt * k2_t1,
                t2 + 0.5 * dt * k2_t2,
                w1 + 0.5 * dt * k2_w1,
                w2 + 0.5 * dt * k2_w2,
                &k3_w1, &k3_w2);
    k3_t1 = w1 + 0.5 * dt * k2_w1;
    k3_t2 = w2 + 0.5 * dt * k2_w2;

    /* k4 */
    compute_acc(p,
                t1 + dt * k3_t1,
                t2 + dt * k3_t2,
                w1 + dt * k3_w1,
                w2 + dt * k3_w2,
                &k4_w1, &k4_w2);
    k4_t1 = w1 + dt * k3_w1;
    k4_t2 = w2 + dt * k3_w2;

    /* Combine RK4 increments for angles */
    p->theta1 += dt * (k1_t1 + 2.0 * k2_t1 + 2.0 * k3_t1 + k4_t1) / 6.0;
    p->theta2 += dt * (k1_t2 + 2.0 * k2_t2 + 2.0 * k3_t2 + k4_t2) / 6.0;

    /* Combine RK4 increments for angular velocities */
    p->omega1 += dt * (k1_w1 + 2.0 * k2_w1 + 2.0 * k3_w1 + k4_w1) / 6.0;
    p->omega2 += dt * (k1_w2 + 2.0 * k2_w2 + 2.0 * k3_w2 + k4_w2) / 6.0;

    /* optional small damping to stabilize numerical drift */
    if (p->damping > 0.0) {
        p->omega1 *= (1.0 - p->damping);
        p->omega2 *= (1.0 - p->damping);
    }
}

/* Compute total mechanical energy (kinetic + potential), used for HUD */
double pendulum_total_energy(Pendulum *p)
{
    if (!p) return 0.0;

    double m1 = p->m1, m2 = p->m2;
    double l1 = p->l1, l2 = p->l2;
    double t1 = p->theta1, t2 = p->theta2;
    double w1 = p->omega1, w2 = p->omega2;

    /* heights (relative) */
    double y1 = -l1 * cos(t1);
    double y2 = y1 - l2 * cos(t2);

    /* velocities */
    double vx1 = l1 * w1 * cos(t1); /* not needed fully, use scalar forms */
    double vy1 = -l1 * w1 * sin(t1);
    double v1sq = vx1*vx1 + vy1*vy1;

    /* velocity of bob 2 by combining contributions */
    double vx2 = vx1 + l2 * w2 * cos(t2);
    double vy2 = vy1 - l2 * w2 * sin(t2);
    double v2sq = vx2*vx2 + vy2*vy2;

    double KE = 0.5 * m1 * v1sq + 0.5 * m2 * v2sq;
    double PE = m1 * G * (y1 + l1 + l2) + m2 * G * (y2 + l1 + l2); /* offset to keep positive */

    return KE + PE;
}
