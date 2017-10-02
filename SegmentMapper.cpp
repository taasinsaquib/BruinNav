#include <stdio.h>
#include "MyMap.h"
#include "provided.h"
#include <vector>
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
    vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
    MyMap<GeoCoord, vector<StreetSegment>> m_map;
    
};

SegmentMapperImpl::SegmentMapperImpl(){
}

SegmentMapperImpl::~SegmentMapperImpl(){
}

void SegmentMapperImpl::init(const MapLoader& ml){
    
    //loop through all the segments
    for(int i = 0; i < ml.getNumSegments(); i++){
        
        StreetSegment s;    //get each segment
        if(ml.getSegment(i, s)){
            
            //map both the start and the end of the segment
            vector<StreetSegment>* start = m_map.find(s.segment.start);
            vector<StreetSegment>* end = m_map.find(s.segment.end);
            
            if(start == nullptr){   //if geocoord hasn't been mapped yet
                vector<StreetSegment> ss;
                ss.push_back(s);
                m_map.associate(s.segment.start, ss);
            }
            else{   //else just push it onto the list in the map
                (*m_map.find(s.segment.start)).push_back(s);
            }
            
            if(end == nullptr){   //if geocoord hasn't been mapped yet
                vector<StreetSegment> ss;
                ss.push_back(s);
                m_map.associate(s.segment.end, ss);
            }
            else{
                (*m_map.find(s.segment.end)).push_back(s);
            }
            
            //loop through each attraction
            vector<Attraction> a = s.attractions;
            for(int j = 0; j < a.size(); j++){
                
                //see if segment must be mapped or added to in the map if it already exaists
                vector<StreetSegment>* coord = m_map.find(a[j].geocoordinates);
                if(coord == nullptr){
                    vector<StreetSegment> ss;
                    ss.push_back(s);
                    m_map.associate(a[j].geocoordinates, ss);
                }
                else{
                    (*m_map.find(a[j].geocoordinates)).push_back(s);
                }
            }
        }
    }
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
    vector<StreetSegment> segments;
    segments.clear();
    
    //search map for geocoord, return empty vector if not found
    const vector<StreetSegment>* s = m_map.find(gc);
    if(s!= nullptr)
        return *s;
    
    return segments;
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
