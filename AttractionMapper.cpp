#include "provided.h"
#include "MyMap.h"
#include <string>
using namespace std;

class AttractionMapperImpl
{
public:
    AttractionMapperImpl();
    ~AttractionMapperImpl();
    void init(const MapLoader& ml);
    bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
    MyMap<string, GeoCoord> m_map;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
    //loop through sgements in the maploader
    for(int i = 0; i < ml.getNumSegments(); i++){
        
        StreetSegment s;
        if(ml.getSegment(i, s)){    //if there is a StreetSegment, get it
            for(int j = 0; j < s.attractions.size(); j ++){
                
                string r;   //make the attraction name lowercase
                for(int k = 0; k < s.attractions[j].name.size(); k++){
                    r += tolower((s.attractions[j].name)[k]);
                }
                
                //associate the name to the map
                m_map.associate(r, s.attractions[j].geocoordinates);
            }
        }
    }
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
    //do a case insensitive search by making it lowercase
    string r;
    for(int k = 0; k < attraction.size(); k++){
        r += tolower((attraction)[k]);
    }
    
    //search the tree
    const GeoCoord* g = m_map.find(r);
    
    if(g != nullptr){
        gc = *g;
        return true;
    }
    
    return false;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
    m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
    delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
    m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
    return m_impl->getGeoCoord(attraction, gc);
}
