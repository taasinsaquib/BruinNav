//
//  support.cpp
//  Project4b
//
//  Created by Taasin Saquib on 3/14/17.
//  Copyright © 2017 Taasin Saquib. All rights reserved.
//

#include <stdio.h>
#include "support.h"

bool operator>(const GeoCoord& g, const GeoCoord& other) {
    return (g.longitude > other.longitude);
}

bool operator<(const GeoCoord& g, const GeoCoord& other){
        return !(g.longitude > other.longitude);
}

bool operator==(const GeoCoord& g, const GeoCoord& other){
    return (other.latitude == g.latitude && other.longitude == g.longitude);
}

bool operator!=(const GeoCoord& g, const GeoCoord& other){
    if(other.latitude != g.latitude || other.longitude != g.longitude)
        return true;
    return false;
}

bool operator==(const StreetSegment& s, const StreetSegment& other){
    return ((other.segment.start == s.segment.start && other.segment.end == s.segment.end) || (other.segment.start == s.segment.end && other.segment.end == s.segment.start));
}
