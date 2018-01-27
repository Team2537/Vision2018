#include "VisionPacket.hpp"

void VisionPacket::addObject(std::vector<Point> object){
    objects.push_back(object);
}

void VisionPacket::addObject(std::vector<cv::Point> convexHull){
    std::vector<Point> object;
    for(int p = 0; p < convexHull.size(); p++){
        Point point;
        point.x = convexHull[p].x;
        point.y = convexHull[p].y;
        object.push_back(point);
    }
    addObject(object);
}

/* The protocol looks like this:
 * >x0,y0|x1,y1#x0,y0|x1,y1>
 *
 * '>' separates packets
 * '#' separates convex hulls/objects
 * '|' separates points
 */
std::string VisionPacket::serialize(){
    std::stringstream serialization;
    serialization << ">";
    for(int y = 0; y < objects.size(); y++){
        /* object delimiter = #
         * we do not place the delimiter for the first object
         */
        if(y > 0){
            serialization << "#";
        }
        for(int x = 0; x < objects[y].size(); x++){
            serialization << objects[y][x].x << "," << objects[y][x].y;
            /* point delimiter = |
             * we do not place the delimiter for the last point
             */
            if(x < objects[y].size() - 1){
                serialization << "|";
            }
        }
    }
    return serialization.str();
}

//static VisionPacket* deserialize(std::string serialization){
//    /* we remove the < > characters that open and close a serialization string */
//    serialization.erase(0, 1);
//    serialization.erase(serialization.length() - 1, 1);
//
//    VisionPacket* visionPacket = new VisionPacket();
//    while(serialization.find("!") != std::string::npos){
//        std::string x = serialization.substr(0, serialization.find(":"));
//        std::string y = serialization.substr(serialization.find(":") + 1, serialization.find("|") - serialization.find(":") + 1);
//        visionPacket->addPoint(atoi(x.c_str()), atoi(y.c_str()));
//    }
//    return visionPacket;
//}
