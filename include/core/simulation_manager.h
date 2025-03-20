#pragma once

#include <iostream>

enum class SimulationState {
    Stopped,
    Running,
    Paused
};

class SimulationManager {
public:
    SimulationState state = SimulationState::Stopped;

    // Start the simulation
    void StartSimulation() {
        // ensure program is stopped or paused
        if (state == SimulationState::Stopped || state == SimulationState::Paused) {
            state = SimulationState::Running;
            std::cout << "Simulation started" << std::endl;
        }
        else {
            std::cout << "Simulation already running" << std::endl;
        }
        
    };

    // Pause the simulation
    void PauseSimulation() {
        // ensure program is running
        if (state == SimulationState::Running) {
            state = SimulationState::Paused;
            std::cout << "Simulation paused" << std::endl;
        }
        else {
            std::cout << "Simulation not running" << std::endl;
        }
    };

    // Stop the simulation
    void StopSimulation() {
        state = SimulationState::Stopped;
        std::cout << "Simulation stopped" << std::endl;
    };

    // Check simulation state
    bool isRunning() const { return state == SimulationState::Running; };
    bool isPaused() const { return state == SimulationState::Paused; };
    bool isStopped() const { return state == SimulationState::Stopped; };
};