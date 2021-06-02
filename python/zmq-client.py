import zmq

context = zmq.Context()

socket = context.socket(zmq.REQ)
socket.connect("tcp://localhost:5555")

#  Do 10 requests, waiting each time for a response
for I in range(10):
    socket.send(b"""
{
  "hello": "world",
  "type": "CONF_GET",
  "payload": [
    {"key": "hello", "val": "world!"},
    {"key": "pp_string", "val": ""},
    {"key": "rx_antenna", "val": ""},
    {"key": "rx_bandwidth", "val": ""},
    {"key": "rx_freq", "val": ""},
    {"key": "rx_gain", "val": ""},
    {"key": "rx_rate", "val": ""},
    {"key": "tx_antenna", "val": ""},
    {"key": "tx_bandwidth", "val": ""},
    {"key": "tx_freq", "val": ""},
    {"key": "tx_gain", "val": ""},
    {"key": "tx_rate", "val": ""}
   ]
}""")

    #  Get the reply.
    message = socket.recv()
    print(str(message.decode()))

    socket.send(b"""
{
  "hello": "world",
  "type": "CONF_SET",
  "payload": [
    {"key": "hello", "val": "world!"},
    {"key": "pp_string", "val": ""},
    {"key": "rx_antenna", "val": "TX/RX"},
    {"key": "rx_bandwidth", "val": "10000"},
    {"key": "rx_freq", "val": "20000"},
    {"key": "rx_gain", "val": "30000"},
    {"key": "rx_rate", "val": "40000"},
    {"key": "tx_antenna", "val": "TX/RX"},
    {"key": "tx_bandwidth", "val": "100000"},
    {"key": "tx_freq", "val": "200000"},
    {"key": "tx_gain", "val": "300000"},
    {"key": "tx_rate", "val": "200000"}
   ]
}""")

    #  Get the reply.
    message = socket.recv()
    print(str(message.decode()))
