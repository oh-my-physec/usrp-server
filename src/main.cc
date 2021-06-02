/* Suppress Boost deprecated header warnings. */
#define BOOST_ALLOW_DEPRECATED_HEADERS

#include <uhd/utils/thread.hpp>
#include <uhd/utils/safe_main.hpp>
#include <boost/thread.hpp>
#include "options.hpp"
#include "usrp.hpp"

static options op;

int UHD_SAFE_MAIN(int argc, char **argv) {
  op.parse_options(argc, argv);
  if (!op.check_options())
    return 1;

  usrp my_usrp(op.device_args, op.zmq_bind);
  boost::thread usrp_zmq_server(&usrp::zmq_server_run, &my_usrp);
  usrp_zmq_server.join();
  return 0;
}
