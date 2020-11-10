#include "confusion_matrix.hpp"
#include <sstream>
#include <string>

string ConfusionMatrix::pad(int number, int length) {
  string num_str = std::to_string(number);
  int padding = length - num_str.length();
  string result;
  for (int i = 0; i < padding; i++) {
    result.push_back(' ');
  }
  result.append(num_str);
  return result;
}

void ConfusionMatrix::increment(int a, int b) {
  matrix[a][b]++;
}

double ConfusionMatrix::calculate_precision() {
  return 1.0 * *truePositives / (*truePositives + *falsePositives);
}

double ConfusionMatrix::calculate_recall() {
  return 1.0 * *truePositives / (*truePositives + *falseNegatives);
}

double ConfusionMatrix::calculate_F1() {
  double precision = calculate_precision();
  double recall = calculate_recall();

  return (2.0 * precision * recall) / (precision + recall);
}

double ConfusionMatrix::calculate_accuracy() {
  return (1.0 * *truePositives + *trueNegatives) /
         (*truePositives + *trueNegatives + *falsePositives + *falseNegatives);
}

string ConfusionMatrix::to_string() {
  stringstream buffer;
  buffer << "Confusion Matrix\n"
         << "precision:  " << calculate_precision() << "\n"
         << "recall:     " << calculate_recall() << "\n"
         << "f1:         " << calculate_F1() << "\n"
         << "accuracy:   " << calculate_accuracy() << "\n\n"
         << "                [Predicted]\n"
         << "                fg   |   bg\n"
         << "            +--------+--------+\n"
         << "         fg | " << pad(*trueNegatives, 6) << " | "
         << pad(*falseNegatives, 6) << " |\n"
         << "[Actual] ---+--------+--------+\n"
         << "         bg | " << pad(*falsePositives, 6) << " | "
         << pad(*trueNegatives, 6) << " |\n"
         << "            +--------+--------+\n";
  return buffer.str();
}
