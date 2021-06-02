/* Suppress Boost deprecated header warnings. */
#define BOOST_ALLOW_DEPRECATED_HEADERS

#include <iostream>
#include <string>
#include <unordered_map>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <uhd/types/tune_request.hpp>
#include <zmq.hpp>
#include "usrp.hpp"
#include "message.hpp"

usrp::usrp(std::string device_args, std::string zmq_bind) :
  device_args(device_args),
  zmq_bind(zmq_bind),
  device(uhd::usrp::multi_usrp::make(device_args)) {}

std::string usrp::get_pp_string() const {
  return device->get_pp_string();
}

std::string usrp::get_rx_antenna() const {
  return device->get_rx_antenna();
}

std::string usrp::get_rx_bandwidth() const {
  return std::to_string(device->get_rx_bandwidth());
}

std::string usrp::get_rx_freq() const {
  return std::to_string(device->get_rx_freq());
}

std::string usrp::get_rx_gain() const {
  return std::to_string(device->get_rx_gain());
}

std::string usrp::get_rx_rate() const {
  return std::to_string(device->get_rx_rate());
}

std::string usrp::get_tx_antenna() const {
  return device->get_tx_antenna();
}

std::string usrp::get_tx_bandwidth() const {
  return std::to_string(device->get_tx_bandwidth());
}

std::string usrp::get_tx_freq() const {
  return std::to_string(device->get_tx_freq());
}

std::string usrp::get_tx_gain() const {
  return std::to_string(device->get_tx_gain());
}

std::string usrp::get_tx_rate() const {
  return std::to_string(device->get_tx_rate());
}

void usrp::set_rx_antenna(std::string &ant) const {
  device->set_rx_antenna(ant);
}

void usrp::set_rx_bandwidth(std::string &bw) const {
  device->set_rx_bandwidth(std::stod(bw));
}

void usrp::set_rx_freq(std::string &freq) const {
  uhd::tune_request_t target_freq(std::stod(freq));
  device->set_rx_freq(target_freq);
}

void usrp::set_rx_gain(std::string &gain) const {
  device->set_rx_gain(std::stod(gain));
}

void usrp::set_rx_rate(std::string &rate) const {
  device->set_rx_rate(std::stod(rate));
}


void usrp::set_tx_antenna(std::string &ant) const {
  device->set_tx_antenna(ant);
}

void usrp::set_tx_bandwidth(std::string &bw) const {
  device->set_tx_bandwidth(std::stod(bw));
}

void usrp::set_tx_freq(std::string &freq) const {
  uhd::tune_request_t target_freq(std::stod(freq));
  device->set_tx_freq(target_freq);
}

void usrp::set_tx_gain(std::string &gain) const {
  device->set_tx_gain(std::stod(gain));
}

void usrp::set_tx_rate(std::string &rate) const {
  device->set_tx_rate(std::stod(rate));
}

std::string usrp::get_device_config(std::string &param) const {
  // TODO: Replace the if-else statement with a hash table.
  if (param == "pp_string")
    return get_pp_string();
  else if (param == "rx_antenna")
    return get_rx_antenna();
  else if (param == "rx_bandwidth")
    return get_rx_bandwidth();
  else if (param == "rx_freq")
    return get_rx_freq();
  else if (param == "rx_gain")
    return get_rx_gain();
  else if (param == "rx_rate")
    return get_rx_rate();
  else if (param == "tx_antenna")
    return get_tx_antenna();
  else if (param == "tx_bandwidth")
    return get_tx_bandwidth();
  else if (param == "tx_freq")
    return get_tx_freq();
  else if (param == "tx_gain")
    return get_tx_gain();
  else if (param == "tx_rate")
    return get_tx_rate();
  return "";
}

void usrp::set_device_config(std::string &param, std::string &val) const {
  // TODO: Replace the if-else statement with a hash table.
  if (param == "rx_antenna")
    set_rx_antenna(val);
  else if (param == "rx_bandwidth")
    set_rx_bandwidth(val);
  else if (param == "rx_freq")
    set_rx_freq(val);
  else if (param == "rx_gain")
    set_rx_gain(val);
  else if (param == "rx_rate")
    set_rx_rate(val);
  else if (param == "tx_antenna")
    set_tx_antenna(val);
  else if (param == "tx_bandwidth")
    set_tx_bandwidth(val);
  else if (param == "tx_freq")
    set_tx_freq(val);
  else if (param == "tx_gain")
    set_tx_gain(val);
  else if (param == "tx_rate")
    set_tx_rate(val);
}

message_payload
usrp::get_device_configs(message_payload &&payload) const {
  // TODO: Replace lock()/unlock() with a lock guard.
  device_lock.lock();
  for (uint64_t I = 0; I < payload.size(); ++I)
    payload[I].second = get_device_config(payload[I].first);
  device_lock.unlock();
  return payload;
}

message_payload
usrp::set_device_configs(message_payload &&payload) const {
  // TODO: Replace lock()/unlock() with a lock guard.
  device_lock.lock();
  for (uint64_t I = 0; I < payload.size(); ++I) {
    set_device_config(payload[I].first, payload[I].second);
    payload[I].second = get_device_config(payload[I].first);
  }
  device_lock.unlock();
  return payload;
}

message usrp::handle_request(message &msg) {
  if (msg.get_type() == message_type::CONF_GET) {
    return message{msg.get_id(), msg.get_type(),
      get_device_configs(msg.get_payload())};
  } else if (msg.get_type() == message_type::CONF_SET) {
    return message{msg.get_id(), msg.get_type(),
      set_device_configs(msg.get_payload())};
  }
  return message{msg.get_id(), msg.get_type(), {}};
}

void usrp::zmq_server_run() {
  zmq::context_t ctx;
  zmq::socket_t socket(ctx, zmq::socket_type::rep);
  socket.bind(zmq_bind);

  for (;;) {
    zmq::message_t req;
    socket.recv(req, zmq::recv_flags::none);
    std::string req_str = req.to_string();
    message msg = message::from_json(req_str);
    message response = handle_request(msg);
    socket.send(zmq::buffer(message::to_json(response)), zmq::send_flags::none);
  }
}
