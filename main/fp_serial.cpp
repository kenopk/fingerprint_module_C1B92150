#include "fp_serial.h"
#include "driver/uart.h"

static bool To_Data_Bits(int Num_Data_Bits, uart_word_length_t &Result);
static bool To_Stop_Bits(int Num_Stop_Bits, uart_stop_bits_t &Result);
//   -1  FP_SERIAL_ERR_ARG          неверный аргумент: нулевой указатель вместо буфера, Length равен
//                                  нулю (TX), скорость 0 или меньше, Data_Bits не из 5...8, Stop_Bits
//                                  не 1 и не 2, номер порта вне допустимого диапазона. Ничего не
//                                  выполнялось. Реакция: исправить вызов.
//   -2  FP_SERIAL_ERR_NOT_OPEN     порт не открыт: функция вызвана до успешного fp_serial_init (или
//                                  init не удался). Реакция: вызвать fp_serial_init.
//   -3  FP_SERIAL_ERR_ALREADY_OPEN порт уже открыт: повторный вызов fp_serial_init. Только init.
//                                  Первый порт при этом не затрагивается.
//   -4  FP_SERIAL_ERR_DRIVER       отказ драйвера платформы: любой вызов IDF вернул ошибку (установка
//                                  драйвера, применение параметров, назначение пинов, запись, чтение,
//                                  сброс буфера, смена скорости). Для init порт остаётся закрытым.
//                                  Это проблема нашей стороны, а не модуля; повторять обычно бессмысленно.
//   -5  FP_SERIAL_ERR_TX_TIMEOUT   не удалось дождаться окончания передачи: байты приняты драйвером, но
//                                  за отведённое время (100 мс) не ушли на линию или ожидание вернуло
//                                  ошибку. Только TX. Данные могли быть отправлены частично.

static int s_port = -1;   // -1 = порт закрыт 

int fp_serial_init(int Baudrate_Prot,int Data_Bits,int Stop_Bits, int8_t RX_Pin,int8_t TX_Pin,int8_t Port)
{
    const uart_port_t Num_Port = static_cast<uart_port_t>(Port);
    if (s_port >= 0) return FP_SERIAL_ERR_ALREADY_OPEN; // порт уже открыт
    if (Baudrate_Prot <= 0 || Data_Bits < 5 || Data_Bits > 8 || (Stop_Bits != 1 && Stop_Bits != 2) ||
        RX_Pin < 0 || TX_Pin < 0 || Num_Port < 0 || Num_Port >= SOC_UART_HP_NUM || RX_Pin == TX_Pin)
        return FP_SERIAL_ERR_ARG; // неверный параметр
    uart_config_t uart_config = {};
    uart_config.baud_rate = Baudrate_Prot;
    if (!To_Data_Bits(Data_Bits, uart_config.data_bits)) {
        return FP_SERIAL_ERR_ARG;
    }
    if (!To_Stop_Bits(Stop_Bits, uart_config.stop_bits)) {
        return FP_SERIAL_ERR_ARG;
    }
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_DEFAULT;

    if (uart_driver_install(static_cast<uart_port_t>(Num_Port), 256, 0, 0, NULL, 0) != ESP_OK) return FP_SERIAL_ERR_DRIVER;
    
    if(uart_param_config(static_cast<uart_port_t>(Num_Port), &uart_config) != ESP_OK){
        uart_driver_delete(static_cast<uart_port_t>(Num_Port));
        return FP_SERIAL_ERR_DRIVER;
    }
    if(uart_set_pin(static_cast<uart_port_t>(Num_Port), TX_Pin, RX_Pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK){
        uart_driver_delete(static_cast<uart_port_t>(Num_Port));
        return FP_SERIAL_ERR_DRIVER;
    }

    s_port = Num_Port;

    return 0;
}
static bool To_Data_Bits(int Num_Data_Bits, uart_word_length_t &Result)
{
    switch(Num_Data_Bits) {
        case 5: Result = UART_DATA_5_BITS; return true;
        case 6: Result = UART_DATA_6_BITS; return true;
        case 7: Result = UART_DATA_7_BITS; return true;
        case 8: Result = UART_DATA_8_BITS; return true;
        default: return false;
    }
}

static bool To_Stop_Bits(int Num_Stop_Bits, uart_stop_bits_t &Result)
{
    switch(Num_Stop_Bits) {
        case 1: Result = UART_STOP_BITS_1; return true;
        case 2: Result = UART_STOP_BITS_2; return true;
        default: return false;
    }
}

int fp_serial_TX(const uint8_t *Data, size_t Length){
    if (s_port < 0) return FP_SERIAL_ERR_NOT_OPEN; // порт не открыт
    // Реализация отправки данных через UART
    return 0;
}