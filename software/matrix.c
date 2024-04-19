// matrix.c
#include "matrix.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Matrix create_matrix(int rows, int cols) {
  Matrix m;
  m.rows = rows;
  m.cols = cols;
  m.data = (double **)malloc(rows * sizeof(double *));
  for (int i = 0; i < rows; i++) {
    m.data[i] = (double *)malloc(cols * sizeof(double));
  }
  return m;
}

void free_matrix(Matrix m) {
  for (int i = 0; i < m.rows; i++) {
    free(m.data[i]);
  }
  free(m.data);
}

Matrix multiply_matrices(Matrix a, Matrix b) {
  Matrix result = create_matrix(a.rows, b.cols);
  for (int i = 0; i < a.rows; i++) {
    for (int j = 0; j < b.cols; j++) {
      result.data[i][j] = 0;
      for (int k = 0; k < a.cols; k++) {
        result.data[i][j] += a.data[i][k] * b.data[k][j];
      }
    }
  }
  return result;
}

void elementwise_multiply(Matrix a, Matrix b, Matrix result) {
  for (int i = 0; i < a.rows; i++) {
    for (int j = 0; j < a.cols; j++) {
      result.data[i][j] = a.data[i][j] * b.data[i][j];
    }
  }
}

double sum_matrix_elements(Matrix m) {
  double sum = 0;
  for (int i = 0; i < m.rows; i++) {
    for (int j = 0; j < m.cols; j++) {
      sum += m.data[i][j];
    }
  }
  return sum;
}

void init_random_normal(Matrix m, double mean, double stddev) {
  for (int i = 0; i < m.rows; i++) {
    for (int j = 0; j < m.cols; j++) {
      double u = rand() / (RAND_MAX + 1.0);
      double v = rand() / (RAND_MAX + 1.0);
      double z = sqrt(-2.0 * log(u)) * cos(2 * M_PI * v);
      m.data[i][j] = mean + z * stddev;
    }
  }
}

// Sigmoid function
double sigmoid(double x) { return 1.0 / (1.0 + exp(-x)); }

void compute_means(Matrix features, double *means) {
  for (int j = 0; j < features.cols; j++) {
    double sum = 0.0;
    for (int i = 0; i < features.rows; i++) {
      sum += features.data[i][j];
    }
    means[j] = sum / features.rows;
  }
}

// calculate stddevs
void compute_stddevs(Matrix features, double *means, double *stddevs) {
  for (int j = 0; j < features.cols; j++) {
    double sum = 0.0;
    for (int i = 0; i < features.rows; i++) {
      sum += pow(features.data[i][j] - means[j], 2);
    }
    stddevs[j] = sqrt(sum / features.rows);
  }
}

// standardize to avoid overflow
void standardize_features(Matrix features, double *means, double *stddevs) {
  for (int i = 0; i < features.rows; i++) {
    for (int j = 0; j < features.cols; j++) {
      if (stddevs[j] != 0) { // 防止除以零
        features.data[i][j] = (features.data[i][j] - means[j]) / stddevs[j];
      }
    }
  }
}

// IC calculation
double pearson_correlation(double *x, double **y, int n) {
    double sum_x = 0, sum_y = 0, sum_x2 = 0, sum_y2 = 0, sum_xy = 0;
    for (int i = 0; i < n; i++) {
        sum_x += x[i];
        sum_y += y[i][0];
        sum_x2 += x[i] * x[i];
        sum_y2 += y[i][0] * y[i][0];
        sum_xy += x[i] * y[i][0];
    }
    double numerator = n * sum_xy - sum_x * sum_y;
    double denominator = sqrt((n * sum_x2 - sum_x * sum_x) * (n * sum_y2 - sum_y * sum_y));
    if (denominator == 0) return 0;  // To avoid division by zero
    return numerator / denominator;
}
