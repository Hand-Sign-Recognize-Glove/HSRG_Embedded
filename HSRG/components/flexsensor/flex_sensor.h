#pragma once

/** 
 * @brief flex sensor의 기본 설정 및 값을 얻어온 후 main에 queue로 전송한다    
 * @param void* pvParameters 로 Task 핸들을 받아온다
 * @retval flex sensor의 값을 리턴한다
*/
void flex_sensor_get_value(void* pvParameters); 