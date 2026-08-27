// GNSS - Global Navigation Satellite System
#include <driver/uart.h>
#include <string.h>

#include "gnss.h"
#include "nmea_rmc.h"
#include "ring_buffer.h"


const char *TAG_GNSS = "[GNSS]";

const uint32_t UART_BAUD_RATE = 38400;
const uint32_t RX_BUFFER_SIZE = 256;
const uint32_t TX_BUFFER_SIZE = 0;
const uint32_t QUEUE_SIZE = 0;

const int8_t UART_NUM = UART_NUM_1;
const uint8_t GPIO_GNSS_UART_TXD = 1;
const uint8_t GPIO_GNSS_UART_RXD = 2;


//--------------------------------------------------
void init_gnss_urat(void){
    ESP_LOGI(TAG_GNSS, "Install UART Driver");
    uart_driver_install(UART_NUM, RX_BUFFER_SIZE, TX_BUFFER_SIZE, QUEUE_SIZE, NULL, 0);

    ESP_LOGI(TAG_GNSS, "Setup UART Configuration");
    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, GPIO_GNSS_UART_TXD, GPIO_GNSS_UART_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}


//--------------------------------------------------
void gnss_rx_task(void *arg){
    ESP_LOGI(TAG_GNSS, "Start UART RX Task");
    uint8_t data[RX_BUFFER_SIZE + NULL_TERMINATOR_BYTES];

    while (1) {
        const int rx_bytes = uart_read_bytes(UART_NUM, data, RX_BUFFER_SIZE, 1000 / portTICK_PERIOD_MS);
        if (rx_bytes <= 0){ continue; }

        data[rx_bytes] = NULL_TERMINATOR;
        ASCII_DATA uart_data = {.data = data, .offset = 0};
        parse_data(&uart_data, rx_bytes);
    }
}


void parse_data(ASCII_DATA *data, int length){
    while (data->offset < length){
        SKIP_NMEA_MSG(data)

        // skip starting character
        data->offset++;

        // Note: offset TT bytes - check only the NMEA message type (SSS)
        // Format: TTSSS
        // TT: Talker Identifier
        // SSS: Sentence formatter (MSG Type: RMC)
        const char *msg_type = (const char *)&data->data[data->offset + TT_BYTES];

        // ignore other messages
        if (strncmp(msg_type, NMEA_RMC_MSG, SSS_BYTES)){
            continue;
        }

        TalkerIdentifier tt = {};
        SentenceFormater sss = {};
        parse_talker_identifier(&tt, data);
        parse_sentence_formater(&sss, data);

        ESP_LOGI(TAG_GNSS, "GNSS SYSTEM: %s", tt.value);
        ESP_LOGI(TAG_GNSS, "GNSS SYSTEM: %s", sss.value);

        // skip first comma - separator
        data->offset++;

        ESP_LOG_BUFFER_HEXDUMP(TAG_GNSS, &data->data[data->offset], 64, ESP_LOG_INFO);

        NMEA_RMC rmc = {};
        parse_nmea_rmc(&rmc, data);
        log_nmea_rmc(stdout, &rmc);

        Coordinate coordinate = {};
        create_coordinate(&coordinate, &rmc.latitude, &rmc.longitude);
        TrackingData tracking_data = {.utc_time = rmc.utc_time, .coordinate = coordinate, .speed = rmc.speed, .angle = rmc.angle};

        ring_buffer_insert(&tracking_ring_buffer, &tracking_data);
    }
}



