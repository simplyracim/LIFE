#pragma once
#include <vector>
#include <string>

enum class LifeMode {
    Current3D,   // Your current 3D rules
    Conway2D,    // Classic Conway applied per XY plane independently
    Custom3D,    // New 3D variant rules
    Custom2D     // New 2D variant rules
};

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

    void setMode(LifeMode mode) { m_mode = mode; }
    LifeMode getMode() const { return m_mode; }

    void setToric(bool toric) { m_toric = toric; }
    bool isToric() const { return m_toric; }

    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& baseName, int step);

private:
    int m_sizeX, m_sizeY, m_sizeZ;
    std::vector<bool> m_grid, m_next;
    LifeMode m_mode = LifeMode::Current3D;
    bool m_toric = false; // toric wrap-around flag

    bool isValidPosition(int x, int y, int z) const;
    int countNeighbors(int x, int y, int z) const;
    int countNeighbors2D(int x, int y, int z) const;
    int getIndex(int x, int y, int z) const;
};
