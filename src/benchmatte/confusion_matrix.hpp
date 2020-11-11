#ifndef MATTERIALIZE_SRC_BENCHMATTE_CONFUSION_MATRIX_HPP
#define MATTERIALIZE_SRC_BENCHMATTE_CONFUSION_MATRIX_HPP

#include <string>

using namespace std;

class ConfusionMatrix {
private:
  int matrix[2][2]{{0, 0}, {0, 0}};
  static string pad(int number, int length);

public:
  void increment(int a, int b);
  double calculate_precision();
  double calculate_recall();
  double calculate_F1();
  double calculate_accuracy();
  string to_string();
};

#endif
