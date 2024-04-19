#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    char date[11];  // YYYY-MM-DD
    float open;
    float high;
    float low;
    float close;
    int volume;
    float dividends;
    float stock_splits;
    float price_30_days_ago;
    float price_momentum;
    float average_volume_short_term;
    float average_volume_long_term;
    float volume_factor;
    float volume_ratio;
    float daily_returns;
    float volatility_30d;
    float rsi;
} StockRecord;

#define MAX_DAYS 1000
#define DAYS_30 30
#define SHORT_TERM 10
#define LONG_TERM 30

// Helper function to calculate moving averages
float moving_average(float values[], int start, int end) {
    float sum = 0.0;
    int count = 0;
    for (int i = start; i <= end; i++) {
        sum += values[i];
        count++;
    }
    return count > 0 ? sum / count : 0.0;
}

// Read CSV data
void read_csv(const char* filename, StockRecord records[], int* record_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Cannot open file.\n");
        return;
    }

    char line[1024];
    int index = 0;
    while (fgets(line, sizeof(line), file)) {
        if (index == 0) {  // Skip header
            index++;
            continue;
        }
        sscanf(line, "%[^,],%f,%f,%f,%f,%d,%f,%f",
               records[index - 1].date, &records[index - 1].open, &records[index - 1].high,
               &records[index - 1].low, &records[index - 1].close, &records[index - 1].volume,
               &records[index - 1].dividends, &records[index - 1].stock_splits);
        index++;
        if (index >= MAX_DAYS + 1) break;  // Prevent overflow
    }
    *record_count = index - 1;
    fclose(file);
}

// Perform calculations
void calculate_factors(StockRecord records[], int n) {
    float gains[n], losses[n], avg_gain, avg_loss, rs, sum_returns = 0.0;
    float volumes[n], closes[n];
    memset(gains, 0, sizeof(gains));
    memset(losses, 0, sizeof(losses));

    for (int i = 1; i < n; i++) {
        records[i].daily_returns = (records[i].close - records[i - 1].close) / records[i - 1].close;
        sum_returns += fabs(records[i].daily_returns);
        gains[i] = fmax(0, records[i].close - records[i - 1].close);
        losses[i] = fmax(0, records[i - 1].close - records[i].close);
        volumes[i] = records[i].volume;
        closes[i] = records[i].close;

        if (i >= DAYS_30) {
            records[i].price_30_days_ago = records[i - DAYS_30].close;
            records[i].price_momentum = (records[i].close - records[i].price_30_days_ago) / records[i].price_30_days_ago;
            records[i].average_volume_long_term = moving_average(volumes, i - LONG_TERM + 1, i);
            records[i].volatility_30d = sqrt(sum_returns / DAYS_30);
            sum_returns -= fabs(records[i - DAYS_30 + 1].daily_returns);
        }
        if (i >= SHORT_TERM) {
            records[i].average_volume_short_term = moving_average(volumes, i - SHORT_TERM + 1, i);
        }
        if (records[i].average_volume_long_term > 0) {
            records[i].volume_factor = records[i].average_volume_short_term / records[i].average_volume_long_term;
            records[i].volume_ratio = records[i].volume / records[i].average_volume_long_term;
        }
    }

    // Calculate RSI
    for (int i = DAYS_30; i < n; i++) {
        avg_gain = moving_average(gains, i - DAYS_30 + 1, i);
        avg_loss = moving_average(losses, i - DAYS_30 + 1, i);
        rs = avg_loss != 0 ? avg_gain / avg_loss : 0;
        records[i].rsi = rs != 0 ? 100.0 - (100.0 / (1.0 + rs)) : 100.0;
    }
}

// Write results to a CSV file
void write_csv(const char* filename, StockRecord records[], int n) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Cannot open file.\n");
        return;
    }

    fprintf(file, "Date,Close,Volume,Price Momentum,Volume Factor,Volume Ratio,Volatility 30D,RSI\n");
    for (int i = 0; i < n; i++) {
        fprintf(file, "%s,%f,%d,%f,%f,%f,%f,%f\n",
                records[i].date, records[i].close, records[i].volume, records[i].price_momentum,
                records[i].volume_factor, records[i].volume_ratio,
                records[i].volatility_30d, records[i].rsi);
    }
    fclose(file);
}

// int main() {
//     StockRecord records[MAX_DAYS];
//     int record_count;
//     read_csv("AFRM.csv", records, &record_count);
//     calculate_factors(records, record_count);
//     write_csv("output.csv", records, record_count);
//     return 0;
// }
