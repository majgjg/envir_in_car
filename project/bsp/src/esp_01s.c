#include "esp_01s.h"
#include <string.h>
#include <stdio.h>
#include "telemetry.h"

#define WIFI_SSID		"vivo X200 Pro mini"
#define WIFI_PASS		"88888888"
#define SERVER_IP		"192.168.221.17"                                                                                                                                                                                 
#define SERVER_PORT		9000


//接收区的环形缓冲
#define UART_RX_BUF_SIZE			1024
static volatile uint16_t rx_write_idx = 0,rx_read_idx = 0;
static uint8_t uart_rx_ringbuf[UART_RX_BUF_SIZE];

static UART_HandleTypeDef *esp_uart = NULL;
static uint8_t uart_rx_byte;

static void uart_rxbuf_push(uint8_t rx_byte)
{
	uint16_t next_write_idx =rx_write_idx + 1;

	if(next_write_idx >= UART_RX_BUF_SIZE)
	{
		next_write_idx = 0;
	}
	
	if(next_write_idx ==  rx_read_idx)
	{
		rx_read_idx++;
		if(rx_read_idx >= UART_RX_BUF_SIZE)
		{
			rx_read_idx = 0;
		}
	}
	
	uart_rx_ringbuf[rx_write_idx] = rx_byte;
	rx_write_idx = next_write_idx;
}

//清除缓冲�?
static void uart_rxbuf_clear(void)
{
	rx_read_idx = 0;
	rx_write_idx = 0;
}

//查找关键�?
static int uart_rxbuf_contains(const char *keyword)
{
	static char temp[UART_RX_BUF_SIZE + 1];
	uint16_t idx = 0;
	uint16_t r = rx_read_idx;

	while(r != rx_write_idx && idx < UART_RX_BUF_SIZE)
	{
		temp[idx++] = (char)uart_rx_ringbuf[r];
		r++;
		if(r >= UART_RX_BUF_SIZE) r=0;
		
	}

	temp[idx] = '\0';
		
	return (strstr(temp,keyword) != NULL);
}

//uart接收中断
static void uart_rx_restart(void)
{
	HAL_UART_Receive_IT(esp_uart, &uart_rx_byte, 1);
}

//uart完成接收后产生回�?
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(esp_uart && huart->Instance == esp_uart->Instance)
	{
		uart_rxbuf_push(uart_rx_byte);
		uart_rx_restart();
	}
}




//AT指令发�?
static void esp_uart_send(const char *str)
{
	HAL_UART_Transmit(esp_uart, (uint8_t *)str, strlen(str),2000);
}

static void esp_send_at_cmd(const char *cmd)
{
	esp_uart_send(cmd);
	esp_uart_send("\r\n");
}

//等待关键字（简单轮询）
static esp_status_t esp_wait_response(const char *keyword, uint32_t timeout_ms)
{
	uint32_t start = HAL_GetTick();
	
	while(HAL_GetTick() - start < timeout_ms)
	{
		if(uart_rxbuf_contains(keyword))
		{
			return ESP_OK;
		}
		HAL_Delay(1);
	}
	return ESP_TIMEOUT;
}


//ESP 连接与上报状态机
typedef enum
{
	ESP_STATE_AT = 0,
	ESP_STATE_SET_MODE,
	ESP_STATE_JOIN_AP,
	ESP_STATE_SET_MUX,
	ESP_STATE_TCP_CONNECT,
	ESP_STATE_RUN,
	ESP_STATE_RETRY
}esp_state_t;

static esp_state_t esp_state = ESP_STATE_AT;
static uint32_t last_send_tick = 0;

//发送一行数据（以\\n结尾�?
static esp_status_t esp_tcp_send_line(const char *line)
{
	char cmd[32];
	uint16_t len = strlen(line);
	uart_rxbuf_clear();
	snprintf(cmd,sizeof(cmd), "AT+CIPSEND=%u",len);
	esp_send_at_cmd(cmd);
	
	if(esp_wait_response(">", 3000) != ESP_OK) return ESP_TIMEOUT;
	
	uart_rxbuf_clear();
    esp_uart_send(line);
	
	if (esp_wait_response("SEND OK", 5000) != ESP_OK)
	return ESP_TIMEOUT;

    return ESP_OK;
}


//初始化：绑定UART并启动接收中�?
void ESP_Init(UART_HandleTypeDef *huart)
{
	esp_uart = huart;
	uart_rxbuf_clear();
	uart_rx_restart();
	esp_state = ESP_STATE_AT;
	
}

//状态机主循环：按步骤入网、建连、发�?
void ESP_Task()
{
	switch(esp_state)
	{
		case ESP_STATE_AT:
/* 测试AT */
			uart_rxbuf_clear();
			esp_send_at_cmd("AT");
			if(esp_wait_response("OK", 1000) == ESP_OK)
			{
				esp_state = ESP_STATE_SET_MODE;
			}
			
			else
			{
				esp_state = ESP_STATE_RETRY;
			}
			
			break;

		case ESP_STATE_SET_MODE:
/* 设置为STA模式 */
			uart_rxbuf_clear();
			esp_send_at_cmd("AT+CWMODE=1");
			if (esp_wait_response("OK", 1000) == ESP_OK)
			{
				esp_state = ESP_STATE_JOIN_AP;
			}

			else
			{
				esp_state = ESP_STATE_RETRY;
			}

			break;

		case ESP_STATE_JOIN_AP:
/* 连接AP */ 
		{
			char cmd[128];
			uart_rxbuf_clear();
			snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"", WIFI_SSID, WIFI_PASS);
			esp_send_at_cmd(cmd);

			if (esp_wait_response("OK", 20000) == ESP_OK)
			{
				esp_state = ESP_STATE_SET_MUX;
			}

			else
			{
				esp_state = ESP_STATE_RETRY;
			}

			break;
		}

		case ESP_STATE_SET_MUX:
/* 单连接模�?*/
			uart_rxbuf_clear();
			esp_send_at_cmd("AT+CIPMUX=0");
			if (esp_wait_response("OK", 1000) == ESP_OK)
			{
				esp_state = ESP_STATE_TCP_CONNECT;
			}

			else
			{
				esp_state = ESP_STATE_RETRY;
			}

			break;

		case ESP_STATE_TCP_CONNECT:
/* 连接TCP服务�?*/
		{
			char cmd[96];
			uart_rxbuf_clear();
			snprintf(cmd, sizeof(cmd),
					 "AT+CIPSTART=\"TCP\",\"%s\",%d",
					 SERVER_IP, SERVER_PORT);
			esp_send_at_cmd(cmd);

			if (esp_wait_response("CONNECT", 3000) == ESP_OK ||
				esp_wait_response("OK", 3000) == ESP_OK)
			{
				esp_state = ESP_STATE_RUN;
				last_send_tick = HAL_GetTick();
			} 
			else 
			{
				esp_state = ESP_STATE_RETRY;
			}
			break;
		}

		case ESP_STATE_RUN:
/* 定时上报（每1秒） */
			if (HAL_GetTick() - last_send_tick >= 1000)
			{
				last_send_tick = HAL_GetTick();

				const char *line = TEL_GetLastLine();
				if (line && line[0] != '\0')
				{
					if (esp_tcp_send_line(line) != ESP_OK)
					{
						esp_state = ESP_STATE_TCP_CONNECT;
					}
				}
			}
			break;

		case ESP_STATE_RETRY:
/* 失败后延时重�?*/
			HAL_Delay(500);
			esp_state = ESP_STATE_AT;
			break;

		default:
			esp_state = ESP_STATE_AT;
			break;
    }
}










































