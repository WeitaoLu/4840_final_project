// matrix.h
#ifndef MATRIX_H
#define MATRIX_H

typedef struct {
  double **data;
  int rows;
  int cols;
} Matrix;

Matrix create_matrix(int rows, int cols);
void free_matrix(Matrix m);
Matrix multiply_matrices(Matrix a, Matrix b);
void elementwise_multiply(Matrix a, Matrix b, Matrix result);
double sum_matrix_elements(Matrix m);
void init_random_normal(Matrix m, double mean, double stddev);
double sigmoid(double x);
void compute_means(Matrix features, double *means);
void compute_stddevs(Matrix features, double *means, double *stddevs);
void standardize_features(Matrix features, double *means, double *stddevs);
double pearson_correlation(double *x, double **y, int n);
#endif
