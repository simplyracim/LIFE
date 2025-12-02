#include "Life.h"
#include <random>
#include <algorithm>

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

    // Use index-based loop instead of auto& to avoid std::vector<bool> proxy issue
    for (size_t i = 0; i < m_grid.size(); ++i) {
        m_grid[i] = (dis(gen) > 0.7f);
    }
}

void Life::update() {
    for (int z = 0; z < m_sizeZ; ++z) {
        for (int y = 0; y < m_sizeY; ++y) {
            for (int x = 0; x < m_sizeX; ++x) {
                int neighbors = countNeighbors(x, y, z);
                bool currentState = getCell(x, y, z);

                if (currentState) {
                    m_next[getIndex(x, y, z)] = (neighbors == 5 || neighbors == 6);
                } else {
                    m_next[getIndex(x, y, z)] = (neighbors == 5);
                }
            }
        }
    }
    std::swap(m_grid, m_next);
}

void Life::clear() {
    std::fill(m_grid.begin(), m_grid.end(), false);
}

bool Life::getCell(int x, int y, int z) const {
    if (isValidPosition(x, y, z))
        return m_grid[getIndex(x, y, z)];
    return false;
}

void Life::setCell(int x, int y, int z, bool state) {
    if (isValidPosition(x, y, z))
        m_grid[getIndex(x, y, z)] = state;
}

float Life::computeDensity(int x, int y, int z, int radius) const {
    int count = 0;

    // Loop through all neighbors including the center
    for (int dz = -radius; dz <= radius; ++dz)
        for (int dy = -radius; dy <= radius; ++dy)
            for (int dx = -radius; dx <= radius; ++dx) {
                int nx = x + dx, ny = y + dy, nz = z + dz;
                if (!isValidPosition(nx, ny, nz)) continue;
                if (getCell(nx, ny, nz)) count++;
            }

    // Max possible neighbors including the center
    int maxNeighbors = 0;
    for (int dz = -radius; dz <= radius; ++dz)
        for (int dy = -radius; dy <= radius; ++dy)
            for (int dx = -radius; dx <= radius; ++dx) {
                int nx = x + dx, ny = y + dy, nz = z + dz;
                if (isValidPosition(nx, ny, nz)) maxNeighbors++;
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
                if (isValidPosition(nx, ny, nz) && getCell(nx, ny, nz))
                    count++;
            }
    return count;
}

int Life::getIndex(int x, int y, int z) const {
    return z * m_sizeX * m_sizeY + y * m_sizeX + x;
}
