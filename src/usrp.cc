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

using mt = message_type;

#define GETTER_SETTER_PAIR(name)                                                \
  { #name, getter_setter_pair {                                                 \
              std::bind(&usrp::get_## name, this),                              \
	      std::bind(&usrp::set_## name, this, std::placeholders::_1) } }

usrp::usrp(std::string device_args, std::string zmq_bind) :
  device_args(device_args),
  zmq_bind(zmq_bind),
  device(uhd::usrp::multi_usrp::make(device_args)),
  getter_setter_pairs{
    GETTER_SETTER_PAIR(pp_string),
    GETTER_SETTER_PAIR(rx_antenna),
    GETTER_SETTER_PAIR(rx_bandwidth),
    GETTER_SETTER_PAIR(rx_freq),
    GETTER_SETTER_PAIR(rx_gain),
    GETTER_SETTER_PAIR(rx_rate),
    GETTER_SETTER_PAIR(rx_sample_per_buffer),
    GETTER_SETTER_PAIR(tx_antenna),
    GETTER_SETTER_PAIR(tx_bandwidth),
    GETTER_SETTER_PAIR(tx_freq),
    GETTER_SETTER_PAIR(tx_gain),
    GETTER_SETTER_PAIR(tx_rate),
    GETTER_SETTER_PAIR(tx_sample_per_buffer),
  } {}

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

std::string usrp::get_rx_sample_per_buffer() const {
  return std::to_string(rx_sample_per_buffer);
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

std::string usrp::get_tx_sample_per_buffer() const {
  return std::to_string(tx_sample_per_buffer);
}

void usrp::set_pp_string(std::string &pp) const {
  /* DO NOTHING. Just work as a placeholder. */
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

void usrp::set_rx_sample_per_buffer(std::string &spb) const {
  rx_sample_per_buffer = std::stoll(spb);
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

void usrp::set_tx_sample_per_buffer(std::string &spb) const {
  tx_sample_per_buffer = std::stoll(spb);
}

std::string usrp::get_device_config(std::string &param) const {
  auto It = getter_setter_pairs.find(param);
  if (It != getter_setter_pairs.cend())
    return It->second.getter();
  return "";
}

void usrp::set_device_config(std::string &param, std::string &val) const {
  auto It = getter_setter_pairs.find(param);
  if (It != getter_setter_pairs.cend()) {
    It->second.setter(val);
  }
}

message_payload
usrp::get_or_set_device_configs(message_payload &&payload) const {
  // TODO: Replace lock()/unlock() with a lock guard.
  device_lock.lock();
  for (uint64_t I = 0; I < payload.size(); ++I) {
    if (payload[I].second != "")
      set_device_config(payload[I].first, payload[I].second);
    payload[I].second = get_device_config(payload[I].first);
  }
  device_lock.unlock();
  return payload;
}

template <typename sample_type>
void usrp::sample_from_file(const std::string &filename) const {
}

template <typename sample_type>
void usrp::sample_to_file(const std::string &filename) const {
}

message usrp::handle_request(message &msg) {
  if (msg.get_type() == mt::CONF) {
    return message{msg.get_id(), msg.get_type(),
      get_or_set_device_configs(msg.get_payload())};
  } else if (msg.get_type() == mt::WORK) {
    return message{msg.get_id(), msg.get_type(), {}};
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
