from neo4j import GraphDatabase
import time

# Connect to Neo4j database

URI = "neo4j://localhost:7687"
with GraphDatabase.driver(URI) as driver:
    dpid_from = 14
    dpid_to = 88
    query = f"MATCH (S1: Switch {{dpid: {dpid_from}}}), (S1: Switch {{dpid: {dpid_to}}}), p = shortestPath((S1)-[:Link*]-(S2)) return p"
    with driver.session() as session:
        start_time = time.time()
        result = session.run(query)
        end_time = time.time()
        print("Time of execution:", (end_time - start_time) * 1000000, "microseconds")
    