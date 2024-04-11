/*
 * task_handler.c
 *

*/

#include "main.h"

/* Function declarations */
void process_cmd(command_t *cmd);
int extract_command(command_t *cmd);
extern UART_HandleTypeDef huart2;

const char *inv_msg = "****Invalid option entered!****\n";

void menu_handler(void *parameters)
{
	const char *menu_msg = "----------------------------------\n"
			"---------------Menu---------------"
			"----------------------------------"
			"Blink LED in pattern    -----> 0\n"
			"Configure date & time   -----> 1\n"
			"Exit                    -----> 2\n"
			"Enter your choice here : ";

	uint32_t notiVal;
	command_t *cmd;
	int option;

	while(1)
	{
		xQueueSend(q_print,&menu_msg,portMAX_DELAY);
		xTaskNotifyWait(0,0,&notiVal,portMAX_DELAY);
		cmd=(command_t *)notiVal;

		if(cmd->length==1)
		{
			option = cmd->data[0]-48;
			switch(option)
			{
			case 0:
				//Notify LED task
				xTaskNotify(led_task_handle,0,eNoAction);
				curr_state = sLedEffect;
				break;
			case 1:
				//Notify RTC task
				xTaskNotify(rtc_task_handle,0,eNoAction);
				curr_state = sRtcReport;
				break;
			case 2:
				//exit
				break;
			default:
				xQueueSend(q_print,&inv_msg,portMAX_DELAY);
				continue;
			}
		}
		else{
			//Invalid
			xQueueSend(q_print,&inv_msg,portMAX_DELAY);
		}

		//Blocked till notification is received from any other task
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
	}

}
void cmd_handler(void *parameters)
{
	BaseType_t ret;
	command_t cmd;

	ret = xTaskNotifyWait(0,0,NULL,portMAX_DELAY);

	if(ret==pdPASS){
		process_cmd(&cmd);
	}
}


void process_cmd(command_t *cmd)
{
	extract_command(cmd);

	switch(curr_state)
	{
	case sMainMenu:
		xTaskNotify(menu_task_handle,(uint32_t)cmd,eSetValueWithOverwrite);
		break;
	case sLedEffect:
		xTaskNotify(led_task_handle,(uint32_t)cmd,eSetValueWithOverwrite);
		break;
	case sRtcMenu:
	case sRtcTimeConfig:
	case sRtcDateConfig:
	case sRtcReport:
		xTaskNotify(rtc_task_handle,(uint32_t)cmd,eSetValueWithOverwrite);
		break;
	}
}


int extract_command(command_t *cmd)
{
	uint8_t val,ind;
	BaseType_t status;

	status = uxQueueMessagesWaiting(q_data);
	if(!status)
	{
		return -1;
	}
	ind=0;
	do
	{
		status = xQueueReceive(q_data,&val,0);
		if(status==pdPASS)
		{
			cmd->data[ind++]=val;
		}
	} while(val != '\n');

	cmd->data[ind-1]='\0';
	cmd->length=ind-1;
	return 0;
}


void rtc_handler(void *parameters)
{
		const char* msg_rtc1 = "========================\n"
								"|         RTC          |\n"
								"========================\n";

		const char* msg_rtc2 = "Configure Time            ----> 0\n"
								"Configure Date            ----> 1\n"
								"Enable reporting          ----> 2\n"
								"Exit                      ----> 4\n"
								"Enter your choice here : ";


		const char *msg_rtc_hh = "Enter hour(1-12):";
		const char *msg_rtc_mm = "Enter minutes(0-59):";
		const char *msg_rtc_ss = "Enter seconds(0-59):";

		const char *msg_rtc_dd  = "Enter date(1-31):";
		const char *msg_rtc_mo  ="Enter month(1-12):";
		const char *msg_rtc_dow  = "Enter day(1-7 sun:1):";
		const char *msg_rtc_yr  = "Enter year(0-99):";

		const char *msg_conf = "Configuration successful\n";
		const char *msg_rtc_report = "Enable time&date reporting(y/n)?: ";


		uint32_t cmd_addr;
		command_t *cmd;


		while(1){
			/*TODO: Notify wait (wait till someone notifies)

			/*TODO : Print the menu and show current date and time information */


			while(1){
					xQueueSend(q_print,&msg_menu,portMAX_DELAY);

					//wait for menu commands
					xTaskNotifyWait(0,0,&cmd_addr,portMAX_DELAY);
					cmd = (command_t*)cmd_addr;

					if(cmd->len == 1)
					{
						option = cmd->payload[0] - 48;
						switch(option)
						{
							case 0:
								curr_state = sLedEffect;
								xTaskNotify(handle_led_task,0,eNoAction);
								break;
							case 1:
								curr_state = sRtcMenu;
								xTaskNotify(handle_rtc_task,0,eNoAction);
								break;
							case 2: /*implement exit */
								break;
							default:
								xQueueSend(q_print,&msg_inv,portMAX_DELAY);
								continue;
						}

					}else{
						//invalid entry
						xQueueSend(q_print,&msg_inv,portMAX_DELAY);
						continue;
					}


					//wait to run again when some other task notifies
					xTaskNotifyWait(0,0,NULL,portMAX_DELAY);

				}//while super loop

}


void led_handler(void *parameters)
{
	const char *led_msg = "----------------------------------\n"
				"------------LED Effect------------\n"
				"----------------------------------\n"
				"Pattern choices : None,e1,e2,e3,e4\n"
				"Enter your choice here : ";
	uint32_t notiVal;
	command_t *cmd;

	while(1)
	{
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
		xQueueSend(q_print,&led_msg,portMAX_DELAY);
		xTaskNotifyWait(0,0,&notiVal,portMAX_DELAY);
		cmd=(command_t *)notiVal;

		if(cmd->length<=4)
		{
			if(!strcmp((char*)cmd->data,"None"))
				stop_led_effect();
			else if(!strcmp((char*)cmd->data,"e1"))
				blink_led_in_pattern(1);
			else if(!strcmp((char*)cmd->data,"e2"))
				blink_led_in_pattern(2);
			else if(!strcmp((char*)cmd->data,"e3"))
				blink_led_in_pattern(3);
			else if(!strcmp((char*)cmd->data,"e4"))
				blink_led_in_pattern(4);
			else
				xQueueSend(q_print,&inv_msg,portMAX_DELAY);
		}
		else
		{
			xQueueSend(q_print,&inv_msg,portMAX_DELAY);
		}

		//Setting main menu as current state & notifying it
		curr_state = sMainMenu;
		xTaskNotify(menu_task_handle,0,eNoAction);
	}

}



void print_handler(void *parameters)
{
	uint32_t *msg;
	while(1)
	{
		xQueueReceive(q_print,&msg,portMAX_DELAY);
		HAL_UART_Transmit(&huart2,(uint8_t *)msg,strlen((char*)msg),HAL_MAX_DELAY);
	}

}
