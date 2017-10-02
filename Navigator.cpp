#include "provided.h"
#include "support.h"
#include "MyMap.h"
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
    
    //store the attraction and segment mappers
    AttractionMapper m_attractionM;
    SegmentMapper m_segmentM;
    
    //node data structure to hold navsegments
    struct Node{
        
        Node(GeoCoord pos, Node* n, GeoCoord end, StreetSegment ss){
            m_ss = ss;
            m_gc = pos;
            m_end = end;
            m_parentNode = n;
            
            if(n == nullptr)
                m_dist =0;
            else
                m_dist = n->m_dist;
            
            priority = calcPriority();
        }
        
        StreetSegment m_ss;
        GeoCoord m_gc;
        GeoCoord m_end;
        Node* m_parentNode;
        double m_dist;
        double m_h;
        double priority;
        vector<NavSegment> m_navs;
        
        double calcPriority(){
            m_h = distanceEarthMiles(m_gc, m_end);
            return m_dist + m_h;
        }
    };
    
    //compare two nodes to see if they are the same
    bool isSame(Node* n1, Node* n2) const{
        return n1->m_gc == n2->m_gc;
    }
    
    //syntax to sort priority queue by calculated priority
    struct cmp{
        bool operator()(Node* a, Node* b){
            return a->priority > b->priority;
        }
    };
    
    string getDirection(GeoCoord g1, GeoCoord g2) const;
    string getTurnDirection(const GeoSegment& line1, const GeoSegment& line2) const;
};

NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

//return the direction betwwen two geocoords
string NavigatorImpl::getDirection(GeoCoord s, GeoCoord e) const{
    
    GeoSegment g;
    g.start = s;
    g.end = e;
    
    double angle = angleOfLine(g);
    string str = "";
    
    if(0 <= angle && angle <= 22.5)
        return "east";
    if(22.5 <= angle && angle <= 67.5)
        return "northeast";
    if(67.5 <= angle && angle <= 112.5)
        return "north";
    if(112.5 <= angle && angle <= 157.5)
        return "northwest";
    if(157.5 <= angle && angle <= 202.5)
        return "west";
    if(202.5 <= angle && angle <= 247.5)
        return "southwest";
    if(247.5 <= angle && angle <= 292.5)
        return "south";
    if(292 <= angle && angle <= 337.5)
        return "southeast";
    if(337.5 <= angle && angle<= 360)
        return "east";
    
    return str;
}

//get turn direction between two geosegments
string NavigatorImpl::getTurnDirection(const GeoSegment& line1, const GeoSegment& line2) const{
    
    double i = angleBetween2Lines(line1, line2);
    
    if(0 <= i && i < 180)
        return "left";
    
    return "right";
}

bool NavigatorImpl::loadMapData(string mapFile)
{
    MapLoader ml;   //load the map with mapLoader
    if(!ml.load(mapFile))
        return false;
    
    m_attractionM.init(ml); //load attraction mapper
    m_segmentM.init(ml);    //load segment mapper
    
    return true;
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
    GeoCoord s,e;   //store start and end geocoords, if they exist
    
    if(!m_attractionM.getGeoCoord(start, s))
        return NAV_BAD_SOURCE;
    
    if(!m_attractionM.getGeoCoord(end, e))
        return NAV_BAD_DESTINATION;
    
    //get start and ending street segments
    vector<StreetSegment> firstseg = m_segmentM.getSegments(s);
    vector<StreetSegment> lastseg = m_segmentM.getSegments(e);
    
    //Create a Node containing the start state Node_start
    Node* Node_start = new Node(s, nullptr,e, firstseg[0]);
    
    //Create a Node containing the goal state Node_goal
    Node* Node_goal = new Node(e, nullptr,e, lastseg[0]);
    
    //if start and end are on same segment
    if(firstseg[0] == lastseg[0]){
        NavSegment first;   //create the first navsegment
        first.m_command = NavSegment::PROCEED;
        first.m_direction = getDirection(s, e);
        first.m_streetName =  firstseg[0].streetName;
        first.m_distance = distanceEarthKM(s, e);
        first.m_geoSegment = GeoSegment(s, e);
        directions.push_back(first);
        return NAV_SUCCESS;
    }
    
    //Create OPEN, CLOSED list for A*
    vector<Node*> OPEN;
    vector<Node*> CLOSED;
    
    //priority queue to choose most efficient geocoord to move to
    priority_queue<Node*, vector<Node*>, cmp> pq;
    
    //Put Node_start on the CLOSED list
    CLOSED.push_back(Node_start);
    
    //push start and end coord of first segment onto priority queue
    Node* n1 = new Node(firstseg[0].segment.start, Node_start, e, firstseg[0]);
    Node* n2 = new Node(firstseg[0].segment.end, Node_start, e, firstseg[0]);
    
    firstseg.clear();
    lastseg.clear();
    
    pq.push(n1);
    pq.push(n2);
    
    OPEN.push_back(n1);
    OPEN.push_back(n2);
    
    //store current node from top of pq
    Node* curr = pq.top();
    
    //store start and end of segment that destination is on to know when we are done
    GeoCoord lastgc1 = lastseg[0].segment.start;
    GeoCoord lastgc2 = lastseg[0].segment.end;
    
    NavSegment first;   //create the first navsegment
    first.m_command = NavSegment::PROCEED;
    first.m_direction = getDirection(s,curr->m_gc);
    first.m_streetName =  curr->m_ss.streetName;
    first.m_distance = distanceEarthMiles(s,curr->m_gc);
    first.m_geoSegment = GeoSegment(s, curr->m_gc);

    curr->m_navs.push_back(first);
    
    bool reachedEnd = false;    //check if the end coordinate has been reached
    Node* last = nullptr;   //use this pointer to keep track of the last node
    
    //while the OPEN list is not empty
    while (OPEN.size() > 0)
    {
        //get lowest priority from OPEN list
        curr = pq.top();
        pq.pop();
        GeoCoord currGC = curr->m_gc; //track which geocoord nav is currently on
        
        //move top of pq from OPEN to CLOSED
        for(vector<Node*>::iterator it = OPEN.begin(); it != OPEN.end();){
            if(isSame(curr,*it)){
                OPEN.erase(it);
                CLOSED.push_back(*it);
                break;
            }
            else
                it++;
        }
        
        //if curr is the same state as Node_goal we are done
        if(currGC == lastgc1 || currGC == lastgc2){
            
            //if destination segment is right after a turn
            if(curr->m_ss.streetName != lastseg[0].streetName){
                NavSegment nav;
                nav.m_command = NavSegment::TURN;
                nav.m_direction = getTurnDirection(curr->m_ss.segment, lastseg[0].segment);
                nav.m_streetName = lastseg[0].streetName;
                nav.m_geoSegment = GeoSegment(currGC, currGC);
                curr->m_ss = lastseg[0];
                curr->m_navs.push_back(nav);
            }
            
            //create the proceed navsegment that comes after a turn
            NavSegment n;
            n.m_command = NavSegment::PROCEED;
            n.m_direction = getDirection(currGC,e);
            n.m_streetName =  Node_goal->m_ss.streetName;
            n.m_distance = distanceEarthMiles(currGC,e);
            n.m_geoSegment = GeoSegment(currGC,e);
            
            last = curr;
            curr->m_navs.push_back(n);
            reachedEnd = true;
            break;  //break from the while loop
        }
        
        //get segments that connect to current geocoord
        vector<StreetSegment> successors = m_segmentM.getSegments(currGC);
        
        //create nodes for each possible segment and push to pq
        for(int i = 0; i < successors.size(); i++){
            
            GeoCoord gc;    //get the coord that is not currGC from possible segment
            if(currGC == successors[i].segment.start)
                gc = successors[i].segment.end;
            else
                gc = successors[i].segment.start;
            
            Node* n = new Node(gc, curr, e, successors[i]);
            bool found = false;
            
            //check if n is on CLOSED list
            for(vector<Node*>::iterator it = CLOSED.begin(); it != CLOSED.end(); it++){
                if ( isSame(*it, n)){
                    found = true;
                    break;
                }
            }
            
            //if not found on CLOSED list, check if its in OPEN list
            if(!found){
                for(vector<Node*>::iterator it = OPEN.begin(); it != OPEN.end(); it++){
                    if ( isSame(*it, n)){
                        found = true;
                        if(n->m_dist > (*it)->m_dist){
                            (*it)->m_parentNode = curr;
                            (*it)->calcPriority();
                            pq.push(*it);
                            
                            break;
                        }
                    }
                }
            }
            
            //only push back if not found in either list
            if(!found){
                OPEN.push_back(n);
                pq.push(n);
            }
            else
                delete n;
        }
        
        bool found2 = false;
        
        //get node with highest priority from pq
        Node* top = pq.top();
        
        NavSegment nav; //create navsegment with correct command
        
        if(top->m_ss.streetName != curr->m_ss.streetName){
            nav.m_command = NavSegment::TURN;
            
            GeoCoord g1, g2;
            GeoSegment s1 = curr->m_ss.segment;
            GeoSegment s2 = top->m_ss.segment;
            
            if(s1.start == currGC)
                g1 = s1.end;
            else
                g1 = s1.start;
            
            if(s2.start == currGC)
                g2 = s2.end;
            else
                g2 = s2.start;
            
            GeoSegment s3(g1, currGC);
            GeoSegment s4(currGC, g2);
            
            nav.m_direction = getTurnDirection(s3, s4);
            nav.m_streetName = top->m_ss.streetName;
            nav.m_geoSegment = GeoSegment(currGC, currGC);
            curr->m_ss = top->m_ss;
            curr->m_navs.push_back(nav);
            
            NavSegment nav2;
            top = pq.top();
            nav2.m_command = NavSegment::PROCEED;
            nav2.m_direction = getDirection(currGC,top->m_gc);
            nav2.m_streetName =  top->m_ss.streetName;
            nav2.m_distance = distanceEarthMiles(currGC,top->m_gc);
            nav2.m_geoSegment = GeoSegment(currGC, top->m_gc);
            curr->m_navs.push_back(nav2);
            
        }
        else{
            nav.m_command = NavSegment::PROCEED;
            nav.m_direction = getDirection(currGC,top->m_gc);
            nav.m_streetName =  top->m_ss.streetName;
            nav.m_distance = distanceEarthMiles(currGC,top->m_gc);
            nav.m_geoSegment = GeoSegment(currGC, top->m_gc);
            curr->m_navs.push_back(nav);
        }
    }
    
    if(reachedEnd){
        vector<Node> fin;  //follow node pointers, store in a vector
        while(last != nullptr){
            fin.push_back(*last);
            last = last->m_parentNode;
        }
        
        reverse(fin.begin(), fin.end());    //nodes are in reverse order
        
        //clear directions
        directions.clear();
        
        //get navsegments stored in each node
        for(int i = 0; i < fin.size(); i++){
            Node c = fin[i];
            for(int j = 0; j < c.m_navs.size(); j++){
                directions.push_back(c.m_navs[j]);
            }
        }
        
        //clear lists to prevent memory leaks
        fin.clear();
        
        for(vector<Node*>::iterator it = CLOSED.begin(); it != CLOSED.end();++it){

                delete *it;
            
        }
        /*
        for(vector<Node*>::iterator it = OPEN.begin(); it != OPEN.end();++it){
            if(
            delete *it;
        }
        */
        //CLOSED.clear();
        //OPEN.clear();

        //delete Node_start;
        //delete Node_goal;
        delete n1;
        //delete n2;
        
        //delete curr;
        //delete last;
        
        return NAV_SUCCESS;
    }
    return NAV_NO_ROUTE;
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}
