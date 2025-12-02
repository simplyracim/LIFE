#pragma once
#include <vector>
#include <glm/glm.hpp>

class Life {
public:
    Life(int sizeX, int sizeY, int sizeZ);

    void randomize();
    void update();
    void clear();

    bool getCell(int x, int y, int z) const;
    void setCell(int x, int y, int z, bool state);

    float computeDensity(int x, int y, int z, int radius) const;

    int getSizeX() const { return m_sizeX; }
    int getSizeY() const { return m_sizeY; }
    int getSizeZ() const { return m_sizeZ; }

private:
    int m_sizeX, m_sizeY, m_sizeZ;
    std::vector<bool> m_grid, m_next;

    bool isValidPosition(int x, int y, int z) const;
    int countNeighbors(int x, int y, int z) const;
    int getIndex(int x, int y, int z) const;
};
