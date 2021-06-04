import zmq
import time
import json

context = zmq.Context()

socket = context.socket(zmq.REQ)
socket.connect("tcp://localhost:5555")

#  Do 10 requests, waiting each time for a response
for I in range(1):
    socket.send(b"""
{
  "hello": "world",
  "type": "CONF",
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
    {"key": "tx_rate", "val": "12837"}
   ]
}""")

    #  Get the reply.
    message = socket.recv()
    print(str(message.decode()), flush=True)

    socket.send(b"""
{
  "hello": "world",
  "type": "CONF",
  "payload": [
    {"key": "hello", "val": "world!"},
    {"key": "pp_string", "val": ""},
    {"key": "rx_antenna", "val": "TX/RX"},
    {"key": "rx_bandwidth", "val": "10000"},
    {"key": "rx_freq", "val": "20000"},
    {"key": "rx_gain", "val": "30000"},
    {"key": "rx_rate", "val": "2000000"},
    {"key": "tx_antenna", "val": "TX/RX"},
    {"key": "tx_bandwidth", "val": "100000"},
    {"key": "tx_freq", "val": "200000"},
    {"key": "tx_gain", "val": "300000"},
    {"key": "tx_rate", "val": "2000000"},
    {"key": "rx_sample_per_buffer", "val": "300000"},
    {"key": "tx_sample_per_buffer", "val": "300000"},
    {"key": "clock_source", "val": "internal"}
   ]
}""")

    # Get the reply.
    message = socket.recv()
    print(str(message.decode()), flush=True)

    socket.send(b"""
{
  "id": "world",
  "type": "WORK",
  "payload": [
    {"key": "funcname", "val" : "launch_sample_to_file"},
    {"key": "filename", "val" : "/tmp/mydata.data"},
    {"key": "rx_sample_per_buffer", "val": "300000"},
    {"key": "tx_sample_per_buffer", "val": "300000"}
   ]
}""")

    # Get the reply.
    message = socket.recv()
    print(str(message.decode()), flush=True)

    time.sleep(1)
    socket.send(b"""
{
  "id": "world",
  "type": "WORK",
  "payload": [
    {"key": "funcname", "val" : "shutdown_sample_to_file"},
    {"key": "filename", "val" : "/tmp/mydata.data"},
    {"key": "rx_sample_per_buffer", "val": "300000"},
    {"key": "tx_sample_per_buffer", "val": "300000"}
   ]
}""")

    # Get the reply.
    message = socket.recv()
    obj = json.loads(str(message.decode()))
    print(obj)
