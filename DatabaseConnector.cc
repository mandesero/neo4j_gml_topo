#include <iostream>
#include <string>
#include <boost/process.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <chrono>
#include <vector>

#include <time.h>

namespace bp = boost::process;

class GDB_Topology {
public:

    GDB_Topology() {
        cypher_shell = bp::child("cypher-shell", bp::std_out > "gcc_out.log", bp::std_in < cypher_in);
    }

    void createSwitch(int dpid) {
        cypher_in << "CREATE (:Switch {dpid:" << dpid << "});";
    }

    void createLink(int dpid_from, int dpid_to, int weight) {
        cypher_in << "MATCH (from:Switch), (to:Switch) WHERE from.dpid = " << dpid_from << " and to.dpid = " << dpid_to << " CREATE (from)-[:Link {mb:" << weight << "}]->(to), (from)<-[:Link {mb:" << weight << "}]-(to);";
    }

    void deleteSwitch(int dpid) {
        cypher_in << "MATCH (s: Switch) WHERE s.dpid = " << dpid << "DETACH DELETE s;";
    }

    void deleteLink(int dpid_from, int dpid_to) {
        cypher_in << "MATCH (from:Switch)-[r:Link]-(to:Switch) WHERE from.dpid = " << dpid_from << " and to.dpid = " << dpid_to << " delete r;";
    }


    void shortestPath(int dpid_from, int dpid_to) {

        bp::ipstream is;
        bp::opstream os;
        bp::child c("cypher-shell", bp::std_in < os, bp::std_out > "shortest.log");

        // Send your query to cypher-shell
        auto start = std::chrono::high_resolution_clock::now();

        os << "MATCH (from:Switch {dpid: " << dpid_from << "})," << "(to: Switch {dpid: " << dpid_to << "}), p = shortestPath((from)-[:Link*]-(to)) return p;";

        c.wait_for(std::chrono::milliseconds(300));
    
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        std::cout << "Query execution time: " << duration.count() << " microseconds" << std::endl;

        // std::chrono::high_resolution_clock::time_point start_time, end_time;
        // start_time = std::chrono::high_resolution_clock::now();

        // cypher_in << "MATCH (from:Switch {dpid: " << dpid_from << "})," << "(to: Switch {dpid: " << dpid_to << "}), p = shortestPath((from)-[:Link*]-(to)) return p;";

        // end_time = std::chrono::high_resolution_clock::now();
        // auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        // std::cout << "Find shortestPath time: " << duration.count() << " nanoseconds\n";
    }

    ~GDB_Topology() {
        cypher_in << ":exit";
        cypher_shell.wait_for(std::chrono::seconds(3));
    }
    
private:
    bp::opstream cypher_in;
    bp::ipstream cypher_out;
    bp::child cypher_shell;
};

int main()
{
    // bp::opstream cypher_in;
    // bp::child cypher_shell("cypher-shell", bp::std_out > "gcc_out.log", bp::std_in < cypher_in);

    // cypher_in << "CALL gds.graph.project('myGraph', 'Location', 'ROAD', { relationshipProperties: 'cost' });" << std::endl;
    // cypher_in.flush();


    // int c1, c2, w;
    // while (true) {
    //     std::cout << "Input" << std::endl;
    //     std::cin >> c1 >> c2 >> w;
    //     if (c1 == 0)
    //         break;
    //     GDB_Topology tmp;
    //     tmp.createSwitch(c1);
    //     tmp.createSwitch(c2);
    //     tmp.createLink(c1, c2, w);
    //     std::cout << "1 and 8" << std::endl;
    //     tmp.shortestPath(1, 8);
    // }

    // srand(time(NULL));
    // GDB_Topology tmp;
    // for (int i = 0; i < 1000; ++i) {
    //     tmp.createSwitch(i + 1);
    //     for (int j = 0; j < i / 10; ++j) {
    //         tmp.createLink(i, j, rand() % 1000 + 100);
    //     }
    // }

    // std::cout << "1 and 8" << std::endl;

    // std::cout << "14 and 88" << std::endl;
    // tmp.shortestPath(14, 88);

    // std::cout << "97 and 4" << std::endl;
    // tmp.shortestPath(97, 4);



    // std::chrono::high_resolution_clock::time_point start_time, end_time;
    // start_time = std::chrono::high_resolution_clock::now();
    // cypher_in << "MATCH (source:Location {name: 'A'})CALL gds.allShortestPaths.dijkstra.stream('myGraph', {    sourceNode: source,    relationshipWeightProperty: 'cost'})YIELD index, path RETURN index, path;" << std::endl;
    // cypher_in.flush();
    // end_time = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
    // std::cout << "Program duration: " << duration.count() << " nanoseconds\n";


    bp::ipstream os;

    std::string s1;
    std::string s2;
    std::cin >> s1 >> s2;
    std::string program = "python3 my_neo4j.py " + s1 + " " + s2;
    std::chrono::high_resolution_clock::time_point start_time, end_time;
    start_time = std::chrono::high_resolution_clock::now();
    bp::child shortestPath(program);

    shortestPath.wait();
    end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Program duration: " << duration.count() << " microseconds\n";






    return 0;
}
