from pygmlparser.Parser import Parser
from pathlib import Path
from random import randint
from functools import total_ordering
from random import sample


@total_ordering
class Topology:
    def __init__(self, path, nodes, edges):
        self.path = path
        self.nodes = nodes
        self.edges = edges
        self.matrix = [[0] * len(nodes) for _ in range(len(nodes))]
        self.density = 2 * len(edges) / (len(nodes) * len(nodes))

    def random_edges_weights(self, a=10, b=50):
        for edge in self.edges:
            self.matrix[edge.source][edge.target] = self.matrix[edge.target][
                edge.source
            ] = randint(a, b)

    def __eq__(self, other):
        return self.density == other.density

    def __lt__(self, other):
        return (self.density, len(self.nodes)) < (other.density, len(self.nodes))

    def __str__(self):
        return f"PATH:    {self.path}\nNODES:   {len(self.nodes)}\nLINKS:   {len(self.edges)}\nDENSITY: {self.density}\n"


class GmlManager:
    def __init__(self, dpath="./GML_files", flog="density.txt"):
        self.dpath = dpath
        self.flog = flog
        self.topologies = []

    def parse(self):
        for PATH in Path(self.dpath).glob("*.gml"):
            parser = Parser()
            parser.loadGML(path=PATH)
            parser.parse()

            self.topologies.append(
                Topology(PATH, parser.graph.graphNodes, parser.graph.graphEdges)
            )
        self.topologies.sort()

    def log_to_file(self):
        with open(self.flog, "w") as file:
            for topo in sorted(self.topologies):
                file.write(
                    f"PATH={topo.path}, nodes={len(topo.nodes)}, density={topo.density}\n"
                )

    def find_topo_by_density(self, a=0, b=1):
        res = []
        for topo in self.topologies:
            if a <= topo.density <= b:
                res.append(topo)
        return res


# a = GmlManager()
# a.parse()
# with open("unlinks.txt", "w") as file:
#     for topo in a.topologies:
#         n = len(topo.edges)
#         t = sample(list(map(str, range(n))), n // 2 + 1)
#         print(",".join(t), file=file)    