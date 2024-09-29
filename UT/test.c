#include <stdio.h>
#include "sai.h"
#include "stub_sai.h"

const char* test_profile_get_value(
    _In_ sai_switch_profile_id_t profile_id,
    _In_ const char* variable)
{
    return 0;
}

int test_profile_get_next_value(
    _In_ sai_switch_profile_id_t profile_id,
    _Out_ const char** variable,
    _Out_ const char** value)
{
    return -1;
}

const service_method_table_t test_services = {
    test_profile_get_value,
    test_profile_get_next_value
};

int main()
{
    sai_status_t              status;
    sai_lag_api_t            *lag_api;
    sai_attribute_t           attrs[2];
    sai_object_id_t           port_list[64];
    sai_object_id_t           lag1_oid;
    sai_object_id_t           lag2_oid;
    sai_object_id_t           port1_oid;
    sai_object_id_t           port2_oid;
    sai_object_id_t           port3_oid;
    sai_object_id_t           port4_oid;
    sai_object_id_t           lag_mebmer1_oid;
    sai_object_id_t           lag_mebmer2_oid;
    sai_object_id_t           lag_mebmer3_oid;
    sai_object_id_t           lag_mebmer4_oid;

    status = sai_api_initialize(0, &test_services);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to query initialize SAI API, status=%d\n", status);
        return 1;
    }

    status = sai_api_query(SAI_API_LAG, (void**)&lag_api);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to query LAG API, status=%d\n", status);
        return 1;
    }

    //Create LAG#1
    status = lag_api->create_lag(&lag1_oid, 0, NULL);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to create a LAG1, status=%d\n", status);
        return 1;
    }

    printf("CREATE LAG: 0x%lX\n", lag1_oid);

    //Create LAG_MEMBER#1 {LAG_ID:LAG#1, PORT_ID:PORT#1}
    attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    attrs[0].value.oid = lag1_oid;

    static int32_t port_id = 1;
    status = stub_create_object(SAI_OBJECT_TYPE_PORT, port_id, &port1_oid);

    attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    attrs[1].value.oid = port1_oid;

    status = lag_api->create_lag_member(&lag_mebmer1_oid, 2, attrs);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to create a LAG_MEMBER1, status=%d\n", status);
        return 1;
    }

    printf("CREATE LAG_MEBMER: 0x%lX\n", lag_mebmer1_oid);

    //Create LAG_MEMBER#2 {LAG_ID:LAG#1, PORT_ID:PORT#2}
    attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    attrs[0].value.oid = lag1_oid;

    port_id = 2;
    status = stub_create_object(SAI_OBJECT_TYPE_PORT, port_id, &port2_oid);

    attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    attrs[1].value.oid = port2_oid;

    status = lag_api->create_lag_member(&lag_mebmer2_oid, 2, attrs);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to create a LAG_MEMBER2, status=%d\n", status);
        return 1;
    }

    printf("CREATE LAG_MEBMER: 0x%lX\n", lag_mebmer2_oid);

    //Create LAG#2
    status = lag_api->create_lag(&lag2_oid, 0, NULL);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to create a LAG2, status=%d\n", status);
        return 1;
    }

    printf("CREATE LAG: 0x%lX\n", lag2_oid);

    //Create LAG_MEMBER#3 {LAG_ID:LAG#2, PORT_ID:PORT#3}
    attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    attrs[0].value.oid = lag2_oid;

    port_id = 3;
    status = stub_create_object(SAI_OBJECT_TYPE_PORT, port_id, &port3_oid);

    attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    attrs[1].value.oid = port3_oid;

    status = lag_api->create_lag_member(&lag_mebmer3_oid, 2, attrs);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to create a LAG_MEMBER3, status=%d\n", status);
        return 1;
    }

    printf("CREATE LAG_MEBMER: 0x%lX\n", lag_mebmer3_oid);

    //Create LAG_MEMBER#4 {LAG_ID:LAG#2, PORT_ID:PORT#4}
    attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    attrs[0].value.oid = lag2_oid;

    port_id = 4;
    status = stub_create_object(SAI_OBJECT_TYPE_PORT, port_id, &port4_oid);

    attrs[1].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    attrs[1].value.oid = port4_oid;

    status = lag_api->create_lag_member(&lag_mebmer4_oid, 2, attrs);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to create a LAG_MEMBER4, status=%d\n", status);
        return 1;
    }

    printf("CREATE LAG_MEBMER: 0x%lX\n", lag_mebmer4_oid);

    // Get LAG#1 PORT_LIST [Expected: (PORT#1, PORT#2)]
    attrs[0].id = SAI_LAG_ATTR_PORT_LIST;
    attrs[0].value.objlist.list = port_list;
    attrs[0].value.objlist.count = 64;
    status = lag_api->get_lag_attribute(lag1_oid, 1, attrs);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to get lag attribute SAI_LAG_ATTR_PORT_LIST for LAG 0x%lX, status=%d\n",lag1_oid,  status);
        return 1;
    }

    // Get LAG#2 PORT_LIST [Expected: (PORT#3, PORT#4)]
    attrs[0].id = SAI_LAG_ATTR_PORT_LIST;
    attrs[0].value.objlist.list = port_list;
    attrs[0].value.objlist.count = 64;
    status = lag_api->get_lag_attribute(lag2_oid, 1, attrs);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to get lag attribute SAI_LAG_ATTR_PORT_LIST for LAG 0x%lX, status=%d\n",lag2_oid,  status);
        return 1;
    }

    // Get LAG_MEMBER#1 LAG_ID [Expected: LAG#1]
    attrs[0].id = SAI_LAG_MEMBER_ATTR_LAG_ID;
    status = lag_api->get_lag_member_attribute(lag_mebmer1_oid, 1, attrs);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to get lag attribute SAI_LAG_MEMBER_ATTR_LAG_ID for LAG_MEBMER 0x%lX, status=%d\n",lag_mebmer1_oid,  status);
        return 1;
    }

    // Get LAG_MEMBER#3 PORT_ID [Expected: PORT#3]
    attrs[0].id = SAI_LAG_MEMBER_ATTR_PORT_ID;
    status = lag_api->get_lag_member_attribute(lag_mebmer3_oid, 1, attrs);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to get lag attribute SAI_LAG_MEMBER_ATTR_PORT_ID for LAG_MEBMER 0x%lX, status=%d\n",lag_mebmer3_oid,  status);
        return 1;
    }

    // Remove LAG_MEMBER#2
    status = lag_api->remove_lag_member(lag_mebmer2_oid);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to remove_lag_member, LAG_MEBMER 0x%lX, status=%d\n", lag_mebmer2_oid, status);
        return 1;
    }

    // Get LAG#1 PORT_LIST [Expected: (PORT#1)]
    attrs[0].id = SAI_LAG_ATTR_PORT_LIST;
    attrs[0].value.objlist.list = port_list;
    attrs[0].value.objlist.count = 64;
    status = lag_api->get_lag_attribute(lag1_oid, 1, attrs);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to get lag attribute SAI_LAG_ATTR_PORT_LIST for LAG 0x%lX, status=%d\n",lag1_oid,  status);
        return 1;
    }

    // Remove LAG_MEMBER#3
    status = lag_api->remove_lag_member(lag_mebmer3_oid);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to remove_lag_member, LAG_MEBMER 0x%lX, status=%d\n", lag_mebmer3_oid, status);
        return 1;
    }

    // Get LAG#2 PORT_LIST [Expected: (PORT#4)]
    attrs[0].id = SAI_LAG_ATTR_PORT_LIST;
    attrs[0].value.objlist.list = port_list;
    attrs[0].value.objlist.count = 64;
    status = lag_api->get_lag_attribute(lag2_oid, 1, attrs);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to get lag attribute SAI_LAG_ATTR_PORT_LIST for LAG 0x%lX, status=%d\n",lag2_oid,  status);
        return 1;
    }

    // Remove LAG_MEMBER#1
    status = lag_api->remove_lag_member(lag_mebmer1_oid);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to remove_lag_member, LAG_MEBMER 0x%lX, status=%d\n", lag_mebmer1_oid, status);
        return 1;
    }

    // Remove LAG_MEMBER#4
    status = lag_api->remove_lag_member(lag_mebmer4_oid);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to remove_lag_member, LAG_MEBMER 0x%lX, status=%d\n", lag_mebmer4_oid, status);
        return 1;
    }

    // Remove LAG#2
    status = lag_api->remove_lag(lag2_oid);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to remove_lag, LAG 0x%lX, status=%d\n", lag2_oid, status);
        return 1;
    }

    // Remove LAG#1
    status = lag_api->remove_lag(lag1_oid);
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to remove_lag, LAG 0x%lX, status=%d\n", lag1_oid, status);
        return 1;
    }

    status = sai_api_uninitialize();
    if (status != SAI_STATUS_SUCCESS) 
    {
        printf("Failed to sai_api_uninitialize, status=%d\n", status);
        return 1;
    }

    return 0;
}