#ifndef _MESSAGE_HPP_
#define _MESSAGE_HPP_

#include <string>
#include <vector>
#include <boost/optional.hpp>

constexpr const char * MSG_ID          = "id";
constexpr const char * MSG_TYPE        = "type";
constexpr const char * MSG_PAYLOAD     = "payload";
constexpr const char * MSG_PAYLOAD_KEY = "key";
constexpr const char * MSG_PAYLOAD_VAL = "val";

typedef enum {
  // Invalid message.
  INVALID,
  // Reset USRP configuration.
  CONF_RESET,
  // Set USRP configuration.
  CONF_SET,
  // Get USRP configuration.
  CONF_GET,
} message_type;

// The payload is an array of key-value pairs.
using message_payload = std::vector<std::pair<std::string, std::string>>;

class message {
private:
  uint64_t id;
  message_type type;
  message_payload payload;
public:
  // Disable default message ctor.
  message() = delete;
  message(uint64_t id, message_type type, message_payload payload);
  uint64_t get_id() const;
  message_type get_type() const;
  message_payload get_payload() const;

  static message from_json(std::string &json);
  static std::string to_json(message &msg);
};

#endif
