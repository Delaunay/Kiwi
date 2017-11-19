#pragma once
#include <chrono>

class OutputStream;

enum class Status {
  PENDING, // waiting for running condition to be satisfied
  RUNNING, // running
  SUCCESS, // run successfuly
  FAILED,  // an error occured while running

  PAUSED, // execution was paused
  KILLED, // job was killed before completion

  IGNORE, // job is not run subsequent job are ran
  HOLD    // job is not run subsequent job are not ran
};

template <typename Input, typename Output> class Task {
public:
  typedef std::chrono::high_resolution_clock::time_point TimePoint;

  virtual Output run(Input in) = 0;

  Output eval(Input in) {
    // Should run
    if (status() != Status::PENDING)
      return None

          set_status(Status::RUNNING);
    _start = std::chrono::high_resolution_clock::now();
    Output out = run(in);
    _end = std::chrono::high_resolution_clock::now();

    return out;
  }

  void set_status(Status status) { _status = status; }

  Status status() { return _status; }

private:
  TimePoint _start;
  TimePoint _end;

  Status _status;
};
