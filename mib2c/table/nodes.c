/*
 * Note: this file originally auto-generated by mib2c using
 *        $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "nodes.h"
#include "nodes_checkfns.h"
#include "nodes_access.h"

static netsnmp_oid_stash_node *undoStorage = NULL;
static netsnmp_oid_stash_node *commitStorage = NULL;

struct undoInfo {
    void           *ptr;
    size_t          len;
};

struct commitInfo {
    void           *data_context;
    int             have_committed;
    int             new_row;
};

void
nodes_free_undoInfo(void *vptr)
{
    struct undoInfo *ui = vptr;
    if (!ui)
        return;
    SNMP_FREE(ui->ptr);
    SNMP_FREE(ui);
}

/** Initialize the nodeTable table by defining its contents and how it's structured */
void
initialize_table_nodeTable(void)
{
    const oid       nodeTable_oid[] = { 1, 3, 6, 1, 4, 1, 2312, 11, 2 };
    netsnmp_table_registration_info *table_info;
    netsnmp_handler_registration *my_handler;
    netsnmp_iterator_info *iinfo;

    DEBUGMSGTL(("nodes:init", "initializing table nodeTable\n"));

    /** create the table registration information structures */
    table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);
    iinfo = SNMP_MALLOC_TYPEDEF(netsnmp_iterator_info);

    my_handler = netsnmp_create_handler_registration("nodeTable",
                                                     nodeTable_handler,
                                                     nodeTable_oid,
                                                     OID_LENGTH
                                                     (nodeTable_oid),
                                                     HANDLER_CAN_RONLY);

    if (!my_handler || !table_info || !iinfo) {
        snmp_log(LOG_ERR, "malloc failed in initialize_table_nodeTable");
        return; /** Serious error. */
    }

    /***************************************************
     * Setting up the table's definition
     */
    netsnmp_table_helper_add_indexes(table_info, ASN_INTEGER,
                                               /** index: nodeIndex */
                                     0);

    /** Define the minimum and maximum accessible columns.  This
        optimizes retrieval. */
    table_info->min_column = 1;
    table_info->max_column = 2;

    /** iterator access routines */
    iinfo->get_first_data_point = nodeTable_get_first_data_point;
    iinfo->get_next_data_point = nodeTable_get_next_data_point;

    /** you may wish to set these as well */
#ifdef MAYBE_USE_THESE
    iinfo->make_data_context = nodeTable_context_convert_function;
    iinfo->free_data_context = nodeTable_data_free;

    /** pick *only* one of these if you use them */
    iinfo->free_loop_context = nodeTable_loop_free;
    iinfo->free_loop_context_at_end = nodeTable_loop_free;
#endif

    /** tie the two structures together */
    iinfo->table_reginfo = table_info;

    /***************************************************
     * registering the table with the master agent
     */
    DEBUGMSGTL(("initialize_table_nodeTable",
                "Registering table nodeTable as a table iterator\n"));
    netsnmp_register_table_iterator(my_handler, iinfo);
}

/** Initializes the nodes module */
void
init_nodes(void)
{

  /** here we initialize all the tables we're planning on supporting */
    initialize_table_nodeTable();
}

/** handles requests for the nodeTable table, if anything else needs to be done */
int
nodeTable_handler(netsnmp_mib_handler *handler,
                  netsnmp_handler_registration *reginfo,
                  netsnmp_agent_request_info *reqinfo,
                  netsnmp_request_info *requests)
{

    netsnmp_request_info *request;
    netsnmp_table_request_info *table_info;
    netsnmp_variable_list *var;
    struct commitInfo *ci = NULL;

    void           *data_context = NULL;

    /** column and row index encoded portion */
    const oid      *const suffix =
        requests->requestvb->name + reginfo->rootoid_len + 1;
    const size_t    suffix_len = requests->requestvb->name_length -
        (reginfo->rootoid_len + 1);

    DEBUGMSGTL(("nodes:handler", "Processing request (%d)\n",
                reqinfo->mode));

    for (request = requests; request; request = request->next) {
        var = request->requestvb;
        if (request->processed != 0)
            continue;

        switch (reqinfo->mode) {
        case MODE_GET:
            data_context = netsnmp_extract_iterator_context(request);
            if (data_context == NULL) {
                netsnmp_set_request_error(reqinfo, request,
                                          SNMP_NOSUCHINSTANCE);
                continue;
            }
            break;

        }

        /** extracts the information about the table from the request */
        table_info = netsnmp_extract_table_info(request);
        /** table_info->colnum contains the column number requested */
        /** table_info->indexes contains a linked list of snmp variable
           bindings for the indexes of the table.  Values in the list
           have been set corresponding to the indexes of the
           request */
        if (table_info == NULL) {
            continue;
        }

        switch (reqinfo->mode) {
        case MODE_GET:
            switch (table_info->colnum) {
            case COLUMN_NODEINDEX:
                {
                    long           *retval;
                    size_t          retval_len = 0;
                    retval = get_nodeIndex(data_context, &retval_len);
                    if (retval)
                        snmp_set_var_typed_value(var, ASN_INTEGER,
                                                 retval, retval_len);
                }
                break;

            case COLUMN_NODEVERSION:
                {
                    char           *retval;
                    size_t          retval_len = 0;
                    retval = get_nodeVersion(data_context, &retval_len);
                    if (retval)
                        snmp_set_var_typed_value(var, ASN_OCTET_STR,
                                                 retval, retval_len);
                }
                break;

            default:
                /** We shouldn't get here */
                snmp_log(LOG_ERR,
                         "problem encountered in nodeTable_handler: unknown column\n");
            }
            break;


        default:
            snmp_log(LOG_ERR,
                     "problem encountered in nodeTable_handler: unsupported mode\n");
        }
    }


    return SNMP_ERR_NOERROR;
}
