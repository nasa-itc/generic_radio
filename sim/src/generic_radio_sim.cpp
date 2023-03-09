#include <ItcLogger/Logger.hpp>
#include <sim_config.hpp>

namespace Nos3
{
    ItcLogger::Logger *sim_logger;
}

int
main(int argc, char *argv[])
{
    /* This is the ONLY simulator specific line! */
    std::string simulator_name = "generic_radio_sim"; 

    /* Determine the configuration and run the simulator */
    Nos3::SimConfig sc(argc, argv);
    Nos3::sim_logger->info("main:  %s simulator starting",
        simulator_name.c_str());
    sc.run_simulator(simulator_name);
    Nos3::sim_logger->info("main:  %s simulator terminating",
        simulator_name.c_str());
}
