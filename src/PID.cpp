/*                                                                         80->|
 * pid.cpp
 *
 * Modifications: James William Dunn
 *          Date: May 29, 2017
 */

#include "PID.h"

using namespace std;

PID::PID() {
  this->cte_sum = 0.0;
  this->prev_cte = 0.0;
  this->prev_cte2 = 0.0;
}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
  this->Kp = -Kp;
  this->Ki = Ki;
  this->Kd = Kd;
}

void PID::UpdateError(double cte, double dt /* milliseconds */) {
  this->cte = cte;
  this->cte_sum += this->cte;
  
  this->p_error = this->Kp * this->cte;
  this->i_error = this->Ki * this->cte_sum;
  // Multiply cte diff by 20/dt to normalize against varying framerates
  this->d_error = this->Kd * (this->cte - this->prev_cte) * 20.0/dt;
  this->prev_cte = this->cte;
}

// Alternate Kd calculation (further research required: Kd needs retuning)
// en.wikipedia.org/wiki/Finite_difference_coefficient 
// d = this->Kd * (1.5 * this->cte - 2 * this->prev_cte + 0.5 * this->prev_cte2);
// this->prev_cte2 = this->prev_cte;

double PID::TotalError() {
  return this->p_error - this->i_error - this->d_error;
}
