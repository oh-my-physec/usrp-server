#ifndef _USRP_HPP_
#define _USRP_HPP_

#include <vector>
#include <functional>
#include <boost/thread.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <zmq.hpp>
#include "message.hpp"

class usrp {
private:
  // ZeroMQ server listen address.
  std::string zmq_bind;
  // USRP device arguments.
  std::string device_args;

  // USRP device should be exclusively accessed.
  mutable boost::mutex device_lock;
  uhd::usrp::multi_usrp::sptr device;

  mutable size_t rx_sample_per_buffer;
  mutable size_t tx_sample_per_buffer;

  // The CPU format is a string that describes the format of host memory.
  // Conversions for the following CPU formats have been implemented:
  // fc64 - complex<double>
  // fc32 - complex<float>
  // sc16 - complex<int>
  // sc8  - complex<int8_t>
  mutable std::string rx_cpu_format = "fc32";
  mutable std::string tx_cpu_format = "fc32";
  // The OTW format is a string that describes the format over-the-wire. The
  // following over-the-wire formats have been implemented:
  // sc16 - Q16  I16
  // sc8  - Q8_1 I8_1 Q8_0 I8_0
  mutable std::string rx_otw_format = "sc16";
  mutable std::string tx_otw_format = "sc16";

  // Settling time before receiving/transmitting signals.
  mutable double rx_settling_time = 0.0;
  mutable double tx_settling_time = 0.0;

  // When rx_keep_sampling is false, the receiver will stop sampling to file.
  // rx_keep_sampling is protected by the sample_to_file_thread_lock.
  mutable bool rx_keep_sampling = false;

  // Thread for sample_to_file.
  mutable boost::mutex sample_to_file_thread_lock;
  boost::thread *sample_to_file_thread = nullptr;

  // Thread for sample_from_file.
  mutable boost::mutex sample_from_file_thread_lock;
  boost::thread *sample_from_file_thread = nullptr;

  // Threads for running jobs.
  boost::thread_group threads;

  struct getter_setter_pair {
    std::function<std::string()> getter;
    std::function<void(std::string&)> setter;
  };
  std::unordered_map<std::string, getter_setter_pair> getter_setter_pairs;
  std::unordered_map<std::string,
		     std::function<message(const message &)>> task_map;
public:
  usrp(usrp &usrp) = delete;
  usrp(std::string device_args, std::string zmq_bind);
  uhd::usrp::multi_usrp::sptr get_device() { return device; }

  std::string get_pp_string() const;
  std::string get_rx_antenna() const;
  std::string get_rx_bandwidth() const;
  std::string get_rx_freq() const;
  std::string get_rx_gain() const;
  std::string get_rx_rate() const;
  std::string get_rx_sample_per_buffer() const;
  std::string get_rx_settling_time() const;
  std::string get_rx_cpu_format() const;
  std::string get_rx_otw_format() const;
  std::string get_tx_antenna() const;
  std::string get_tx_bandwidth() const;
  std::string get_tx_freq() const;
  std::string get_tx_gain() const;
  std::string get_tx_rate() const;
  std::string get_tx_sample_per_buffer() const;
  std::string get_tx_settling_time() const;
  std::string get_tx_cpu_format() const;
  std::string get_tx_otw_format() const;
  std::string get_clock_source() const;

  void set_pp_string(std::string &pp) const;
  void set_rx_antenna(std::string &ant) const;
  void set_rx_bandwidth(std::string &bw) const;
  void set_rx_freq(std::string &freq) const;
  void set_rx_gain(std::string &gain) const;
  void set_rx_rate(std::string &rate) const;
  void set_rx_sample_per_buffer(std::string &spb) const;
  void set_rx_settling_time(std::string &time) const;
  void set_rx_cpu_format(std::string &fmt) const;
  void set_rx_otw_format(std::string &fmt) const;
  void set_tx_antenna(std::string &ant) const;
  void set_tx_bandwidth(std::string &bw) const;
  void set_tx_freq(std::string &freq) const;
  void set_tx_gain(std::string &gain) const;
  void set_tx_rate(std::string &rate) const;
  void set_tx_sample_per_buffer(std::string &spb) const;
  void set_tx_settling_time(std::string &time) const;
  void set_tx_cpu_format(std::string &fmt) const;
  void set_tx_otw_format(std::string &fmt) const;
  void set_clock_source(std::string &clock_source) const;

  std::string get_device_config(std::string &param) const;
  void set_device_config(std::string &param, std::string &val) const;
  message_payload get_or_set_device_configs(message_payload &&params) const;

  bool rx_is_sampling_to_file() const;

  template <typename sample_type>
  void sample_from_file_generic(const std::string &filename) const;
  void sample_from_file(const std::string &filename) const;
  message launch_sample_from_file(const message &msg);
  void shutdown_sample_from_file();

  template <typename sample_type>
  void sample_to_file_generic(const std::string &filename) const;
  void sample_to_file(const std::string &filename) const;
  message launch_sample_to_file(const message &msg);
  void shutdown_sample_to_file();
  message launch_shutdown_sample_to_file(const message &msg);

  void zmq_server_run();
  message handle_request(message &msg);
  message process_conf_req(message &msg);
  message process_work_req(message &msg);

  void force_shutdown_all_jobs();
  ~usrp();
};

#endif
