/***********************************************************************************************************************
* File Name        : can_cafd_task.ino
* Device(s)        : Arduino UNO R4 Minima 
* MCU              : RA4M1(Single core)
***********************************************************************************************************************/
#include <SPI.h>
#include <EEPROM.h>
#include "r4_mcp2518fd_can.h"

/**************************************************************************************
 Version
 *************************************************************************************/
#define FW_VER_INF "VER.00-03 (2025-04-21)"

/**************************************************************************************
 Macros
 *************************************************************************************/
#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0]))

/**************************************************************************************
 Arduino Config
 *************************************************************************************/
//Baudrate
#define UART_BAUDRATE 115200
//GPIO
#define GPIO_OUP_MCP2518FD_SPI_CS1 9
#define GPIO_INP_MCP2518FD_INT 4

/**************************************************************************************
 Declaration for CAN
 **************************************************************************************/
#define CAN20_MAX_DATA_LEN 8
#define CANFD_MAX_DATA_LEN 64
/**************************************************************************************
 Typedef for CAN
 *************************************************************************************/
//! CAN operation enum
typedef enum {
  CAN_OPERATION_CAN20 = 0,
  CAN_OPERATION_CANFD
} can_operation_t;

//! CAN operation type
typedef struct {
  const char *name;
  can_operation_t operation;
  byte id;
} CAN_OPERATION_TABLE_T;

//! CAN2.0 config type
typedef struct {
  uint32_t bitrate;
  byte mode;
} can20_config_t;

//! CANFD config type
typedef struct {
  uint32_t bitrate;
  byte mode;
} canfd_config_t;

//! CAN2.0 bitrate config tabel
typedef struct {
  const char *name;  // 設定名
  uint32_t bitrate;  // スピード設定（通常は通信速度）
  byte id;           // キーとしての役割
} CAN20_BITRATE_CONFIG_TABLE_T;

//! CANFD bitrate config tabel
typedef struct {
  const char *name;  // 設定名
  uint32_t bitrate;  // スピード設定（通常は通信速度）
  byte id;           // キーとしての役割
} CANFD_BITRATE_CONFIG_TABLE_T;

//! CHIP mode config tabel
typedef struct {
  const char *name;  // 設定名
  byte mode;         // モード設定
  byte id;           // キーとしての役割
} CHIP_MODE_CONFIG_TABLE_T;

/**************************************************************************************
 Table for CAN
 *************************************************************************************/
const CAN_OPERATION_TABLE_T CAN_OPERATION_TABLE[] = {
  { "CAN2.0", CAN_OPERATION_CAN20, 0x01 },
  { "CANFD", CAN_OPERATION_CANFD, 0x02 }
};

//! CAN2.0 BITRARE CONFIG TABLE
const CAN20_BITRATE_CONFIG_TABLE_T CAN20_BITRATE_CONFIG_TABLE[] = {
  { "5KBPS", CAN20_5KBPS, 0x01 },
  { "10KBPS", CAN20_10KBPS, 0x02 },
  { "20KBPS", CAN20_20KBPS, 0x03 },
  { "25KBPS", CAN20_25KBPS, 0x04 },
  { "31K25BPS", CAN20_31K25BPS, 0x05 },
  { "33KBPS", CAN20_33KBPS, 0x06 },
  { "40KBPS", CAN20_40KBPS, 0x07 },
  { "50KBPS", CAN20_50KBPS, 0x08 },
  { "80KBPS", CAN20_80KBPS, 0x09 },
  { "83K3BPS", CAN20_83K3BPS, 0x0A },
  { "95KBPS", CAN20_95KBPS, 0x0B },
  { "100KBPS", CAN20_100KBPS, 0x0C },
  { "125KBPS", CAN20_125KBPS, 0x0D },
  { "200KBPS", CAN20_200KBPS, 0x0E },
  { "250KBPS", CAN20_250KBPS, 0x0F },
  { "500KBPS", CAN20_500KBPS, 0x10 },
  { "666KBPS", CAN20_666KBPS, 0x11 },
  { "800KBPS", CAN20_800KBPS, 0x12 },
  { "1000KBPS", CAN20_1000KBPS, 0x13 }
};

//! CANFD BITRATE CONFIG TABLE
const CANFD_BITRATE_CONFIG_TABLE_T CANFD_BITRATE_CONFIG_TABLE[] = {
  { "125K500K", CAN_125K_500K, 0x01 },
  { "250K500K", CAN_250K_500K, 0x02 },
  { "250K750K", CAN_250K_750K, 0x03 },
  { "250K1M", CAN_250K_1M, 0x04 },
  { "250K1M5", CAN_250K_1M5, 0x05 },
  { "250K2M", CAN_250K_2M, 0x06 },
  { "250K3M", CAN_250K_3M, 0x07 },
  { "250K4M", CAN_250K_4M, 0x08 },
  { "500K1M", CAN_500K_1M, 0x09 },
  { "500K2M", CAN_500K_2M, 0x0A },
  { "500K3M", CAN_500K_3M, 0x0B },
  { "500K4M", CAN_500K_4M, 0x0C },
  { "500K5M", CAN_500K_5M, 0x0D },
  { "500K6M5", CAN_500K_6M5, 0x0E },
  { "500K8M", CAN_500K_8M, 0x0F },
  { "500K10M", CAN_500K_10M, 0x10 },
  { "1000K4M", CAN_1000K_4M, 0x11 },
  { "1000K8M", CAN_1000K_8M, 0x12 }
};

//! CHIP MODE CONFIG TABLE
const CHIP_MODE_CONFIG_TABLE_T CAN20_CHIP_MODE_CONFIG_TABLE[] = {
  { "NORMAL_MODE", CAN_NORMAL_MODE, 0x01 },  //CANFDとCAN2.0の両対応。CAN2.0の通信速度で設定すると送信できない
  { "SLEEP_MODE", CAN_SLEEP_MODE, 0x02 },
  { "INTERNAL_LOOPBACK_MODE", CAN_INTERNAL_LOOPBACK_MODE, 0x03 },
  { "LISTEN_ONLY_MODE", CAN_LISTEN_ONLY_MODE, 0x04 },
  { "CONFIGURATION_MODE", CAN_CONFIGURATION_MODE, 0x05 },
  { "EXTERNAL_LOOPBACK_MODE", CAN_EXTERNAL_LOOPBACK_MODE, 0x06 },
  { "CLASSIC_MODE", CAN_CLASSIC_MODE, 0x07 },  //CAN2.0のみ対応。CAN2.0で動作させるときは、このモードに入れる
  { "RESTRICTED_MODE", CAN_RESTRICTED_MODE, 0x08 },
  { "INVALID_MODE", CAN_INVALID_MODE, 0x09 }
};

//! CHIP MODE CONFIG TABLE
const CHIP_MODE_CONFIG_TABLE_T CANFD_CHIP_MODE_CONFIG_TABLE[] = {
  { "NORMAL_MODE", CAN_NORMAL_MODE, 0x01 },  //CANFDとCAN2.0の両対応。CAN2.0の通信速度で設定すると送信できない
  { "SLEEP_MODE", CAN_SLEEP_MODE, 0x02 },
  { "INTERNAL_LOOPBACK_MODE", CAN_INTERNAL_LOOPBACK_MODE, 0x03 },
  { "LISTEN_ONLY_MODE", CAN_LISTEN_ONLY_MODE, 0x04 },
  { "CONFIGURATION_MODE", CAN_CONFIGURATION_MODE, 0x05 },
  { "EXTERNAL_LOOPBACK_MODE", CAN_EXTERNAL_LOOPBACK_MODE, 0x06 },
  { "CLASSIC_MODE", CAN_CLASSIC_MODE, 0x07 },  //CAN2.0のみ対応。CAN2.0で動作させるときは、このモードに入れる
  { "RESTRICTED_MODE", CAN_RESTRICTED_MODE, 0x08 },
  { "INVALID_MODE", CAN_INVALID_MODE, 0x09 }
};

/**************************************************************************************
 Declaration for EEPROM
 **************************************************************************************/
#define EEPROM_APP_INIT_ADDR 0x00
#define EEPROM_CAN_OPEATION_ADDR 0x02
#define EEPROM_CAN20_BITRATE_ADDR 0x04
#define EEPROM_CANFD_BITRATE_ADDR 0x06
#define EEPROM_CAN20_CHIP_MODE_ADDR 0x08
#define EEPROM_CANFD_CHIP_MODE_ADDR 0x0A
//default value
#define EEPROM_INIT_DONE 0x02                //Ver.00-01
#define EEPROM_CAN_OPEATION_DEF_VAL 0x02     //CAN_OPERATION_CANFD
#define EEPROM_CAN20_BITRATE_DEF_VAL 0x10    //CAN20_500KBPS
#define EEPROM_CANFD_BITRATE_DEF_VAL 0x09    //CAN_500K_1M
#define EEPROM_CAN20_CHIP_MODE_DEF_VAL 0x07  //CLASSIC_MODE
#define EEPROM_CANFD_CHIP_MODE_DEF_VAL 0x01  //NORMAL_MODE

/**************************************************************************************
 Typedef for EEPROM
 **************************************************************************************/
//! EEPROM address and default value
typedef struct {
  byte addr;     // EEPROMのアドレス
  byte def_val;  // 初期値
} EEPROM_INIT_ENTRY_T;

/**************************************************************************************
 Table for EEPROM
 **************************************************************************************/
EEPROM_INIT_ENTRY_T EEPROM_INIT_ENTRY[] = {
  { EEPROM_APP_INIT_ADDR, EEPROM_INIT_DONE },
  { EEPROM_CAN_OPEATION_ADDR, EEPROM_CAN_OPEATION_DEF_VAL },
  { EEPROM_CAN20_BITRATE_ADDR, EEPROM_CAN20_BITRATE_DEF_VAL },
  { EEPROM_CANFD_BITRATE_ADDR, EEPROM_CANFD_BITRATE_DEF_VAL },
  { EEPROM_CAN20_CHIP_MODE_ADDR, EEPROM_CAN20_CHIP_MODE_DEF_VAL },
  { EEPROM_CANFD_CHIP_MODE_ADDR, EEPROM_CANFD_CHIP_MODE_DEF_VAL }
};

/**************************************************************************************
 Typedef for UART
 *************************************************************************************/
typedef enum {
  UART_EVENT_NONE,
  //operation
  UART_EVENT_SETTING_OPERATION,
  //can2.0
  UART_EVENT_CAN20_SEND,
  UART_EVENT_CAN20_SETTING_BITRATE,
  UART_EVENT_CAN20_SETTING_CHIP_MODE,
  //canfd
  UART_EVENT_CANFD_SEND,
  UART_EVENT_CANFD_SETTING_BITRATE,
  UART_EVENT_CANFD_SETTING_CHIP_MODE,
  //common
  UART_EVENT_LISTEN_EN,
  UART_EVENT_LISTEN_DIS,
  UART_EVENT_READ_PARA,
  UART_EVENT_MCU_RESET,
  UART_EVENT_CMD_ERROR
} user_uart_event_t;
//
typedef struct {
  unsigned long id;
  unsigned char data[CANFD_MAX_DATA_LEN];
  uint8_t len;
  uint32_t bitrate;
  byte mode;
  can_operation_t operation;
} user_uart_input_t;

/**************************************************************************************
 Global value
**************************************************************************************/
can_operation_t g_can_operation;
can20_config_t g_can20_config;
canfd_config_t g_canfd_config;
bool g_listen_enable_flag;

/**************************************************************************************
 Class for CAN 
 **************************************************************************************/
mcp2518fd CAN(GPIO_OUP_MCP2518FD_SPI_CS1);  // Set CS pin

/**************************************************************************************
 Local value for CAN 
 **************************************************************************************/
static unsigned char can20_rx_buf[CAN20_MAX_DATA_LEN];
static unsigned char canfd_rx_buf[CANFD_MAX_DATA_LEN];
static uint8_t can20_rx_len = 0;
static uint8_t canfd_rx_len = 0;

/**************************************************************************************
 Functions for CAN
 **************************************************************************************/
//Set CAN OPERATION CAN2.0
void set_can_operation_can20(can_operation_t *op) {
  *op = CAN_OPERATION_CAN20;
}

//Set CAN OPERATION CANFD
void set_can_operation_canfd(can_operation_t *op) {
  *op = CAN_OPERATION_CANFD;
}

//Set CAN2.0 BITARTE
void can20_set_bitrate(can20_config_t *cg, uint32_t br) {
  cg->bitrate = br;
}

//Set CAN2.0 chip mode
void can20_set_chip_mode(can20_config_t *cg, byte mode) {
  cg->mode = mode;
}

//Send CAN2.0 data
void can20_send_data(unsigned long id, unsigned char *data, uint8_t len) {
  byte ext = (id > 0x7FF) ? 1 : 0;  // 11ビット範囲外なら拡張フレーム
  CAN.setFdfFlagDisable();
  CAN.sendMsgBuf(id, ext, len, data);
  /* print log */
  log_message("INFO", "TX CAN2.0 Frame");
  //ID
  char buffer[13];  // バッファサイズは必要な長さに応じて調整
  if (ext) {
    snprintf(buffer, sizeof(buffer), "%08lX", id);  // 拡張フォーマット用
  } else {
    snprintf(buffer, sizeof(buffer), "%03lX", id);  // 標準フォーマット用 11 bit
  }
  Serial.print("<ID>");
  Serial.println(buffer);  // フォーマット済み文字列を出力
  //LEN
  Serial.print("<LEN>");
  Serial.println(len);
  //DATA
  Serial.print("<DATA>");
  for (int i = 0; i < len; i++) {
    if (data[i] < 0x10) {
      Serial.print("0");  // 1桁の場合は先頭に0を付ける
    }
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");
  //log
  delay(1);  //送信完了wait
}

///Set CANFD BITARTE
void canfd_set_bitrate(canfd_config_t *cg, uint32_t br) {
  cg->bitrate = br;
}

//Set CANFD chip mode
void canfd_set_chip_mode(canfd_config_t *cg, byte mode) {
  cg->mode = mode;
}

//Send CANFD data
void canfd_send_data(unsigned long id, unsigned char *data, uint8_t len) {
  byte ext = (id > 0x7FF) ? 1 : 0;  // 11ビット範囲外なら拡張フレーム
  CAN.setFdfFlagEnable();
  CAN.sendMsgBuf(id, ext, CANFD::len2dlc(len), data);
  /* print log */
  log_message("INFO", "TX CANFD Frame");
  //ID
  char buffer[13];  // バッファサイズは必要な長さに応じて調整
  if (ext) {
    snprintf(buffer, sizeof(buffer), "%08lX", id);  // 拡張フォーマット用
  } else {
    snprintf(buffer, sizeof(buffer), "%03lX", id);  // 標準フォーマット用
  }
  Serial.print("<ID>");
  Serial.println(buffer);  // フォーマット済み文字列を出力
  //LEN
  Serial.print("<LEN>");
  Serial.println(len);
  //DATA
  Serial.print("<DATA>");
  for (int i = 0; i < len; i++) {
    if (data[i] < 0x10) {
      Serial.print("0");  // 1桁の場合は先頭に0を付ける
    }
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println(" ");
  delay(1);  //送信完了wait
}

//Init CAN2.0
void can20_init(can20_config_t *cg) {
  CAN.setMode(cg->mode);  //CAN2.0でモード設定すると送信できない。
  while (0 != CAN.begin(cg->bitrate))
    ;
}

//Receive CAN2.0
void can20_receive_data(void) {
  if (CAN_MSGAVAIL == CAN.checkReceive())  // heck if data coming
  {
    CAN.readMsgBuf(&can20_rx_len, can20_rx_buf);  // You should call readMsgBuff before getCanId
    unsigned long id = CAN.getCanId();
    unsigned char ext = CAN.isExtendedFrame();

    /* print log */
    log_message("INFO", "RX Frame");
    //ID
    char buffer[13];  // バッファサイズは必要な長さに応じて調整
    if (ext) {
      snprintf(buffer, sizeof(buffer), "%08lX", id);  // 拡張フォーマット用
    } else {
      snprintf(buffer, sizeof(buffer), "%03lX", id);  // 標準フォーマット用
    }
    Serial.print("<ID>");
    Serial.println(buffer);  // フォーマット済み文字列を出力
    //LEN
    Serial.print("<LEN>");
    Serial.println(can20_rx_len);
    //DATA
    Serial.print("<DATA>");
    for (int i = 0; i < can20_rx_len; i++) {
      if (can20_rx_buf[i] < 0x10) {
        Serial.print("0");  // 1桁の場合は先頭に0を付ける
      }
      Serial.print(can20_rx_buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");
  }
}

//
void canfd_receive_data(void) {
  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    CAN.readMsgBuf(&canfd_rx_len, canfd_rx_buf);  // You should call readMsgBuff before getCanId
    unsigned long id = CAN.getCanId();
    unsigned char ext = CAN.isExtendedFrame();

    /* print log */
    log_message("INFO", "RX Frame");
    //ID
    char buffer[13];  // バッファサイズは必要な長さに応じて調整
    if (ext) {
      snprintf(buffer, sizeof(buffer), "%08lX", id);  // 拡張フォーマット用
    } else {
      snprintf(buffer, sizeof(buffer), "%03lX", id);  // 標準フォーマット用
    }
    Serial.print("<ID>");
    Serial.println(buffer);  // フォーマット済み文字列を出力
    //LEN
    Serial.print("<LEN>");
    Serial.println(canfd_rx_len);
    //DATA
    Serial.print("<DATA>");
    for (int i = 0; i < canfd_rx_len; i++) {
      if (canfd_rx_buf[i] < 0x10) {
        Serial.print("0");  // 1桁の場合は先頭に0を付ける
      }
      Serial.print(canfd_rx_buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");
  }
}


//Init CANFD
void canfd_init(canfd_config_t *cg) {
  CAN.setMode(cg->mode);
  while (0 != CAN.begin(cg->bitrate))
    ;
}

//Print Parameter
void print_can_para(can_operation_t *op, can20_config_t *can20_cg, canfd_config_t *canfd_cg) {
  log_separator();
#if 1
  //VERSION
  log_message("INFO", FW_VER_INF);

  //CAN OPERATION
  for (uint8_t i = 0; i < ARRAY_LENGTH(CAN_OPERATION_TABLE); i++) {
    if (CAN_OPERATION_TABLE[i].operation == *op) {
      log_message("INFO", "CAN OPERATION : %s", CAN_OPERATION_TABLE[i].name);
    }
  }

  //CAN2.0 BITRATE
  for (int i = 0; i < ARRAY_LENGTH(CAN20_BITRATE_CONFIG_TABLE); i++) {
    if (CAN20_BITRATE_CONFIG_TABLE[i].bitrate == can20_cg->bitrate) {
      log_message("INFO", "CAN2.0 BITRATE : %s", CAN20_BITRATE_CONFIG_TABLE[i].name);
    }
  }


  //CAN2.0 CHIP MODE
  for (int i = 0; i < ARRAY_LENGTH(CAN20_CHIP_MODE_CONFIG_TABLE); i++) {
    if (CAN20_CHIP_MODE_CONFIG_TABLE[i].mode == can20_cg->mode) {
      log_message("INFO", "CAN2.0 CHIP MODE : %s", CAN20_CHIP_MODE_CONFIG_TABLE[i].name);
    }
  }
#endif

  //CANFD BITRATE
  for (int i = 0; i < ARRAY_LENGTH(CANFD_BITRATE_CONFIG_TABLE); i++) {
    if (CANFD_BITRATE_CONFIG_TABLE[i].bitrate == canfd_cg->bitrate) {
      log_message("INFO", "CANFD BITRATE : %s", CANFD_BITRATE_CONFIG_TABLE[i].name);
    }
  }

  //CANFD CHIP MODE
  for (int i = 0; i < ARRAY_LENGTH(CANFD_CHIP_MODE_CONFIG_TABLE); i++) {
    if (CANFD_CHIP_MODE_CONFIG_TABLE[i].mode == canfd_cg->mode) {
      log_message("INFO", "CANFD CHIP MODE : %s", CANFD_CHIP_MODE_CONFIG_TABLE[i].name);
    }
  }

  log_separator();
}

/**************************************************************************************
 Local value for UART
 **************************************************************************************/
typedef struct {
  char buf[150];
  int ptr;
} uart_rx_data_t;
static uart_rx_data_t uart_rx_data;
/**************************************************************************************
 Functions for UART
 **************************************************************************************/
user_uart_event_t user_uart_event_task(user_uart_input_t *inp) {
  // UARTの受信処理
  if (Serial.available() <= 0) {  // Serial.available() の条件を修正
    return UART_EVENT_NONE;
  }

  // バッファオーバーフローのチェック
  if (uart_rx_data.ptr >= sizeof(uart_rx_data.buf) - 1) {  // sizeof()の使用法を修正
    uart_rx_data.ptr = 0;                                  // RXバッファをクリア
    while (Serial.available() > 0) Serial.read();          // 受信バッファのデータをすべて読み出してクリア
    log_message("WARN", "UART buffer overflow");
    return UART_EVENT_NONE;
  }
  // データの読み取り
  uart_rx_data.buf[uart_rx_data.ptr] = Serial.read();

  // 終端文字のチェック
  if (uart_rx_data.buf[uart_rx_data.ptr] != '\r') {
    uart_rx_data.ptr++;  // ポインタを進める
    return UART_EVENT_NONE;
  }

  // 終端処理
  uart_rx_data.buf[uart_rx_data.ptr] = '\0';    // NULL文字を追加
  String input_str = String(uart_rx_data.buf);  // バッファをString型に変換

  // RXバッファのポインタをリセット
  uart_rx_data.ptr = 0;
  //
  int first_hash_index = input_str.indexOf('#');
  /*******************************************************************/
  // CAN Send Command
  /*******************************************************************/
  if (first_hash_index != -1) {
    String before_first_hash_str = input_str.substring(0, first_hash_index);  // 最初の#の前の部分
    String after_first_hash_str = input_str.substring(first_hash_index + 1);  // 最初の#の後の部分
    int second_hash_idx = after_first_hash_str.indexOf('#');


    /* ##コマンド */
    if (second_hash_idx == 0) {
      // Check ID
      if (!parse_can_id_data(&inp->id, before_first_hash_str)) {
        log_message("ERROR", "ID error");
        return UART_EVENT_CMD_ERROR;
      }
      // Check DATA
      if (!parse_canfd_hex_data(inp->data, input_str.substring(first_hash_index + 2), &inp->len)) {
        log_message("ERROR", "DATA error");
        return UART_EVENT_CMD_ERROR;
      }
      // Check OPERATION
      if (g_can_operation == CAN_OPERATION_CAN20) {
        log_message("ERROR", "CMD error");
        return UART_EVENT_CMD_ERROR;
      }
      //log_message("INFO", "<CMD:CANFD> ID:%X, LEN:%d", inp->id, inp->len);
      return UART_EVENT_CANFD_SEND;

    }


    /* #コマンド */
    else {
      // Check ID
      if (!parse_can_id_data(&inp->id, before_first_hash_str)) {
        log_message("ERROR", "ID error");
        return UART_EVENT_CMD_ERROR;
      }
      // Check DATA
      if (!parse_can20_hex_data(inp->data, input_str.substring(first_hash_index + 1), &inp->len)) {
        log_message("ERROR", "DATA error");
        return UART_EVENT_CMD_ERROR;
      }

      //log_message("INFO", "<CMD:CAN20> ID:%X, LEN:%d", inp->id, inp->len);
      return UART_EVENT_CAN20_SEND;
    }
  }
  /*******************************************************************/
  // CAN Setting
  /*******************************************************************/
  if (input_str.indexOf('@') == 0) {
    int equal_idx = input_str.indexOf('=');
    String cmd_str = input_str.substring(1, equal_idx);   // =の前の部分
    String arg_str = input_str.substring(equal_idx + 1);  // 最初の#の後の部分
    //
    if (equal_idx != 0 && arg_str.length() > 0) {


      /* @bコマンド */
      if (cmd_str == "b") {
        for (int i = 0; i < ARRAY_LENGTH(CAN20_BITRATE_CONFIG_TABLE); i++) {
          if (CAN20_BITRATE_CONFIG_TABLE[i].name == arg_str) {
            inp->bitrate = CAN20_BITRATE_CONFIG_TABLE[i].bitrate;
            //
            log_message("INFO", "CAN2.0 BITRATE : %s", CAN20_BITRATE_CONFIG_TABLE[i].name);
            return UART_EVENT_CAN20_SETTING_BITRATE;
          }
        }
      }


      /* @fbコマンド */
      else if (cmd_str == "fb") {
        for (int i = 0; i < ARRAY_LENGTH(CANFD_BITRATE_CONFIG_TABLE); i++) {
          if (CANFD_BITRATE_CONFIG_TABLE[i].name == arg_str) {
            inp->bitrate = CANFD_BITRATE_CONFIG_TABLE[i].bitrate;
            //
            log_message("INFO", "CANFD BITRATE : %s", CANFD_BITRATE_CONFIG_TABLE[i].name);
            return UART_EVENT_CANFD_SETTING_BITRATE;
          }
        }
      }


      /* @mコマンド */
      else if (cmd_str == "m") {
        for (int i = 0; i < ARRAY_LENGTH(CAN20_CHIP_MODE_CONFIG_TABLE); i++) {
          if (CAN20_CHIP_MODE_CONFIG_TABLE[i].name == arg_str) {
            inp->mode = CAN20_CHIP_MODE_CONFIG_TABLE[i].mode;
            //
            log_message("INFO", "CAN2.0 CHIP MODE : %s", CAN20_CHIP_MODE_CONFIG_TABLE[i].name);
            return UART_EVENT_CAN20_SETTING_CHIP_MODE;
          }
        }
      }


      /* @fmコマンド */
      else if (cmd_str == "fm") {
        for (int i = 0; i < ARRAY_LENGTH(CANFD_CHIP_MODE_CONFIG_TABLE); i++) {
          if (CANFD_CHIP_MODE_CONFIG_TABLE[i].name == arg_str) {
            inp->mode = CANFD_CHIP_MODE_CONFIG_TABLE[i].mode;
            //
            log_message("INFO", "CANFD CHIP MODE : %s", CANFD_CHIP_MODE_CONFIG_TABLE[i].name);
            return UART_EVENT_CANFD_SETTING_CHIP_MODE;
          }
        }
      }
    }


    /* @can2.0コマンド */
    if (cmd_str == "can2.0") {
      inp->operation = CAN_OPERATION_CAN20;
      return UART_EVENT_SETTING_OPERATION;
    }


    /* @canfdコマンド */
    if (cmd_str == "canfd") {
      inp->operation = CAN_OPERATION_CANFD;
      return UART_EVENT_SETTING_OPERATION;
    }


    /* @pコマンド */
    if (cmd_str == "p") {
      return UART_EVENT_READ_PARA;
    }

    /* @rコマンド */
    if (cmd_str == "r") {
      return UART_EVENT_MCU_RESET;
    }

    /* @lコマンド */
    if (cmd_str == "l") {
      return UART_EVENT_LISTEN_EN;
    }

    /* @Lコマンド */
    if (cmd_str == "L") {
      return UART_EVENT_LISTEN_DIS;
    }
  }
  /*******************************************************************/
  // CMD Error
  /*******************************************************************/
  log_message("ERROR", "Input cmd error");

  return UART_EVENT_NONE;
}

//
static bool parse_can_id_data(unsigned long *id, String parse_str) {
  // 空の文字列は数字とみなさない
  if (parse_str.length() == 0) return false;

  char *endPtr;
  // 16進数で解析
  long value = strtol(parse_str.c_str(), &endPtr, 16);

  // エラーチェック: 無効な値や範囲外の場合
  if (endPtr == parse_str.c_str() || *endPtr != '\0' || value < 0x00 || value > 0x1FFFFFFF) {
    return false;
  }

  *id = (unsigned long)value;
  return true;
}
//
static bool parse_can20_hex_data(unsigned char *data, String parse_str, uint8_t *len) {
  int str_len = parse_str.length();  // 文字列の長さを取得
  if (str_len == 0 || str_len > CAN20_MAX_DATA_LEN * 2 || str_len % 2 == 1) {
    return false;  // データ長チェック
  }

  int start_idx = 0;
  uint8_t data_index = 0;  // 格納したデータのインデックス（バイト数）

  while (start_idx < str_len) {
    if (!process_token(data + data_index, parse_str.substring(start_idx, start_idx + 2))) {
      return false;  // トークンの処理に失敗した場合
    }
    start_idx = start_idx + 2;
    //
    data_index++;  // 次のデータ格納位置に移動
  }
  // データの長さ（バイト数）を引数のlenに設定
  *len = data_index;

  return true;
}

//
static bool parse_canfd_hex_data(unsigned char *data, String parse_str, uint8_t *len) {
  const uint8_t dlc_table[] = { 12, 16, 20, 24, 32, 48, 64 };

  int str_len = parse_str.length();  // 入力文字列長
  if (str_len == 0 || str_len > CANFD_MAX_DATA_LEN * 2 || str_len % 2 == 1) {
    return false;  // 0長、奇数長、長すぎNG
  }

  int start_idx = 0;
  uint8_t data_index = 0;

  while (start_idx < str_len) {
    if (!process_token(data + data_index, parse_str.substring(start_idx, start_idx + 2))) {
      return false;  // 2文字ずつ処理、失敗ならNG
    }
    start_idx += 2;
    data_index++;
  }

  // 8バイト以下ならそのまま（0埋めしたければここで対応）
  if (data_index <= 8) {
    *len = data_index;
    return true;
  }

  // 9バイト以上：CAN FD DLCの対応範囲から最小サイズを探す
  for (uint8_t i = 0; i < sizeof(dlc_table) / sizeof(dlc_table[0]); i++) {
    if (data_index <= dlc_table[i]) {
      // 足りない分は0で埋める
      memset(data + data_index, 0x00, dlc_table[i] - data_index);
      *len = dlc_table[i];  // 長さ（DLC換算後）を設定
      return true;
    }
  }

  return false;  // 64バイトを超えていた場合
}

// トークン処理用のヘルパー関数
static bool process_token(unsigned char *data, String token) {
  char *endPtr;
  long value = strtol(token.c_str(), &endPtr, 16);

  if (endPtr == token.c_str() || *endPtr != '\0' || value < 0x00 || value > 0xFF) {
    return false;
  }

  *data = (unsigned char)value;
  return true;
}


/**************************************************************************************
 Functions for EEPROM
 **************************************************************************************/
void eeprom_init(void) {
  byte addr1_value = EEPROM.read(EEPROM_APP_INIT_ADDR);
  byte addr2_value = EEPROM.read(EEPROM_APP_INIT_ADDR + 1);
  //
  //log_message("INFO", "Starting EEPROM init...");
  //初期値書き込み済み確認
  if (addr1_value != EEPROM_INIT_DONE || addr2_value != EEPROM_INIT_DONE) {
    log_message("WARN", "EEPROM setup is not done. Writing default values...");
    eeprom_write_default_value();
  }
  //EEPROMのエラーチェック
  if (!eeprom_error_check()) {
    log_message("INFO", "Writing default values to EEPROM...");
    eeprom_write_default_value();
  }

  //log_message("INFO", "EEPROM init done");
}

//
static void eeprom_write_default_value(void) {
  for (uint8_t i = 0; i < ARRAY_LENGTH(EEPROM_INIT_ENTRY); i++) {
    EEPROM.write(EEPROM_INIT_ENTRY[i].addr, EEPROM_INIT_ENTRY[i].def_val);
    EEPROM.write(EEPROM_INIT_ENTRY[i].addr + 1, EEPROM_INIT_ENTRY[i].def_val);
  }
}

//
static bool eeprom_error_check(void) {
  for (uint8_t i = 0; i < ARRAY_LENGTH(EEPROM_INIT_ENTRY); i++) {
    byte temp1 = EEPROM.read(EEPROM_INIT_ENTRY[i].addr);
    byte temp2 = EEPROM.read(EEPROM_INIT_ENTRY[i].addr + 1);
    // error check
    if (temp1 != temp2) {
      log_message("ERROR", "EEPROM stored value is invalid");
      return false;
    }
  }  //
  can_operation_t dumy_op;
  uint32_t dumy_br;
  byte dumy_mode;
  if (!eeprom_read_can_operation(&dumy_op) || !eeprom_read_can20_bitrate(&dumy_br) || !eeprom_read_canfd_bitrate(&dumy_br) || !eeprom_read_can20_chip_mode(&dumy_mode) || !eeprom_read_canfd_chip_mode(&dumy_mode)) {
    log_message("ERROR", "EEPROM stored value is invalid");
    return false;
  }
  //
  return true;
}
//
bool eeprom_read_can_operation(can_operation_t *op) {
  byte id = EEPROM.read(EEPROM_CAN_OPEATION_ADDR);
  //
  for (uint8_t i = 0; i < ARRAY_LENGTH(CAN_OPERATION_TABLE); i++) {
    if (CAN_OPERATION_TABLE[i].id == id) {
      *op = CAN_OPERATION_TABLE[i].operation;
      return true;
    }
  }
  log_message("ERROR", "Faile read CAN OPERATIONS");
  return false;
}

//
bool eeprom_read_can20_bitrate(uint32_t *br) {
  byte id = EEPROM.read(EEPROM_CAN20_BITRATE_ADDR);
  //
  for (int i = 0; i < ARRAY_LENGTH(CAN20_BITRATE_CONFIG_TABLE); i++) {
    if (CAN20_BITRATE_CONFIG_TABLE[i].id == id) {
      *br = CAN20_BITRATE_CONFIG_TABLE[i].bitrate;
      return true;
    }
  }
  log_message("ERROR", "Faile read CAN20 BITRATE");
  return false;
}

//
bool eeprom_read_canfd_bitrate(uint32_t *br) {
  byte id = EEPROM.read(EEPROM_CANFD_BITRATE_ADDR);
  //
  for (int i = 0; i < ARRAY_LENGTH(CANFD_BITRATE_CONFIG_TABLE); i++) {
    if (CANFD_BITRATE_CONFIG_TABLE[i].id == id) {
      *br = CANFD_BITRATE_CONFIG_TABLE[i].bitrate;
      return true;
    }
  }
  log_message("ERROR", "Faile read CANFD BITRATE");
  return false;
}

//
bool eeprom_read_can20_chip_mode(byte *mode) {
  byte id = EEPROM.read(EEPROM_CAN20_CHIP_MODE_ADDR);
  //
  for (int i = 0; i < ARRAY_LENGTH(CAN20_CHIP_MODE_CONFIG_TABLE); i++) {
    if (CAN20_CHIP_MODE_CONFIG_TABLE[i].id == id) {
      *mode = CAN20_CHIP_MODE_CONFIG_TABLE[i].mode;
      return true;
    }
  }
  log_message("ERROR", "Faile read CAN2.0 CHIP MODE");
  return false;
}

//
bool eeprom_read_canfd_chip_mode(byte *mode) {
  byte id = EEPROM.read(EEPROM_CANFD_CHIP_MODE_ADDR);
  //
  for (int i = 0; i < ARRAY_LENGTH(CANFD_CHIP_MODE_CONFIG_TABLE); i++) {
    if (CANFD_CHIP_MODE_CONFIG_TABLE[i].id == id) {
      *mode = CANFD_CHIP_MODE_CONFIG_TABLE[i].mode;
      return true;
    }
  }
  log_message("ERROR", "Faile read CANFD CHIP MODE");
  return false;
}

//
bool eeprom_write_can_operation(can_operation_t op) {
  for (int i = 0; i < ARRAY_LENGTH(CAN_OPERATION_TABLE); i++) {
    if (CAN_OPERATION_TABLE[i].operation == op) {
      EEPROM.write(EEPROM_CAN_OPEATION_ADDR, CAN_OPERATION_TABLE[i].id);
      EEPROM.write(EEPROM_CAN_OPEATION_ADDR + 1, CAN_OPERATION_TABLE[i].id);
      return true;
    }
  }
  log_message("ERROR", "Faile write CAN OPERATIONS");
  return false;
}

//
bool eeprom_write_can20_bitrate(uint32_t br) {
  for (int i = 0; i < ARRAY_LENGTH(CAN20_BITRATE_CONFIG_TABLE); i++) {
    if (CAN20_BITRATE_CONFIG_TABLE[i].bitrate == br) {
      EEPROM.write(EEPROM_CAN20_BITRATE_ADDR, CAN20_BITRATE_CONFIG_TABLE[i].id);
      EEPROM.write(EEPROM_CAN20_BITRATE_ADDR + 1, CAN20_BITRATE_CONFIG_TABLE[i].id);
      return true;
    }
  }
  log_message("ERROR", "Faile write CAN20 BITRATE");
  return false;
}

//
bool eeprom_write_canfd_bitrate(uint32_t br) {
  for (int i = 0; i < ARRAY_LENGTH(CANFD_BITRATE_CONFIG_TABLE); i++) {
    if (CANFD_BITRATE_CONFIG_TABLE[i].bitrate == br) {
      EEPROM.write(EEPROM_CANFD_BITRATE_ADDR, CANFD_BITRATE_CONFIG_TABLE[i].id);
      EEPROM.write(EEPROM_CANFD_BITRATE_ADDR + 1, CANFD_BITRATE_CONFIG_TABLE[i].id);
      return true;
    }
  }
  log_message("ERROR", "Faile write CANFD BITRATE");
  return false;
}

//
bool eeprom_write_can20_chip_mode(byte mode) {
  for (int i = 0; i < ARRAY_LENGTH(CAN20_CHIP_MODE_CONFIG_TABLE); i++) {
    if (CAN20_CHIP_MODE_CONFIG_TABLE[i].mode == mode) {
      EEPROM.write(EEPROM_CAN20_CHIP_MODE_ADDR, CAN20_CHIP_MODE_CONFIG_TABLE[i].id);
      EEPROM.write(EEPROM_CAN20_CHIP_MODE_ADDR + 1, CAN20_CHIP_MODE_CONFIG_TABLE[i].id);
      return true;
    }
  }
  log_message("ERROR", "Faile write CAN2.0 CHIP MODE");
  return false;
}

//
bool eeprom_write_canfd_chip_mode(byte mode) {
  for (int i = 0; i < ARRAY_LENGTH(CANFD_CHIP_MODE_CONFIG_TABLE); i++) {
    if (CANFD_CHIP_MODE_CONFIG_TABLE[i].mode == mode) {
      EEPROM.write(EEPROM_CANFD_CHIP_MODE_ADDR, CANFD_CHIP_MODE_CONFIG_TABLE[i].id);
      EEPROM.write(EEPROM_CANFD_CHIP_MODE_ADDR + 1, CANFD_CHIP_MODE_CONFIG_TABLE[i].id);
      return true;
    }
  }
  log_message("ERROR", "Faile write CANFD CHIP MODE");
  return false;
}


/**************************************************************************************
 Functions for log
 **************************************************************************************/
#define PRINTF_BUF_SIZE 256
// 区切り線を出力
void log_separator(void) {
  Serial.println("====================================");
}

// ログメッセージを出力 (printf対応)
void log_message(const char *tag, const char *fmt, ...) {
  char buf[PRINTF_BUF_SIZE];

  // ログフォーマットのヘッダ部分
  Serial.print("[");
  Serial.print(tag);
  Serial.print("] ");

  // メッセージ部分をフォーマット
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, PRINTF_BUF_SIZE, fmt, args);
  va_end(args);

  // シリアルポートに出力
  Serial.println(buf);
}

/**************************************************************************************
 CAN LOOP
 **************************************************************************************/
void user_uart_event_handler(user_uart_event_t event, user_uart_input_t input) {
  //
  switch (event) {
    /*************************************/
    case UART_EVENT_CAN20_SEND:
      if (g_listen_enable_flag) {
        /* OPERATION:CAN2.0 */
        if (g_can_operation == CAN_OPERATION_CAN20) {
          //CLASSIC MODEに設定
          can20_set_chip_mode(&g_can20_config, CAN_CLASSIC_MODE);
          can20_init(&g_can20_config);
          //送信
          can20_send_data(input.id, input.data, input.len);
          //LISTE MODに設定
          can20_set_chip_mode(&g_can20_config, CAN_LISTEN_ONLY_MODE);
          can20_init(&g_can20_config);
        }
        /* OPERATION:CANFD */
        if (g_can_operation == CAN_OPERATION_CANFD) {
          //CLASSIC MODEに設定
          canfd_set_chip_mode(&g_canfd_config, CAN_CLASSIC_MODE);
          canfd_init(&g_canfd_config);
          //送信
          canfd_send_data(input.id, input.data, input.len);
          //LISTE MODに設定
          canfd_set_chip_mode(&g_canfd_config, CAN_LISTEN_ONLY_MODE);
          canfd_init(&g_canfd_config);
        }
      } else {
        can20_send_data(input.id, input.data, input.len);
      }
      break;
    /*************************************/
    case UART_EVENT_CANFD_SEND:
      if (g_listen_enable_flag) {
        //CLASSIC MODEに設定
        canfd_set_chip_mode(&g_canfd_config, CAN_NORMAL_MODE);
        canfd_init(&g_canfd_config);
        //CANFDで送信
        canfd_send_data(input.id, input.data, input.len);
        //LISTE MODに設定
        canfd_set_chip_mode(&g_canfd_config, CAN_LISTEN_ONLY_MODE);
        canfd_init(&g_canfd_config);
      } else {
        canfd_send_data(input.id, input.data, input.len);
      }
      break;
    /*************************************/
    case UART_EVENT_CAN20_SETTING_BITRATE:
      //set bitrate
      can20_set_bitrate(&g_can20_config, input.bitrate);
      //write eeprom
      eeprom_write_can20_bitrate(g_can20_config.bitrate);
      //can init
      if (g_can_operation == CAN_OPERATION_CAN20) {
        can20_init(&g_can20_config);
      }
      break;
    /*************************************/
    case UART_EVENT_CANFD_SETTING_BITRATE:
      //set bitrate
      canfd_set_bitrate(&g_canfd_config, input.bitrate);
      //write eeprom
      eeprom_write_canfd_bitrate(g_canfd_config.bitrate);
      //can init
      if (g_can_operation == CAN_OPERATION_CANFD) {
        canfd_init(&g_canfd_config);
      }
      break;
    /*************************************/
    case UART_EVENT_CAN20_SETTING_CHIP_MODE:
      //set mode
      can20_set_chip_mode(&g_can20_config, input.mode);
      //write eeprom
      eeprom_write_can20_chip_mode(g_can20_config.mode);
      //can init
      if (CAN_OPERATION_CAN20 == g_can_operation) {
        can20_init(&g_can20_config);
      }
      break;
    /*************************************/
    case UART_EVENT_CANFD_SETTING_CHIP_MODE:
      //set mode
      canfd_set_chip_mode(&g_canfd_config, input.mode);
      //write eeprom
      eeprom_write_canfd_chip_mode(g_canfd_config.mode);
      //can init
      if (CAN_OPERATION_CANFD == g_can_operation) {
        canfd_init(&g_canfd_config);
      }
      break;
    /*************************************/
    case UART_EVENT_SETTING_OPERATION:
      /* set can operation */
      if (CAN_OPERATION_CAN20 == input.operation) {
        set_can_operation_can20(&g_can_operation);
      }
      /* set can operation */
      if (CAN_OPERATION_CANFD == input.operation) {
        set_can_operation_canfd(&g_can_operation);
      }
      //write eepro
      eeprom_write_can_operation(g_can_operation);
      //restart
      log_message("INFO", "Restart MCU");
      NVIC_SystemReset();
      break;
    /*************************************/
    case UART_EVENT_LISTEN_DIS:
      g_listen_enable_flag = false;
      can20_set_chip_mode(&g_can20_config, CAN_CLASSIC_MODE);
      canfd_set_chip_mode(&g_canfd_config, CAN_NORMAL_MODE);

      /* OPERATION:CAN2.0 */
      if (g_can_operation == CAN_OPERATION_CAN20) {
        can20_init(&g_can20_config);
      }
      /* OPERATION:CANFD */
      if (g_can_operation == CAN_OPERATION_CANFD) {
        canfd_init(&g_canfd_config);
      }
      log_message("INFO", "LISTEN disable");
      break;
    /*************************************/
    case UART_EVENT_LISTEN_EN:
      g_listen_enable_flag = true;
      can20_set_chip_mode(&g_can20_config, CAN_LISTEN_ONLY_MODE);
      canfd_set_chip_mode(&g_canfd_config, CAN_LISTEN_ONLY_MODE);

      /* OPERATION:CAN2.0 */
      if (g_can_operation == CAN_OPERATION_CAN20) {
        can20_init(&g_can20_config);
      }
      /* OPERATION:CANFD */
      if (g_can_operation == CAN_OPERATION_CANFD) {
        canfd_init(&g_canfd_config);
      }
      log_message("INFO", "LISTEN enable");
      break;
    /*************************************/
    case UART_EVENT_READ_PARA:
      print_can_para(&g_can_operation, &g_can20_config, &g_canfd_config);
      break;
    /*************************************/
    case UART_EVENT_MCU_RESET:
      log_message("INFO", "Restart MCU");
      NVIC_SystemReset();
      break;
    /*************************************/
    case UART_EVENT_NONE: break;
    /*************************************/
    default:
      break;
  }
}

/**************************************************************************************
 Setup
 **************************************************************************************/
void setup() {
  //System init
  Serial.begin(UART_BAUDRATE);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB
  }
  delay(3000);

  //EEPROM
  eeprom_init();

  //Global
  eeprom_read_can_operation(&g_can_operation);
  eeprom_read_can20_bitrate(&g_can20_config.bitrate);
  eeprom_read_canfd_bitrate(&g_canfd_config.bitrate);
  eeprom_read_can20_chip_mode(&g_can20_config.mode);
  eeprom_read_canfd_chip_mode(&g_canfd_config.mode);
  g_listen_enable_flag = false;

  //CAN20 init
  if (g_can_operation == CAN_OPERATION_CAN20) {
    can20_init(&g_can20_config);
  }
  //CANFD init
  if (g_can_operation == CAN_OPERATION_CANFD) {
    canfd_init(&g_canfd_config);
  }

  print_can_para(&g_can_operation, &g_can20_config, &g_canfd_config);
  log_message("INFO", "------------start------------");
}

/**************************************************************************************
 Loop
 **************************************************************************************/
void loop() {
  user_uart_event_t event;
  user_uart_input_t input;
  /* UARTイベント処理 */
  event = user_uart_event_task(&input);
  user_uart_event_handler(event, input);
  /* CAN2.0 受信処理 */
  if (g_can_operation == CAN_OPERATION_CAN20) {
    can20_receive_data();
  }
  /* CANFD 受信処理 */
  if (g_can_operation == CAN_OPERATION_CANFD) {
    canfd_receive_data();
  }
}
