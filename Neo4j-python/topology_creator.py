#!/usr/bin/env python3

from neo4j import GraphDatabase
from parsgml import GmlManager
from config import NEO4J_URI, USER, PASSWORD


class Neo4j_Manager:
    def __init__(self, n_topo):
        self.driver = GraphDatabase.driver(NEO4J_URI, auth=(USER, PASSWORD))
        self.topo = Neo4j_Manager.get_topology(n_topo)
        self.delete_topo()
        self.create_topo()
        self.create_graph_proection()

    def create_graph_proection(self):
        query = f"""CALL gds.graph.project(
            'myGraph',
            'Switch',
            'Link',
            {{
                relationshipProperties: 'mb'
            }}
        )
        """
        with self.driver.session() as session:
            result = session.run(query)

    def create_topo(self):
        query = "CREATE"
        for dpid in self.topo.nodes:
            query += f"(S{dpid + 1}:Switch {{dpid: {dpid + 1}}}),"

        for link in self.topo.edges:
            # mb = self.topo.matrix[link.source][link.target]
            mb = 1
            query += f"(S{link.source + 1})-[:Link {{mb: {mb}}}]->(S{link.target + 1}),"
            query += f"(S{link.source + 1})<-[:Link {{mb: {mb}}}]-(S{link.target + 1}),"

        with self.driver.session() as session:
            result = session.run(query[:-1])

    def delete_link(self, dpid_from, dpid_to):
        query = f"MATCH (S1:Switch {{dpid:{dpid_from}}}), (S2:Switch {{dpid:{dpid_to}}}), (S1)-[r:Link]-(S2) delete r"
        with self.driver.session() as session:
            result = session.run(query)

    def delete_topo(self):
        query = "MATCH (n) DETACH DELETE n"
        with self.driver.session() as session:
            result = session.run(query)

    def __del__(self):
        self.driver.close()

    @staticmethod
    def get_topology(n_topo=0):
        manager = GmlManager()
        manager.parse()
        return manager.topologies[n_topo]
