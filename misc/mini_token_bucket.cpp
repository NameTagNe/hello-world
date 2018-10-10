// 2018 Sep. A variant of token bucket implementation.

struct TokenBucket
{
  double fill_interval_;
  double gen_interval_;
  double reset_time_;
  // reset_time_ is equivalent to last_consume_time + token_num and
  // the calculated token num is auto renormalized on limit change.
  
  TokenBucket()
  : fill_interval_(0.0)
  , gen_interval_(0.0)
  , reset_time_(0.0) {}
  
  // eg, for 10req/30s, capacity=10, period=30
  void set_limit(double capacity, double period)
  {
    fill_interval_ = period;
    gen_interval_ = period / capacity;
  }
  
  static double get_monotonic_time()
  {
      struct timespec ts;
      clock_gettime(CLOCK_MONOTONIC, &ts);
      // add an offset to avoid too small timestamp
      return (24 * 60 * 60) + ts.tv_sec + ts.tv_nsec/1000000000.0;
  }
  
  bool consume(unsigned num)
  {
    double curr_time = get_monotonic_time();
    double new_reset_time = std::max(reset_time_, curr_time - fill_interval_) + gen_interval_ * num;
    if (curr_time >= new_reset_time)
    {
      reset_time_ = new_reset_time; // reset_time_ is monotonic increasing
      return true;
    }
    return false;
  }
};

