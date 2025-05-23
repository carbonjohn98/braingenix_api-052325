//=================================================================//
// This file is part of the BrainGenix-API Neuron Emulation System //
//=================================================================//

/*
    Description: This file provides bgStatusCode.
    Additional Notes: None
    Date Created: 2024-02-06
*/

#pragma once

// Standard Libraries (BG convention: use <> instead of "")

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")

namespace BG {
namespace API {
namespace API {

enum BGStatusCode {
    BGStatusSuccess = 0,
    BGStatusGeneralFailure = 1,
    BGStatusInvalidParametersPassed = 2,
    BGStatusUpstreamGatewayUnavailable = 3,
    BGStatusUnauthorizedInvalidNoToken = 4,
    BGStatusSimulationBusy = 5,
    NUMBGStatusCode
};

}; // Close Namespace API
}; // Close Namespace API
}; // Close Namespace BG
