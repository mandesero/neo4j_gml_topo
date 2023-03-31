#!/usr/bin/env python3

from neo4j import GraphDatabase
import sys
from config import NEO4J_URI, USER, PASSWORD


import time


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
        # print("update graph")
    else:
        tx.run(query_2)

# @timer
def shortestPath(tx, dpid_from, dpid_to, update=True):
    # if update:
    #     update_graph_project(tx, update=update)
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
    query_1 = f'''CALL gds.graph.drop('myGraph')'''

    query_2 = f'''CALL gds.graph.project(
        'myGraph',
        'Switch',
        'Link',
        {{
            relationshipProperties: 'mb'
        }}
    )
    '''
    tx.run(query_1)
    tx.run(query_2)
    t0 = time.time()
    result = tx.run(query)
    t1 = (time.time() - t0) * 10**6
    records = list(result)
    try:
        total_cost, path = records[0]
    except:
        total_cost, path = None, None
    summary = result.consume()
    return t1, total_cost, path, summary.result_available_after

from parsgml import GmlManager
from topology_creator import Neo4j_Manager
import csv

if __name__ == "__main__":
    with GraphDatabase.driver(NEO4J_URI, auth=(USER, PASSWORD)) as driver:
        dpid_from, dpid_to = 1, 111
        n_topo = 1

        fieldnames = ['execution_time', 'total_cost', 'path', 'query_execution_time']

        manager = Neo4j_Manager(n_topo)
        with open(f"out_{n_topo}.csv", 'w', encoding='UTF8', newline='') as file:
            with driver.session(database="neo4j") as session:

                writer = csv.DictWriter(file, fieldnames=fieldnames)
                writer.writeheader()
                rows = []
                path = session.execute_read(shortestPath, dpid_from=dpid_from, dpid_to=dpid_to, update=True)
                rows.append(
                    {
                        'execution_time': path[0],
                        'total_cost': path[1],
                        'path': path[2],
                        'query_execution_time': path[3],
                    }
                )

                unlks = list(map(int, open("unlinks.txt", "r").readlines()[n_topo].split(',')))
                topo = manager.topo
                for i in unlks:
                    edge = topo.edges[i]
                    manager.delete_link(edge.source + 1, edge.target + 1)
                    print("delete_link ", edge.source + 1, edge.target + 1)
                    path = session.execute_read(shortestPath, dpid_from=dpid_from, dpid_to=dpid_to, update=True)
                    rows.append(
                    {
                        'execution_time': path[0],
                        'total_cost': path[1],
                        'path': path[2],
                        'query_execution_time': path[3],
                    }
                    )
                writer.writerows(rows)
