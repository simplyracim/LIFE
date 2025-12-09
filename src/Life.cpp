#include <iomanip>
#include <filesystem>
#include <random>
#include <algorithm>
#include "Life.h"
#include <fstream>
#include <iostream>
#include <sstream>

Life::Life(int sizeX, int sizeY, int sizeZ)
    : m_sizeX(sizeX), m_sizeY(sizeY), m_sizeZ(sizeZ)
{
    m_grid.resize(m_sizeX * m_sizeY * m_sizeZ, false);
    m_next.resize(m_sizeX * m_sizeY * m_sizeZ, false);
}

void Life::randomize() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (size_t i = 0; i < m_grid.size(); ++i)
        m_grid[i] = (dis(gen) > 0.7f);
}

void Life::update() {
    if (m_mode == LifeMode::Current3D) {
        for (int z = 0; z < m_sizeZ; ++z)
            for (int y = 0; y < m_sizeY; ++y)
                for (int x = 0; x < m_sizeX; ++x) {
                    int neighbors = countNeighbors(x, y, z);
                    bool currentState = getCell(x, y, z);
                    m_next[getIndex(x, y, z)] = currentState ? (neighbors == 5 || neighbors == 6) : (neighbors == 5);
                }
    } 
    else if (m_mode == LifeMode::Conway2D) {
        for (int z = 0; z < m_sizeZ; ++z)
            for (int y = 0; y < m_sizeY; ++y)
                for (int x = 0; x < m_sizeX; ++x) {
                    int neighbors = countNeighbors2D(x, y, z);
                    bool currentState = getCell(x, y, z);
                    m_next[getIndex(x, y, z)] = currentState ? (neighbors == 2 || neighbors == 3) : (neighbors == 3);
                }
    }
    else if (m_mode == LifeMode::Custom3D) {
        for (int z = 0; z < m_sizeZ; ++z)
            for (int y = 0; y < m_sizeY; ++y)
                for (int x = 0; x < m_sizeX; ++x) {
                    int neighbors = countNeighbors(x, y, z);
                    bool currentState = getCell(x, y, z);
                    m_next[getIndex(x, y, z)] = currentState ? (neighbors >= 4 && neighbors <= 6) : (neighbors == 5);
                }
    }
    else if (m_mode == LifeMode::Custom2D) {
        for (int z = 0; z < m_sizeZ; ++z)
            for (int y = 0; y < m_sizeY; ++y)
                for (int x = 0; x < m_sizeX; ++x) {
                    int neighbors = countNeighbors2D(x, y, z);
                    bool currentState = getCell(x, y, z);
                    m_next[getIndex(x, y, z)] = currentState ? (neighbors == 2 || neighbors == 4) : (neighbors == 3 || neighbors == 6);
                }
    }

    std::swap(m_grid, m_next);
}

void Life::clear() {
    std::fill(m_grid.begin(), m_grid.end(), false);
}

bool Life::getCell(int x, int y, int z) const {
    if (m_toric) {
        x = (x + m_sizeX) % m_sizeX;
        y = (y + m_sizeY) % m_sizeY;
        z = (z + m_sizeZ) % m_sizeZ;
        return m_grid[getIndex(x, y, z)];
    }
    return isValidPosition(x, y, z) ? m_grid[getIndex(x, y, z)] : false;
}

void Life::setCell(int x, int y, int z, bool state) {
    if (isValidPosition(x, y, z))
        m_grid[getIndex(x, y, z)] = state;
}

float Life::computeDensity(int x, int y, int z, int radius) const {
    int count = 0;
    int maxNeighbors = 0;

    for (int dz = -radius; dz <= radius; ++dz)
        for (int dy = -radius; dy <= radius; ++dy)
            for (int dx = -radius; dx <= radius; ++dx) {
                int nx = x + dx, ny = y + dy, nz = z + dz;
                if (m_toric || isValidPosition(nx, ny, nz)) {
                    maxNeighbors++;
                    if (getCell(nx, ny, nz)) count++;
                }
            }

    return maxNeighbors > 0 ? float(count) / float(maxNeighbors) : 0.0f;
}

bool Life::isValidPosition(int x, int y, int z) const {
    return x >= 0 && x < m_sizeX &&
           y >= 0 && y < m_sizeY &&
           z >= 0 && z < m_sizeZ;
}

int Life::countNeighbors(int x, int y, int z) const {
    int count = 0;
    for (int dz = -1; dz <= 1; ++dz)
        for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0 && dz == 0) continue;
                int nx = x + dx, ny = y + dy, nz = z + dz;

                if (m_toric) {
                    nx = (nx + m_sizeX) % m_sizeX;
                    ny = (ny + m_sizeY) % m_sizeY;
                    nz = (nz + m_sizeZ) % m_sizeZ;
                }

                if (isValidPosition(nx, ny, nz) && getCell(nx, ny, nz))
                    count++;
            }
    return count;
}

int Life::countNeighbors2D(int x, int y, int z) const {
    int count = 0;
    for (int dy = -1; dy <= 1; ++dy)
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx, ny = y + dy;

            if (m_toric) {
                nx = (nx + m_sizeX) % m_sizeX;
                ny = (ny + m_sizeY) % m_sizeY;
            }

            if (isValidPosition(nx, ny, z) && getCell(nx, ny, z))
                count++;
        }
    return count;
}

int Life::getIndex(int x, int y, int z) const {
    return z * m_sizeX * m_sizeY + y * m_sizeX + x;
}

bool Life::loadFromFile(const std::string& filename) {
    std::ifstream file("IO/" + filename + "/initial.txt");
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    clear();
    std::string line;

    if (!std::getline(file, line)) return false;
    std::string patternName = line;

    if (!std::getline(file, line)) return false;
    size_t p1 = line.find('x');
    size_t p2 = line.rfind('x');
    if (p1 == std::string::npos || p2 == std::string::npos || p1 == p2) {
        std::cerr << "Invalid grid size format: " << line << std::endl;
        return false;
    }

    m_sizeX = std::stoi(line.substr(0, p1));
    m_sizeY = std::stoi(line.substr(p1 + 1, p2 - p1 - 1));
    m_sizeZ = std::stoi(line.substr(p2 + 1));

    m_grid.assign(m_sizeX * m_sizeY * m_sizeZ, false);
    m_next.assign(m_sizeX * m_sizeY * m_sizeZ, false);

    int z = -1;
    int y = 0;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line.size() >= 2 && line[0] == 'L' && isdigit(line[1])) {
            z = std::stoi(line.substr(1));
            y = 0;
            continue;
        }

        if (z < 0 || z >= m_sizeZ) continue;
        if (y >= m_sizeY) continue;

        for (int x = 0; x < (int)line.size() && x < m_sizeX; ++x) {
            char c = line[x];
            if (c == '0' || c == '1') setCell(x, y, z, c == '1');
        }
        ++y;
    }

    std::cout << "Loaded pattern '" << patternName
              << "' of size " << m_sizeX << "x" << m_sizeY << "x" << m_sizeZ
              << std::endl;

    return true;
}

bool Life::saveToFile(const std::string& name, int step) {
    std::string dir = "IO/" + name + "/log";
    std::filesystem::create_directories(dir);

    std::ostringstream oss;
    oss << dir << "/" << std::setw(5) << std::setfill('0') << step << ".txt";
    std::string filepath = oss.str();

    std::ofstream out(filepath);
    if (!out.is_open()) {
        std::cerr << "Failed to write file: " << filepath << std::endl;
        return false;
    }

    // We can use a reference to a common output stream
    std::ostream& console = std::cout;

    // Write header
    out << name << "\n";
    console << name << "\n";

    out << m_sizeX << "x" << m_sizeY << "x" << m_sizeZ << "\n";
    console << m_sizeX << "x" << m_sizeY << "x" << m_sizeZ << "\n";

    for (int z = 0; z < m_sizeZ; ++z) {
        out << "L" << z << "\n";
        console << "L" << z << "\n";

        for (int y = 0; y < m_sizeY; ++y) {
            for (int x = 0; x < m_sizeX; ++x) {
                char c = getCell(x, y, z) ? '1' : '0';
                out << c;
                console << c;
            }
            out << "\n";
            console << "\n";
        }
    }

    out.close();

    return true;
}
