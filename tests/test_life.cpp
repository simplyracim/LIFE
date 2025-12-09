#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "Life.h"
#include <cstdio>    // for std::remove
#include <fstream>
#include <iostream>
#include <filesystem>

TEST_CASE("Life grid initialization and size") {
    std::cout << "[TEST] Grid initialization and size" << std::endl;
    Life life(4, 5, 3);

    for (int z = 0; z < 3; ++z)
        for (int y = 0; y < 5; ++y)
            for (int x = 0; x < 4; ++x)
                REQUIRE(life.getCell(x, y, z) == false);
}

TEST_CASE("Life setCell and getCell") {
    std::cout << "[TEST] setCell and getCell" << std::endl;
    Life life(3, 3, 3);

    life.setCell(1, 1, 1, true);
    REQUIRE(life.getCell(1, 1, 1) == true);

    life.setCell(1, 1, 1, false);
    REQUIRE(life.getCell(1, 1, 1) == false);

    REQUIRE(life.getCell(-1, 0, 0) == false);
    REQUIRE(life.getCell(0, 3, 0) == false);
    REQUIRE(life.getCell(0, 0, 3) == false);
}

TEST_CASE("Life randomize") {
    std::cout << "[TEST] Randomize grid" << std::endl;
    Life life(4, 4, 4);
    life.randomize();

    bool hasAlive = false;
    for (int z = 0; z < 4; ++z)
        for (int y = 0; y < 4; ++y)
            for (int x = 0; x < 4; ++x)
                if (life.getCell(x, y, z))
                    hasAlive = true;

    REQUIRE(hasAlive == true); 
}

TEST_CASE("Life clear") {
    std::cout << "[TEST] Clear grid" << std::endl;
    Life life(3, 3, 3);

    life.setCell(1, 1, 1, true);
    life.clear();

    for (int z = 0; z < 3; ++z)
        for (int y = 0; y < 3; ++y)
            for (int x = 0; x < 3; ++x)
                REQUIRE(life.getCell(x, y, z) == false);
}

TEST_CASE("Life computeDensity") {
    std::cout << "[TEST] Compute density" << std::endl;
    Life life(3, 3, 3);

    REQUIRE(life.computeDensity(1, 1, 1, 1) == 0.0f);

    life.setCell(1, 1, 1, true);
    float density = life.computeDensity(1, 1, 1, 1);
    REQUIRE(density > 0.0f);
    REQUIRE(density <= 1.0f);

    life.setCell(0, 0, 0, true);
    float edgeDensity = life.computeDensity(0, 0, 0, 1);
    REQUIRE(edgeDensity > 0.0f);
    REQUIRE(edgeDensity <= 1.0f);
}

TEST_CASE("Life update Conway2D rules") {
    std::cout << "[TEST] Update Conway2D rules" << std::endl;
    Life life(3, 3, 1);
    life.setMode(LifeMode::Conway2D);

    life.setCell(0, 1, 0, true);
    life.setCell(1, 1, 0, true);
    life.setCell(2, 1, 0, true);

    life.update();

    REQUIRE(life.getCell(1, 0, 0) == true);
    REQUIRE(life.getCell(1, 1, 0) == true);
    REQUIRE(life.getCell(1, 2, 0) == true);

    REQUIRE(life.getCell(0, 1, 0) == false);
    REQUIRE(life.getCell(2, 1, 0) == false);
}

TEST_CASE("Life save and load") {
    std::cout << "[TEST] Save and load" << std::endl;

    const std::string patternName = "test_pattern";
    const std::string ioDir = "IO/" + patternName;
    const std::string initialFile = ioDir + "/initial.txt";

    // --- Prepare initial file for loadFromFile ---
    std::filesystem::create_directories(ioDir);
    std::ofstream initOut(initialFile);
    initOut << patternName << "\n";      // Pattern name
    initOut << "2x2x1\n";                 // Grid size
    initOut << "L0\n";                     // Layer 0
    initOut << "10\n";                     // Row 0
    initOut << "01\n";                     // Row 1
    initOut.close();
    std::cout << " - Initial pattern file created: " << initialFile << std::endl;

    // --- Load into Life instance ---
    Life loaded(1, 1, 1);
    std::cout << " - Loading into new Life instance..." << std::endl;
    REQUIRE(loaded.loadFromFile(patternName) == true);

    // --- Verify loaded cells ---
    REQUIRE(loaded.getCell(0, 0, 0) == true);
    REQUIRE(loaded.getCell(1, 1, 0) == true);
    REQUIRE(loaded.getCell(0, 1, 0) == false);
    REQUIRE(loaded.getCell(1, 0, 0) == false);

    std::cout << " - Load test passed." << std::endl;

    // Cleanup
    std::filesystem::remove_all(ioDir);
    std::cout << " - Temporary files cleaned." << std::endl;
}
