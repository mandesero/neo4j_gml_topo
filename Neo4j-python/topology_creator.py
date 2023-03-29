#!/usr/bin/env python3


from neo4j import GraphDatabase
import sys
from config import NEO4J_URI


class Neo4j_Manager:
    def __init__(self):
        self.driver = GraphDatabase.driver(NEO4J_URI)
        
    def create_nodes(self, name):
        with self.driver.session() as session:
            result = session.run("CREATE (n:Node {name: $name})", name=name)
            
            # если транзакция завершилась успешно, вернуть True, иначе False
            if result.summary().counters.nodes_created > 0:
                return True
            else:
                return False




    def __del__(self):
        self.driver.close()


if __name__ == "__main__":
    with GraphDatabase.driver(NEO4J_URI) as driver:
        pass