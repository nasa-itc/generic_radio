#include <generic_radio_hardware_model.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(Generic_radioHardwareModel,"GENERIC_RADIO");

    extern ItcLogger::Logger *sim_logger;

    Generic_radioHardwareModel::Generic_radioHardwareModel(const boost::property_tree::ptree& config) : SimIHardwareModel(config), _enabled(0), _count(0), _config(0), _prox_signal(0)
    {
        /* Get the NOS engine connection string */
        std::string connection_string = config.get("common.nos-connection-string", "tcp://0.0.0.0:12001"); 
        sim_logger->info("Generic_radioHardwareModel::Generic_radioHardwareModel:  NOS Engine connection string: %s.", connection_string.c_str());

        /* Get a data provider */
        std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "GENERIC_RADIO_PROVIDER");
        _generic_radio_dp = SimDataProviderFactory::Instance().Create(dp_name, config);
        sim_logger->info("Generic_radioHardwareModel::Generic_radioHardwareModel:  Data provider %s created.", dp_name.c_str());

        /* Get on a protocol bus */
        /* Note: Initialized defaults in case value not found in config file */
        _fsw_ci.ip = "0.0.0.0";
        _fsw_ci.port = TX_FSW_PORT;
        _fsw_to.ip = "0.0.0.0";
        _fsw_to.port = RX_FSW_PORT;
        _fsw_radio.ip = "0.0.0.0";
        _fsw_radio.port = 5015;
        _radio_cmd.ip = "0.0.0.0";
        _radio_cmd.port = 5014;
        _gsw_cmd.ip = "0.0.0.0";
        _gsw_cmd.port = 6010;
        _gsw_tlm.ip = "0.0.0.0";
        _gsw_tlm.port = 6011;

        _gsw2_cmd.ip = "0.0.0.0";
        _gsw2_cmd.port = 8013;
        _gsw2_tlm.ip = "0.0.0.0";
        _gsw2_tlm.port = 8011;

        _prox_rcv.ip = "0.0.0.0";
        _prox_rcv.port = 7012;
        _prox_fsw.ip = "0.0.0.0";
        _prox_fsw.port = 7010;
        _prox_fwd.ip = "0.0.0.0";
        _prox_fwd.port = 7011;
        _prox_dest.ip = "0.0.0.0";
        _prox_dest.port = 7013;

        // int tcp_true = TCP_GROUND; //if 1, use tcp with cryptolib, if 0 use udp.
        char *tcp_env = getenv("TCP_GROUND");
        int tcp_true = tcp_env ? atoi(tcp_env): 1;
        // int multi_gds = 1;
        char *multi_gds_env = getenv("MULTI_GDS");
        int multi_gds = multi_gds_env ? atoi(multi_gds_env) : 0;


        // sleep(5);

        if (config.get_child_optional("simulator.hardware-model.connections")) 
        {
            /* Loop through the connections for hardware model */
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections"))
            {
                /* v.second is the child tree (v.first is the name of the child) */
                if (v.second.get("name", "").compare("fsw") == 0)
                {
                    /* Configuration found */
                    _fsw_ci.ip = v.second.get("ip", _fsw_ci.ip);
                    _fsw_ci.port = v.second.get("ci-port", _fsw_ci.port);
                    
                    _fsw_to.port = v.second.get("to-port", _fsw_to.port);

                    _fsw_radio.ip = v.second.get("ip", _fsw_radio.ip);
                    _fsw_radio.port = v.second.get("radio-port", _fsw_radio.port);

                    _prox_fsw.ip = v.second.get("ip", _prox_fsw.ip);
                }

                if (v.second.get("name", "").compare("radio") == 0)
                {
                    /* Configuration found */
                    _radio_cmd.ip = v.second.get("ip", _radio_cmd.ip);
                    _radio_cmd.port = v.second.get("cmd-port", _radio_cmd.port);
                    
                    _fsw_to.ip = v.second.get("ip", _fsw_to.ip);
                    _gsw_cmd.ip = v.second.get("ip", _gsw_cmd.ip);;

                    _prox_rcv.ip = v.second.get("ip", _prox_rcv.ip);
                    _prox_fwd.ip = v.second.get("ip", _prox_fwd.ip);
                }

                if (v.second.get("name", "").compare("gsw") == 0)
                {
                    /* Configuration found */
                    
                    _gsw_cmd.port = v.second.get("cmd-port", _gsw_cmd.port);
                    
                    _gsw_tlm.ip = v.second.get("ip", _gsw_tlm.ip);
                    _gsw_tlm.port = v.second.get("tlm-port", _gsw_tlm.port);
                }

                if (v.second.get("name", "").compare("gsw2") == 0)
                {
                    /* Configuration found */
                    _gsw2_cmd.port = v.second.get("cmd-port", _gsw2_cmd.port);
                    
                    _gsw2_tlm.ip = v.second.get("ip", _gsw2_tlm.ip);
                    _gsw2_tlm.port = v.second.get("tlm-port", _gsw2_tlm.port);
                }

                if (v.second.get("name", "").compare("prox") == 0)
                {
                    // /* Configuration found */
                    // _prox_rcv.port = v.second.get("rcv-port", _prox_rcv.port);
                    // _prox_fsw.port = v.second.get("fsw-port", _prox_fsw.port);
                    // _prox_fwd.port = v.second.get("fwd-port", _prox_fwd.port);
                    // _prox_dest.ip = v.second.get("ip", _prox_dest.ip);
                    // _prox_dest.port = v.second.get("dest-port", _prox_dest.port);
                    /* Configuration found */
                    _prox_rcv.ip = v.second.get("ip", _prox_rcv.ip);
                    _prox_rcv.port = v.second.get("rcv-port", _prox_rcv.port);
                    
                    _prox_fsw.port = v.second.get("fsw-port", _prox_fsw.port);
                    
                    _prox_fwd.ip = v.second.get("ip", _prox_fwd.ip);     
                    _prox_fwd.port = v.second.get("fwd-port", _prox_fwd.port);
                    
                    _prox_dest.ip = v.second.get("ip", _prox_dest.ip);
                    _prox_dest.port = v.second.get("dest-port", _prox_dest.port);
                }
            }
        }

        /* Get on the command bus*/
        std::string time_bus_name = "command";
        if (config.get_child_optional("hardware-model.connections")) 
        {
            /* Loop through the connections for the hardware model */
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("hardware-model.connections"))
            {
                /* v.first is the name of the child */
                /* v.second is the child tree */
                if (v.second.get("type", "").compare("time") == 0) // 
                {
                    time_bus_name = v.second.get("bus-name", "command");
                    /* Found it... don't need to go through any more items*/
                    break; 
                }
            }
        }
        _time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));
        _time_bus->add_time_tick_callback(std::bind(&Generic_radioHardwareModel::process_forward_loop_message_queue, this, std::placeholders::_1));
        _time_bus->add_time_tick_callback(std::bind(&Generic_radioHardwareModel::process_tcp_forward_loop_message_queue, this, std::placeholders::_1));
        _time_bus->add_time_tick_callback(std::bind(&Generic_radioHardwareModel::process_forward_loop_multi_message_queue, this, std::placeholders::_1));
        sim_logger->info("Generic_radioHardwareModel::Generic_radioHardwareModel:  Now on time bus named %s.", time_bus_name.c_str());

        /* Forwarding threads */
        if(tcp_true == 1)
        {
            //TCP with Cryptolib
            new std::thread(&Generic_radioHardwareModel::tcp_forward_loop, this, &_gsw_cmd, &_fsw_ci, 1); //fsw_ci needs to be udp, tcp to udp, rcv_sock is gsw_cmd (8010 5010)
            new std::thread(&Generic_radioHardwareModel::tcp_forward_loop, this, &_fsw_to, &_gsw_tlm, 0); //forwarding udp data to tcp, fsw_to needs to be udp, rcv_sock is fsw_to (8011 5011)
        }
        else if (multi_gds == 1)
        {
            // UDP with cryptolib

            // One thread to read FSW telemetry ONCE and forward to BOTH GSWs
            new std::thread(&Generic_radioHardwareModel::forward_loop_multi, this, &_fsw_to, &_gsw_tlm, &_gsw2_tlm);
            
            // Two separate threads to listen for incoming commands from each GSW and forward to FSW
            new std::thread(&Generic_radioHardwareModel::forward_loop, this, &_gsw_cmd, &_fsw_ci, 1);
            // new std::thread(&Generic_radioHardwareModel::forward_loop, this, &_gsw2_cmd, &_fsw_ci); //gsw2 not needed 8010 already listening at 0.0.0.0
        }
        else
        {
            //UDP with cryptolib
            new std::thread(&Generic_radioHardwareModel::forward_loop, this, &_gsw_cmd, &_fsw_ci, 1);
            new std::thread(&Generic_radioHardwareModel::forward_loop, this, &_fsw_to, &_gsw_tlm, 0);
        }

        new std::thread(&Generic_radioHardwareModel::forward_loop, this, &_prox_rcv, &_prox_fsw, -1);
        new std::thread(&Generic_radioHardwareModel::forward_loop, this, &_prox_fwd, &_prox_dest, -1);

        /* Construction complete */
        sim_logger->info("Generic_radioHardwareModel::Generic_radioHardwareModel:  Construction complete.");
    }


    Generic_radioHardwareModel::~Generic_radioHardwareModel(void)
    {        
        /* Close the protocol bus */
        close(_fsw_radio.sockfd);
        close(_radio_cmd.sockfd);

        /* Clean up the data provider */
        delete _generic_radio_dp;
        _generic_radio_dp = nullptr;

        /* The bus will clean up the time node */
    }


    void Generic_radioHardwareModel::run(void)
    {
        int status;
        uint8_t sock_buffer[256];
        size_t bytes_recvd;

        struct sockaddr_in radio_addr;
        int sockaddr_size = sizeof(struct sockaddr_in);

        udp_init(&_radio_cmd);

        sim_logger->info("Generic_radioHardwareModel::run: %s:%d to %s:%d", _radio_cmd.ip.c_str(), _radio_cmd.port, _fsw_radio.ip.c_str(), _fsw_radio.port);

        while(_keep_running)
        {
            bytes_recvd = 0;

            /* Receive */
            status = recvfrom(_radio_cmd.sockfd, sock_buffer, sizeof(sock_buffer), 0, (sockaddr*) &radio_addr, (socklen_t*) &sockaddr_size);
            if (status != -1)
            {
                bytes_recvd = status;

                /* Debug print */
                sim_logger->trace("Generic_radioHardwareModel::run: %s:%d received %ld bytes", _radio_cmd.ip.c_str(), _radio_cmd.port, bytes_recvd);

                /* Process Command */
                process_radio_command(sock_buffer, bytes_recvd);
            }        
        }
        close(_radio_cmd.sockfd);
    }


    /* Automagically set up by the base class to be called */
    void Generic_radioHardwareModel::command_callback(NosEngine::Common::Message msg)
    {
        /* Get the data out of the message */
        NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
        sim_logger->info("Generic_radioHardwareModel::command_callback:  Received command: %s.", dbf.data);

        /* Do something with the data */
        std::string command = dbf.data;
        std::string response = "Generic_radioHardwareModel::command_callback:  INVALID COMMAND! (Try HELP)";
        boost::to_upper(command);
        if (command.compare(0,4,"HELP") == 0) 
        {
            response = "Generic_radioHardwareModel::command_callback: Valid commands are HELP, ENABLE, DISABLE, or STOP";
        }
        else if (command.compare(0,6,"ENABLE") == 0) 
        {
            _enabled = GENERIC_RADIO_SIM_SUCCESS;
            response = "Generic_radioHardwareModel::command_callback:  Enabled";
            _keep_running = true;
        }
        else if (command.compare(0,7,"DISABLE") == 0) 
        {
            _enabled = GENERIC_RADIO_SIM_ERROR;
            _count = 0;
            _config = 0;
            _prox_signal = 0;
            response = "Generic_radioHardwareModel::command_callback:  Disabled";
        }
        else if (command.compare(0,4,"STOP") == 0) 
        {
            response = "Generic_radioHardwareModel::command_callback:  Stopping";
        }

        /* Send a reply */
        sim_logger->info("Generic_radioHardwareModel::command_callback:  Sending reply: %s.", response.c_str());
        _command_node->send_reply_message_async(msg, response.size(), response.c_str());
    }


    int32_t Generic_radioHardwareModel::host_to_ip(const char * hostname, char* ip)
    {
        struct addrinfo hints, *res, *p;
        int status;
        void *addr;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET; // Uses IPV4 only.  AF_UNSPEC for IPV6 Support
        hints.ai_socktype = SOCK_STREAM;

        if ((status = getaddrinfo(hostname, NULL, &hints, &res)) != 0)
        {
            return 1;
        }

        for (p = res; p != NULL; p = p->ai_next)
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);

            // Convert IP to String
            if (inet_ntop(p->ai_family, addr, ip, INET_ADDRSTRLEN) == NULL)
            {
                freeaddrinfo(res);
                return 1;
            }

            freeaddrinfo(res);
            return 0; // IP Found
        }
        freeaddrinfo(res);
        return 1; // IP NOT Found
    }

    int32_t Generic_radioHardwareModel::udp_init(udp_info_t* sock)
    {
        int status = 0;
        int optval;
        socklen_t optlen;

        /* Create */
        sock->sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if(sock->sockfd == -1)
        {
            sim_logger->info("udp_init:  Socket create error with ip %s, and port %d", sock->ip.c_str(), sock->port);
        }

        /* Determine IP */
        struct sockaddr_in saddr;
        saddr.sin_family = AF_INET;
        
        if(sock->ip == "0.0.0.0")
        {
            // Bind to all interfaces
            saddr.sin_addr.s_addr = INADDR_ANY;
            sim_logger->info("udp_init - Binding to ALL interfaces (0.0.0.0); Port = %d", sock->port);
        }
        else if(inet_addr(sock->ip.c_str()) != INADDR_NONE)
        {
            // Valid IP address
            saddr.sin_addr.s_addr = inet_addr(sock->ip.c_str());
            sim_logger->info("udp_init - Binding to IP = %s; Port = %d", sock->ip.c_str(), sock->port);
        }
        else
        {
            // Reosolving Hostname
            char ip[16];
            int check = host_to_ip(sock->ip.c_str(), ip);
            sim_logger->info("udp_init - Initial = %s; Updated = %s; Port = %d", sock->ip.c_str(), ip, sock->port);
            if(check == 0)
            {
                saddr.sin_addr.s_addr = inet_addr(ip);
            }
            else
            {
                sim_logger->error("udp_init - Failed to resolve hostname: %s", sock->ip.c_str());
                
                status = GENERIC_RADIO_SIM_ERROR;

                return status;
            }
        }
        saddr.sin_port = htons(sock->port);

        /* Bind */
        if (sock->port != TX_FSW_PORT)
        {
            status = bind(sock->sockfd, (struct sockaddr *) &saddr, sizeof(saddr));
            if (status != 0)
            {
                sim_logger->error("udp_init:  Socket bind error with ip %s, and port %d", sock->ip.c_str(), sock->port);

                status = GENERIC_RADIO_SIM_ERROR;
                return status;
            }
        }

        /* Keep Alive */
        optval = 1;
        optlen = sizeof(optval);
        setsockopt(sock->sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);    

        return status;
    }
    

    int32_t Generic_radioHardwareModel::tcp_init(udp_info_t* sock)
    {
        int status;
        int optval;
        socklen_t optlen;

        // Create socket
        sock->sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock->sockfd == -1)
        {
            sim_logger->error("tcp_init: Socket create error with ip %s, port %d", sock->ip.c_str(), sock->port);
            return GENERIC_RADIO_SIM_ERROR;
        }

        // Fill in server address (where this socket listens)
        struct sockaddr_in saddr;
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(sock->port);

        if (inet_addr(sock->ip.c_str()) != INADDR_NONE)
        {
            saddr.sin_addr.s_addr = inet_addr(sock->ip.c_str());
        }
        else
        {
            char ip[16] = {0};
            if (host_to_ip(sock->ip.c_str(), ip) == 0)
            {
                saddr.sin_addr.s_addr = inet_addr(ip);
            }
            else
            {
                sim_logger->error("tcp_init: Invalid IP resolution for %s", sock->ip.c_str());
                return GENERIC_RADIO_SIM_ERROR;
            }
        }

        // Bind to local IP and port
        status = bind(sock->sockfd, (struct sockaddr*)&saddr, sizeof(saddr));
        if (status != 0)
        {
            sim_logger->error("tcp_init: Bind failed on %s:%d", sock->ip.c_str(), sock->port);
            return GENERIC_RADIO_SIM_ERROR;
        }

        sim_logger->info("tcp_init: Bound to %s:%d", sock->ip.c_str(), sock->port);

        // Listen for connections
        status = listen(sock->sockfd, 5);
        if (status != 0)
        {
            sim_logger->error("tcp_init: Listen failed on port %d", sock->port);
            return GENERIC_RADIO_SIM_ERROR;
        }

        sim_logger->info("Waiting for TCP connection...\n");

        // Accept incoming connection
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int clientfd = accept(sock->sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (clientfd < 0)
        {
            sim_logger->error("tcp_init: Accept failed");
            return GENERIC_RADIO_SIM_ERROR;
        }

        // Extract and log client info
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        int client_port = ntohs(client_addr.sin_port);

        sim_logger->info("tcp_init: Connection accepted from %s:%d", client_ip, client_port);

        // Enable TCP keep-alive
        optval = 1;
        optlen = sizeof(optval);
        setsockopt(clientfd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen);

        // Save the accepted client socket fd (you need a place for this in your struct)
        sock->clientfd = clientfd; // You must define this in your `udp_info_t` if not already

        return 0;
    }


    void Generic_radioHardwareModel::forward_loop(udp_info_t* rcv_sock, udp_info_t* fwd_sock, int direction)
    {
        /*
        if direction = 0, this means going from fsw to radio to cryptolib
        if direction = 1, this means going from cryptolib to radio to fsw
        if direction = -1, this means crosslink between satellites
        */
        int status;
        uint8_t sock_buffer[8192];
        size_t bytes_recvd;

        struct sockaddr_in rcv_addr;
        struct sockaddr_in fwd_addr;
        int sockaddr_size = sizeof(struct sockaddr_in);

        bool communication_capable;
        double delay;

        fwd_addr.sin_family = AF_INET;
        if(inet_addr(fwd_sock->ip.c_str()) != INADDR_NONE)
        {
            fwd_addr.sin_addr.s_addr = inet_addr(fwd_sock->ip.c_str());
        }
        else
        {
            char ip[16];
            int check = host_to_ip(fwd_sock->ip.c_str(), ip);
            sim_logger->info("forward_loop - Initial = %s; Updated = %s; Port = %d \n", fwd_sock->ip.c_str(), ip, fwd_sock->port);
            if(check == 0)
            {
                fwd_addr.sin_addr.s_addr = inet_addr(ip);
            }
        }
        fwd_addr.sin_port = htons(fwd_sock->port);

        udp_init(rcv_sock);
        if (direction == 1) {
            _rcv_sock_udp_uplink = rcv_sock;
            _fwd_addr_udp_uplink = fwd_addr;
        } else {
            _rcv_sock_udp_downlink = rcv_sock;
            _fwd_addr_udp_downlink = fwd_addr;
        }

        sim_logger->debug("Generic_radioHardwareModel::forward_loop: %s:%d to %s:%d", rcv_sock->ip.c_str(), rcv_sock->port, fwd_sock->ip.c_str(), fwd_sock->port);

        while(_keep_running)
        {
            bytes_recvd = 0;

            /* Receive */
            status = recvfrom(rcv_sock->sockfd, sock_buffer, sizeof(sock_buffer), 0, (sockaddr*) &rcv_addr, (socklen_t*) &sockaddr_size);
            boost::shared_ptr<Generic_radioDataPoint> data_point = boost::dynamic_pointer_cast<Generic_radioDataPoint>(_generic_radio_dp->get_data_point());
            delay = 0;
            if (direction == 1) {
                if (data_point->get_uplink_delay_on()) delay = data_point->get_uplink_delay();
                if (data_point->get_uplink_close_criteria() == "occulted") {
                    if (data_point->get_uplink_occulted()) {
                        communication_capable = false;
                    } else {
                        communication_capable = true;
                    }
                } else if (data_point->get_uplink_close_criteria() == "cnr") {
                    if (data_point->get_uplink_cnr_limit() <= data_point->get_uplink_cnr()) {
                        communication_capable = true;
                    } else {
                        communication_capable = false;
                    }
                } else { // uplink_close_criteria == "none" or anything else
                    communication_capable = true;
                }
            } else { // direction = 0
                if (data_point->get_downlink_delay_on()) delay = data_point->get_downlink_delay();
                if (data_point->get_downlink_close_criteria() == "occulted") {
                    if (data_point->get_downlink_occulted()) {
                        communication_capable = false;
                    } else {
                        communication_capable = true;
                    }
                } else if (data_point->get_downlink_close_criteria() == "cnr") {
                    if (data_point->get_downlink_cnr_limit() <= data_point->get_downlink_cnr()) {
                        communication_capable = true;
                    } else {
                        communication_capable = false;
                    }
                } else { // downlink_close_criteria == "none" or anything else
                    communication_capable = true;
                }
            }
            sim_logger->trace("Generic_radioHardwareModel::forward_loop: delay = %f", delay);
            if (status != -1 && communication_capable)
            {
                bytes_recvd = status;

                message_to_send_t message;
                message.time_to_send = _absolute_start_time + _sim_microseconds_per_tick * _time_bus->get_time() / 1000000.0 + delay;
                memcpy(message.buffer, sock_buffer, bytes_recvd);
                message.buffer_size = bytes_recvd;
                std::lock_guard<std::mutex> lock(_message_queue_udp_mutex);
                if (direction == 1) {
                    _message_queue_udp_uplink.push(message);
                } else {
                    _message_queue_udp_downlink.push(message);
                }
                // Lock is released when scope ends
            }
        }
        close(rcv_sock->sockfd);
    }

    void Generic_radioHardwareModel::process_forward_loop_message_queue(NosEngine::Common::SimTime time)
    {
        int status;
        message_to_send_t message;
        std::lock_guard<std::mutex> lock(_message_queue_udp_mutex);
        while(!_message_queue_udp_uplink.empty()) {
            message = _message_queue_udp_uplink.front();
            if (message.time_to_send <= _absolute_start_time + _sim_microseconds_per_tick * time / 1000000.0) {
                _message_queue_udp_uplink.pop();
                /* Debug print */
                sim_logger->trace("Generic_radioHardwareModel::process_forward_loop_message_queue: %s:%d received %ld bytes", 
                    _rcv_sock_udp_uplink->ip.c_str(), _rcv_sock_udp_uplink->port, message.buffer_size);

                /* Forward */
                status = sendto(_rcv_sock_udp_uplink->sockfd, message.buffer, message.buffer_size, 0, (sockaddr*) &_fwd_addr_udp_uplink, sizeof(_fwd_addr_udp_uplink));
                if ((status == -1) || (status != (int)message.buffer_size))
                {
                    sim_logger->error("Generic_radioHardwareModel::process_forward_loop_message_queue: %s:%d only forwarded %d/%ld bytes", 
                        _rcv_sock_udp_uplink->ip.c_str(), _rcv_sock_udp_uplink->port, status, message.buffer_size);
                }
            } else {
                break;
            }
        }
        while(!_message_queue_udp_downlink.empty()) {
            message = _message_queue_udp_downlink.front();
            if (message.time_to_send <= _absolute_start_time + _sim_microseconds_per_tick * time / 1000000.0) {
                _message_queue_udp_downlink.pop();
                /* Debug print */
                sim_logger->trace("Generic_radioHardwareModel::forward_loop: %s:%d received %ld bytes", 
                    _rcv_sock_udp_downlink->ip.c_str(), _rcv_sock_udp_downlink->port, message.buffer_size);

                /* Forward */
                status = sendto(_rcv_sock_udp_downlink->sockfd, message.buffer, message.buffer_size, 0, (sockaddr*) &_fwd_addr_udp_downlink, sizeof(_fwd_addr_udp_downlink));
                if ((status == -1) || (status != (int)message.buffer_size))
                {
                    sim_logger->error("Generic_radioHardwareModel::forward_loop: %s:%d only forwarded %d/%ld bytes", 
                        _rcv_sock_udp_downlink->ip.c_str(), _rcv_sock_udp_downlink->port, status, message.buffer_size);
                }
            } else {
                break;
            }
        }
        // Lock is released when scope ends
    }

    void Generic_radioHardwareModel::tcp_forward_loop(udp_info_t* rcv_sock, udp_info_t* fwd_sock, int direction)
    {
       /*
       if direction = 0, this means going from udp to tcp (fsw to radio to cryptolib)
       if direction = 1, this means going from tcp to udp (cryptolib to radio to fsw)
       */
       bool communication_capable;
       double delay;

       if (direction == 0)
       {
        //forwarding udp data to tcp (going to radio then to cyrptolib) (recv_sock needs to be udp)

            int status;
            uint8_t sock_buffer[8192];
            size_t bytes_recvd;

            struct sockaddr_in rcv_addr;
            int sockaddr_size = sizeof(struct sockaddr_in);

            // Initialize UDP receiving socket
            udp_init(rcv_sock);
            _rcv_sock_tcp_downlink = rcv_sock;
            _fwd_sock_tcp_downlink = fwd_sock;

            // Initialize TCP socket for forwarding
            fwd_sock->sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (fwd_sock->sockfd < 0)
            {
                sim_logger->error("Generic_radioHardwareModel::tcp_forward_loop: Failed to create TCP socket");
                return;
            }

            struct sockaddr_in tcp_addr;
            tcp_addr.sin_family = AF_INET;
            tcp_addr.sin_port = htons(fwd_sock->port);
            char ip[INET_ADDRSTRLEN] = {0};

            if (inet_addr(fwd_sock->ip.c_str()) != INADDR_NONE)
            {
                tcp_addr.sin_addr.s_addr = inet_addr(fwd_sock->ip.c_str());
            }
            else
            {

                int check = host_to_ip(fwd_sock->ip.c_str(), ip);

                int retries = 0;
                while(check!=0)
                {
                    if (retries >= 30) 
                    {
                        sim_logger->error("Failed to resolve host Cryptolib IP after 30 attempts.");
                        return;
                    }
                    check =  host_to_ip(fwd_sock->ip.c_str(), ip);
                    sim_logger->info("Generic_radioHardwareModel::tcp_forward_loop: Failed to resolve IP for %s, retrying to resolve", fwd_sock->ip.c_str());
                    sleep(1);
                    retries++;
                }

                sim_logger->info("tcp_forward_loop - Initial = %s; Updated = %s; Port = %d", fwd_sock->ip.c_str(), ip, fwd_sock->port);
                if (check != 0)
                {
                    sim_logger->error("Generic_radioHardwareModel::tcp_forward_loop: Failed to resolve IP for %s", fwd_sock->ip.c_str());
                    return;
                }
                tcp_addr.sin_addr.s_addr = inet_addr(ip);
            }
            
            sim_logger->info("tcp_forward_loop - Hostname = %s, Resolved IP = %s, Port = %d", fwd_sock->ip.c_str(), ip, fwd_sock->port);

            sim_logger->info("Waiting for Cyrptolib TCP server to become available");
            int retries = 0;
            while (connect(fwd_sock->sockfd, (struct sockaddr*)&tcp_addr, sizeof(tcp_addr)) < 0)
            {
                if (retries >= 30) 
                {
                    sim_logger->error("Failed to connect to Cryptolib TCP server after 30 attempts.");
                    sim_logger->error("Generic_radioHardwareModel::tcp_forward_loop: Failed to connect to TCP server %s:%d", fwd_sock->ip.c_str(), fwd_sock->port);
                    return;
                }
                sim_logger->debug("Connection refused, retrying in 1 second...");
                sleep(1);
                retries++;
            }
            sim_logger->info("Successfully connected to TCP server!");
 

            sim_logger->debug("Generic_radioHardwareModel::tcp_forward_loop: (UDP->TCP): UDP %s:%d to TCP %s:%d",
                            rcv_sock->ip.c_str(), rcv_sock->port, fwd_sock->ip.c_str(), fwd_sock->port);

            while (_keep_running)
            {
                bytes_recvd = 0;

                // Receive from UDP
                status = recvfrom(rcv_sock->sockfd, sock_buffer, sizeof(sock_buffer), 0,
                                (sockaddr*)&rcv_addr, (socklen_t*)&sockaddr_size);
                boost::shared_ptr<Generic_radioDataPoint> data_point = boost::dynamic_pointer_cast<Generic_radioDataPoint>(_generic_radio_dp->get_data_point());
                delay = 0;
                if (data_point->get_downlink_delay_on()) delay = data_point->get_downlink_delay();
                if (data_point->get_downlink_close_criteria() == "occulted") {
                    if (data_point->get_downlink_occulted()) {
                        communication_capable = false;
                    } else {
                        communication_capable = true;
                    }
                } else if (data_point->get_downlink_close_criteria() == "cnr") {
                    if (data_point->get_downlink_cnr_limit() <= data_point->get_downlink_cnr()) {
                        communication_capable = true;
                    } else {
                        communication_capable = false;
                    }
                } else { // downlink_close_criteria == "none" or anything else
                    communication_capable = true;
                }
                sim_logger->trace("Generic_radioHardwareModel::tcp_forward_loop: delay = %f", delay);
                if (status != -1 && communication_capable)
                {
                    bytes_recvd = status;

                    message_to_send_t message;
                    message.time_to_send = _absolute_start_time + _sim_microseconds_per_tick * _time_bus->get_time() / 1000000.0 + delay;
                    memcpy(message.buffer, sock_buffer, bytes_recvd);
                    message.buffer_size = bytes_recvd;
                    std::lock_guard<std::mutex> lock(_message_queue_tcp_downlink_mutex);
                    _message_queue_tcp_downlink.push(message);
                    // lock is released when scope ends
                }
            }

            close(rcv_sock->sockfd);
            close(fwd_sock->sockfd);
       }
       else
       {
        //forwarding tcp data to udp (going to fsw) fwd sock needs to be udp
            int status;
            uint8_t sock_buffer[8192];
            size_t bytes_recvd;

            struct sockaddr_in fwd_addr;

            // Resolve UDP forwarding address
            fwd_addr.sin_family = AF_INET;
            if (inet_addr(fwd_sock->ip.c_str()) != INADDR_NONE)
            {
                fwd_addr.sin_addr.s_addr = inet_addr(fwd_sock->ip.c_str());
            }
            else
            {
                char ip[16] = {0};
                int check = host_to_ip(fwd_sock->ip.c_str(), ip);
                sim_logger->info("tcp_forward_loop - Initial = %s; Updated = %s; Port = %d", fwd_sock->ip.c_str(), ip, fwd_sock->port);
                if (check == 0)
                {
                    fwd_addr.sin_addr.s_addr = inet_addr(ip);
                }
            }
            fwd_addr.sin_port = htons(fwd_sock->port);

            tcp_init(rcv_sock);

            udp_init(fwd_sock); // Only init the UDP socket since TCP is already connected
            _rcv_sock_tcp_uplink = rcv_sock;
            _fwd_sock_tcp_uplink = fwd_sock;
            _fwd_addr_tcp_uplink = fwd_addr;

            sim_logger->debug("Generic_radioHardwareModel::tcp_forward_loop (TCP→UDP): TCP %s:%d to UDP %s:%d",
                            rcv_sock->ip.c_str(), rcv_sock->port, fwd_sock->ip.c_str(), fwd_sock->port);

            while (_keep_running)
            {
                /* Receive from TCP socket */
                status = recv(rcv_sock->clientfd, sock_buffer, sizeof(sock_buffer), 0);
                boost::shared_ptr<Generic_radioDataPoint> data_point = boost::dynamic_pointer_cast<Generic_radioDataPoint>(_generic_radio_dp->get_data_point());
                delay = 0;
                if (data_point->get_uplink_delay_on()) delay = data_point->get_uplink_delay();
                if (data_point->get_uplink_close_criteria() == "occulted") {
                    if (data_point->get_uplink_occulted()) {
                        communication_capable = false;
                    } else {
                        communication_capable = true;
                    }
                } else if (data_point->get_uplink_close_criteria() == "cnr") {
                    if (data_point->get_uplink_cnr_limit() <= data_point->get_uplink_cnr()) {
                        communication_capable = true;
                    } else {
                        communication_capable = false;
                    }
                } else { // uplink_close_criteria == "none" or anything else
                    communication_capable = true;
                }
                sim_logger->trace("Generic_radioHardwareModel::tcp_forward_loop: delay = %f", delay);
                if (status > 0 && communication_capable)
                {
                    bytes_recvd = status;

                    message_to_send_t message;
                    message.time_to_send = _absolute_start_time + _sim_microseconds_per_tick * _time_bus->get_time() / 1000000.0 + delay;
                    memcpy(message.buffer, sock_buffer, bytes_recvd);
                    message.buffer_size = bytes_recvd;
                    std::lock_guard<std::mutex> lock(_message_queue_tcp_uplink_mutex);
                    _message_queue_tcp_uplink.push(message);
                    // lock is released when scope ends
                }
            }

            close(rcv_sock->sockfd);
            close(fwd_sock->sockfd);
        }
        
    }

    void Generic_radioHardwareModel::process_tcp_forward_loop_message_queue(NosEngine::Common::SimTime time)
    {
        int status;
        message_to_send_t message;
        {
            std::lock_guard<std::mutex> lock(_message_queue_tcp_downlink_mutex);
            while(!_message_queue_tcp_downlink.empty()) {
                message = _message_queue_tcp_downlink.front();
                if (message.time_to_send <= _absolute_start_time + _sim_microseconds_per_tick * time / 1000000.0) {
                    _message_queue_tcp_downlink.pop();
                    // log to check status of bytes received from udp to be forwarded to tcp
                    sim_logger->trace("Generic_radioHardwareModel::process_tcp_forward_loop_message_queue: received %ld bytes from UDP %s:%d",
                        message.buffer_size, _rcv_sock_tcp_downlink->ip.c_str(), _rcv_sock_tcp_downlink->port);

                    // Forward to TCP
                    status = send(_fwd_sock_tcp_downlink->sockfd, message.buffer, message.buffer_size, 0);
                    if ((status == -1) || (status != (int)message.buffer_size))
                    {
                        sim_logger->error("Generic_radioHardwareModel::process_tcp_forward_loop_message_queue: only forwarded %d/%ld bytes to TCP %s:%d",
                                        status, message.buffer_size, _fwd_sock_tcp_downlink->ip.c_str(), _fwd_sock_tcp_downlink->port);
                    }   
                } else {
                    break;
                }
            } 
            // lock is released when scope ends
        }    

        {
            std::lock_guard<std::mutex> lock(_message_queue_tcp_uplink_mutex);
            while(!_message_queue_tcp_uplink.empty()) {
                message = _message_queue_tcp_uplink.front();
                if (message.time_to_send <= _absolute_start_time + _sim_microseconds_per_tick * time / 1000000.0) {
                    _message_queue_tcp_uplink.pop();
                    sim_logger->debug("Generic_radioHardwareModel::process_tcp_forward_loop_message_queue: TCP %s:%d received %ld bytes",
                        _rcv_sock_tcp_uplink->ip.c_str(), _rcv_sock_tcp_uplink->port, message.buffer_size);

                    /* Forward to UDP socket */
                    status = sendto(_fwd_sock_tcp_uplink->sockfd, message.buffer, message.buffer_size, 0,
                                    (sockaddr*)&_fwd_addr_tcp_uplink, sizeof(_fwd_addr_tcp_uplink));
                    if ((status == -1) || (status != (int)message.buffer_size))
                    {
                        sim_logger->error("Generic_radioHardwareModel::process_tcp_forward_loop_message_queue: UDP forward only sent %d/%ld bytes",
                                        status, message.buffer_size);
                    }
                } else {
                    break;
                }
            }
            // lock is released when scope ends
        }
    }


void Generic_radioHardwareModel::setup_fwd_addr(udp_info_t* sock, struct sockaddr_in& addr)
    {
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        if(inet_addr(sock->ip.c_str()) != INADDR_NONE) {
            addr.sin_addr.s_addr = inet_addr(sock->ip.c_str());
        } else {
            char ip[16];
            if(host_to_ip(sock->ip.c_str(), ip) == 0) {
                addr.sin_addr.s_addr = inet_addr(ip);
            }
        }
        addr.sin_port = htons(sock->port);
    }    

void Generic_radioHardwareModel::forward_loop_multi(udp_info_t* rcv_sock, udp_info_t* fwd_sock1, udp_info_t* fwd_sock2)
    {
        int status;
        uint8_t sock_buffer[8192];
        size_t bytes_recvd;

        struct sockaddr_in rcv_addr;
        struct sockaddr_in fwd_addr1, fwd_addr2;
        int sockaddr_size = sizeof(struct sockaddr_in);

        bool communication_capable;
        double delay;

        setup_fwd_addr(fwd_sock1, fwd_addr1);
        setup_fwd_addr(fwd_sock2, fwd_addr2);

        udp_init(rcv_sock);
        _rcv_sock_multi_downlink = rcv_sock;
        _fwd_addr1_multi_downlink = fwd_addr1;
        _fwd_addr2_multi_downlink = fwd_addr2;

        sim_logger->info("Generic_radioHardwareModel::forward_loop_multi: %s:%d to %s:%d AND %s:%d", rcv_sock->ip.c_str(), rcv_sock->port, fwd_sock1->ip.c_str(), fwd_sock1->port, fwd_sock2->ip.c_str(), fwd_sock2->port);
        // debugging where gsw2 is sending data
        sim_logger->info("forward_loop_multi: GSW2 resolved to IP %s on Port %d", 
                         inet_ntoa(fwd_addr2.sin_addr), 
                         ntohs(fwd_addr2.sin_port));
        sim_logger->info("forward_loop_multi: GSW1 resolved to IP %s on Port %d", 
                         inet_ntoa(fwd_addr1.sin_addr), 
                         ntohs(fwd_addr1.sin_port));
        while(_keep_running)
        {
            bytes_recvd = 0;

            /* Receive from FSW */
            status = recvfrom(rcv_sock->sockfd, sock_buffer, sizeof(sock_buffer), 0, (sockaddr*) &rcv_addr, (socklen_t*) &sockaddr_size);
            boost::shared_ptr<Generic_radioDataPoint> data_point = boost::dynamic_pointer_cast<Generic_radioDataPoint>(_generic_radio_dp->get_data_point());
            delay = 0;
            if (data_point->get_downlink_delay_on()) delay = data_point->get_downlink_delay();
            if (data_point->get_downlink_close_criteria() == "occulted") {
                if (data_point->get_downlink_occulted()) {
                    communication_capable = false;
                } else {
                    communication_capable = true;
                }
            } else if (data_point->get_downlink_close_criteria() == "cnr") {
                if (data_point->get_downlink_cnr_limit() <= data_point->get_downlink_cnr()) {
                    communication_capable = true;
                } else {
                    communication_capable = false;
                }
            } else { // downlink_close_criteria == "none" or anything else
                communication_capable = true;
            }
            sim_logger->trace("Generic_radioHardwareModel::forward_loop_multi: delay = %f", delay);
            if (status != -1 && communication_capable)
            {
                bytes_recvd = status;

                message_to_send_t message;
                message.time_to_send = _absolute_start_time + _sim_microseconds_per_tick * _time_bus->get_time() / 1000000.0 + delay;
                memcpy(message.buffer, sock_buffer, bytes_recvd);
                message.buffer_size = bytes_recvd;
                std::lock_guard<std::mutex> lock(_message_queue_multi_downlink_mutex);
                _message_queue_multi_downlink.push(message);
                // lock is released when scope ends
            }
        }
        close(rcv_sock->sockfd);
    }

    void Generic_radioHardwareModel::process_forward_loop_multi_message_queue(NosEngine::Common::SimTime time)
    {
        int status;
        message_to_send_t message;
        std::lock_guard<std::mutex> lock(_message_queue_multi_downlink_mutex);
        while(!_message_queue_multi_downlink.empty()) {
            message = _message_queue_multi_downlink.front();
            if (message.time_to_send <= _absolute_start_time + _sim_microseconds_per_tick * time / 1000000.0) {
                _message_queue_multi_downlink.pop();
                //debuging multi loop
                sim_logger->trace("process_forward_loop_multi_message_queue: Received %ld bytes from FSW on port %d", message.buffer_size, _rcv_sock_multi_downlink->port);

                //Forwared to gsw1
                status = sendto(_rcv_sock_multi_downlink->sockfd, message.buffer, message.buffer_size, 0, (sockaddr*) &_fwd_addr1_multi_downlink, sizeof(_fwd_addr1_multi_downlink));
                if ((status == -1) || (status != (int)message.buffer_size)) {
                    sim_logger->error("process_forward_loop_multi_message_queue: Failed to send to GSW1");
                }

                // forward to gsw2
                status = sendto(_rcv_sock_multi_downlink->sockfd, message.buffer, message.buffer_size, 0, (sockaddr*) &_fwd_addr2_multi_downlink, sizeof(_fwd_addr2_multi_downlink));
                if ((status == -1) || (status != (int)message.buffer_size)) {
                    sim_logger->error("process_forward_loop_multi_message_queue: Failed to send to GSW2");
                }        
            } else {
                break;
            }
        }
        // lock is released when scope ends
    }

    /* Custom function to prepare the Generic_radio HK telemetry */
    void Generic_radioHardwareModel::create_generic_radio_hk(std::uint8_t out_data[16])
    {
        //boost::shared_ptr<Generic_radioDataPoint> data_point = boost::dynamic_pointer_cast<Generic_radioDataPoint>(_generic_radio_dp->get_data_point());

        /* Streaming data header - 0xDEAD */
        out_data[0] = 0xDE;
        out_data[1] = 0xAD;
        
        /* Sequence count */
        out_data[2] = (_count >> 24) & 0x000000FF; 
        out_data[3] = (_count >> 16) & 0x000000FF; 
        out_data[4] = (_count >>  8) & 0x000000FF; 
        out_data[5] =  _count & 0x000000FF;
        
        /* Configuration */
        out_data[6] = (_config >> 24) & 0x000000FF; 
        out_data[7] = (_config >> 16) & 0x000000FF; 
        out_data[8] = (_config >>  8) & 0x000000FF; 
        out_data[9] =  _config & 0x000000FF;

        /* Proximity Signal Strength */
        /* TODO: Determine via 42 data */
        out_data[10] = (_prox_signal >> 24) & 0x000000FF; 
        out_data[11] = (_prox_signal >> 16) & 0x000000FF; 
        out_data[12] = (_prox_signal >>  8) & 0x000000FF; 
        out_data[13] =  _prox_signal & 0x000000FF;

        /* Streaming data trailer - 0xBEEF */
        out_data[14] = 0xBE;
        out_data[15] = 0xEF;
    }


    /* Protocol callback */
    void Generic_radioHardwareModel::process_radio_command(const uint8_t *buf, size_t len)
    {
        std::uint8_t out_data[16]; 
        int status = GENERIC_RADIO_SIM_SUCCESS;
        std::uint8_t valid = GENERIC_RADIO_SIM_SUCCESS;
        
        struct sockaddr_in fwd_addr;
        fwd_addr.sin_family = AF_INET;
        if(inet_addr(_fsw_radio.ip.c_str()) != INADDR_NONE)
        {
            fwd_addr.sin_addr.s_addr = inet_addr(_fsw_radio.ip.c_str());
        }
        else
        {
            char ip[16];
            int check = host_to_ip(_fsw_radio.ip.c_str(), ip);
            if(check == 0)
            {
                fwd_addr.sin_addr.s_addr = inet_addr(ip);
            }
        }
        fwd_addr.sin_port = htons(_fsw_radio.port);

        /* Retrieve data and log in man readable format */
        std::vector<uint8_t> in_data(buf, buf + len);
        sim_logger->trace("Generic_radioHardwareModel::process_radio_command:  REQUEST %s",
            SimIHardwareModel::uint8_vector_to_hex_string(in_data).c_str());

        /* Check simulator is enabled */
        if (_enabled != GENERIC_RADIO_SIM_SUCCESS)
        {
            sim_logger->debug("Generic_radioHardwareModel::process_radio_command:  Generic_radio sim disabled!");
            valid = GENERIC_RADIO_SIM_ERROR;
        }
        else
        {
            /* Check if message is incorrect size */
            if (in_data.size() != 9)
            {
                sim_logger->debug("Generic_radioHardwareModel::process_radio_command:  Invalid command size of %ld received!", in_data.size());
                valid = GENERIC_RADIO_SIM_ERROR;
            }
            else
            {
                /* Check header - 0xDEAD */
                if ((in_data[0] != 0xDE) || (in_data[1] !=0xAD))
                {
                    sim_logger->debug("Generic_radioHardwareModel::process_radio_command:  Header incorrect!");
                    valid = GENERIC_RADIO_SIM_ERROR;
                }
                else
                {
                    /* Check trailer - 0xBEEF */
                    if ((in_data[7] != 0xBE) || (in_data[8] !=0xEF))
                    {
                        sim_logger->debug("Generic_radioHardwareModel::process_radio_command:  Trailer incorrect!");
                        valid = GENERIC_RADIO_SIM_ERROR;
                    }
                }
            }

            if (valid == GENERIC_RADIO_SIM_SUCCESS)
            {   
                /* Process command */
                switch (in_data[2])
                {
                    case 0:
                        /* Request HK */
                        sim_logger->trace("Generic_radioHardwareModel::process_radio_command:  Send HK command received!");
                        _count++;
                        create_generic_radio_hk(out_data);
                        status = sendto(_radio_cmd.sockfd, out_data, 16, 0, (sockaddr*) &fwd_addr, sizeof(fwd_addr));
                        if ((status == -1) || (status != 16))
                        {
                            sim_logger->debug("Generic_radioHardwareModel::process_radio_command: sendto returned %d", status);
                        }
                        break;

                    case 1:
                        /* Configuration */
                        sim_logger->debug("Generic_radioHardwareModel::process_radio_command:  Configuration command received!");
                        _count++;
                        _config  = in_data[3] << 24;
                        _config |= in_data[4] << 16;
                        _config |= in_data[5] << 8;
                        _config |= in_data[6];
                        break;

                    default:
                        /* Unused command code */
                        sim_logger->debug("Generic_radioHardwareModel::process_radio_command:  Unused command %d received!", in_data[2]);
                        break;
                }
            }
        }
    }
}
