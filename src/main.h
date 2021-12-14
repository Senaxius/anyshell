/*****************************Defines*****************************/
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <memory>
#include <stdexcept>
#include <unistd.h>
#include <cstdlib>
#include <thread>
#include <vector>
#include <signal.h>

#include "../lib/mysql/mysql.h"

using namespace std;

struct server_details {
    const char *domain, *user, *password, *database, *SQL_port, *SSH_port;
};
struct user_details {
    int ssh_enabled;
    char user[20], hostname[20], port[6], localIP[20], publicIP[20];
    int ID;
};
struct host_details {
    char user[20], hostname[20], host_port[6], server_port[6], localIP[20], publicIP[20], ID[3];
};

/****************************Functions****************************/
std::string exec(const char *cmd);
void setting_ctrl_c();
void ctrl_c_handler(int s);
bool isNumber(char number[]);

void remove_from_list(list<int> *list, int a);
int check_connection(list<int> *list, int a);

void get_server_config(const std::string &config_path, server_details *server);
void get_user_config(user_details *anyshell_user);
void get_localIP(char *output);
void get_publicIP(char *output);
void get_user(char *user);
void get_hostname(char *hostname);
void get_databases(list<string> &databases);
void get_ID(MYSQL *conn, const char* table, char * ID);
int check_ssh_setup();

std::string exec(const char* cmd);

MYSQL *mysql_connection_setup(struct server_details mysql_details);
MYSQL_RES *mysql_run(MYSQL *connection, const char *sql_query);

void print_hosts(MYSQL *conn, int verbose);
void print_help();

void sql_update(MYSQL *conn, user_details *user_details);
void request(MYSQL *conn, int Host_ID, host_details *host_details);
void request_update(server_details server_details, host_details host_details);
void unrequest(MYSQL *conn, host_details *host_details);

void host(int ID, int port, user_details *user_details, server_details server_details, list<int> *connections, int *sshd);
// void host(int ID, int port, user_details *user_details, server_details server_details);
void connect(char *user, char *host, char *port);
/****************************Variables****************************/
static string str;
static char sql_query[500], command[200];

static list<string> databases;
static list<int> connections;

static struct server_details anyshell_server {"0", "0", "0", "0", "0", "0"};
static struct user_details anyshell_user {0, "0", "0", "0", "0", "0", 0};
static struct host_details anyshell_host {"0", "0", "0", "0", "0", "0", "0"};

static MYSQL *conn;
static MYSQL_RES *res;
static MYSQL_ROW row;
static char ssh_hostname[20];
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

    // cout << anyshell_user.user << endl;
    // cout << anyshell_user.hostname << endl;
    // cout << anyshell_user.localIP << endl;
    // cout << anyshell_user.publicIP << endl;
    // cout << anyshell_user.ssh_enabled << endl;

// for (auto i = myList.begin(); i!=myList.end(); i++){
//     cout << *i << " ";
// }
// remove_from_list(&connections, 1);