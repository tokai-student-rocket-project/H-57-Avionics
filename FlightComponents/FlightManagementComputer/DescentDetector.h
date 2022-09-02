#ifndef DESCENT_DETECTOR_H_
#define DESCENT_DETECTOR_H_

class DescentDetector
{
public:
  DescentDetector(double weight);

  void update(double altitude);

  bool is_descending_;
  
private:
  const double DESCENT_DIFFERENT_ = 0.0;
  const int MINIMUM_DESCENT_COUNT_ = 10;

  double weight_;
  double average_;
  double average_old_;
  int descent_count_;
};

#endif