#include "multi_ep_fixer.h"

/* Store previous write attr hook for the Identify cluster. */
zb_zcl_cluster_write_attr_hook_t m_p_identify_write_hook;

static zb_void_t sort_ep_desc_list(zb_uint8_t dst_ep)
{
    zb_uint8_t          dst_ep_desc_index   = 0;
    zb_af_device_ctx_t  *p_dev_ctx          = zb_zcl_get_ctx()->device_ctx;

    /* Find index of dest_ep desc in ep_desc list. */
    while ((p_dev_ctx->ep_desc_list)[dst_ep_desc_index]->ep_id != dst_ep)
    {
        dst_ep_desc_index++;
    }

    /* Check if desc of dest_ep is stored as last element of the ep desc list. */
    if (dst_ep_desc_index < (p_dev_ctx->ep_count - 1))
    {
        /* Store last ep_desc as temp ep_desc  */
        zb_af_endpoint_desc_t *p_temp_ep_desc = (p_dev_ctx->ep_desc_list)[(p_dev_ctx->ep_count - 1)];
        /* Put desired ep_desc as last */
        (p_dev_ctx->ep_desc_list)[(p_dev_ctx->ep_count - 1)] = (p_dev_ctx->ep_desc_list)[dst_ep_desc_index];
        /* Restore temp ep_desc in new place */
        (p_dev_ctx->ep_desc_list)[dst_ep_desc_index] = p_temp_ep_desc;
    }
}

static zb_uint8_t multi_ep_handler(zb_bufid_t bufid)
{
    zb_zcl_parsed_hdr_t *cmd_info = ZB_BUF_GET_PARAM(bufid, zb_zcl_parsed_hdr_t);
    /* Nbr of endpoint to which frame is to be delivered. */
    zb_uint8_t          dst_ep    = cmd_info->addr_data.common_data.dst_endpoint;

    if ((cmd_info->cmd_id == ZB_ZCL_CMD_CONFIG_REPORT)
        || (cmd_info->cmd_id == ZB_ZCL_CMD_REPORT_ATTRIB)
        || (cmd_info->cmd_id == ZB_ZCL_CMD_READ_REPORT_CFG))
    {
        sort_ep_desc_list(dst_ep);
    }
    return ZB_FALSE;
}

static void multi_ep_identify_cluster_write_attr_hook(zb_uint8_t endpoint, zb_uint16_t attr_id, zb_uint8_t *new_value)
{
    sort_ep_desc_list(endpoint);
    m_p_identify_write_hook(endpoint, attr_id, new_value);
}

static void multi_ep_generic_cluster_write_attr_hook(zb_uint8_t endpoint, zb_uint16_t attr_id, zb_uint8_t *new_value)
{
    ZVUNUSED(attr_id);
    ZVUNUSED(new_value);

    sort_ep_desc_list(endpoint);
}

/* Function which initializes fixer methods and hooks. */
void init_multi_ep_fixers(void)
{
    zb_zcl_globals_t * p_zcl_ctx = zb_zcl_get_ctx();
    zb_uint8_t endpoint = 0;
    zb_uint8_t index = 0;

    ZB_ASSERT(p_zcl_ctx);

    /* Find Identify attr hook, store previous value if exist and assing new identify cluster wrtie attr hook. */
    while (index < p_zcl_ctx->zcl_handlers_cnt)
    {
        if ((p_zcl_ctx->zcl_handlers[index].cluster_id == ZB_ZCL_CLUSTER_ID_IDENTIFY)
            && (p_zcl_ctx->zcl_handlers[index].cluster_write_attr_hook != NULL))
        {
            m_p_identify_write_hook = p_zcl_ctx->zcl_handlers[index].cluster_write_attr_hook;
            p_zcl_ctx->zcl_handlers[index].cluster_write_attr_hook = multi_ep_identify_cluster_write_attr_hook;
        }
        /* For clusters with ID >= 0x03, if there is no attr hook assigned, assign generic one. If already exists, assert. */
        else if (p_zcl_ctx->zcl_handlers[index].cluster_id >= ZB_ZCL_CLUSTER_ID_IDENTIFY)
        {
            ZB_ASSERT(p_zcl_ctx->zcl_handlers[index].cluster_write_attr_hook == NULL);

            p_zcl_ctx->zcl_handlers[index].cluster_write_attr_hook = multi_ep_generic_cluster_write_attr_hook;
        }
        index++;
    }

    /* For each endpoint register device callback. Make sure that there is no endpoint handler set already. */
    for (index = 0; index < p_zcl_ctx->device_ctx->ep_count; index++)
    {
        endpoint = p_zcl_ctx->device_ctx->ep_desc_list[index]->ep_id;

        ZB_ASSERT(zb_af_get_endpoint_desc((endpoint))->device_handler == NULL);
        ZB_AF_SET_ENDPOINT_HANDLER(endpoint, multi_ep_handler);
    }
}