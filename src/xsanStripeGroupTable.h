/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.iterate.conf 15999 2007-03-25 22:32:02Z dts12 $
 */
#ifndef XSANSTRIPGROUPTABLE_H
#define XSANSTRIPGROUPTABLE_H

/* Typical data structure for a row entry */
struct xsanStripeGroupTable_entry 
{
    /* Index values */
    long xsanVolumeIndex;
    long xsanStripeGroupIndex;

    /* Column values */
    char *xsanStripeGroupName;
    size_t xsanStripeGroupName_len;
    long xsanStripeGroupStatus;
    U64 xsanStripeGroupTotalBlocks;
    U64 xsanStripeGroupReservedBlocks;
    U64 xsanStripeGroupFreeBlocks;
    u_long xsanStripeGroupTotalKBlocks;
    u_long xsanStripeGroupReservedKBlocks;
    u_long xsanStripeGroupFreeKBlocks;
    u_long xsanStripeGroupUtilization;
    char *xsanStripeGroupMultiPathMethod;
    size_t xsanStripeGroupMultiPathMethod_len;
    long xsanStripeGroupStripeDepth;
    long xsanStripeGroupStripeBreadth;
    char *xsanStripeGroupAffinityKey;
    size_t xsanStripeGroupAffinityKey_len;
    u_long xsanStripeGroupRTLimitIOPerSecond;
    u_long xsanStripeGroupNonRTReserveIOPerSecond;
    u_long xsanStripeGroupRTCommittedIOPerSecond;
    u_long xsanStripeGroupNonRTClients;
    u_long xsanStripeGroupNonRTHintIOPerSecond;
    u_long xsanStripeGroupTotalMBytes;
    u_long xsanStripeGroupFreeMBytes;
    u_long xsanStripeGroupUsedMBytes;

    /* Obsolescence */
    time_t last_seen;

    /* Affinity */
    long xsanAffinityIndex;
    
    /* Illustrate using a simple linked list */
    struct xsanStripeGroupTable_entry *next;
};

/* function declarations */
void init_xsanStripeGroupTable(void);
void initialize_table_xsanStripeGroupTable(void);
Netsnmp_Node_Handler xsanStripeGroupTable_handler;
Netsnmp_First_Data_Point  xsanStripeGroupTable_get_first_data_point;
Netsnmp_Next_Data_Point   xsanStripeGroupTable_get_next_data_point;

/* column number definitions for table xsanStripeGroupTable */
       #define COLUMN_XSANVOLUMEINDEX		1
       #define COLUMN_XSANSTRIPEGROUPINDEX		2
       #define COLUMN_XSANSTRIPEGROUPNAME		3
       #define COLUMN_XSANSTRIPEGROUPSTATUS		4
       #define COLUMN_XSANSTRIPEGROUPTOTALBLOCKS		5
       #define COLUMN_XSANSTRIPEGROUPRESERVEDBLOCKS		6
       #define COLUMN_XSANSTRIPEGROUPFREEBLOCKS		7
       #define COLUMN_XSANSTRIPEGROUPTOTALKBLOCKS		8
       #define COLUMN_XSANSTRIPEGROUPRESERVEDKBLOCKS		9
       #define COLUMN_XSANSTRIPEGROUPFREEKBLOCKS		10
       #define COLUMN_XSANSTRIPEGROUPUTILIZATION		11
       #define COLUMN_XSANSTRIPEGROUPMULTIPATHMETHOD		12
       #define COLUMN_XSANSTRIPEGROUPSTRIPEDEPTH		13
       #define COLUMN_XSANSTRIPEGROUPSTRIPEBREADTH		14
       #define COLUMN_XSANSTRIPEGROUPAFFINITYKEY		15
       #define COLUMN_XSANSTRIPEGROUPRTLIMITIOPERSECOND		16
       #define COLUMN_XSANSTRIPEGROUPNONRTRESERVEIOPERSECOND		17
       #define COLUMN_XSANSTRIPEGROUPRTCOMMITTEDIOPERSECOND		18
       #define COLUMN_XSANSTRIPEGROUPNONRTCLIENTS		19
       #define COLUMN_XSANSTRIPEGROUPNONRTHINTIOPERSECOND		20
       #define COLUMN_XSANSTRIPEGROUPTOTALMBYTES		21
       #define COLUMN_XSANSTRIPEGROUPFREEMBYTES		22
       #define COLUMN_XSANSTRIPEGROUPUSEDMBYTES		23
       
#endif /* XSANSTRIPGROUPTABLE_H */

void update_stripegroups (char *data, size_t data_len, long xsanVolumeIndex);
