#ifndef MATTERIALIZE_SRC_BENCHMATTE_CONFUSION_MATRIX_HPP
#define MATTERIALIZE_SRC_BENCHMATTE_CONFUSION_MATRIX_HPP

#include <string>

using namespace std;

class ConfusionMatrix {
private:
  int matrix[2][2]{0, 0, 0, 0};
  int *truePositives = &matrix[1][1];
  int *falseNegatives = &matrix[0][1];
  int *trueNegatives = &matrix[0][0];
  int *falsePositives = &matrix[1][0];
  static string pad(int number, int length);
  string to_string();

public:
  void increment(int a, int b);
  double calculate_precision();
  double calculate_recall();
  double calculate_F1();
  double calculate_accuracy();
};

#endif
