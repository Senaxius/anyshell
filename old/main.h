/*****************************Defines*****************************/
#include "../lib/mysql/mysql.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <unistd.h>
#include <limits>
#include <ctime>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <list>

#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

/****************************Functions****************************/
struct connection_details {
    const char *server, *user, *password, *database;
};

MYSQL *mysql_connection_setup(struct connection_details mysql_details);
MYSQL_RES *mysql_run(MYSQL *connection, const char *sql_query);

std::string exec(const char* cmd);

void get_localIP(char *output);
void get_publicIP(char *output);

void host_up(const char *database, int port, char *ssh_user, char *ssh_host, char *ssh_port);
void host_down(const char *database, int port, char *ssh_host);

int socket_check(const char *socket);
int check_ssh_setup();
void connect(char *user, char *host, char *port);

void print_hosts(MYSQL *conn);
void request(MYSQL *conn, int ID);
void unrequest(MYSQL *conn, int ID);
void get_ID(MYSQL *conn, const char* table, char * ID);
void get_user(char *user);

void sql_update(MYSQL *conn);
/****************************Variables****************************/
static ifstream file;
static char sql_query[500], command[200], socket[100];
static char hostname[20], user[20], port[6], localIP[20], publicIP[20], ID[3];
static char server_domain[30], server_IP[15], server_user[10], server_ssh_port[6], server_port[6], server_database[20];

static int input;
static int ssh_enabled;

static struct connection_details anyshell_server{
    "noftp.ddns.net",
    "senaex",
    "Quande-0918",
    "senaex"
};
/*****************************Garbage*****************************/
// conn = mysql_connection_setup(anyshell_server);
// res = mysql_run(conn,
//     "INSERT INTO hosts (`ID`, `Name`, `User`, `Port`, `publicIP`,
//     `localIP`, `online`, `last-online`, `requested`) " "VALUES ('0',
//     'Arch-PC', 'senaex', '22', '87.98.45.78', '192.168.178.21', '1',
//     '2038-01-19 03:14:07', '0');");
// res = mysql_run(conn, "DELETE FROM server WHERE ID='12'");
// res = mysql_run(conn, "SELECT * FROM hosts;");
// res = mysql_run(conn, "SHOW TABLES");
// res = mysql_run(conn, "SELECT * FROM server WHERE User = 'lennart';");
// res = mysql_run(conn, "SELECT * FROM server WHERE User LIKE 'len%' AND
// Domain LIKE 'no%';"); res = mysql_run(conn, "SELECT * FROM server WHERE
// (User LIKE 'sen%') OR (User LIKE 'len%');"); res = mysql_run(conn,
// "SELECT * FROM server WHERE User = 'senaex';"); res = mysql_run(conn,
//     "UPDATE hosts "
//     "SET localIP='192.197.178.156'"
//     "WHERE ID=1"
// );
// ID, Name, User, Port, publicIP, localIP, requested, last-online, online
// while ((row = mysql_fetch_row(res)) != NULL){
//     // the below row[] parametes may change depending on the size of the
//     table and your objective std::cout << row[0] << " | " << row[1] << "
//     | " << row[2] << " | " << row[3] << " | " << row[4] << std::endl;
// }

// mysql_free_result(res);
// mysql_close(conn);