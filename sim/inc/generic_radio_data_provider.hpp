#ifndef NOS3_GENERIC_RADIODATAPROVIDER_HPP
#define NOS3_GENERIC_RADIODATAPROVIDER_HPP

#include <boost/property_tree/xml_parser.hpp>
#include <ItcLogger/Logger.hpp>
#include <generic_radio_data_point.hpp>
#include <sim_i_data_provider.hpp>

namespace Nos3
{
    class Generic_radioDataProvider : public SimIDataProvider
    {
    public:
        /* Constructors */
        Generic_radioDataProvider(const boost::property_tree::ptree& config);

        /* Accessors */
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        /* Disallow these */
        ~Generic_radioDataProvider(void) {};
        Generic_radioDataProvider& operator=(const Generic_radioDataProvider&) {return *this;};

        mutable double _request_count;
    };
}

#endif
