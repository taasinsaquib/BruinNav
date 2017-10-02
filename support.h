//
//  support.h
//  Project4b
//
//  Created by Taasin Saquib on 3/14/17.
//  Copyright © 2017 Taasin Saquib. All rights reserved.
//


#ifndef support_h
#define support_h

#include "provided.h"

bool operator>(const GeoCoord& g, const GeoCoord& other) ;

bool operator<(const GeoCoord& g, const GeoCoord& other) ;

bool operator==(const GeoCoord& g, const GeoCoord& other) ;

bool operator!=(const GeoCoord& g, const GeoCoord& other) ;

bool operator==(const StreetSegment& s, const StreetSegment& other) ;

#endif /* support_h */
