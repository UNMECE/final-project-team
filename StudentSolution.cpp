#include "acequia_manager.h"
#include <iostream>
#include <vector>
#include <algorithm>

void solveProblems(AcequiaManager& manager) {
    auto canals = manager.getCanals();
    auto regions = manager.getRegions();

    // Helper function: Find a canal between two regions
    auto findCanal = [&](Region* from, Region* to) -> Canal* {
        for (auto* canal : canals) {
            if (canal->sourceRegion == from && canal->destinationRegion == to) {
                return canal;
            }
        }
        return nullptr;
    };

    while (!manager.isSolved && manager.hour != manager.SimulationMax) {
        for (Region* target : regions) {
            // Skip if not in drought or already satisfied
            if (!target->isInDrought || target->waterLevel >= target->waterNeed)
                continue;

            double deficit = target->waterNeed - target->waterLevel;

            Region* bestDonor = nullptr;
            double bestSurplus = 0.0;

            // Find the best donor region (not in drought and has surplus)
            for (Region* donor : regions) {
                if (donor == target) continue;

                double surplus = donor->waterLevel - donor->waterNeed;
                if (!donor->isInDrought && surplus > bestSurplus) {
                    bestDonor = donor;
                    bestSurplus = surplus;
                }
            }

            // If we found a good donor and there's a canal
            if (bestDonor && bestSurplus > 0.0) {
                Canal* canal = findCanal(bestDonor, target);
                if (canal) {
                    // Compute the needed flow rate (scale to deficit)
                    double rate = std::min(1.0, deficit / 100.0); // adjust divisor based on your units
                    canal->setFlowRate(rate);
                    canal->toggleOpen(true);
                }
            }
        }

        // Optional: shut down canals that are no longer needed
        for (auto* canal : canals) {
            Region* dest = canal->destinationRegion;
            if (dest->waterLevel >= dest->waterNeed || dest->isFlooded) {
                canal->toggleOpen(false);
                canal->setFlowRate(0.0);
            }
        }

        manager.nexthour();
    }
}
