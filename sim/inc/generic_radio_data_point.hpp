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
        Generic_radioDataPoint(int16_t uplink, std::string uplink_close_criteria, double uplink_cnr_limit,
            int16_t downlink, std::string downlink_close_criteria, double downlink_cnr_limit, const boost::shared_ptr<Sim42DataPoint> dp);
        Generic_radioDataPoint(void);

        /* Accessors */
        /* Provide the hardware model a way to get the specific data out of the data point */
        std::string to_string(void) const;
        bool        get_uplink_occulted(void) const {parse_data_point(); return _uplink_occulted;}
        bool        get_uplink_delay(void) const {parse_data_point(); return _uplink_delay;}
        double      get_uplink_cnr(void) const {parse_data_point(); return _uplink_cnr;}
        bool        get_downlink_occulted(void) const {parse_data_point(); return _downlink_occulted;}
        bool        get_downlink_delay(void) const {parse_data_point(); return _downlink_delay;}
        double      get_downlink_cnr(void) const {parse_data_point(); return _downlink_cnr;}
    
        std::string get_uplink_close_criteria(void) {return _uplink_close_criteria;}
        double      get_uplink_cnr_limit(void) {return _uplink_cnr_limit;}
        std::string get_downlink_close_criteria(void) {return _downlink_close_criteria;}
        double      get_downlink_cnr_limit(void) {return _downlink_cnr_limit;}
    private:
        /* Disallow these */
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
        std::string    _uplink_close_criteria;
        double         _uplink_cnr_limit;
        int16_t        _downlink;
        std::string    _downlink_close_criteria;
        double         _downlink_cnr_limit;
        mutable bool   _not_parsed;
        mutable bool   _uplink_occulted;
        mutable double _uplink_delay;
        mutable double _uplink_cnr;
        mutable bool   _downlink_occulted;
        mutable double _downlink_delay;
        mutable double _downlink_cnr;
    };
}

#endif
