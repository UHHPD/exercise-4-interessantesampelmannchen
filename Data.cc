#include "Data.hh"

#include <iostream>
#include <fstream>
#include <cassert>
#include <stdexcept>
#include <cmath>

using namespace std;

double background(double x) {
  return 0.005 - 0.00001 * x + 0.08 * exp(-0.015 * x);
}

Data::Data(const std::string& filename) {
  ifstream file(filename);
  expName = filename;
  // check if file opened correctly
  if (!file.is_open()) {
    throw std::runtime_error("Error could not open file with name: " +
                             filename);
  }

  int size = -1;
  file >> size;

  // read in data from file: m_siz+1 bin edges
  for (int i = 0; i < size + 1; ++i) {
    double edge;
    file >> edge;
    m_bins.push_back(edge);
  }
  // read in data from file: m_siz bin contents
  for (int i = 0; i < size; ++i) {
    double entries;
    file >> entries;
    m_data.push_back(entries);
  }

  // read in data from file: m_siz bin errors
  for (int i = 0; i < size; ++i) {
    double error;
    file >> error;
    m_errors.push_back(error);
  }

  // done! close the file
  file.close();

  assertSizes();
};

void Data::assertSizes() { assert(m_data.size() + 1 == m_bins.size()); }

int Data::checkCompatibility(const Data& in, int n) {
  int n_incompatible = 0;
  for (int i = 0; i < m_data.size(); ++i) {
    double diff = std::abs(m_data[i] - in.measurement(i));
    double sigma = sqrt(pow(m_errors[i], 2) + pow(in.error(i), 2));
    if (diff > n * sigma) {
      ++n_incompatible;
    }
  }
  return n_incompatible;
}


Data Data::average(const Data& in) {
  Data out;
  for (int i = 0; i < m_data.size(); ++i) {
    double weight1 = pow(m_errors[i], -2);
    double weight2 = pow(in.error(i), -2);
    double average = (weight1 * m_data[i] + weight2 * in.measurement(i)) /
                     (weight1 + weight2);
    double error = sqrt(1 / (weight1 + weight2));
    out.m_data.push_back(average);
    out.m_errors.push_back(error);
  }
  out.m_bins = m_bins;
  out.assertSizes();
  return out;
}

double Data::chi2() {
  double chi2 = 0;
  for (int i = 0; i < m_data.size(); ++i) {
    double diff = m_data[i] - background(binCenter(i));
    chi2 += pow(diff / m_errors[i], 2);
  }
  return chi2 / 52;
}