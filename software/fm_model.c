// fm_model.c
#include "fm_model.h"
#include <float.h> // just use this .h to check isnan, may delete
#include <math.h>
#include <stdlib.h>

// Helper function to calculate interactions
double calculate_interaction(Matrix X, Matrix factors, int row) {
  double interaction = 0.0;
  for (int j = 0; j < factors.cols; j++) {
    double sum_square = 0.0, square_sum = 0.0;

    for (int i = 0; i < X.cols; i++) {
      double d = X.data[row][i] * factors.data[i][j];
      sum_square += d;
      square_sum += d * d;
    }
    interaction += 0.2 * (sum_square * sum_square - square_sum);
  }

  if (isnan(interaction)) {
    printf("Warning: interaction computed as NaN\n");
    interaction = 0.0; // replace with a suitable value if needed
  }
  // return 0; //use this to disable interaction calculation
  return interaction;
}

// Fit the FM model
void shuffle(int *array, int n) {
  if (n > 1) {
    for (int i = 0; i < n - 1; i++) {
      int j = i + rand() / (RAND_MAX / (n - i) + 1);
      int t = array[j];
      array[j] = array[i];
      array[i] = t;
    }
  }
}

// use mini-batch gradient descent
void fit(FMModel *model, Matrix X, Matrix y, int feature_potential,
         double initial_alpha, int iter, int batch_size, double decay_rate) {
  model->bias = 0.0;
  model->weights = create_matrix(X.cols, 1);
  model->factors = create_matrix(X.cols, feature_potential);

  init_random_normal(model->factors, 0, 0.2);

  int *indices = malloc(X.rows * sizeof(int));
  for (int i = 0; i < X.rows; i++) {
    indices[i] = i;
  }

  double alpha = initial_alpha; // Initialize learning rate

  for (int it = 0; it < iter; it++) {
    shuffle(indices, X.rows); // shuffle batch
    double total_loss = 0.0;  // Accumulate loss here
    int total_samples = 0;
    for (int start = 0; start < X.rows; start += batch_size) {
      double bias_grad = 0.0;
      Matrix weights_grad = create_matrix(X.cols, 1);
      Matrix factors_grad = create_matrix(X.cols, feature_potential);

      int end = start + batch_size < X.rows ? start + batch_size : X.rows;
      for (int idx = start; idx < end; idx++) {
        int x = indices[idx];
        double interaction = calculate_interaction(X, model->factors, x);
        double p = model->bias;
        for (int i = 0; i < X.cols; i++) {
          p += X.data[x][i] * model->weights.data[i][0];
        }
        p += interaction;

        double loss = sigmoid(y.data[x][0] * p)-1;
        total_loss += loss * loss; // Squaring to consider positive values
        total_samples++;

        bias_grad += alpha * loss * y.data[x][0];
        for (int i = 0; i < X.cols; i++) {
          weights_grad.data[i][0] += alpha * loss * y.data[x][0] * X.data[x][i];
          for (int j = 0; j < feature_potential; j++) {
            double term =
                X.data[x][i] *
                (X.data[x][i] * model->factors.data[i][j] - interaction);
            factors_grad.data[i][j] += alpha * loss * y.data[x][0] * term;
          }
        }
      }

      // update model parameters
      model->bias -= bias_grad / (end - start);
      for (int i = 0; i < X.cols; i++) {
        model->weights.data[i][0] -= weights_grad.data[i][0] / (end - start);
        for (int j = 0; j < feature_potential; j++) {
          model->factors.data[i][j] -= factors_grad.data[i][j] / (end - start);
        }
      }
      free_matrix(weights_grad);
      free_matrix(factors_grad);
    }
    // Apply decay to the learning rate
    alpha *= decay_rate;

    // Print average loss for each epoch
    printf("Epoch %d, Average Loss: %f, Learning Rate: %f\n", it + 1,
           sqrt(total_loss / total_samples), alpha);
  }
  free(indices);
}

// Predict function for FM model
double *predict(FMModel *model, Matrix X) {
  //print weight
  for (int i = 0; i < X.cols; i++) {
    printf("Weight %f",model->weights.data[i][0]);
  }
  
  double *predictions = (double *)malloc(X.rows * sizeof(double));
  for (int x = 0; x < X.rows; x++) {
    double interaction = calculate_interaction(X, model->factors, x);
    double p = model->bias;
    for (int i = 0; i < X.cols; i++) {
      p += X.data[x][i] * model->weights.data[i][0];
    }
    p += interaction;
    predictions[x] = sigmoid(p);
  }
  return predictions;
}
