#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include "stdlib.h"

typedef struct{
    uint16_t x;
    uint16_t y;
} Point;

class VisionPacket{

public:

    std::string serialize();

    void addObject(std::vector<Point> object);

    void addObject(std::vector<cv::Point> convexHull);

    static VisionPacket* deserialize(std::string serialization);

protected:

    std::vector<std::vector<Point>> objects;

};
