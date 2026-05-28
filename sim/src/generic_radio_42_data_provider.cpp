#include <generic_radio_42_data_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(Generic_radio42DataProvider,"GENERIC_RADIO_42_PROVIDER");

    extern ItcLogger::Logger *sim_logger;

    Generic_radio42DataProvider::Generic_radio42DataProvider(const boost::property_tree::ptree& config) : SimData42SocketProvider(config)
    {
        sim_logger->trace("Generic_radio42DataProvider::Generic_radio42DataProvider:  Constructor executed");

        connect_reader_thread_as_42_socket_client(
            config.get("simulator.hardware-model.data-provider.hostname", "localhost"),
            config.get("simulator.hardware-model.data-provider.port", 4242) );

        _comm_uplink = config.get("simulator.hardware-model.data-provider.comm-uplink", 0);
        _uplink_close_criteria = config.get("simulator.hardware-model.data-provider.uplink-close-criteria", "none");
        _uplink_cnr_limit = config.get("simulator.hardware-model.data-provider.uplink-cnr-limit", 15);
        _uplink_delay_on = config.get("simulator.hardware-model.data-provider.uplink-delay-on", false);
        _comm_downlink = config.get("simulator.hardware-model.data-provider.comm-downlink", 1);
        _downlink_close_criteria = config.get("simulator.hardware-model.data-provider.downlink-close-criteria", "none");
        _downlink_cnr_limit = config.get("simulator.hardware-model.data-provider.downlink-cnr-limit", 15);
        _downlink_delay_on = config.get("simulator.hardware-model.data-provider.downlink-delay-on", false);
    }

    boost::shared_ptr<SimIDataPoint> Generic_radio42DataProvider::get_data_point(void) const
    {
        sim_logger->trace("Generic_radio42DataProvider::get_data_point:  Executed");

        /* Get the 42 data */
        const boost::shared_ptr<Sim42DataPoint> dp42 = boost::dynamic_pointer_cast<Sim42DataPoint>(SimData42SocketProvider::get_data_point());

        /* Prepare the specific data */
        SimIDataPoint *dp = new Generic_radioDataPoint(_comm_uplink, _uplink_close_criteria, _uplink_cnr_limit, _uplink_delay_on,
            _comm_downlink, _downlink_close_criteria, _downlink_cnr_limit, _downlink_delay_on, dp42);

        return boost::shared_ptr<SimIDataPoint>(dp);
    }
}
