#ifndef NOS3_GENERIC_RADIODATAPOINT_HPP
#define NOS3_GENERIC_RADIODATAPOINT_HPP

#include <boost/shared_ptr.hpp>
#include <sim_42data_point.hpp>

namespace Nos3
{
    /* Standard for a data point used transfer data between a data provider and a hardware model */
    class Generic_radioDataPoint : public SimIDataPoint
    {
    public:
        /* Constructors */
        Generic_radioDataPoint(int16_t uplink, int16_t downlink, const boost::shared_ptr<Sim42DataPoint> dp);
        Generic_radioDataPoint(bool uplink_occulted, double uplink_delay, bool downlink_occulted, double downlink_delay);

        /* Accessors */
        /* Provide the hardware model a way to get the specific data out of the data point */
        std::string to_string(void) const;
    
    private:
        /* Disallow these */
        Generic_radioDataPoint(void) {};
        Generic_radioDataPoint(const Generic_radioDataPoint&) {};
        ~Generic_radioDataPoint(void) {};

        /// @name Private mutators
        //@{
        inline void parse_data_point(void) const {if (_not_parsed) do_parsing();}
        void do_parsing(void) const;
        //@}

        /* Specific data you need to get from the data provider to the hardware model */
        /* You only get to this data through the accessors above */
        mutable Sim42DataPoint _dp;
        int16_t        _uplink;
        int16_t        _downlink;
        mutable bool   _not_parsed;
        mutable bool   _uplink_occulted;
        mutable double _uplink_delay;
        mutable bool   _downlink_occulted;
        mutable double _downlink_delay;
    };
}

#endif
