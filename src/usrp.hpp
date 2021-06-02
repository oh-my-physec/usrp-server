#ifndef _USRP_HPP_
#define _USRP_HPP_

#include <vector>
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

public:
  usrp(std::string device_args, std::string zmq_bind);
  uhd::usrp::multi_usrp::sptr get_device() { return device; }

  std::string get_pp_string() const;
  std::string get_rx_antenna() const;
  std::string get_rx_bandwidth() const;
  std::string get_rx_freq() const;
  std::string get_rx_gain() const;
  std::string get_rx_rate() const;
  std::string get_tx_antenna() const;
  std::string get_tx_bandwidth() const;
  std::string get_tx_freq() const;
  std::string get_tx_gain() const;
  std::string get_tx_rate() const;

  void set_rx_antenna(std::string &ant) const;
  void set_rx_bandwidth(std::string &bw) const;
  void set_rx_freq(std::string &freq) const;
  void set_rx_gain(std::string &gain) const;
  void set_rx_rate(std::string &rate) const;
  void set_tx_antenna(std::string &ant) const;
  void set_tx_bandwidth(std::string &bw) const;
  void set_tx_freq(std::string &freq) const;
  void set_tx_gain(std::string &gain) const;
  void set_tx_rate(std::string &rate) const;

  std::string get_device_config(std::string &param) const;
  message_payload
  get_device_configs(message_payload &&params) const;

  void set_device_config(std::string &param, std::string &val) const;
  message_payload set_device_configs(message_payload &&params) const;

  void zmq_server_run();
  message handle_request(message &msg);
};

#endif
