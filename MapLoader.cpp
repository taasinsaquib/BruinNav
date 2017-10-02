#include "provided.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

class MapLoaderImpl
{
public:
    MapLoaderImpl();
    ~MapLoaderImpl();
    bool load(string mapFile);
    size_t getNumSegments() const;
    bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
    vector<StreetSegment> m_segments;
    int m_nSegments;
};

MapLoaderImpl::MapLoaderImpl()
{
    m_nSegments = 0;
}

MapLoaderImpl::~MapLoaderImpl()
{
}

bool MapLoaderImpl::load(string mapFile)
{
    //use ifstream to load file
    ifstream ifs(mapFile);
    
    
    string str; //store current line in a string variable
    //StreetSegment seg;
    
    int linesToRead = 0;    //keep track of how many lines to read for current StreetSegment
    int currSegment = -1;
    
    while (getline(ifs, str)) {
        
        if(linesToRead == 0){
            StreetSegment s;
            m_segments.push_back(s);
            m_nSegments++;
            linesToRead = 3;
            currSegment++;
        }
        
        switch (linesToRead) {
                
            case 1:{
                int i = stoi(str);
                
                if(i != 0){     //then there are attractions to add
                    linesToRead -= i;
                }
                
                linesToRead--;
                break;
            }
            case 2:{
                
                string lat, lng;
                bool blat = false;
                bool blng = false;
                vector<GeoCoord> coords;
                
                for(int i = 0; i < str.length() + 2; i++){  //loop through string to get coordinates
                    
                    if(blat && blng){
                        GeoCoord G(lat, lng);
                        coords.push_back(G);
                        lat = "";
                        lng = "";
                        blat = false;
                        blng = false;
                    }
                    
                    if( (str[i] == ' ' && str[i-1] != ',') || i == str.length())
                        blng = true;
                    
                    if(str[i] == ',')
                        blat = true;
                    
                    if(str[i] != ' ' && i < str.length()){
                        if(!blat)   //keep concatenating if latitude isn't finished
                            lat += str[i];
                        else if(str[i] != ',')        //concatenate to long
                            lng += str[i];
                    }
                }
                
                GeoSegment GS(coords[0], coords[1]);
                m_segments[currSegment].segment = GS;
                
                linesToRead--;
                break;
            }
                
            case 3:
                m_segments[currSegment].streetName = str;
                linesToRead--;
                break;
                
            default:{    //create the attractions
                
                Attraction a;
                string s, s1, s2;
                bool n = false;
                bool c1 = false;
                
                for(int i = 0; i < str.length(); i++){
                    
                    if(str[i] == '|' && !n){  //finished getting name of attraction
                        a.name = s;
                        n = true;
                    }
                    
                    else{
                        
                        if(str[i] == ',' && n){  //get to comma, first coordinate is done
                            c1 = true;
                        }
                        else{
                            if(!n){              //use s to store name and first coordinate
                                //s += tolower(str[i]);
                                s += str[i];
                            }
                            else if(n && !c1)
                                s1 += str[i];
                            else if(str[i] != ' ')  //concatenate to s2 if name and first coordinate are done
                                s2 += str[i];
                            
                        }
                    }
                }
                
                a.geocoordinates = GeoCoord(s1,s2);
                (m_segments[currSegment].attractions).push_back(a);
                
                linesToRead++;
                break;
            }
        }
    }
    
    return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
    return m_nSegments;
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
    if(segNum > getNumSegments() - 1)
        return false;
    
    seg = m_segments[segNum];
    return true;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
    m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
    delete m_impl;
}

bool MapLoader::load(string mapFile)
{
    return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
    return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
    return m_impl->getSegment(segNum, seg);
}
