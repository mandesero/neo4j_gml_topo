#!/usr/bin/env python3

from neo4j import GraphDatabase
import sys


import time

def timer(func):
    def wrapped(*args, **kwargs):
        t0 = time.time()
        res = func(*args, **kwargs)
        return (time.time() - t0) * 1000, res
    return wrapped


NEO4J_URI = "neo4j://localhost:7687"

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
    total_cost, path = records[0]
    summary = result.consume()
    print("The query returned in {time} ms.".format(
        time=summary.result_available_after,
    ))
    return total_cost, path


if __name__ == "__main__":
    with GraphDatabase.driver(NEO4J_URI) as driver:
        dpid_from, dpid_to = list(map(int, sys.argv[1:]))
        with driver.session(database="neo4j") as session:
            path = session.execute_read(shortestPath, dpid_from=dpid_from, dpid_to=dpid_to, update=False)
        with open("gcc_out.log", "w") as file:
            file.write(str(path))
        
