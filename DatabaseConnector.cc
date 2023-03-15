#include <iostream>
#include <string>
#include <boost/process.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <chrono>
#include <vector>

namespace bp = boost::process;

class GDB_Topology {
public:

    GDB_Topology() {
        bp::child cypher_shell("cypher-shell", bp::std_out > "gcc_out.log", bp::std_in < cypher_in);
    }

    void createSwitch(bp::opstream &in, int dpid) {
        in << "CREATE (:Switch {dpid:" << dpid << "});";
    }

    void createLink(bp::opstream &in, int dpid_from, int dpid_to, int weight) {
        in << "MATCH (from:Switch), (to:Switch) WHERE from.dpid = " << dpid_from << " and to.dpid = " << dpid_to << " CREATE (from)-[:Link {mb:" << weight << "}]->(to), (from)<-[:Link {mb:" << weight << "}]-(to);";
    }

    void deleteSwitch(bp::opstream &in, int dpid) {
        in << "MATCH (s: Switch) WHERE s.dpid = " << dpid << "DETACH DELETE s;";
    }

    void deleteLink(bp::opstream &in, int dpid_from, int dpid_to) {
        in << "MATCH (from:Switch)-[r:Link]-(to:Switch) WHERE from.dpid = " << dpid_from << " and to.dpid = " << dpid_to << " delete r;";
    }  
    
private:
    bp::opstream cypher_in;

}

int main()
{
    bp::opstream cypher_in;
    bp::child cypher_shell("cypher-shell", bp::std_out > "gcc_out.log", bp::std_in < cypher_in);





    // cypher_in << "CALL gds.graph.project('myGraph', 'Location', 'ROAD', { relationshipProperties: 'cost' });" << std::endl;
    // cypher_in.flush();
    createSwitch(cypher_in, 1);
    createSwitch(cypher_in, 2);
    createLink(cypher_in, 1, 2, 10);
    // std::chrono::high_resolution_clock::time_point start_time, end_time;
    // start_time = std::chrono::high_resolution_clock::now();
    // cypher_in << "MATCH (source:Location {name: 'A'})CALL gds.allShortestPaths.dijkstra.stream('myGraph', {    sourceNode: source,    relationshipWeightProperty: 'cost'})YIELD index, path RETURN index, path;" << std::endl;
    // cypher_in.flush();
    // end_time = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
    // std::cout << "Program duration: " << duration.count() << " nanoseconds\n";

    // cypher_shell.wait();
    cypher_shell.wait_for(std::chrono::seconds(3));

    return 0;
}
