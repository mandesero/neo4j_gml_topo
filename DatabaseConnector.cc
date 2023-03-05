#include <neo4j-client.h>

class DatabaseConnector {
public:
    DatabaseConnector(const char *uri, const char *username, const char *password) {
        neo4j_client_init();

        neo4j_config_t *config = neo4j_new_config();
        neo4j_config_set_username(config, username);
        neo4j_config_set_password(config, password);

        connection = neo4j_connect(uri, config);
        if (neo4j_connection_unrecoverable_error(connection)) {
            neo4j_perror(stderr, errno, "Connection failed");
            exit(EXIT_FAILURE);
        }

        neo4j_config_free(config);
    }

    ~DatabaseConnector() {
        neo4j_close(connection);
        neo4j_client_cleanup();
    }

    neo4j_connection_t *get_connection() const {
        return connection;
    }

private:
    neo4j_connection_t *connection;
};

int main() {
    const char *uri = "bolt://localhost:7687";
    const char *username = "neo4j";
    const char *password = "password";

    DatabaseConnector connector(uri, username, password);

    // Use the connector to interact with the database...
    neo4j_connection_t *conn = connector.get_connection();
    // execute Cypher queries using neo4j_run

    return 0;
}