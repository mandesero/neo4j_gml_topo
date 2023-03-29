#!/usr/bin/env python3

from neo4j import GraphDatabase
import sys
from config import NEO4J_URI, USER, PASSWORD


import time

def timer(func):
    def wrapped(*args, **kwargs):
        t0 = time.time()
        res = func(*args, **kwargs)
        return (time.time() - t0) * 1000, res
    return wrapped


def is_graph_project_exist(tx, name):
    query = f'''
        RETURN gds.graph.exists('{name}') as ex
    '''
    return list(tx.run(query))[0].get("ex")


def update_graph_project(tx, name="myGraph", update=False):

    query_1 = f'''CALL gds.graph.drop('{name}')'''

    query_2 = f'''CALL gds.graph.project(
        '{name}',
        'Switch',
        'Link',
        {{
            relationshipProperties: 'mb'
        }}
    )
    '''
    if update:
        tx.run(query_1)
        tx.run(query_2)
        print("update graph")
    else:
        tx.run(query_2)

@timer
def shortestPath(tx, dpid_from, dpid_to, update=True):
    if update:
        update_graph_project(tx, update=update)
    query = f'''
    MATCH (source:Switch {{dpid: {dpid_from}}}), (target:Switch {{dpid: {dpid_to}}})
    CALL gds.shortestPath.dijkstra.stream('myGraph', {{
      sourceNode: source,
      targetNode: target,
      relationshipWeightProperty: 'mb'
        }})
    YIELD totalCost, nodeIds
    RETURN totalCost, nodeIds
    '''
    result = tx.run(query)
    records = list(result)
    try:
        total_cost, path = records[0]
    except:
        total_cost, path = None, None
    summary = result.consume()
    return total_cost, path, summary.result_available_after

from parsgml import GmlManager
from topology_creator import Neo4j_Manager

if __name__ == "__main__":
    with GraphDatabase.driver(NEO4J_URI, auth=(USER, PASSWORD)) as driver:
        dpid_from, dpid_to = 1, 88
        manager = Neo4j_Manager(0)
        with open("gcc_out.log", "w") as file:
            with driver.session(database="neo4j") as session:
                path = session.execute_read(shortestPath, dpid_from=dpid_from, dpid_to=dpid_to, update=True)
                print(str(path), file=file)

                unlks = list(map(int, open("unlinks.txt", "r").readline().split(',')))
                topo = manager.topo
                for i in unlks:
                    edge = topo.edges[i]
                    manager.delete_link(edge.source + 1, edge.target + 1)
                    print("delete_link ", edge.source + 1, edge.target + 1)
                    path = session.execute_read(shortestPath, dpid_from=dpid_from, dpid_to=dpid_to, update=True)
                    print(str(path), file=file)
