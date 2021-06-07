/* Suppress Boost deprecated header warnings. */
#define BOOST_ALLOW_DEPRECATED_HEADERS

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <uhd/types/tune_request.hpp>
#include <zmq.hpp>
#include "usrp.hpp"
#include "message.hpp"
#include "wave_table.hpp"

using mt = message_type;
using wt = wave_type;

#define GETTER_SETTER_PAIR(name)                                                \
  { #name, getter_setter_pair {                                                 \
              std::bind(&usrp::get_## name, this),                              \
	      std::bind(&usrp::set_## name, this, std::placeholders::_1) } }

#define TASK_PAIR(task)                                                         \
  { #task, std::bind(&usrp::launch_## task, this, std::placeholders::_1) }

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
    GETTER_SETTER_PAIR(rx_settling_time),
    GETTER_SETTER_PAIR(rx_cpu_format),
    GETTER_SETTER_PAIR(rx_otw_format),
    GETTER_SETTER_PAIR(tx_antenna),
    GETTER_SETTER_PAIR(tx_bandwidth),
    GETTER_SETTER_PAIR(tx_freq),
    GETTER_SETTER_PAIR(tx_gain),
    GETTER_SETTER_PAIR(tx_rate),
    GETTER_SETTER_PAIR(tx_sample_per_buffer),
    GETTER_SETTER_PAIR(tx_settling_time),
    GETTER_SETTER_PAIR(tx_cpu_format),
    GETTER_SETTER_PAIR(tx_otw_format),
    GETTER_SETTER_PAIR(clock_source),
  },
  task_map{
    TASK_PAIR(sample_to_file),
    TASK_PAIR(shutdown_sample_to_file),
    TASK_PAIR(sample_from_file),
  } {}

#define STATUS_OK   "OK"
#define STATUS_FAIL "FAIL"

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

std::string usrp::get_rx_settling_time() const {
  return std::to_string(rx_settling_time);
}

std::string usrp::get_rx_cpu_format() const {
  return rx_cpu_format;
}

std::string usrp::get_rx_otw_format() const {
  return rx_otw_format;
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

std::string usrp::get_tx_settling_time() const {
  return std::to_string(tx_settling_time);
}

std::string usrp::get_tx_cpu_format() const {
  return tx_cpu_format;
}

std::string usrp::get_tx_otw_format() const {
  return tx_otw_format;
}

std::string usrp::get_clock_source() const {
  return device->get_clock_source(/*mboard=*/0);
}

void usrp::set_pp_string(std::string &pp) {
  /* DO NOTHING. Just work as a placeholder. */
}

void usrp::set_rx_antenna(std::string &ant) {
  device->set_rx_antenna(ant);
}

void usrp::set_rx_bandwidth(std::string &bw) {
  device->set_rx_bandwidth(std::stod(bw));
}

void usrp::set_rx_freq(std::string &freq) {
  uhd::tune_request_t target_freq(std::stod(freq));
  device->set_rx_freq(target_freq);
}

void usrp::set_rx_gain(std::string &gain) {
  device->set_rx_gain(std::stod(gain));
}

void usrp::set_rx_rate(std::string &rate) {
  device->set_rx_rate(std::stod(rate));
}

void usrp::set_rx_sample_per_buffer(std::string &spb) {
  rx_sample_per_buffer = std::stoll(spb);
}

void usrp::set_rx_settling_time(std::string &time) {
  rx_settling_time = std::stod(time);
}

void usrp::set_rx_cpu_format(std::string &fmt) {
  rx_cpu_format = fmt;
}

void usrp::set_rx_otw_format(std::string &fmt) {
  rx_otw_format = fmt;
}

void usrp::set_tx_antenna(std::string &ant) {
  device->set_tx_antenna(ant);
}

void usrp::set_tx_bandwidth(std::string &bw) {
  device->set_tx_bandwidth(std::stod(bw));
}

void usrp::set_tx_freq(std::string &freq) {
  uhd::tune_request_t target_freq(std::stod(freq));
  device->set_tx_freq(target_freq);
}

void usrp::set_tx_gain(std::string &gain) {
  device->set_tx_gain(std::stod(gain));
}

void usrp::set_tx_rate(std::string &rate) {
  device->set_tx_rate(std::stod(rate));
}

void usrp::set_tx_sample_per_buffer(std::string &spb) {
  tx_sample_per_buffer = std::stoll(spb);
}

void usrp::set_tx_settling_time(std::string &time) {
  tx_settling_time = std::stod(time);
}

void usrp::set_tx_cpu_format(std::string &fmt) {
  tx_cpu_format = fmt;
}

void usrp::set_tx_otw_format(std::string &fmt) {
  tx_otw_format = fmt;
}

void usrp::set_clock_source(std::string &clock_source) {
  // We must check the clock_source or set_clock_source() will throw an
  // exception.
  if (clock_source != "internal" && clock_source != "external" &&
      clock_source != "mimo" && clock_source != "gpsdo")
    return;
  device->set_clock_source(clock_source);
}

std::string usrp::get_device_config(std::string &param) const {
  auto It = getter_setter_pairs.find(param);
  if (It != getter_setter_pairs.cend())
    return It->second.getter();
  return "";
}

void usrp::set_device_config(std::string &param, std::string &val) {
  auto It = getter_setter_pairs.find(param);
  if (It != getter_setter_pairs.cend()) {
    It->second.setter(val);
  }
}

static std::string&
get_from_payload_or(std::unordered_map<std::string, std::string> &m,
		    std::string &&key, std::string &&fallback) {
  auto It = m.find(key);
  if (It != m.cend())
    return It->second;
  return fallback;
}

message_payload
usrp::get_or_set_device_configs(message_payload &&payload) {
  boost::unique_lock<boost::mutex> lk(device_lock);
  for (uint64_t I = 0; I < payload.size(); ++I) {
    if (payload[I].second != "")
      set_device_config(payload[I].first, payload[I].second);
    payload[I].second = get_device_config(payload[I].first);
  }
  return payload;
}

template <typename sample_type>
void usrp::sample_from_file_generic(const std::string &filename) const {
  uhd::stream_args_t stream_args(tx_cpu_format, tx_otw_format);
  uhd::tx_streamer::sptr tx_stream = device->get_tx_stream(stream_args);

  uhd::tx_metadata_t md;
  std::vector<sample_type> buffer(tx_sample_per_buffer);
  std::ifstream ifile(filename.c_str(), std::ifstream::binary);
  if (!ifile) {
    UHD_LOG_ERROR("TX-STREAM",
		  "Cannot open file: " << filename << ".");
    return;
  }

  // Loop until the entire file is transmitted.
  while (!md.end_of_burst) {
    ifile.read((char *)&buffer[0], buffer.size() * sizeof(sample_type));
    size_t tx_samples_num = size_t(ifile.gcount() / sizeof(sample_type));

    md.end_of_burst = ifile.eof();

    const size_t samples_sent = tx_stream->send(&buffer[0], tx_samples_num, md);

    if (samples_sent != tx_samples_num) {
      UHD_LOG_ERROR("TX-STREAM",
		    "The tx_stream timed out sending " << tx_samples_num
		    << " samples (" << samples_sent << " sent).");
    }
  }

  ifile.close();
}

template <typename sample_type>
void usrp::sample_to_file_generic(const std::string &filename) const {
  uhd::stream_args_t stream_args(rx_cpu_format, rx_otw_format);
  // Currently, we won't develop applications with multiple channels.
  // stream_args.channel = rx_channel_nums;
  uhd::rx_streamer::sptr rx_stream = device->get_rx_stream(stream_args);

  // Prepare buffers for received samples and metadata.
  uhd::rx_metadata_t md;
  std::vector<sample_type> buffer(rx_sample_per_buffer);

  std::ofstream ofile(filename.c_str(), std::ofstream::binary);
  bool overflow_message = true;
  double timeout = 0.1f;

  // Setup streaming
  uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);

  // TODO: Add support for delay sampling.
  stream_cmd.stream_now = true;
  stream_cmd.time_spec = uhd::time_spec_t();
  rx_stream->issue_stream_cmd(stream_cmd);

  while (rx_keep_sampling) {
    size_t rx_samples_num = rx_stream->recv(&buffer[0], rx_sample_per_buffer,
					    md, timeout);
    // Small timeout for subsequent receiving.
    timeout = 0.1f;

    if (md.error_code == uhd::rx_metadata_t::ERROR_CODE_OVERFLOW) {
      if (overflow_message) {
	overflow_message = false;
	std::cerr << "Got an overflow indication" << std::endl;
      }
      continue;
    }

    if (md.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE)
      UHD_LOG_ERROR("RX-STREAM", md.strerror());

    ofile.write((const char *)&buffer[0], rx_samples_num * sizeof(sample_type));
  }

  // Shutdown rx.
  uhd::stream_cmd_t
    shutdown_rx_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
  rx_stream->issue_stream_cmd(shutdown_rx_cmd);
  ofile.close();
}

void usrp::sample_to_file(const std::string &filename) const {
  if (rx_cpu_format == "fc64")
    sample_to_file_generic<std::complex<double>>(filename);
  else if (rx_cpu_format == "fc32")
    sample_to_file_generic<std::complex<float>>(filename);
  else if (rx_cpu_format == "sc16")
    sample_to_file_generic<std::complex<short>>(filename);
}

bool usrp::rx_is_sampling_to_file() const {
  boost::unique_lock<boost::mutex> lk(sample_to_file_thread_lock);
  return sample_to_file_thread != nullptr;
}

message usrp::launch_sample_to_file(const message &msg) {
  auto &payload = msg.get_payload_as_map();
  std::string &filename = get_from_payload_or(payload, "filename",
					      /*fallback=*/"");
  if (filename == "")
    return message{msg.get_id(), msg.get_type(),
		   {{"status", STATUS_FAIL},
		    {"explain", "Missing argument: 'filename'"}}};

  std::ofstream ofile(filename.c_str(), std::ofstream::binary);
  if (!ofile)
    return message{msg.get_id(), msg.get_type(),
		   {{"status", STATUS_FAIL},
		    {"explain", "Cannot open file: " + filename}}};
  ofile.close();
  boost::unique_lock<boost::mutex> lk(sample_to_file_thread_lock);
  if (sample_to_file_thread == nullptr) {
    // Spawn a thread to start sample_to_file work.
    // rx_keep_sampling must be protected by the sample_to_file_thread_lock.
    rx_keep_sampling = true;
    sample_to_file_thread =
      threads.create_thread(std::bind(&usrp::sample_to_file, this, filename));
    return message{msg.get_id(), msg.get_type(),
		   {{"status", STATUS_OK}}};
  }
  return message{msg.get_id(), msg.get_type(),
		 {{"status", STATUS_FAIL},
		  {"explain", "Task: 'sample_to_file' is running"}}};
}

void usrp::shutdown_sample_to_file() {
  boost::unique_lock<boost::mutex> lk(sample_to_file_thread_lock);
  // rx_keep_sampling must be protected by the sample_to_file_thread_lock.
  rx_keep_sampling = false;
  if (sample_to_file_thread != nullptr) {
    // Wait this thread util stopping.
    sample_to_file_thread->join();
    threads.remove_thread(sample_to_file_thread);
    sample_to_file_thread = nullptr;
  }
}

message usrp::launch_shutdown_sample_to_file(const message &msg) {
  shutdown_sample_to_file();
  return message{msg.get_id(), msg.get_type(),
		 {{"status", STATUS_OK}}};
}

void usrp::sample_from_file(const std::string &filename) const {
  if (rx_cpu_format == "fc64")
    sample_from_file_generic<std::complex<double>>(filename);
  else if (rx_cpu_format == "fc32")
    sample_from_file_generic<std::complex<float>>(filename);
  else if (rx_cpu_format == "sc16")
    sample_from_file_generic<std::complex<short>>(filename);
}

message usrp::launch_sample_from_file(const message &msg) {
  auto &payload = msg.get_payload_as_map();
  std::string &filename = get_from_payload_or(payload, "filename",
					      /*fallback=*/"");
  if (filename == "")
    return {msg.get_id(), msg.get_type(),
	    {{"status", STATUS_FAIL},
	     {"explain", "Missing argument: 'filename'"}}};
  std::ifstream ifile(filename.c_str(), std::ifstream::binary);
  if (!ifile)
    return {msg.get_id(), msg.get_type(),
	    {{"status", STATUS_FAIL},
	     {"explain", "Cannot open file: " + filename}}};
  ifile.close();
  sample_from_file(filename);
  return {msg.get_id(), msg.get_type(), {{"status", STATUS_OK}}};
}

void usrp::shutdown_sample_from_file() {
  // TODO:
}

void usrp::force_shutdown_all_jobs() {
  shutdown_sample_to_file();
  threads.join_all();
}

message usrp::process_conf_req(message &msg) {
  return message{msg.get_id(), msg.get_type(),
    get_or_set_device_configs(msg.get_payload())};
}

message usrp::process_work_req(message &msg) {
  auto &payload = msg.get_payload_as_map();
  if (payload.size() == 0)
    return message{msg.get_id(), msg.get_type(),
		   {{"status", STATUS_FAIL},
		    {"explain", "The length of message.payload is 0"}}};

  std::string &task_name = get_from_payload_or(payload, "task", "");
  if (task_name == "")
    return message{msg.get_id(), msg.get_type(),
		   {{"status", STATUS_FAIL},
		    {"explain", "Missing argument: 'task'"}}};

  auto It = task_map.find(task_name);
  if (It == task_map.cend())
    return message{msg.get_id(), msg.get_type(),
		   {{"status", STATUS_FAIL},
		    {"explain", "Unknown task: " + task_name}}};
  return It->second(msg);
}

message usrp::handle_request(message &msg) {
  if (msg.get_type() == mt::MT_CONF) {
    return process_conf_req(msg);
  } else if (msg.get_type() == mt::MT_WORK) {
    return process_work_req(msg);
  }
  return message{msg.get_id(), msg.get_type(), {}};
}

void usrp::zmq_server_run() {
  zmq::context_t ctx;
  zmq::socket_t socket(ctx, zmq::socket_type::rep);
  socket.bind(zmq_bind);

  zmq::pollitem_t items[] = {
    { socket, 0, ZMQ_POLLIN, 0 },
  };

  for (;;) {
    zmq::poll(&items[0], 1, 1000);

    if (items[0].revents & ZMQ_POLLIN) {
      zmq::message_t req;
      socket.recv(req, zmq::recv_flags::none);
      std::string req_str = req.to_string();
      message msg = message::from_json(req_str);
      message response = handle_request(msg);
      socket.send(zmq::buffer(message::to_json(response)),
		  zmq::send_flags::none);
    }
  }
}

usrp::~usrp() {
  force_shutdown_all_jobs();
}
