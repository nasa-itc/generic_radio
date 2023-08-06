#ifndef NOS3_GENERIC_RADIO42DATAPROVIDER_HPP
#define NOS3_GENERIC_RADIO42DATAPROVIDER_HPP

#include <boost/property_tree/ptree.hpp>
#include <ItcLogger/Logger.hpp>
#include <generic_radio_data_point.hpp>
#include <sim_data_42socket_provider.hpp>

namespace Nos3
{
    /* Standard for a 42 data provider */
    class Generic_radio42DataProvider : public SimData42SocketProvider
    {
    public:
        /* Constructors */
        Generic_radio42DataProvider(const boost::property_tree::ptree& config);

        /* Accessors */
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        /* Disallow these */
        ~Generic_radio42DataProvider(void) {};
        Generic_radio42DataProvider& operator=(const Generic_radio42DataProvider&) {return *this;};

        int16_t _sc;  /* Which spacecraft number to parse out of 42 data */
    };
}

#endif
