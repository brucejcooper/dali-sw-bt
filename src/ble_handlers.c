#include "ble_handlers.h"

#include <da1458x_config_basic.h>
#include <da1458x_config_advanced.h>
#include <user_config.h>
#include <rwip_config.h>
#include "prf_utils.h"

#include <ke_msg.h>
#include <custs1.h>
#include <custs1_task.h>
#include <user_custs1_def.h>
#include <user_custs1_def.h>
#include <gapm_task.h>
#include <gapc_task.h>
#include "updi.h"
#include "user_app.h"

#include <debug.h>

static const char* msgidToString(ke_msg_id_t msgid)
{
    const char* result = NULL;
    switch (msgid)
    {
        case CUSTS1_CREATE_DB_REQ:
        {
            result = "CUSTS1_CREATE_DB_REQ";
            break;
        }
        case CUSTS1_CREATE_DB_CFM:
        {
            result = "CUSTS1_CREATE_DB_CFM";
            break;
        }
        case CUSTS1_ENABLE_REQ:
        {
            result = "CUSTS1_ENABLE_REQ";
            break;
        }
        case CUSTS1_VAL_SET_REQ:
        {
            result = "CUSTS1_VAL_SET_REQ";
            break;
        }
        case CUSTS1_VALUE_REQ_IND:
        {
            result = "CUSTS1_VALUE_REQ_IND";
            break;
        }
        case CUSTS1_VALUE_REQ_RSP:
        {
            result = "CUSTS1_VALUE_REQ_RSP";
            break;
        }
        case CUSTS1_VAL_NTF_REQ:
        {
            result = "CUSTS1_VAL_NTF_REQ";
            break;
        }
        case CUSTS1_VAL_NTF_CFM:
        {
            result = "CUSTS1_VAL_NTF_CFM";
            break;
        }
        case CUSTS1_VAL_IND_REQ:
        {
            result = "CUSTS1_VAL_IND_REQ";
            break;
        }
        case CUSTS1_VAL_IND_CFM:
        {
            result = "CUSTS1_VAL_IND_CFM";
            break;
        }
        case CUSTS1_VAL_WRITE_IND:
        {
            result = "CUSTS1_VAL_WRITE_IND";
            break;
        }
        case CUSTS1_DISABLE_IND:
        {
            result = "CUSTS1_DISABLE_IND";
            break;
        }
        case CUSTS1_ERROR_IND:
        {
            result = "CUSTS1_ERROR_IND";
            break;
        }
        case CUSTS1_ATT_INFO_REQ:
        {
            result = "CUSTS1_ATT_INFO_REQ";
            break;
        }
        case CUSTS1_ATT_INFO_RSP:
        {
            result = "CUSTS1_ATT_INFO_RSP";
            break;
        }
        case GAPM_CMP_EVT:
        {
            result = "GAPM Command complete";
            break;
        }
        case GAPC_PARAM_UPDATED_IND:
            result = "connection parameters udpated";
            break;
        default:
        {
            break;
        }
    }
    return result;
}


void user_svc1_read_userrow(ke_msg_id_t const msgid,
                            struct custs1_value_req_ind const *param,
                            ke_task_id_t const dest_id,
                            ke_task_id_t const src_id)
{
    struct custs1_value_req_rsp *rsp = KE_MSG_ALLOC_DYN(CUSTS1_VALUE_REQ_RSP,
                                                        prf_get_task_from_id(TASK_ID_CUSTS1),
                                                        TASK_APP,
                                                        custs1_value_req_rsp,
                                                        32);
    // Provide the connection index.
    rsp->conidx  = app_env[param->conidx].conidx;
    // Provide the attribute index.
    rsp->att_idx = param->att_idx;


    wdg_reload(200); // Give it up to 2000 ms to read the value.
    rsp->length  = 32;
    updi_err_t err = updi_read_user_row(rsp->value);
    if (err) {
        DEBUG_PRINT_STRING("Error fetching UPDI ");
        DEBUG_PRINT_INT(err);
        DEBUG_PRINT_STRING("\r\n");
        rsp->length = 0;
        rsp->status  = ATT_ERR_APP_ERROR;
    } else {
        rsp->status  = ATT_ERR_NO_ERROR;
    }

    // Send message
    ke_msg_send(rsp);
}


void user_catch_rest_hndl(ke_msg_id_t const msgid, void const *param, ke_task_id_t const dest_id, ke_task_id_t const src_id)
{
    switch(msgid)
    {
        case CUSTS1_VALUE_REQ_IND:
        {
            struct custs1_value_req_ind const *msg_param = (struct custs1_value_req_ind const *)(param);
            switch (msg_param->att_idx) {
                case SVC1_IDX_USERROW_VAL:
                {
                    user_svc1_read_userrow(msgid, msg_param, dest_id, src_id);
                } 
                break;

                default:
                {
                    // Send Error message
                    struct custs1_value_req_rsp *rsp = KE_MSG_ALLOC(CUSTS1_VALUE_REQ_RSP,
                                                                    src_id,
                                                                    dest_id,
                                                                    custs1_value_req_rsp);

                    // Provide the connection index.
                    rsp->conidx  = app_env[msg_param->conidx].conidx;
                    // Provide the attribute index.
                    rsp->att_idx = msg_param->att_idx;
                    // Force current length to zero.
                    rsp->length = 0;
                    // Set Error status
                    rsp->status  = ATT_ERR_APP_ERROR;
                    // Send message
                    ke_msg_send(rsp);
                } 
                break;
            }
        }
        break;

        
        default:
        {
            // We are receiving a Generic Task Layer handler mssg. 
            char *msg = (char *) msgidToString(msgid);
            DEBUG_PRINT_STRING("handler: ");
            if (msg) {
                DEBUG_PRINT_STRING(msg);
            } else {
                DEBUG_PRINT_INT(msgid);
            }
            DEBUG_PRINT_STRING("\r\n");

            break;
        }
    }
}

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    DEBUG_PRINT_STRING("user_on_connection()\r\n");
    default_app_on_connection(connection_idx, param);


    updi_send_break();
    updi_sib_t sib;
    updi_err_t err = updi_get_sib(&sib);
    if (err) {
        DEBUG_PRINT_STRING("Error getting SIB\r\n");
    }
    DEBUG_PRINT_STRING("SIB ");
    DEBUG_PRINT_STRING((char *) &sib);
    DEBUG_PRINT_STRING("\r\n");

}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    DEBUG_PRINT_STRING("user_on_disconnect()\r\n");
    default_app_on_disconnect(param);

    updi_reset_device();
}
