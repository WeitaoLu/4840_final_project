// fm_model.h
#ifndef FM_MODEL_H
#define FM_MODEL_H

#include "matrix.h"

typedef struct {
    double bias;         // Scalar bias term w0 in the model
    Matrix weights;      // Weight vector w in the model
    Matrix factors;      // Factorization matrix V in the model
} FMModel;

void fit(FMModel *model, Matrix X, Matrix y, int feature_potential, double alpha, int iter , int batch_size,  double decay_rate);
double* predict(FMModel *model, Matrix X);

#endif
