#include "cmd_db.h"

void cmd_db_init(struct cmd_db *ix) 
{
	ix->get = cmd_db_get;
	ix->proc = cmd_db_proc;
}

int cmd_db_get(struct cmd *c) 
{
	/*@todo - implement arg reader*/
	return 0;
}

void cmd_db_proc(char *cmd) 
{
	/*@todo - implement cmd procedure*/
	return;
} 

