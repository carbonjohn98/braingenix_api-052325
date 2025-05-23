//=================================================================//
// This file is part of the BrainGenix-API Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides RPC management functionality for the system.
    Additional Notes: None
    Date Created: 2023-06-26
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")
#include <iostream>
#include <memory>

// Third-Party Libraries (BG convention: use <> instead of "")
#include <rpc/server.h>
#include <nlohmann/json.hpp>


// Internal Libraries (BG convention: use <> instead of "")
#include <RPC/StaticRoutes.h>
#include <RPC/RouteAndHandler.h>

#include <BG/Common/Logger/Logger.h>

#include <Server/Server.h>

#include <Config/Config.h>
#include <Util/RPCHelpers.h>



namespace BG {
namespace API {
namespace API {

/**
 * @brief Manages the API remote procedure call (RPC) host.
 *
 * The RPCManager class owns the RPC server and handles its initialization and destruction at the end of the program's run.
 * It also takes a copy of the systemwide configuration struct to configure the RPC server (e.g., host and port).
 */
class RPCManager {

private:

    Config::Config* Config_; /**Pointer to configuration struct owned by rest of system*/
    std::unique_ptr<rpc::server> RPCServer_; /**Instance of RPC Server from rpclib*/
    BG::Common::Logger::LoggingSystem* Logger_ = nullptr; /**Pointer to the instance of the logging system*/
    Server::Server* Server_;



    std::map<std::string, std::function<std::string(std::string _JSONRequest)>> RequestHandlers_;


   /**
     * @brief Adds the given route 
     * 
     * @param _RouteName
     * @param _Handler 
     */
    void AddRequestHandler(std::string _RouteName, RouteAndHandler _Handler);



public:

    /**
     * @brief Construct a new RPCManager object
     * Give this a pointer to an initialized configuration object.
     * Also it takes a pointer to the logging system instance.
     * 
     * @param _Config 
     * @param _Logger
     */
    RPCManager(Config::Config* _Config, BG::Common::Logger::LoggingSystem* _Logger, Server::Server* _Server);


    /**
     * @brief Destroy the RPCManager object
     * 
     */
    ~RPCManager();



    /*
     * @brief Adds a route to the API RPC Handler.
     * 
     * @param _RouteHandle 
     * @param _Function 
     */
    void AddRoute(std::string _RouteHandle, std::function<std::string(std::string _JSONRequest)> _Function);

    


    std::string NESRequest(std::string _JSONRequest, int _SimulationIDOverride = -1); // Generic JSON-based API requests.
    std::string EVMRequest(std::string _JSONRequest, int _SimulationIDOverride = -1); // Generic JSON-based API requests.


    /**
     * @brief Registers a callback to the API service.
     *
     * Registers a callback function for a specific route. Assumes the callback function may be accessed from any thread.
     *
     * @param _RouteName Name of the route to register.
     * @param _Logger Pointer to the logging system instance.
     * @param _CallbackFunction Callback function to be registered.
     */
    template <typename F> void AddRoute(std::string _RouteName, BG::Common::Logger::LoggingSystem* _Logger, F _CallbackFunction) {
        std::string LogMsg = "Registering Callback For Route " + _RouteName;
        _Logger->Log(LogMsg, 0);
        RPCServer_->bind(_RouteName.c_str(), _CallbackFunction);
    }

};

}; // Close Namespace API
}; // Close Namespace API
}; // Close Namespace BG