#include <generic_radio_data_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(Generic_radioDataProvider,"GENERIC_RADIO_PROVIDER");

    extern ItcLogger::Logger *sim_logger;

    Generic_radioDataProvider::Generic_radioDataProvider(const boost::property_tree::ptree& config) : SimIDataProvider(config)
    {
        sim_logger->trace("Generic_radioDataProvider::Generic_radioDataProvider:  Constructor executed");
    }

    boost::shared_ptr<SimIDataPoint> Generic_radioDataProvider::get_data_point(void) const
    {
        sim_logger->trace("Generic_radioDataProvider::get_data_point:  Executed");

        /* Prepare the specific data */
        SimIDataPoint *dp = new Generic_radioDataPoint();

        return boost::shared_ptr<SimIDataPoint>(dp);
    }
}
