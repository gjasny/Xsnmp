/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.iterate.conf 15999 2007-03-25 22:32:02Z dts12 $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <fcntl.h>
#include <pcre.h>
#include "log.h"
#include "util.h"
#include "command.h"
#include "main.h"
#include "raidSetTable.h"
#include "raidDriveTable.h"

static struct timeval cache_timestamp = { 0, 0 };
static int last_index_used = 0;
void update_drivelist();

#define MAX_CACHE_AGE 10
#define OVECCOUNT 90

/** Initializes the raidDriveTable module */
void
init_raidDriveTable(void)
{
  /* here we initialize all the tables we're planning on supporting */
    initialize_table_raidDriveTable();
}

/** Initialize the raidDriveTable table by defining its contents and how it's structured */
void
initialize_table_raidDriveTable(void)
{
    static oid raidDriveTable_oid[] = {1,3,6,1,4,1,20038,2,1,6,3};
    size_t raidDriveTable_oid_len   = OID_LENGTH(raidDriveTable_oid);
    netsnmp_handler_registration    *reg;
    netsnmp_iterator_info           *iinfo;
    netsnmp_table_registration_info *table_info;

    reg = netsnmp_create_handler_registration(
              "raidDriveTable",     raidDriveTable_handler,
              raidDriveTable_oid, raidDriveTable_oid_len,
              HANDLER_CAN_RONLY
              );

    table_info = SNMP_MALLOC_TYPEDEF( netsnmp_table_registration_info );
    netsnmp_table_helper_add_indexes(table_info,
                           ASN_INTEGER,  /* index: raidDriveIndex */
                           0);
    table_info->min_column = COLUMN_RAIDDRIVELOCATION;
    table_info->max_column = COLUMN_RAIDDRIVESETINDEX;
    
    iinfo = SNMP_MALLOC_TYPEDEF( netsnmp_iterator_info );
    iinfo->get_first_data_point = raidDriveTable_get_first_data_point;
    iinfo->get_next_data_point  = raidDriveTable_get_next_data_point;
    iinfo->table_reginfo        = table_info;
    
    netsnmp_register_table_iterator( reg, iinfo );

    /* Initialise the contents of the table here */
    update_drivelist();
}

    /* Typical data structure for a row entry */
struct raidDriveTable_entry {
    /* Index values */
    long raidDriveIndex;

    /* Column values */
    char *raidDriveLocation;
    size_t raidDriveLocation_len;
    char *raidDriveSetName;
    size_t raidDriveSetName_len;
    uint32_t raidDriveSize;
    long raidDriveStatus;
    char *raidDriveStatusMessage;
    size_t raidDriveStatusMessage_len;
    long raidDriveSetIndex;

    /* Obsolete Checking */
    time_t last_seen;

    /* Illustrate using a simple linked list */
    int   valid;
    struct raidDriveTable_entry *next;
};

struct raidDriveTable_entry  *raidDriveTable_head;

/* create a new row in the (unsorted) table */
struct raidDriveTable_entry *
raidDriveTable_createEntry(long  raidDriveIndex) 
{
    struct raidDriveTable_entry *entry;

    entry = SNMP_MALLOC_TYPEDEF(struct raidDriveTable_entry);
    if (!entry)
        return NULL;

    entry->raidDriveIndex = raidDriveIndex;
    entry->next = raidDriveTable_head;
    raidDriveTable_head = entry;
    return entry;
}

/* remove a row from the table */
void
raidDriveTable_removeEntry( struct raidDriveTable_entry *entry ) {
    struct raidDriveTable_entry *ptr, *prev;

    if (!entry)
        return;    /* Nothing to remove */

    for ( ptr  = raidDriveTable_head, prev = NULL;
          ptr != NULL;
          prev = ptr, ptr = ptr->next ) {
        if ( ptr == entry )
            break;
    }
    if ( !ptr )
        return;    /* Can't find it */

    if ( prev == NULL )
        raidDriveTable_head = ptr->next;
    else
        prev->next = ptr->next;

    if (entry->raidDriveLocation) free (entry->raidDriveLocation);
    if (entry->raidDriveSetName) free (entry->raidDriveSetName);
    if (entry->raidDriveStatusMessage) free (entry->raidDriveStatusMessage);
    SNMP_FREE( entry );
}


/* Example iterator hook routines - using 'get_next' to do most of the work */
netsnmp_variable_list *
raidDriveTable_get_first_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    *my_loop_context = raidDriveTable_head;
    return raidDriveTable_get_next_data_point(my_loop_context, my_data_context,
                                    put_index_data,  mydata );
}

netsnmp_variable_list *
raidDriveTable_get_next_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    struct raidDriveTable_entry *entry = (struct raidDriveTable_entry *)*my_loop_context;
    netsnmp_variable_list *idx = put_index_data;

    if ( entry ) {
        snmp_set_var_typed_integer( idx, ASN_INTEGER, entry->raidDriveIndex );
        idx = idx->next_variable;
        *my_data_context = (void *)entry;
        *my_loop_context = (void *)entry->next;
        return put_index_data;
    } else {
        return NULL;
    }
}

/* Drive List Update */

void update_drivelist()
{
  /* Calls 'raidutil list raidsetinfo' */
  struct timeval now;
  gettimeofday (&now, NULL);
  char *data = NULL;
  size_t data_len = 0;
  if (xsan_debug())
  {
    /* Use example data */
    int fd = open ("../examples/raidutil_list_driveinfo.txt", O_RDONLY);
    data = malloc (65536);
    data_len = read (fd, data, 65535);
    data[data_len] = '\0';
    close (fd);
  }
  else
  {
    /* Use live data */
    data = x_command_run("raidutil list driveinfo", 0);
    if (!data) return;
    data_len = strlen(data);
  }

  /* Regex and loop through each raid set row */
  const char *error;
  int erroffset;
  int ovector[OVECCOUNT];
  /* Looks like 'Bay #2  Raid5Set        2.00TB   IsMemberOfRAIDSet:Raid5Set IsReliable' */
  pcre *re = pcre_compile("^(Bay \\#\\d+)\\s+([^ ]*)\\s+(\\d+\\.\\d\\d)([MGT])B\\s+(?:IsMemberOfRAIDSet:[^ ]*)\\s+(.*)$", PCRE_MULTILINE, &error, &erroffset, NULL);

  if (re == NULL) { x_printf ("ERROR: update_setlist failed to compile regex"); free (data); return; }

  ovector[0] = 0;
  ovector[1] = 0;
  while(1)
  {
    int options = 0;                 /* Normally no options */
    int start_offset = ovector[1];   /* Start at end of previous match */

    if (ovector[0] == ovector[1])
    {
      if (ovector[0] == (int)data_len) break;
    }

    int rc = pcre_exec( re,                   /* the compiled pattern */
                         NULL,                 /* no extra data - we didn't study the pattern */
                         data,              /* the subject string */
                         data_len,       /* the length of the subject */
                         start_offset,         /* starting offset in the subject */
                         options,              /* options */
                         ovector,              /* output vector for substring information */
                         OVECCOUNT);           /* number of elements in the output vector */

    if (rc == PCRE_ERROR_NOMATCH)
    {
      if (options == 0) break;
      ovector[1] = start_offset + 1;
      continue;    /* Go round the loop again */
    }

    /* Other matching errors are not recoverable. */
    if (rc > 0)
    {
      /* Matched an RAID Drive. Vectors:
       * 0=FullString 1(2)=Location 2(4)=RAIDSet 3(6)=Size 4(8)=SizeUnits 5(10)=Flags
      */

      /* Get Name */
      char *name_str;
      asprintf (&name_str, "%.*s", ovector[3] - ovector[2], data + ovector[2]);
      trim_end(name_str);
      x_debug ("update_drivelist Matched %.*s\n", ovector[3] - ovector[2], data + ovector[2]);

      /* Find/Create Entry */
      struct raidDriveTable_entry *entry = raidDriveTable_head;
      while (entry)
      {
        if (!strcmp(name_str, entry->raidDriveLocation)) break;
        entry = entry->next;
      }
      if (!entry)
      {
        last_index_used++;
        entry = raidDriveTable_createEntry(last_index_used);
        entry->raidDriveLocation = strdup(name_str);
        entry->raidDriveLocation_len = strlen (entry->raidDriveLocation);
      }
      entry->last_seen = now.tv_sec;
      free (name_str);
      name_str = NULL;

      /* Extract Data from Regex Match */
      extract_string_in_range(data+ovector[4], ovector[5]-ovector[4], &entry->raidDriveSetName, &entry->raidDriveSetName_len);
      entry->raidDriveSize = extract_uint_in_range(data+ovector[6], ovector[7]-ovector[6]);
      scale_value_to_m(data[ovector[8]], &entry->raidDriveSize);

      /* Extract Flags */
      extract_string_in_range(data+ovector[10], ovector[11]-ovector[10], &entry->raidDriveStatusMessage, &entry->raidDriveStatusMessage_len);
      if (entry->raidDriveStatusMessage && strlen(entry->raidDriveStatusMessage) > 0)
      {
        /* Check for status indicators */
        if (strstr(entry->raidDriveStatusMessage, "IsReliable")) entry->raidDriveStatus = 1;
        else if (strstr(entry->raidDriveStatusMessage, "IsFailurePredicted")) entry->raidDriveStatus = 2;
        else if (strstr(entry->raidDriveStatusMessage, "IsFailed")) entry->raidDriveStatus = 3;
        else if (strstr(entry->raidDriveStatusMessage, "IsMissing")) entry->raidDriveStatus = 4;
        else if (strstr(entry->raidDriveStatusMessage, "IncompatibleMetadata")) entry->raidDriveStatus = 5;
        else if (strstr(entry->raidDriveStatusMessage, "IsSpare")) entry->raidDriveStatus = 6;
        else if (strstr(entry->raidDriveStatusMessage, "IsNotAssigned")) entry->raidDriveStatus = 7;
      }

      /* Find Index */
      struct raidSetTable_entry *set = raidSetTable_get_head();
      while (set)
      {
        if (!strcmp(entry->raidDriveSetName, set->raidSetName)) break;
        set = set->next;
      }
      if (set)
      {
        entry->raidDriveSetIndex = set->raidSetIndex;
      }
      else
      {
        entry->raidDriveSetIndex = 0;
      }
    }
    else
    {
      pcre_free(re);    /* Release memory used for the compiled pattern */
      return;
    }
  }

  pcre_free(re);    /* Release memory used for the compiled pattern */

  /* Check for obsolete entries */
  struct raidDriveTable_entry *entry = raidDriveTable_head;
  while (entry)
  {
    struct raidDriveTable_entry *next = entry->next;
    if (entry->last_seen != now.tv_sec)
    {
      /* Entry is obsolete */
      raidDriveTable_removeEntry(entry);
    }
    entry = next;
  }

  /* Update cache timestamp */
  gettimeofday(&cache_timestamp, NULL);

  /* Clean up */
  free (data);
  data = NULL;
  data_len = 0;
}

void update_drivelist_ifnecessary()
{
  struct timeval now;
  gettimeofday(&now, NULL);
  if (now.tv_sec - cache_timestamp.tv_sec > MAX_CACHE_AGE)
  { update_drivelist(); }
}

/** handles requests for the raidDriveTable table */
int
raidDriveTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {

    netsnmp_request_info       *request;
    netsnmp_table_request_info *table_info;
    struct raidDriveTable_entry          *table_entry;

    switch (reqinfo->mode) {
        /*
         * Read-support (also covers GetNext requests)
         */
      case MODE_GET:
        update_drivelist_ifnecessary();
        for (request=requests; request; request=request->next) {
            table_entry = (struct raidDriveTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_RAIDDRIVELOCATION:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                 (u_char*)table_entry->raidDriveLocation,
                                          table_entry->raidDriveLocation_len);
                break;
            case COLUMN_RAIDDRIVESETNAME:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                 (u_char*)table_entry->raidDriveSetName,
                                          table_entry->raidDriveSetName_len);
                break;
            case COLUMN_RAIDDRIVESIZE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_GAUGE,
                                            table_entry->raidDriveSize);
                break;
            case COLUMN_RAIDDRIVESTATUS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->raidDriveStatus);
                break;
            case COLUMN_RAIDDRIVESTATUSMESSAGE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                 (u_char*)table_entry->raidDriveStatusMessage,
                                          table_entry->raidDriveStatusMessage_len);
                break;
            case COLUMN_RAIDDRIVESETINDEX:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->raidDriveSetIndex);
                break;
            default:
                netsnmp_set_request_error(reqinfo, request,
                                          SNMP_NOSUCHOBJECT);
                break;
            }
        }
        break;

    }
    return SNMP_ERR_NOERROR;
}
