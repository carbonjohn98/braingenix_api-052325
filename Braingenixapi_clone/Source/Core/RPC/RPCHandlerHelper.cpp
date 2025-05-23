//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <thread>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")

#include <RPC/RPCHandlerHelper.h>



namespace BG {
namespace API {
namespace API {



// Handy class for standard handler data.
HandlerData::HandlerData(const std::string& _JSONRequest, BG::Common::Logger::LoggingSystem* _Logger, std::string _RoutePath) {

    // ManTaskData = called_by_manager_task;
    JSONRequestStr = _JSONRequest;
    Logger_ = _Logger;
    RoutePath_ = _RoutePath;

    RequestJSON = nlohmann::json::parse(_JSONRequest);

    // bool isloadingsim = (ManTaskData != nullptr); // Man.IsLoadingSim();
    // if (isloadingsim && (_Source == "SimulationLoad")) { // *** PERHAPS WE CAN ALLOW THIS (AS WE USE LOCAL PARAMS NOW)?
    //     Man.Logger()->Log("Recursive SimulationLoad attempted.", 8);
    //     Status = BGStatusCode::BGStatusGeneralFailure;
    //     return; // Prevents recursion.
    // }

    // // If this Handler was called while loading and a Simulation was not created
    // // yet then only allow requests that do not need one ("NoSimulation"), such
    // // as "SimulationCreate" and "APIRequest".
    // if (isloadingsim && (!ManTaskData->HasReplacementSimID()) && (!NoSimulation)) {
    //     Man.Logger()->Log(Source+" needs a Sim, but we are still Loading and have not come across SimulationCreate yet.", 8);
    //     Status = BGStatusCode::BGStatusInvalidParametersPassed;
    //     return; // When loading, the first valid request must be SimulationCreate.
    // }

    // if (isloadingsim) {
    //     SimulationID = ManTaskData->ReplaceSimulationID;
    // } else {

    // }

    // Man.Logger()->Log(Source+" called, on Sim " + SimIDStr(), 3);
}


// // See how this is used in Manager::SimulationCreate().
// Simulator::Simulation* HandlerData::NewSimulation() {
//     ThisSimulation = Man.MakeSimulation();
//     SimulationID = ThisSimulation->ID;
//     Man.Logger()->Log("New Sim " + SimIDStr(), 3);
//     return ThisSimulation;
// }

bool HandlerData::HasError() const {
    return (Status != BGStatusCode::BGStatusSuccess);
}

BGStatusCode HandlerData::GetStatus() const {
    return Status;
}

// Note: This is purposely NOT const nlohmann::json &, because we do NOT
//       want to permit calls with rvalues that default-construct a
//       nlohmann::json, as that is a way to accidentally a JSON object
//       into JSON containing a single string, e.g. by accidentally
//       passing ResponseJSON.dump() instead of ResponseJSON.
// Note: Calling this from APIRequest uses store==false, because we only
//       want to store the calls from their actual handlers, not from the
//       APIRequest batch handler. We don't want to double-count the calls,
//       and we want to store the individual oAPI, because they may be
//       intended for different simulations (dependeing on their SimulationID).
std::string HandlerData::ResponseAndStoreRequest(nlohmann::json& ResponseJSON,  bool store) {
    // if (store && (Status == BGStatusCode::BGStatusSuccess)) {
    //     if (ThisSimulation != nullptr) {
    //         ThisSimulation->StoreRequestHandled(Source, _RH.at(Source).Route, JSONRequestStr);
    //     }
    // }

    return ResponseJSON.dump();
}
std::string HandlerData::ErrResponse(int _Status) {
    Status = BGStatusCode(_Status);
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = _Status;
    return ResponseAndStoreRequest(ResponseJSON);
}
std::string HandlerData::ErrResponse(BGStatusCode _Status) {
    return ErrResponse(int(_Status));
}
std::string HandlerData::ErrResponse() {
    return ErrResponse(int(Status));
}

std::string HandlerData::ResponseWithID(const std::string& IDName, int IDValue) {
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = int(Status);
    ResponseJSON[IDName] = IDValue;
    return ResponseAndStoreRequest(ResponseJSON);
}
std::string HandlerData::ResponseWithID(const std::string& IDName, const std::string& IDValue) {
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = int(Status);
    ResponseJSON[IDName] = IDValue;
    return ResponseAndStoreRequest(ResponseJSON);
}
std::string HandlerData::StringResponse(std::string _Key, std::string _Value) {
    nlohmann::json ResponseJSON;
    ResponseJSON["StatusCode"] = int(Status);
    ResponseJSON[_Key] = _Value;
    return ResponseJSON.dump();
}

const nlohmann::json& HandlerData::ReqJSON() const {
    return RequestJSON;
}

bool HandlerData::FindPar(const std::string& ParName, nlohmann::json::iterator& Iterator, nlohmann::json& _JSON) {
    Iterator = _JSON.find(ParName);
    if (Iterator == _JSON.end()) {
        Logger_->Log("Error Finding Parameter '" + ParName + "', Request Is: " + _JSON.dump(), 7);
        Status = BGStatusCode::BGStatusInvalidParametersPassed;
        return false;
    }
    return true;
}

bool HandlerData::FindPar(const std::string& ParName, nlohmann::json::iterator& Iterator) {
    return FindPar(ParName, Iterator, RequestJSON);
}

bool HandlerData::GetParBool(const std::string& ParName, bool& Value, nlohmann::json& _JSON) {
    nlohmann::json::iterator it;
    if (!FindPar(ParName, it, _JSON)) {
        return false;
    }
    if (!it.value().is_boolean()) {
        Logger_->Log("Error Parameter '" + ParName + "', Wrong Type (expected bool) Request Is: " + _JSON.dump(), 7);
        Status = BGStatusCode::BGStatusInvalidParametersPassed;
        return false;
    }
    Value = it.value().template get<bool>();
    return true;
}

bool HandlerData::GetParBool(const std::string& ParName, bool& Value) {
    return GetParBool(ParName, Value, RequestJSON);
}

bool HandlerData::GetParInt(const std::string& ParName, int& Value, nlohmann::json& _JSON) {
    nlohmann::json::iterator it;
    if (!FindPar(ParName, it, _JSON)) {
        return false;
    }
    if (!it.value().is_number()) {
        Logger_->Log("Error Parameter '" + ParName + "', Wrong Type (expected number) Request Is: " + _JSON.dump(), 7);
        Status = BGStatusCode::BGStatusInvalidParametersPassed;
        return false;
    }
    Value = it.value().template get<int>();
    return true;
}

bool HandlerData::GetParInt(const std::string& ParName, int& Value) {
    return GetParInt(ParName, Value, RequestJSON);
}

bool HandlerData::GetParFloat(const std::string& ParName, float& Value, nlohmann::json& _JSON) {
    nlohmann::json::iterator it;
    if (!FindPar(ParName, it, _JSON)) {
        return false;
    }
    if (!it.value().is_number()) {
        Logger_->Log("Error Parameter '" + ParName + "', Wrong Type (expected number) Request Is: " + _JSON.dump(), 7);
        Status = BGStatusCode::BGStatusInvalidParametersPassed;
        return false;
    }
    Value = it.value().template get<float>();
    return true;
}

bool HandlerData::GetParFloat(const std::string& ParName, float& Value) {
    return GetParFloat(ParName, Value, RequestJSON);
}

bool HandlerData::GetParString(const std::string& ParName, std::string& Value, nlohmann::json& _JSON) {
    nlohmann::json::iterator it;
    if (!FindPar(ParName, it, _JSON)) {
        return false;
    }
    if (!it.value().is_string()) {
        Logger_->Log("Error Parameter '" + ParName + "', Wrong Type (expected string) Request Is: " + _JSON.dump(), 7);
        Status = BGStatusCode::BGStatusInvalidParametersPassed;
        return false;
    }
    Value = it.value().template get<std::string>();
    return true;
}

bool HandlerData::GetParString(const std::string& ParName, std::string& Value) {
    return GetParString(ParName, Value, RequestJSON);
}


bool HandlerData::GetParVecInt(const std::string& ParName, std::vector<int>& Value, nlohmann::json& _JSON) {
    nlohmann::json::iterator it;
    if (!FindPar(ParName, it, _JSON)) {
        return false;
    }
    if (!it.value().is_array()) {
        Logger_->Log("Error Parameter '" + ParName + "', Wrong Type (expected array) Request Is: " + _JSON.dump(), 7);
        Status = BGStatusCode::BGStatusInvalidParametersPassed;
        return false;
    }
    for (auto& element : it.value()) {
        if (!element.is_number()) {
            Logger_->Log("Error Parameter '" + ParName + "', Wrong Type (expected int) Request Is: " + _JSON.dump(), 7);
            Status = BGStatusCode::BGStatusInvalidParametersPassed;
            return false;
        }
        Value.emplace_back(element.template get<int>());
    }
    return true;
}

bool HandlerData::GetParVecInt(const std::string& ParName, std::vector<int>& Value) {
    return GetParVecInt(ParName, Value, RequestJSON);
}

// If Parname="" then _JSON.is_array() must be true.
bool HandlerData::GetParVecFloat(const std::string& ParName, std::vector<float>& Value, nlohmann::json& _JSON) {
    if (ParName.empty()) {
        if (!_JSON.is_array()) {
            Logger_->Log("Error Parameter '" + ParName + "', Wrong Type (expected array) Request Is: " + _JSON.dump(), 7);
            Status = BGStatusCode::BGStatusInvalidParametersPassed;
            return false;
        }
        for (auto& element : _JSON) {
            if (!element.is_number()) {
                Logger_->Log("Error Parameter '" + ParName + "', Wrong Type (expected float) Request Is: " + _JSON.dump(), 7);
                Status = BGStatusCode::BGStatusInvalidParametersPassed;
                return false;
            }
            Value.emplace_back(element.template get<float>());
        }
        return true;
    } else {
        nlohmann::json::iterator it;
        if (!FindPar(ParName, it, _JSON)) {
            return false;
        }
        if (!it.value().is_array()) {
            Logger_->Log("Error Parameter '" + ParName + "', Wrong Type (expected array) Request Is: " + _JSON.dump(), 7);
            Status = BGStatusCode::BGStatusInvalidParametersPassed;
            return false;
        }
        for (auto& element : it.value()) {
            if (!element.is_number()) {
                Logger_->Log("Error Parameter '" + ParName + "', Wrong Type (expected float) Request Is: " + _JSON.dump(), 7);
                Status = BGStatusCode::BGStatusInvalidParametersPassed;
                return false;
            }
            Value.emplace_back(element.template get<float>());
        }
        return true;
    }
}

bool HandlerData::GetParVecFloat(const std::string& ParName, std::vector<float>& Value) {
    return GetParVecFloat(ParName, Value, RequestJSON);
}



}; // Close Namespace API
}; // Close Namespace API
}; // Close Namespace BG