#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <cmath>


#include "Data.hh"
#include "run.h"

// generic function comparing two values of some type T used later for int and
// double
template <class T>
bool testEqual(const std::string& name, T expected, T real) {
  if (expected != real) {
    std::cout << "(" << name << ": <" << expected << ">"
              << ", " << real << ") ";
    return false;
  }
  return true;
}

bool testReadingSize() {
  std::cout << "testReadingSize...";
  Data datA("testA");
  return testEqual<int>("size", 1, datA.size());
}

bool testReadingMeasurement() {
  std::cout << "testReadingMeasurement...";
  Data datA("testA");
  return testEqual("measurement", 10., datA.measurement(0));
}

bool testReadingBinEdges() {
  std::cout << "testReadingBinEdges...";
  Data datA("testA");
  return testEqual("bin low", 0., datA.binLow(0)) &&
         testEqual("bin high", 1., datA.binHigh(0));
}

bool testReadingErrors() {
  std::cout << "testReadingErrors...";
  Data datA("testA");
  return testEqual("error", 2., datA.error(0));
}

bool testCopyConstructor() {
  std::cout << "testCopyConstructor...";
  Data a("testA");
  Data b(a);
  Data c = a;  // equivalent to Data c(a)
  return testEqual("measurement", 10., b.measurement(0)) &&
         testEqual("measurement", 10., c.measurement(0));
}

bool testCheckCompatibility() {
  std::cout << "testCheckCompatibility...";
  Data a("testA");
  Data b("testB");
  return testEqual("incompatible", 1, a.checkCompatibility(b, 1)) &&
         testEqual("compatible", 0, a.checkCompatibility(b, 2)) &&
         testEqual("compatible", 0, a.checkCompatibility(a, 1));
}

void runTests() {
  std::cout << "running tests...\n";
  std::vector<std::function<bool()> > tests(
      {testReadingSize, testReadingMeasurement, testReadingBinEdges,
       testReadingErrors, testCopyConstructor, testCheckCompatibility});
  for (auto test : tests)
    std::cout << (test() ? " ok" : " FAILED!") << std::endl;
}

void printNumberIncompatiblePoints(const std::string& filename1, const std::string& filename2, int i, int j, int sigma, std::vector<Data> &data)
{
  std::cout << "Number of incompatible points between " << filename1 << " and " << filename2 << " for " << sigma << " sigma: " << data[i].checkCompatibility(data[j], 1) << std::endl;
}

void exerciseBreak()
{
  std::cout << "******************************************************" << std::endl;
}

int main() {
  using namespace std;

  exerciseBreak();
  runTests();
  exerciseBreak();
  // create an object which holds data of experiment A
  std::vector<string> filenames({"A", "B", "C", "D"});
  std::vector<Data> data;

  for (string filename : filenames) {
    data.push_back(Data("exp_" + filename));
  }

  cout << "bin 27: from " << data[0].binLow(27) << " to " << data[0].binHigh(27)
      << endl;
  for (int i = 0; i < data.size(); ++i) {
    cout << "measurement of experiment " << filenames[i] << " in bin 27: " << data[i].measurement(27)
        << endl;
  }

  exerciseBreak();

  cout << "Checking compatibility of experiments" << endl;
  // Check data compatibility in the 27th bin
  for (int i = 0; i < data.size(); ++i) {
    for (int j = i + 1; j < data.size(); ++j) {
      if (i == j) {
        continue;
      }
      double diff = abs(data[i].measurement(27) - data[j].measurement(27));
      double sigma = sqrt(pow(data[i].error(27), 2) + pow(data[j].error(27), 2));
      if (diff > 2 * sigma) {
        cout << "Data in bin 27 of experiment " << filenames[i] << " and " << filenames[j] << " are not compatible." << endl;
      }
      else {
        cout << "Data in bin 27 of experiment " << filenames[i] << " and " << filenames[j] << " are compatible." << endl;
      }
    }
  }
  exerciseBreak();
  cout << "Checking number of incompatible points between the experiments" << endl;
  for (int i = 0; i < data.size(); ++i) {
    for (int j =  i + 1; j < data.size(); ++j) {
      if (i == j) {
        continue;
      }
      printNumberIncompatiblePoints(filenames[i], filenames[j], i, j, 1, data);
      printNumberIncompatiblePoints(filenames[i], filenames[j], i, j, 2, data);
      printNumberIncompatiblePoints(filenames[i], filenames[j], i, j, 3, data);
    }
  }

  exerciseBreak();
  cout << "Averaging compatible data" << endl;
  
  for (int i = 0; i < data.size(); ++i) {
    for (int j =  i + 1; j < data.size(); ++j) {
      if (i == j) {
        continue;
      }
      if (data[i].checkCompatibility(data[j], 2) < 2) {
        Data average = data[i].average(data[j]);
        cout << "Average of " << filenames[i] << " and " << filenames[j] << " is compatible with both experiments." << endl;
        cout << "average measurement of experiment ";
        for (int k = 0; k < average.size(); ++k) {
          cout << average.measurement(k);
        }
        cout << endl;
      }
      else {
        cout << "Average of " << filenames[i] << " and " << filenames[j] << " is not compatible with both experiments." << endl;
      }
    }
  }

  exerciseBreak();

  for (Data d : data) {
    cout << "chi2 of experiment " << d.getName() << " " << d.chi2() << endl;
  }

  exerciseBreak();

  Data totalAverage = data[0];
  for (int i = 1; i < data.size(); ++i) {
    totalAverage = totalAverage.average(data[i]);
  }

  cout << "chi2 of total average " << totalAverage.chi2() << endl;

  exerciseBreak();
  return 0;
}
