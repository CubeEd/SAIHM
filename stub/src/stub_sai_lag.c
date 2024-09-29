#include "sai.h"
#include "stub_sai.h"
#include "assert.h"

#define MAX_NUMBER_OF_LAG_MEMBERS 16
#define MAX_NUMBER_OF_LAGS 5

static bool lag_db_is_initialized = false;

typedef struct _lag_member_db_entry_t 
{
    bool            is_used;
    sai_object_id_t port_oid;
    sai_object_id_t lag_oid;
} lag_member_db_entry_t;

typedef struct _lag_db_entry_t 
{
    bool            is_used;
    short           num_of_members;
    sai_object_id_t members_ids[MAX_NUMBER_OF_LAG_MEMBERS];
} lag_db_entry_t;

struct lag_db_t 
{
    lag_db_entry_t        lags[MAX_NUMBER_OF_LAGS];
    lag_member_db_entry_t members[MAX_NUMBER_OF_LAG_MEMBERS];
} lag_db;

static void initialize_lag_db()
{
    // Initialize lags
    for (int i = 0; i < MAX_NUMBER_OF_LAGS; i++)
    {
        lag_db.lags[i].is_used = false;
        lag_db.lags[i].num_of_members = 0;
        
        for (int j = 0; j < MAX_NUMBER_OF_LAG_MEMBERS; j++)
        {
            lag_db.lags[i].members_ids[j] = 0;  
        }
    }

    // Initialize members
    for (int i = 0; i < MAX_NUMBER_OF_LAG_MEMBERS; i++)
    {
        lag_db.members[i].is_used = false;
        lag_db.members[i].port_oid = 0;  
        lag_db.members[i].lag_oid = 0;   
    }
}

static const sai_attribute_entry_t lag_attribs[] = 
{
    { SAI_LAG_ATTR_PORT_LIST, false, false, false, true,
      "List of ports in LAG", SAI_ATTR_VAL_TYPE_OBJLIST },
    { END_FUNCTIONALITY_ATTRIBS_ID, false, false, false, false,
      "", SAI_ATTR_VAL_TYPE_UNDETERMINED }
};

static const sai_vendor_attribute_entry_t lag_vendor_attribs[] = 
{
    { SAI_LAG_ATTR_PORT_LIST,
      { false, false, false, false },
      { false, false, false, false },
      NULL, NULL,
      NULL, NULL }
};

static const sai_attribute_entry_t lag_member_attribs[] = 
{
    { SAI_LAG_MEMBER_ATTR_LAG_ID, true, true, false, true,
      "LAG ID", SAI_ATTR_VAL_TYPE_OID },
    { SAI_LAG_MEMBER_ATTR_PORT_ID, true, true, false, true,
      "PORT ID", SAI_ATTR_VAL_TYPE_OID },
    { END_FUNCTIONALITY_ATTRIBS_ID, false, false, false, false,
      "", SAI_ATTR_VAL_TYPE_UNDETERMINED }
};

sai_status_t get_lag_member_attribute(_In_ const sai_object_key_t   *key,
                                      _Inout_ sai_attribute_value_t *value,
                                      _In_ uint32_t                  attr_index,
                                      _Inout_ vendor_cache_t        *cache,
                                      void                          *arg)
{
    sai_status_t status;
    uint32_t     db_index;

    status = stub_object_to_type(key->object_id, SAI_OBJECT_TYPE_LAG_MEMBER, &db_index);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Cannot get LAG DB index.\n");
        return status;
    }

    switch ((int64_t)arg) 
    {
    case SAI_LAG_MEMBER_ATTR_LAG_ID:
        value->oid = lag_db.members[db_index].lag_oid;
        break;
    case SAI_LAG_MEMBER_ATTR_PORT_ID:
        value->oid = lag_db.members[db_index].port_oid;
        break;
    default:
        printf("Got unexpected attribute ID\n");
        return SAI_STATUS_FAILURE;
    }

    return SAI_STATUS_SUCCESS;
}

static const sai_vendor_attribute_entry_t lag_member_vendor_attribs[] = {
    { SAI_LAG_MEMBER_ATTR_LAG_ID,
      { true, false, false, true },
      { true, false, false, true },
      get_lag_member_attribute, (void*) SAI_LAG_MEMBER_ATTR_LAG_ID,
      NULL, NULL },
    { SAI_LAG_MEMBER_ATTR_PORT_ID,
      { true, false, false, true },
      { true, false, false, true },
      get_lag_member_attribute, (void*) SAI_LAG_MEMBER_ATTR_PORT_ID,
      NULL, NULL }
};
sai_status_t stub_create_lag(
    _Out_ sai_object_id_t* lag_id,
    _In_ uint32_t attr_count,
    _In_ sai_attribute_t *attr_list)
{
    if(lag_db_is_initialized == false)
    {
        initialize_lag_db();
        lag_db_is_initialized=true;
    }

    sai_status_t status;

    status = check_attribs_metadata(attr_count, attr_list, lag_attribs, lag_vendor_attribs, SAI_OPERATION_CREATE);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed attributes check\n");
        return status;
    }

    uint32_t lag_db_id = 0;
    for (; lag_db_id < MAX_NUMBER_OF_LAGS; lag_db_id++) 
    {
        if (!lag_db.lags[lag_db_id].is_used) 
        {
            break;
        }
    }

    if (lag_db_id == MAX_NUMBER_OF_LAGS) 
    {
        printf("Cannot create LAG: limit is reached\n");
        return SAI_STATUS_FAILURE;
    }

    printf("lag_db_id: %d\n", lag_db_id);

    lag_db.lags[lag_db_id].is_used = true;
    status = stub_create_object(SAI_OBJECT_TYPE_LAG, lag_db_id, lag_id);

    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Cannot create a LAG OID\n");
        return status;
    }

    char list_str[MAX_LIST_VALUE_STR_LEN];                                                       
    sai_attr_list_to_str(attr_count, attr_list, lag_attribs, MAX_LIST_VALUE_STR_LEN, list_str);

    printf("CREATE LAG: 0x%lX (%s)\n", *lag_id, list_str);

    return status;
}

sai_status_t stub_remove_lag(
    _In_ sai_object_id_t  lag_id)
{
    sai_status_t status;
    uint32_t     lag_db_id;
    status = stub_object_to_type(lag_id, SAI_OBJECT_TYPE_LAG, &lag_db_id);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Cannot get LAG DB ID.\n");
        return status;
    }

    lag_db.lags[lag_db_id].is_used = false;
    if(lag_db.lags[lag_db_id].num_of_members > 0)
    {
        printf("cannot remove LAG, LAG: 0x%u has members.\n", lag_db_id);
        return SAI_STATUS_FAILURE;
    }

    memset(lag_db.lags[lag_db_id].members_ids, 0, sizeof(lag_db.lags[lag_db_id].members_ids));
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_set_lag_attribute(
    _In_ sai_object_id_t  lag_id,
    _In_ const sai_attribute_t *attr)
{
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_get_lag_attribute(
    _In_ sai_object_id_t lag_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list)
{
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_create_lag_member(
    _Out_ sai_object_id_t* lag_member_id,
    _In_ uint32_t attr_count,
    _In_ sai_attribute_t *attr_list)
{
    sai_status_t status;
    status = check_attribs_metadata(attr_count, attr_list, lag_member_attribs, lag_member_vendor_attribs, SAI_OPERATION_CREATE);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed attributes check\n");
        return status;
    }

    const sai_attribute_value_t *lag_id, *port_id;
    uint32_t lag_id_idx, port_id_idx;
    status = find_attrib_in_list(attr_count, attr_list, SAI_LAG_MEMBER_ATTR_LAG_ID, &lag_id, &lag_id_idx);

    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("SAI_LAG_MEMBER_ATTR_LAG_ID attribute not found.\n");
        return status;
    }

    uint32_t     lag_db_id;
    status = stub_object_to_type(lag_id->oid, SAI_OBJECT_TYPE_LAG, &lag_db_id);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Cannot get LAG DB ID.\n");
        return status;
    }

    if(lag_db.lags[lag_db_id].is_used == false)
    {
        printf("LAG ID 0x%lx does not exist\n", lag_id->oid);
        return SAI_STATUS_FAILURE;
    }

    status = find_attrib_in_list(attr_count, attr_list, SAI_LAG_MEMBER_ATTR_PORT_ID, &port_id, &port_id_idx);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("SAI_LAG_MEMBER_ATTR_PORT_ID attribute not found.\n");
        return status;
    }

    uint32_t lag_memeber_db_id = 0;
    for (; lag_memeber_db_id < MAX_NUMBER_OF_LAG_MEMBERS; lag_memeber_db_id++) 
    {
        if (!lag_db.members[lag_memeber_db_id].is_used) 
        {
            break;
        }
    }

    if (lag_memeber_db_id == MAX_NUMBER_OF_LAG_MEMBERS) 
    {
        printf("Cannot create LAG MEMBER: limit is reached\n");
        return SAI_STATUS_FAILURE;
    }

    status = stub_create_object(SAI_OBJECT_TYPE_LAG_MEMBER, lag_memeber_db_id, lag_member_id);
    if (status != SAI_STATUS_SUCCESS)
    {
        printf("Cannot create a LAG member OID\n");
        return status;
    }
    
    lag_db_entry_t* lag = &lag_db.lags[lag_db_id];
    lag->members_ids[lag->num_of_members++] = *lag_member_id;
    
    lag_db.members[lag_memeber_db_id].is_used = true;
    lag_db.members[lag_memeber_db_id].lag_oid = lag_id->oid;
    lag_db.members[lag_memeber_db_id].port_oid = port_id->oid;
    char list_str[MAX_LIST_VALUE_STR_LEN];                                                       
    sai_attr_list_to_str(attr_count, attr_list, lag_member_attribs, MAX_LIST_VALUE_STR_LEN, list_str);
    printf("CREATE LAG_MEMBER: 0x%lX (%s)\n", *lag_member_id, list_str);
    return status;
}

sai_status_t stub_remove_lag_member(
    _In_ sai_object_id_t  lag_member_id)
{
    sai_status_t status;
    uint32_t     lag_member_db_id;
    uint32_t     lag_db_id;

    status = stub_object_to_type(lag_member_id, SAI_OBJECT_TYPE_LAG_MEMBER, &lag_member_db_id);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Cannot get LAG MEBMER ID. : %s: %d\n", __FILE__, __LINE__);
        return status;
    }
    
    status = stub_object_to_type(lag_db.members[lag_member_db_id].lag_oid, SAI_OBJECT_TYPE_LAG, &lag_db_id);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Cannot get LAG DB ID.: %s: %d\n", __FILE__, __LINE__);
        return status;
    }

    lag_db.members[lag_member_db_id].is_used = false;
    lag_db.lags[lag_db_id].num_of_members -= 1;
    lag_db.lags[lag_db_id].is_used = false;
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_set_lag_member_attribute(
    _In_ sai_object_id_t  lag_member_id,
    _In_ const sai_attribute_t *attr)
{
    return SAI_STATUS_SUCCESS;
}

sai_status_t stub_get_lag_member_attribute(
    _In_ sai_object_id_t lag_member_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list)
{
    const sai_object_key_t key = { .object_id = lag_member_id };
    return sai_get_attributes(&key, NULL, lag_member_attribs, lag_member_vendor_attribs, attr_count, attr_list);
}

const sai_lag_api_t lag_api = {
    stub_create_lag,
    stub_remove_lag,
    stub_set_lag_attribute,
    stub_get_lag_attribute,
    stub_create_lag_member,
    stub_remove_lag_member,
    stub_set_lag_member_attribute,
    stub_get_lag_member_attribute
};