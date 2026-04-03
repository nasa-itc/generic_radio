#include <ItcLogger/Logger.hpp>
#include <generic_radio_data_point.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    Generic_radioDataPoint::Generic_radioDataPoint(int16_t uplink, int16_t downlink, const boost::shared_ptr<Sim42DataPoint> dp) : 
        _dp(*dp), _uplink(uplink), _downlink(downlink), _not_parsed(true)
    {
        sim_logger->trace("Generic_radioDataPoint::Generic_radioDataPoint:  42 Constructor executed");

        /* Initialize data */
        _uplink_occulted   = false;
        _uplink_delay      = 0.0;
        _downlink_occulted = false;
        _downlink_delay    = 0.0;
    }

    Generic_radioDataPoint::Generic_radioDataPoint(bool uplink_occulted, double uplink_delay, bool downlink_occulted, double downlink_delay) : 
        _not_parsed(false), _uplink_occulted(uplink_occulted), _uplink_delay(uplink_delay), _downlink_occulted(downlink_occulted), _downlink_delay(downlink_delay)
    {
    }

    void Generic_radioDataPoint::do_parsing(void) const
    {
        try {
            /*
            ** Declare 42 telemetry string prefix
            ** 42 variables defined in `42/Include/42types.h`
            ** 42 data stream defined in `42/Source/IPC/SimWriteToSocket.c`
            */
            std::string uplink_occulted_key;
            uplink_occulted_key.append("CommLink[").append(std::to_string(_uplink)).append("].PathIsOcculted"); // CommLink[N].PathIsOcculted
            std::string uplink_delay_key;
            uplink_delay_key.append("CommLink[").append(std::to_string(_uplink)).append("].Delay"); // CommLink[N].Delay

            /* Parse 42 telemetry */
            std::string uplink_occulted_value = _dp.get_value_for_key(uplink_occulted_key);
            std::string uplink_delay_value = _dp.get_value_for_key(uplink_delay_key);

            _uplink_occulted = (uplink_occulted_value == "1");
            _uplink_delay = std::stof(uplink_delay_value);

            std::string downlink_occulted_key;
            downlink_occulted_key.append("CommLink[").append(std::to_string(_downlink)).append("].PathIsOcculted"); // CommLink[N].PathIsOcculted
            std::string downlink_delay_key;
            downlink_delay_key.append("CommLink[").append(std::to_string(_downlink)).append("].Delay"); // CommLink[N].Delay

            /* Parse 42 telemetry */
            std::string downlink_occulted_value = _dp.get_value_for_key(downlink_occulted_key);
            std::string downlink_delay_value = _dp.get_value_for_key(downlink_delay_key);

            _downlink_occulted = (downlink_occulted_value == "1");
            _downlink_delay = std::stof(downlink_delay_value);

            /* Debug print */
            sim_logger->trace("Generic_radioDataPoint::do_parsing:  Parsed uplink occulted = %s, uplink delay = %f, downlink occulted = %s, downlink delay = %f", 
                _uplink_occulted?"True":"False", _uplink_delay, _downlink_occulted?"True":"False", _downlink_delay);

            _not_parsed = false;
        } 
        catch(const std::exception& e) 
        {
            /* Report error */
            sim_logger->error("Generic_radioDataPoint::do_parsing:  Parsing exception %s", e.what());
        }
    }

    /* Used for printing a representation of the data point */
    std::string Generic_radioDataPoint::to_string(void) const
    {
        sim_logger->trace("Generic_radioDataPoint::to_string:  Executed");
        
        std::stringstream ss;

        ss << std::fixed << std::setfill(' ');
        ss << std::setprecision(std::numeric_limits<double>::digits10); /* Full double precision */
        ss << "Generic_radio Data Point:   Uplink occulted: ";
        ss << (_uplink_occulted ? "True" : "False");
        ss << ", Uplink delay" << _uplink_delay;
        ss << ", Downlink occulted: ";
        ss << (_downlink_occulted ? "True" : "False");
        ss << ", Downlink delay" << _downlink_delay;

        return ss.str();
    }
} /* namespace Nos3 */
