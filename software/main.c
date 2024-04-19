#include "fm_model.h"
#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Function to load data from a file
// Function to load data from a CSV file
void load_data(const char *filename, Matrix *features, Matrix *labels) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Failed to open file");
    exit(EXIT_FAILURE);
  }

  // Read the file line by line
  for (int i = 0; i < features->rows; i++) {
    if (fscanf(file, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", &features->data[i][0],
               &features->data[i][1], &features->data[i][2],
               &features->data[i][3], &features->data[i][4],
               &features->data[i][5], &features->data[i][6],
               &labels->data[i][0]) == EOF) {
      break; // Stop reading if we reach end of file prematurely
    }
  }

  fclose(file);
}

int main() {
  // Assume data has 100 rows and we are using 5 features
  int rows = 340;       // total number of data points
  int cols = 7;         // number of features
  int train_rows = 330; // 10% for training

  // Creating matrices for features and labels
  Matrix features = create_matrix(rows, cols);
  Matrix labels = create_matrix(rows, 1);
 
  // Load data from a pre-processed file
  load_data("processed_AFRM.csv", &features, &labels);
  double *means = malloc(cols * sizeof(double));
  double *stddevs = malloc(cols * sizeof(double));
  compute_means(features, means);
  compute_stddevs(features, means, stddevs);
  standardize_features(features, means, stddevs);
 
  // Splitting data into train and test manually
  Matrix features_train = create_matrix(train_rows, cols);
  Matrix labels_train = create_matrix(train_rows, 1);
  Matrix features_test = create_matrix(rows - train_rows, cols);
  Matrix labels_test = create_matrix(rows - train_rows, 1);
  // Copying data to train and test matrices
  for (int i = 0; i < train_rows; i++) {
    for (int j = 0; j < cols; j++) {
      features_train.data[i][j] = features.data[i][j];
    }
    labels_train.data[i][0] = labels.data[i][0];
  }
  // Copying data to test matrices
  for (int i = train_rows; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      features_test.data[i - train_rows][j] = features.data[i][j];
    }
    labels_test.data[i - train_rows][0] = labels.data[i][0];
  }

  // Initialize the model
  FMModel model;
  fit(&model, features_train, labels_train, 7, 0.01, 200, 250,0.95);
  // Parameters are void fit(FMModel *model, Matrix X, Matrix y, int
  // feature_potential, double learning rate, int epoch , int batch_size, double
  // learning_rate_decay_rate); Predict
  double *predictions = predict(&model, features_test);

  // Print predictions
  for (int i = 0; i < rows - train_rows; i++) {
    printf("Factor_Value: %f, Actual: %f\n", predictions[i]-0.5,
           labels_test.data[i][0]);
  }
  double ic =
      pearson_correlation(predictions, labels_test.data, rows - train_rows);
  printf("Information Coefficient (IC): %f\n", ic);

  // predictions = predict(&model, features_train);
  // for (int i = 0; i < train_rows; i++) {
  //   printf("Prediction on Training: %f, Actual: %f\n", predictions[i],
  //          labels_train.data[i][0]);
  // }
  // ic = pearson_correlation(predictions, labels_train.data, 10);
  // printf("Information Coefficient (IC) on Training: %f\n", ic);

  // Cleanup
  free_matrix(features);
  free_matrix(labels);
  free_matrix(features_train);
  free_matrix(labels_train);
  free_matrix(features_test);
  free_matrix(labels_test);
  free(predictions);

  return 0;
}
