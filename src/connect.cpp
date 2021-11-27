#include "main.h"

MYSQL *mysql_connection_setup(struct server_details mysql_details) {
    MYSQL *connection = mysql_init(NULL);

    // connect database
    if (!mysql_real_connect(connection, mysql_details.domain,
                            mysql_details.user, mysql_details.password,
                            mysql_details.database, atoi(mysql_details.SQL_port), NULL, 0)) {
        std::cout << "Connection Error: " << mysql_error(connection)
                  << std::endl;
        exit(1);
    }
    return connection;
}

MYSQL_RES *mysql_run(MYSQL *connection, const char *sql_query) {
    // send query to db
    if (mysql_query(connection, sql_query)) {
        std::cout << "MySQL Query Error: " << mysql_error(connection)
                  << std::endl;
        exit(1);
    }
    return mysql_use_result(connection);
}

void sql_update(MYSQL *conn, user_details *user_details) {
    sprintf(sql_query,
            "UPDATE hosts "
            "SET "
            "`last-online`=CURRENT_TIMESTAMP, "
            "`online`='1', "
            "`localIP`='%s', "
            "`publicIP`='%s' "
            "WHERE Name='%s';",
            user_details->localIP, user_details->publicIP, user_details->hostname);
    res = mysql_run(conn, sql_query);
    mysql_free_result(res);
}

void request(MYSQL *conn, int host_ID, host_details *host_details){
    // get host details
    sprintf(sql_query, "SELECT * FROM hosts WHERE ID='%i';", host_ID);
    res = mysql_run(conn, sql_query);
    while ((row = mysql_fetch_row(res)) != NULL) {
        strcpy(host_details->hostname, row[1]);
        strcpy(host_details->user, row[2]);
        strcpy(host_details->host_port, row[3]);
        strcpy(host_details->publicIP, row[4]);
        strcpy(host_details->localIP, row[5]);
    }
    // get unique request ID
    get_ID(conn, "requests", host_details->ID);
    // request host in requests table
    sprintf(sql_query,
            "INSERT INTO requests (`ID`, `Name`, `User`, `Port`, `last-used`) " 
            "VALUES ('%s', '%s', '%s', '%s', CURRENT_TIMESTAMP);", 
            host_details->ID, host_details->hostname, host_details->user, host_details->host_port);
    res = mysql_run(conn, sql_query);
}
void request_update(MYSQL *conn, host_details *host_details){
    sprintf(sql_query,
            "UPDATE requests "
            "SET "
            "`last-used`=CURRENT_TIMESTAMP, "
            "WHERE ID='%s';",
            host_details->ID);
    res = mysql_run(conn, sql_query);
    mysql_free_result(res);

}

void unrequest(MYSQL *conn, host_details *host_details){
    sprintf(sql_query,
            "DELETE FROM requests "
            "WHERE ID='%s';",
            host_details->ID);
    res = mysql_run(conn, sql_query);
}